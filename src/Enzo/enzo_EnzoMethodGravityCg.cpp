// See LICENSE_CELLO file for license and copyright information

/// @file     enzo_EnzoMethodGravityCg.cpp
/// @author   James Bordner (jobordner@ucsd.edu)
/// @date     2014-10-21 17:25:09
/// @brief    Implements the EnzoMethodGravityCg class
///
/// function [x] = conjgrad(A,b,x)
///     r=b-A*x;
///     p=r;
///     rsold=r'*r;
///  
///     for i=1:1e6
///         Ap=A*p;
///         alpha=rsold/(p'*Ap);
///         x=x+alpha*p;
///         r=r-alpha*Ap;
///         rsnew=r'*r;
///         if sqrt(rsnew)<1e-10
///               break;
///         end
///         p=r+rsnew/rsold*p;
///         rsold=rsnew;
///     end
/// end
/// nabla ^ 2 (potential) = 4 pi G density
///
///======================================================================
///
/// nabla ^ 2 (potential) = 4 pi G density
///
/// cg_begin:
///
///    B = 4 * PI * G * density
///
///    R = MATVEC (A,X) ==> cg_begin_1
///
/// cg_begin_1:
///
///    R = B - R;
///    P = R
///    iter_ = 0
///    rr_ = DOT(R,R) ==> cg_loop_1
///
/// cg_loop_1:
///
///    if (maximum iteration reached) ==> cg_end (return_error_max_iter_reached)
///    AP = MATVEC (A,P) ==> cg_loop_2
///
/// cg_loop_2:
///
///    pap = DOT(P, AP) ==> cg_loop_3
///
/// cg_loop_3:
///
///    alpha_ = rr_ / pap
///    X = X + alpha_ * P;
///    R = R - alpha_ * AP;
///    rr_new_ = DOT(R,R) ==> cg_loop_4
///
/// cg_loop_4:
///
///    if (rr_new_ < resid_tol*resid_tol) ==> cg_end(return_converged)
///
///    P = R + rr_new_ / rr_ * P;
/// 
///    rr_ = rr_new_
///    iter = iter + 1
///    ==> cg_loop_1()
///
/// cg_end (return):
///
///    if (return == return_converged) {
///       potential = X
///       ==> cg_exit()
///    } else {
///       ERROR (return-)
///    }
///
/// cg_exit()
///
///    deallocate
///    NEXT()


#include "cello.hpp"

#include "enzo.hpp"

#include "enzo.decl.h"

#define CK_TEMPLATES_ONLY
#include "enzo.def.h"
#undef CK_TEMPLATES_ONLY

//----------------------------------------------------------------------

EnzoMethodGravityCg::EnzoMethodGravityCg 
(const FieldDescr * field_descr,
 int iter_max, double res_tol) 
  : Method(), 
    iter_max_(iter_max), 
    res_tol_(res_tol),
    precision_(precision_unknown),
    /// Input / output vectors
    idensity_(0),
    ipotential_(0),
    /// CG temporary vectors
    ib_(0),
    ix_(0),
    ir_(0),
    ip_(0),
    iap_(0),
    /// vector attributes
    nx_(0),ny_(0),nz_(0),
    mx_(0),my_(0),mz_(0),
    gx_(0),gy_(0),gz_(0),
    /// CG scalars
    iter_(0),
    alpha_(0),
    rr_(0),
    rr_new_(0)
{
  idensity_   = field_descr->field_id("density");
  ipotential_ = field_descr->field_id("potential");
  ib_  = field_descr->field_id("B");
  ix_  = field_descr->field_id("X");
  ir_  = field_descr->field_id("R");
  ip_  = field_descr->field_id("P");
  iap_ = field_descr->field_id("AP");

  WARNING ("EnzoMethodGravityCg::EnzoMethodGravityCg()",
	   "Assuming same ghost depth and precision for all fields");

  field_descr->ghosts    (idensity_,&gx_,&gy_,&gz_);
  precision_ = field_descr->precision(idensity_);

}

//----------------------------------------------------------------------

void EnzoMethodGravityCg::pup (PUP::er &p)
{

  // NOTE: change this function whenever attributes change

  TRACEPUP;

  Method::pup(p);

  p | iter_max_;
  p | res_tol_;
  p | precision_;

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

  WARNING("EnzoMethodGravityCg::pup()", 
	  "skipping transient attributes: iter_,alpha_,rr_,rr_new_");

}

//----------------------------------------------------------------------

