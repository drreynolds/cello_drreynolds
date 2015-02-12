// See LICENSE_CELLO file for license and copyright information

/// @file     enzo_EnzoMethodGravityCg.cpp
/// @author   James Bordner (jobordner@ucsd.edu)
/// @date     2014-10-21 17:25:09
/// @brief    Implements the EnzoMethodGravityCg class
//
// function [x] = conjgrad(A,b,x)
//     r=b-A*x;
//     p=r;
//     rsold=r'*r;
//  
//     for i=1:1e6
//         Ap=A*p;
//         alpha=rsold/(p'*Ap);
//         x=x+alpha*p;
//         r=r-alpha*Ap;
//         rsnew=r'*r;
//         if sqrt(rsnew)<1e-10
//               break;
//         end
//         p=r+rsnew/rsold*p;
//         rsold=rsnew;
//     end
// end
// nabla ^ 2 (potential) = 4 pi G density
//
//======================================================================
//
// nabla ^ 2 (potential) = 4 pi G density
//
// cg_begin:
//
//    B = 4 * PI * G * density
//
//    R = MATVEC (A,X) ==> cg_begin_1
//
// cg_begin_1:
//
//    R = B - R;
//    P = R
//    iter_ = 0
//    rr0_ = rr_ = DOT(R,R) ==> cg_loop_1
//
// cg_loop_1:
//
//    if (maximum iteration reached) ==> cg_end (return_error_max_iter_reached)
//    AP = MATVEC (A,P) ==> cg_loop_2
//
// cg_loop_2:
//
//    pap = DOT(P, AP) ==> cg_loop_3
//
// cg_loop_3:
//
//    alpha_ = rr_ / pap
//    X = X + alpha_ * P;
//    R = R - alpha_ * AP;
//    rr_new = DOT(R,R) ==> cg_loop_4
//
// cg_loop_4:
//
//    if (rr_new < resid_tol*resid_tol) ==> cg_end(return_converged)
//
//    P = R + rr_new / rr_ * P;
// 
//    rr_ = rr_new
//    iter = iter + 1
//    ==> cg_loop_1()
//
// cg_end (return):
//
//    if (return == return_converged) {
//       potential = X
//       ==> cg_exit()
//    } else {
//       ERROR (return-)
//    }
//
// cg_exit()
//
//    deallocate
//    NEXT()



#include "cello.hpp"

#include "enzo.hpp"

#include "enzo.decl.h"

#define CK_TEMPLATES_ONLY
#include "enzo.def.h"
#undef CK_TEMPLATES_ONLY

//----------------------------------------------------------------------

EnzoMethodGravityCg::EnzoMethodGravityCg 
(const FieldDescr * field_descr,
 int iter_max, double res_tol,
 bool is_singular) 
  : Method(), 
    is_leaf_(false),
    iter_max_(iter_max), 
    res_tol_(res_tol),
    rr0_(0),
    hx_(0),hy_(0),hz_(0),
    idensity_(0),  ipotential_(0),
    ib_(0), ix_(0), ir_(0), ip_(0), iap_(0),
    nx_(0),ny_(0),nz_(0),
    mx_(0),my_(0),mz_(0),
    gx_(0),gy_(0),gz_(0),
    iter_(0),
    alpha_(0), pap_(0), rr_(0), rr_new_(0),
    block_count_(0),
    is_singular_(is_singular)
{
  idensity_   = field_descr->field_id("density");
  ipotential_ = field_descr->field_id("potential");
  ib_  = field_descr->field_id("B");
  ix_  = field_descr->field_id("X");
  ir_  = field_descr->field_id("R");
  ip_  = field_descr->field_id("P");
  iap_ = field_descr->field_id("AP");

  static bool display_warning = true;
  if (display_warning) {
    WARNING ("EnzoMethodGravityCg::EnzoMethodGravityCg()",
	     "Assuming same ghost depth and dimensions for all fields");
    display_warning = false;
  }

  field_descr->ghosts    (idensity_,&gx_,&gy_,&gz_);

}

//----------------------------------------------------------------------

void EnzoMethodGravityCg::pup (PUP::er &p)
{

  // NOTE: change this function whenever attributes change

  TRACEPUP;

  Method::pup(p);

  p | is_singular_;
  p | is_leaf_;

  p | iter_max_;
  p | res_tol_;

  p | idensity_;
  p | ipotential_;
  p | ib_;
  p | ix_;
  p | ir_;
  p | ip_;
  p | iap_;

  p | nx_;
  p | ny_;
  p | nz_;

  p | mx_;
  p | my_;
  p | mz_;

  p | gx_;
  p | gy_;
  p | gz_;

  p | hx_;
  p | hy_;
  p | hz_;

  WARNING("EnzoMethodGravityCg::pup()", 
	  "skipping transient attributes: iter_,alpha_,rr_,rr_new, pap, etc.");

}

