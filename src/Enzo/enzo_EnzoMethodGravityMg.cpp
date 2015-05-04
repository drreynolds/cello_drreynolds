// See LICENSE_CELLO file for license and copyright information

/// @file     enzo_EnzoMethodGravityMg.cpp
/// @author   James Bordner (jobordner@ucsd.edu)
/// @date     2014-10-21 17:25:09
/// @brief    Implements the EnzoMethodGravityMg class
///
/// Multigrid method on an adaptive mesh.  We use the MLAT (Multilevel
/// Adaptive Technique) of Brandt '77, based on the FAS formulation of
/// Multigrid.
///
/// From "Multilevel Adaptive Methods for Partial Differential Equations", McCormick, 1989, p90
///
///  [WARNING: Bug in doxygen gets formulas in wrong place below processed html output; latex output is good]
/// \f$ u^{h} \leftarrow \mbox{MG}^{h} (u^{h}; f^{h}) \f$
///
///   - if \f$ h = h_{c} \f$
///      -# \f$ solve L^{h} u^{h} = f^{h} \f$
///      -# return
///   - else
///      -# \f$ u^{h} \leftarrow G^{h} (u^{h}; f^{h}) \f$
///      -# \f$ \tau_{h}^{2d} \leftarrow L^{2h} (I_{h}^{2h} u^{h}) - I_{h}^{2h}(L^{h}(u^{h})) \f$ 
///      -# \f$ f^{2h} \leftarrow I_{h}^{2h} f^{h} + \tau_{h}^{2h} \f$ 
///      -# \f$ u^{2h} \leftarrow I_{h}^{2h}u^{h} \f$ 
///      -# \f$ u^{2h} \leftarrow MG^{2h}(u^{2h}; f^{2h}) \f$ 
///      -# \f$ u^{h} \leftarrow u^{h} + I^{h}_{2h}(u^{2h} - I_{h}^{2h}u^{h}) \f$ 
///      -# \f$ u^{h} \leftarrow G^{h}(u^{h}; f^{h}) \f$ 
///
/// ---------------------
/// (*) can skip second refresh if e.g. nu_pre < # ghost zones
///
/// Required Fields
///
/// - B                          linear system right-hand side
/// - C                          correction computed as A C = R
/// - R                          residual R = B - A*X
/// - X                          current solution to A*X = B
/// - potential                  computed gravitational potential
/// - density                    density field
/// - acceleration_x             acceleration along X-axis
/// - acceleration_y (rank >= 2) acceleration along Y-axis
/// - acceleration_z (rank >= 3) acceleration along Z-axis

#include "cello.hpp"

#include "enzo.hpp"

#include "enzo.decl.h"

#define CK_TEMPLATES_ONLY
#include "enzo.def.h"
#undef CK_TEMPLATES_ONLY

//----------------------------------------------------------------------

EnzoMethodGravityMg::EnzoMethodGravityMg 
(FieldDescr * field_descr, int rank,
 double grav_const, int iter_max, double res_tol, int monitor_iter,
 bool is_singular,
 Compute * smooth,
 Restrict * restrict,
 Prolong * prolong) 
  : Method(), 
    A_(new EnzoMatrixLaplace),
    smooth_(smooth),
    restrict_(restrict),
    prolong_(prolong),
    is_singular_(is_singular),
    rank_(rank),
    grav_const_(grav_const),
    iter_max_(iter_max), 
    res_tol_(res_tol),
    monitor_iter_(monitor_iter),
    rr0_(0),rr_(0), rr_min_(0),rr_max_(0),
    idensity_(0),  ipotential_(0),
    ib_(0), ix_(0), ir_(0), ic_(0),
    nx_(0),ny_(0),nz_(0),
    mx_(0),my_(0),mz_(0),
    gx_(0),gy_(0),gz_(0),
    iter_(0),
    level_(0),
    is_active_(0)
{
  idensity_   = field_descr->field_id("density");
  ipotential_ = field_descr->field_id("potential");

  ib_ = field_descr->field_id("B");
  ix_ = field_descr->field_id("X");
  ir_ = field_descr->field_id("R");
  ic_ = field_descr->field_id("C");
}

//----------------------------------------------------------------------

EnzoMethodGravityMg::~EnzoMethodGravityMg () throw()
{
  delete smooth_;
  delete prolong_;
  delete restrict_;

  smooth_ = NULL;
  prolong_ = NULL;
  restrict_ = NULL;
}


//----------------------------------------------------------------------