void EnzoMethodGravityCg::compute ( CommBlock * comm_block) throw()
{

  EnzoBlock * enzo_block = static_cast<EnzoBlock*> (comm_block);
  Field field = enzo_block->block()->field();

  WARNING ("EnzoMethodGravityCg::EnzoMethodGravityCg()",
	   "Assuming same ghost depth and dimensions for all fields");
  field.size                (&nx_,&ny_,&nz_);
  field.dimensions(idensity_,&mx_,&my_,&mz_);

  if      (precision_ == precision_single)    compute_<float>(enzo_block);
  else if (precision_ == precision_double)    compute_<double>(enzo_block);
  else if (precision_ == precision_quadruple) compute_<long double>(enzo_block);
  else 
    ERROR1("EnzoMethodGravityCg()", "precision %d not recognized", precision_);
}

//======================================================================

template <class T>
void EnzoMethodGravityCg::compute_ (EnzoBlock * enzo_block) throw()
///   - X = 0
///   - B = 4 * PI * G * density
///   - R = P = B ( residual with X = 0)
///   - iter_ = 0
///   - rr_ = DOT(R,R) ==> cg_loop_1
{

  Field field = enzo_block->block()->field();

  T * density = (T*) field.values(idensity_);

  T * B = (T*) field.values(ib_);
  T * X = (T*) field.values(ix_);
  T * R = (T*) field.values(ir_);
  T * P = (T*) field.values(ip_);

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

  ///   - iter_ = 0

  iter_ = 0;

  double rr = dot_(R,R);
  
  CkCallback cb(CkIndex_EnzoBlock::r_cg_loop_1<T>(NULL), 
		      enzo_block->proxy_array());

  enzo_block->contribute (sizeof(double), &rr, 
			  CkReduction::sum_double, 
			  cb);
}

//----------------------------------------------------------------------

template <class T>
void EnzoBlock::r_cg_loop_1 (CkReductionMsg * msg)
/// - EnzoBlock accumulate global contribution to DOT(R,R)
/// ==> cg_loop_1
{
  double rr = ((double*)msg->getData())[0];
  delete msg;
  EnzoMethodGravityCg * method = 
    static_cast<EnzoMethodGravityCg*> (this->method());

  method->cg_loop_1<T>(this,rr);
}

//----------------------------------------------------------------------

template <class T>
void EnzoMethodGravityCg::cg_loop_1 (EnzoBlock * enzo_block,double rr) throw()
/// - if (maximum iteration reached) ==> cg_end (return_error_max_iter_reached)
/// - AP = MATVEC (A,P) ==> cg_loop_2
{
  rr_ = rr;

  if (iter_ > iter_max_) cg_end(return_error_max_iter_reached);

  Field field = enzo_block->block()->field();

  T * P  = (T*) field.values(ip_);
  T * AP = (T*) field.values(iap_);

  matvec_(AP,P);

  CkCallback cb(CkIndex_EnzoBlock::r_cg_loop_2<T>(NULL), 
		      enzo_block->proxy_array());

  enzo_block->contribute (cb);

  // ==> cg_loop_2()

}

//----------------------------------------------------------------------

template <class T>
void EnzoBlock::r_cg_loop_2 (CkReductionMsg * msg)
/// - EnzoBlock accumulate global contribution to DOT(R,R)
/// ==> cg_loop_1
{
  delete msg;
  EnzoMethodGravityCg * method = 
    static_cast<EnzoMethodGravityCg*> (this->method());

  method -> cg_loop_2<T>(this);
}

//----------------------------------------------------------------------

template <class T>
void EnzoMethodGravityCg::cg_loop_2(EnzoBlock * enzo_block) throw ()
///
///    pap = DOT(P, AP) ==> cg_loop_3
///
{

  Field field = enzo_block->block()->field();

  T * AP = (T*) field.values(iap_);
  T * P  = (T*) field.values(ip_);

  double pap = dot_(P,AP);

  CkCallback cb(CkIndex_EnzoBlock::r_cg_loop_3<T>(NULL), 
		      enzo_block->proxy_array());

  enzo_block->contribute (sizeof(double), &pap, 
			  CkReduction::sum_double, 
			  cb);

}

//----------------------------------------------------------------------

template <class T>
void EnzoBlock::r_cg_loop_3 (CkReductionMsg * msg)
/// - EnzoBlock accumulate global contribution to DOT(R,R)
/// ==> cg_loop_1
//
/// @todo implement local pap = A * P
{
  double pap = ((double*)msg->getData())[0];
  delete msg;
  EnzoMethodGravityCg * method = 
    static_cast<EnzoMethodGravityCg*> (this->method());

  method -> cg_loop_3<T>(this,pap);

}

//----------------------------------------------------------------------