//----------------------------------------------------------------------

void EnzoMethodGravityCg::compute ( Block * block) throw()
{

  set_leaf(block);

  Field field = block->data()->field();

  EnzoBlock * enzo_block = static_cast<EnzoBlock*> (block);

  field.size                (&nx_,&ny_,&nz_);
  field.dimensions(idensity_,&mx_,&my_,&mz_);

  int precision = field.precision(idensity_);

  if      (precision == precision_single)    compute_<float>(enzo_block);
  else if (precision == precision_double)    compute_<double>(enzo_block);
  else if (precision == precision_quadruple) compute_<long double>(enzo_block);
  else 
    ERROR1("EnzoMethodGravityCg()", "precision %d not recognized", precision);
}

//======================================================================

extern CkReduction::reducerType r_method_gravity_cg_type;

template <class T>
void EnzoMethodGravityCg::compute_ (EnzoBlock * enzo_block) throw()
///   - X = 0
///   - B = 4 * PI * G * density
///   - R = P = B ( residual with X = 0)
///   - iter_ = 0
///   - rr_ = DOT(R,R) ==> cg_loop_0
{

  long double r_rsc[3];

  iter_ = 0;

  Field field = enzo_block->data()->field();

  T * density = (T*) field.values(idensity_);
    
  T * B = (T*) field.values(ib_);
  T * X = (T*) field.values(ix_);
  T * R = (T*) field.values(ir_);
  T * P = (T*) field.values(ip_);

  if (enzo_block->is_leaf()) {

    double xm,ym,zm;
    double xp,yp,zp;
    double hx,hy,hz;
    enzo_block->data()->lower(&xm,&ym,&zm);
    enzo_block->data()->upper(&xp,&yp,&zp);
    field.cell_width(xm,xp,&hx,
		     ym,yp,&hy,
		     zm,zp,&hz);
    ///   - X = 0
    ///   - B = -h^2 * 4 * PI * G * density
    ///   - R = P = B ( residual with X = 0);

    for (int iz=0; iz<mz_; iz++) {
      for (int iy=0; iy<my_; iy++) {
	for (int ix=0; ix<mx_; ix++) {
	  int i = ix + mx_*(iy + my_*iz);
	  X[i] = 0.0;
	  P[i] = R[i] = B[i] = 
	    - 4.0 * (cello::pi) * (cello::G_cgs) * density[i];
	}
      }
    }
  }

  r_rsc[0] = dot_(R,R);
  r_rsc[1] = sum_(B);
  r_rsc[2] = count_(B);

  CkCallback cb(CkIndex_EnzoBlock::r_cg_loop_0a<T>(NULL), 
		      enzo_block->proxy_array());

  enzo_block->contribute (3*sizeof(long double), &r_rsc, 
			  r_method_gravity_cg_type, 
			  cb);
}

//----------------------------------------------------------------------

//----------------------------------------------------------------------

template <class T>
void EnzoBlock::r_cg_loop_0a (CkReductionMsg * msg)
/// - EnzoBlock accumulate global contribution to DOT(R,R)
/// ==> refresh P for AP = MATVEC (A,P)
{
  EnzoMethodGravityCg * method = 
    static_cast<EnzoMethodGravityCg*> (this->method());

  long double rr = ((long double*)msg->getData())[0];
  long double bs = ((long double*)msg->getData())[1];
  long double bc = ((long double*)msg->getData())[2];

  method->set_rr(rr);
  method->set_bs(bs);
  method->set_bc(bc);
  delete msg;

  refresh_sync_  = "contribute";
  refresh_phase_ = phase_enzo_matvec;

  control_next(phase_refresh_enter,"neighbor");
  
}

//----------------------------------------------------------------------

template <class T>
void EnzoBlock::r_cg_loop_0b (CkReductionMsg * msg)
/// ==> refresh P for AP = MATVEC (A,P)
{
  EnzoMethodGravityCg * method = 
    static_cast<EnzoMethodGravityCg*> (this->method());
  int iter = ((int*)msg->getData())[0];
  method->set_iter(iter);
  delete msg;

  control_next(phase_refresh_enter,"neighbor");
  
}

//----------------------------------------------------------------------