void EnzoMethodGravityMg::compute ( Block * block) throw()
{
  
  set_active(block);

  Field field = block->data()->field();

  EnzoBlock * enzo_block = static_cast<EnzoBlock*> (block);

  field.size                (&nx_,&ny_,&nz_);
  field.dimensions(idensity_,&mx_,&my_,&mz_);
  field.ghosts    (idensity_,&gx_,&gy_,&gz_);

  int precision = field.precision(idensity_);

  if      (precision == precision_single)    compute_<float>      (enzo_block);
  else if (precision == precision_double)    compute_<double>     (enzo_block);
  else if (precision == precision_quadruple) compute_<long double>(enzo_block);
  else 
    ERROR1("EnzoMethodGravityMg()", "precision %d not recognized", precision);
}

//----------------------------------------------------------------------

extern CkReduction::reducerType r_method_gravity_mg_type;

template <class T>
void EnzoMethodGravityMg::compute_ (EnzoBlock * enzo_block) throw()
//     X = initial guess
//     B = right-hand side
//     R = B - A*X
//     solve(M*Z = R)
//     D = Z
//     shift (B)
{

  set_active(enzo_block);

  iter_ = 0;

  Data * data = enzo_block->data();
  Field field = data->field();

  T * density = (T*) field.values(idensity_);
    
  T * B = (T*) field.values(ib_);
  T * X = (T*) field.values(ix_);
  T * R = (T*) field.values(ir_);
  T * C = (T*) field.values(ic_);

  // Initialize B, X, R, C

  if (is_active_) {

    /// X = 0
    /// B = -h^2 * 4 * PI * G * density
    /// R = B ( residual with X = 0 )
    /// C = 0

    for (int iz=0; iz<mz_; iz++) {
      for (int iy=0; iy<my_; iy++) {
	for (int ix=0; ix<mx_; ix++) {
	  int i = ix + mx_*(iy + my_*iz);
	  B[i] = - 4.0 * (cello::pi) * grav_const_ * density[i];
	  X[i] = 0.0;
	  R[i] = B[i];
	  C[i] = 0.0;
	}
      }
    }
  }

  mg_end<T>(enzo_block,return_unknown);

}

//----------------------------------------------------------------------

void EnzoMethodGravityMg::pre_smooth (EnzoBlock * enzo_block, int level)
{
}

//----------------------------------------------------------------------

void EnzoMethodGravityMg::compute_residual (EnzoBlock * enzo_block, int level)
{
}

//----------------------------------------------------------------------

void EnzoMethodGravityMg::restrict_residual (EnzoBlock * enzo_block, int level)
{
}

//----------------------------------------------------------------------

void EnzoMethodGravityMg::coarse_solve (EnzoBlock * enzo_block, int level)
{
}

//----------------------------------------------------------------------

void EnzoMethodGravityMg::prolong_correction (EnzoBlock * enzo_block, int level)
{
}

//----------------------------------------------------------------------

void EnzoMethodGravityMg::update_solution (EnzoBlock * enzo_block, int level)
{
}

//----------------------------------------------------------------------

void EnzoMethodGravityMg::post_smooth (EnzoBlock * enzo_block, int level)
{
}

//======================================================================

void EnzoMethodGravityMg::monitor_output_(EnzoBlock * enzo_block) throw()
{

  Monitor * monitor = enzo_block->simulation()->monitor();

  monitor->print ("Enzo", "MG iter %04d  rr %g [%g %g]",
		  iter_,
		  (double)(rr_    / rr0_),
		  (double)(rr_min_/ rr0_),
		  (double)(rr_max_/ rr0_));
}

//----------------------------------------------------------------------

// void EnzoBlock::enzo_matvec_()
// {
//   EnzoMethodGravityMg * method = 
//     static_cast<EnzoMethodGravityMg*> (this->method());

//   Field field = data()->field();
//   int precision = field.precision(field.field_id("density")); assuming 

//   EnzoBlock * enzo_block = static_cast<EnzoBlock*> (this);

//   if      (precision == precision_single)    
//     method->mg_shift_1<float>(enzo_block);
//   else if (precision == precision_double)    
//     method->mg_shift_1<double>(enzo_block);
//   else if (precision == precision_quadruple) 
//     method->mg_shift_1<long double>(enzo_block);
//   else 
//     ERROR1("EnzoMethodGravityMg()", "precision %d not recognized", precision);
// }

//----------------------------------------------------------------------


template <class T>
void EnzoMethodGravityMg::mg_end (EnzoBlock * enzo_block,int retval) throw ()
{
  set_active(enzo_block);

  enzo_block->clear_refresh();

  Data * data = enzo_block->data();
  Field field = data->field();

  T * X         = (T*) field.values(ix_);
  T * potential = (T*) field.values(ipotential_);

  copy_(potential,X,mx_,my_,mz_,is_active_);

  FieldDescr * field_descr = field.field_descr();

  EnzoComputeAcceleration compute_acceleration (field_descr,rank_, true,2);

  compute_acceleration.compute(enzo_block);

  if (enzo_block->index().is_root()) {
    monitor_output_ (enzo_block);
  }

  enzo_block->compute_done();
}

//----------------------------------------------------------------------