template <class T>
void EnzoMethodGravityCg::cg_loop_3 (EnzoBlock * enzo_block,double pap) throw ()
///
/// - alpha_ = rr_ / pap_
/// - X = X + alpha_ * P;
/// - R = R - alpha_ * AP;
/// - rr_new_ = DOT(R,R) ==> cg_loop_4
///
{

  Field field = enzo_block->block()->field();

  T * X  = (T*) field.values(ix_);
  T * P  = (T*) field.values(ip_);
  T * R  = (T*) field.values(ir_);
  T * AP = (T*) field.values(iap_);

  alpha_ = rr_ / pap;
  zaxpy_ (X,alpha_,P,X);
  zaxpy_ (R,alpha_,AP,R);

  double rr_new = dot_(R,R);

  CkCallback cb(CkIndex_EnzoBlock::r_cg_loop_4<T>(NULL), 
		      enzo_block->proxy_array());

  enzo_block->contribute (sizeof(double), &rr_new, 
			  CkReduction::sum_double, 
			  cb);


}

//----------------------------------------------------------------------

template <class T>
void EnzoBlock::r_cg_loop_4 (CkReductionMsg * msg)
/// - EnzoBlock accumulate global contribution to DOT(R,R)
/// ==> cg_loop_1
{
  double rr_new = ((double*)msg->getData())[0];
  printf ("%s:%d TRACE r_cg_loop_4 rr_new = %g\n",__FILE__,__LINE__,rr_new);
  delete msg;
  EnzoMethodGravityCg * method = 
    static_cast<EnzoMethodGravityCg*> (this->method());

  method -> cg_loop_4<T>(this,rr_new);

}

//----------------------------------------------------------------------

template <class T>
void EnzoMethodGravityCg::cg_loop_4 (EnzoBlock * enzo_block,double rr_new) throw ()
///
///    if (rr_new_ < resid_tol*resid_tol) ==> cg_end(return_converged)
///
///    P = R + rr_new_ / rr_ * P;
/// 
///    rr_ = rr_new_
///    iter = iter + 1
///    ==> cg_loop_1()
{
  Field field = enzo_block->block()->field();

  T * P  = (T*) field.values(ip_);
  T * R  = (T*) field.values(ir_);

  T a = rr_new / rr_;
  zaxpy_ (P,a,P,R);

  ++ iter_;

  cg_loop_1<T>(enzo_block,rr_new);
}

//----------------------------------------------------------------------

void EnzoMethodGravityCg::cg_end (int retval) throw ()
///    if (return == return_converged) {
///       potential = X
///       ==> cg_exit()
///    } else {
///       ERROR (return-)
///    }
{
}

//----------------------------------------------------------------------

void EnzoMethodGravityCg::cg_exit_() throw()
/// deallocate temporary vectors
{

}

//======================================================================

template <class T>
T EnzoMethodGravityCg::dot_ (const T * X, const T * Y) const throw()
{
  const int i0 = gx_ + mx_*(gy_ + my_*gz_);
  long double rr = 0.0;
  for (int iz=0; iz<nz_; iz++) {
    for (int iy=0; iy<ny_; iy++) {
      for (int ix=0; ix<nx_; ix++) {
	int i = i0 + ix + mx_*(iy + my_*iz);
	rr += X[i]*Y[i];
      }
    }
  }
  return rr;
}

//----------------------------------------------------------------------

template <class T>
void EnzoMethodGravityCg::zaxpy_ (T * Z, double a, const T * X, const T * Y) const throw()
{
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
void EnzoMethodGravityCg::matvec_ (T * Y, const T * X) const throw()
/// Compute y = A*x, where A is the discrete Laplacian times (- h^2)
{
  const int idx = 1;
  const int idy = mx_;
  const int idz = mx_*my_;

  int rank = ((mz_ == 1) ? ((my_ == 1) ? 1 : 2) : 3);

  const int i0 = gx_ + mx_*(gy_ + my_*gz_);

  if (rank == 1) {
    for (int ix=0; ix<nx_; ix++) {
      int i = i0 + ix;
      Y[i] = 2.0*X[i] - 
	( X[i+idx] + X[i-idx] );
    }
  } else if (rank == 2) {
    for (int iy=0; iy<ny_; iy++) {
      for (int ix=0; ix<nx_; ix++) {
	int i = i0 + ix + mx_*iy;
	Y[i] = 4.0*X[i] - 
	  ( X[i+idx] + X[i-idx] +
	    X[i+idy] + X[i-idy] );
      }
    }
  } else if (rank == 3) {
    for (int iz=0; iz<nz_; iz++) {
      for (int iy=0; iy<ny_; iy++) {
	for (int ix=0; ix<nx_; ix++) {
	  int i = i0 + ix + mx_*(iy + my_*iz);
	  Y[i] = 6.0*X[i] - 
	    ( X[i+idx] + X[i-idx] +
	      X[i+idy] + X[i-idy] +
	      X[i+idz] + X[i-idz] );
	}
      }
    }
  }
}