// SEE mesh_Block.cpp for definition

void EnzoBlock::enzo_matvec_()
{
  EnzoMethodGravityCg * method = 
    static_cast<EnzoMethodGravityCg*> (this->method());

  Field field = data()->field();
  int precision = field.precision(field.field_id("density")); // assuming 

  EnzoBlock * enzo_block = static_cast<EnzoBlock*> (this);

  if      (precision == precision_single)    
    method->cg_loop_2<float>(enzo_block);
  else if (precision == precision_double)    
    method->cg_loop_2<double>(enzo_block);
  else if (precision == precision_quadruple) 
    method->cg_loop_2<long double>(enzo_block);
  else 
    ERROR1("EnzoMethodGravityCg()", "precision %d not recognized", precision);
}

//----------------------------------------------------------------------

template <class T>
void EnzoMethodGravityCg::cg_loop_2 (EnzoBlock * enzo_block) throw()
/// - if (maximum iteration reached) ==> cg_end (return_error_max_iter_reached)
///   pap = DOT(P, AP) ==> r_cg_loop_3
{
  set_leaf(enzo_block);

  if (iter_ == 0)  {
      if (is_singular_) {
	// shift rhs B by projection of B onto e: B~ <== B - (e*eT)/(eT*e) b
	// eT*e == n === zone count (bc)
	// eT*b == sum_i=1,n B[i]
	Field field = enzo_block->data()->field();
	T * P  = (T*) field.values(ip_);
	T * R  = (T*) field.values(ir_);
	T * B  = (T*) field.values(ib_);
	T shift = T(-bs_/bc_);
	// printf ("shift = %e\n",shift);
	shift_ (R,shift,R);
	shift_ (B,shift,B);
	shift_ (P,shift,P);
	rr_ = fabs(1 - shift) * rr_;
      }
      rr0_ = rr_;
  } else {
    rr_ = rr_new_;
  }

  if (iter_ >= iter_max_)  {

    cg_end<T>(enzo_block,return_error_max_iter_reached);

  } else {

    double pap = 0.0;

    Data * data = enzo_block->data();
    Field field = data->field();

    T * P  = (T*) field.values(ip_);
    T * AP = (T*) field.values(iap_);

    // compute cell widths for A coefficients 
    double xm,ym,zm;
    data->lower(&xm,&ym,&zm);
    double xp,yp,zp;
    data->upper(&xp,&yp,&zp);
    field.cell_width (xm,xp,&hx_,
		      ym,yp,&hy_,
		      zm,zp,&hz_);

    matvec_(AP,P); // AP = A * P (assumes P ghosts refreshed)

    pap = dot_(P,AP);

    CkCallback cb(CkIndex_EnzoBlock::r_cg_loop_3<T>(NULL), 
		  enzo_block->proxy_array());

    enzo_block->contribute (sizeof(double), &pap, 
			    CkReduction::sum_double, 
			    cb);
  }
}

//----------------------------------------------------------------------

template <class T>
void EnzoBlock::r_cg_loop_3 (CkReductionMsg * msg)
/// - EnzoBlock accumulate global contribution to DOT(R,R)
/// ==> cg_loop_4
//
/// @todo implement local pap = A * P
{
  EnzoMethodGravityCg * method = 
    static_cast<EnzoMethodGravityCg*> (this->method());

  double pap = ((double*)msg->getData())[0];
  method->set_pap(pap);
  delete msg;

  method -> cg_loop_4<T>(this);

}

//----------------------------------------------------------------------

template <class T>
void EnzoMethodGravityCg::cg_loop_4 (EnzoBlock * enzo_block) throw ()
///
/// - alpha_ = rr_ / pap_
/// - X = X + alpha_ * P;
/// - R = R - alpha_ * AP;
/// - rr_new = DOT(R,R) ==> cg_loop_4
///
{
  set_leaf(enzo_block);

  double r_rsc[3];

  Field field = enzo_block->data()->field();

  T * X  = (T*) field.values(ix_);
  T * P  = (T*) field.values(ip_);
  T * R  = (T*) field.values(ir_);
  T * AP = (T*) field.values(iap_);

  alpha_ = rr_ / pap_;
  zaxpy_ (X,   alpha_,P,X);
  zaxpy_ (R, - alpha_,AP,R);
  r_rsc[0] = dot_(R,R);
  r_rsc[1] = sum_(R);
  r_rsc[2] = count_(R);

  block_count_ = 0;

  CkCallback cb(CkIndex_EnzoBlock::r_cg_loop_5<T>(NULL), 
		      enzo_block->proxy_array());

  enzo_block->contribute (3*sizeof(double), &r_rsc, 
			  CkReduction::sum_double, 
			  cb);
}

//----------------------------------------------------------------------

template <class T>
void EnzoBlock::r_cg_loop_5 (CkReductionMsg * msg)
/// - EnzoBlock accumulate global contribution to DOT(R,R)
/// ==> cg_loop_6
{
  EnzoMethodGravityCg * method = 
    static_cast<EnzoMethodGravityCg*> (this->method());

  double rr = ((double*)msg->getData())[0];
  double bs = ((double*)msg->getData())[1];
  double bc = ((double*)msg->getData())[2];

  method->set_rr_new(rr);
  method->set_bs(bs);
  method->set_bc(bc);

  delete msg;

  method -> cg_loop_6<T>(this);

}

//----------------------------------------------------------------------

template <class T>
void EnzoMethodGravityCg::cg_loop_6 (EnzoBlock * enzo_block) throw ()
///
///    if (rr_new_ < resid_tol*resid_tol) ==> cg_end(return_converged)
///
///    P = R + rr_new_ / rr_ * P;
/// 
///    rr_ = rr_new_
///    iter = iter + 1
///    ==> cg_loop_1()
{

  set_leaf(enzo_block);

  if (rr_new_ / rr0_ < res_tol_) {

    rr_ = rr_new_;
    cg_end<T> (enzo_block,return_converged);

  } else {

    Field field = enzo_block->data()->field();

    T * P  = (T*) field.values(ip_);
    T * R  = (T*) field.values(ir_);

    T a = rr_new_ / rr_;
    zaxpy_ (P,a,P,R);

    CkCallback cb(CkIndex_EnzoBlock::r_cg_loop_0b<T>(NULL), 
		  enzo_block->proxy_array());

    int iter = (CkMyPe() == 0 && block_count_++ == 0) ? iter_ + 1 : 0;

    if (enzo_block->index().is_root()) {
      printf ("%s:%d\n",__FILE__,__LINE__);
      printf ("iter  = %d\n",iter_);
      printf ("rr = %Le\n",rr_);
      printf ("bs = %Le\n",bs_);
      printf ("bc = %Le\n",bc_);
    }

    enzo_block->contribute (sizeof(int), &iter, 
			    CkReduction::sum_int, 
			    cb);
  }
}

//----------------------------------------------------------------------


template <class T>
void EnzoMethodGravityCg::cg_end (EnzoBlock * enzo_block,int retval) throw ()
///    if (return == return_converged) {
///       potential = X
///       ==> cg_exit()
///    } else {
///       ERROR (return-)
///    }
{
  set_leaf(enzo_block);

  enzo_block->clear_refresh();

  Field field = enzo_block->data()->field();

  T * X         = (T*) field.values(ix_);
  T * potential = (T*) field.values(ipotential_);

  if (enzo_block->index().is_root()) {
    printf ("%s:%d cg_end retval %d  iter %d  rr/rr0 = %g\n",
	    __FILE__,__LINE__,retval,iter_,rr_/rr0_);
  }

  copy_(potential,X);

  enzo_block->compute_stop();
}

//----------------------------------------------------------------------

void EnzoMethodGravityCg::cg_exit_() throw()
/// deallocate temporary vectors
{
}

//======================================================================

template <class T>
void EnzoMethodGravityCg::copy_ (T * X, const T * Y) const throw()
{
  if (! is_leaf_ ) return;

  const int i0 = gx_ + mx_*(gy_ + my_*gz_);
  for (int iz=0; iz<nz_; iz++) {
    for (int iy=0; iy<ny_; iy++) {
      for (int ix=0; ix<nx_; ix++) {
	int i = i0 + ix + mx_*(iy + my_*iz);
	X[i] = Y[i];
      }
    }
  }
}

//======================================================================

template <class T>
long double EnzoMethodGravityCg::dot_ (const T * X, const T * Y) const throw()
{
  if (! is_leaf_ ) return 0.0;

  const int i0 = gx_ + mx_*(gy_ + my_*gz_);
  long double value = 0.0;
  for (int iz=0; iz<nz_; iz++) {
    for (int iy=0; iy<ny_; iy++) {
      for (int ix=0; ix<nx_; ix++) {
	int i = i0 + ix + mx_*(iy + my_*iz);
	value += X[i]*Y[i];
      }
    }
  }

  return value;
}

//----------------------------------------------------------------------

template <class T>
void EnzoMethodGravityCg::zaxpy_ (T * Z, double a, const T * X, const T * Y) const throw()
{
  if (! is_leaf_ ) return;

  const int i0 = gx_ + mx_*(gy_ + my_*gz_);
  for (int iz=0; iz<nz_; iz++) {
    for (int iy=0; iy<ny_; iy++) {
      for (int ix=0; ix<nx_; ix++) {
	int i = i0 + ix + mx_*(iy + my_*iz);
	Z[i] = a * X[i] + Y[i];
      }
    }
  }
}

//----------------------------------------------------------------------

template <class T>
long double EnzoMethodGravityCg::sum_ (const T * X) const throw()
{
  if (! is_leaf_ ) return 0.0;

  const int i0 = gx_ + mx_*(gy_ + my_*gz_);
  long double value = 0.0;
  for (int iz=0; iz<nz_; iz++) {
    for (int iy=0; iy<ny_; iy++) {
      for (int ix=0; ix<nx_; ix++) {
	int i = i0 + ix + mx_*(iy + my_*iz);
	value += X[i];
      }
    }
  }

  return value;
}

//----------------------------------------------------------------------

template <class T>
void EnzoMethodGravityCg::shift_ (T * X, const T a, const T * Y) const throw()
{
  if (! is_leaf_ ) return;

  for (int iz=0; iz<mz_; iz++) {
    for (int iy=0; iy<my_; iy++) {
      for (int ix=0; ix<mx_; ix++) {
	int i = ix + mx_*(iy + my_*iz);
	X[i] = a + Y[i];
      }
    }
  }
}

//----------------------------------------------------------------------

template <class T>
void EnzoMethodGravityCg::scale_ (T * Y, T a, const T * X) const throw()
{
  if (! is_leaf_ ) return;

  const int i0 = gx_ + mx_*(gy_ + my_*gz_);
  for (int iz=0; iz<nz_; iz++) {
    for (int iy=0; iy<ny_; iy++) {
      for (int ix=0; ix<nx_; ix++) {
	int i = i0 + ix + mx_*(iy + my_*iz);
	Y[i] = a * X[i];
      }
    }
  }
}

//----------------------------------------------------------------------

template <class T>
T EnzoMethodGravityCg::count_ (T * X) const throw()
{
  if (! is_leaf_ ) return 0.0;

  T count = 0.0;
  const int i0 = gx_ + mx_*(gy_ + my_*gz_);
  for (int iz=0; iz<nz_; iz++) {
    for (int iy=0; iy<ny_; iy++) {
      for (int ix=0; ix<nx_; ix++) {
	int i = i0 + ix + mx_*(iy + my_*iz);
	++count;
      }
    }
  }
  return count;
}

//----------------------------------------------------------------------

template <class T>
void EnzoMethodGravityCg::matvec_ (T * Y, const T * X) const throw()
/// Compute y = A*x, where A is the discrete Laplacian times (- h^2)
{
  if (! is_leaf_ ) return;

  const int idx = 1;
  const int idy = mx_;
  const int idz = mx_*my_;

  int rank = ((mz_ == 1) ? ((my_ == 1) ? 1 : 2) : 3);

  const int i0 = gx_ + mx_*(gy_ + my_*gz_);

  if (rank == 1) {
    for (int ix=0; ix<nx_; ix++) {
      int i = i0 + ix;
      Y[i] = ( X[i-idx] - 2.0*X[i] + X[i+idx] ) / (hx_*hx_);
    }
  } else if (rank == 2) {
    for (int iy=0; iy<ny_; iy++) {
      for (int ix=0; ix<nx_; ix++) {
	int i = i0 + ix + mx_*iy;
	Y[i] = ( X[i+idx] - 2.0*X[i] + X[i-idx]) / (hx_*hx_)
	  +    ( X[i+idy] - 2.0*X[i] + X[i-idy]) / (hy_*hy_);
      }
    }
  } else if (rank == 3) {
    for (int iz=0; iz<nz_; iz++) {
      for (int iy=0; iy<ny_; iy++) {
	for (int ix=0; ix<nx_; ix++) {
	  int i = i0 + ix + mx_*(iy + my_*iz);
	Y[i] = ( X[i+idx] - 2.0*X[i] + X[i-idx]) / (hx_*hx_)
	  +    ( X[i+idy] - 2.0*X[i] + X[i-idy]) / (hy_*hy_)
	  +    ( X[i+idz] - 2.0*X[i] + X[i-idz]) / (hz_*hz_);
	}
      }
    }
  }
 
}
