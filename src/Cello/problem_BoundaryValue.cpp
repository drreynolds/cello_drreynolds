// See LICENSE_CELLO file for license and copyright information

/// @file     problem_BoundaryValue.cpp
/// @author   James Bordner (jobordner@ucsd.edu)
/// @date     2014-04-02
/// @brief    Implementation of the default BoundaryValue boundary value class

#include "problem.hpp"

//----------------------------------------------------------------------

void BoundaryValue::enforce 
(const FieldDescr * field_descr,
 CommBlock * comm_block, face_enum face, axis_enum axis) const throw()
{
  if ( ! applies_(axis,face)) return;

  if (face == face_all) {
    enforce(field_descr,comm_block,face_lower,axis);
    enforce(field_descr,comm_block,face_upper,axis);
  } else if (axis == axis_all) {
    enforce(field_descr,comm_block,face,axis_x);
    enforce(field_descr,comm_block,face,axis_y);
    enforce(field_descr,comm_block,face,axis_z);
  } else {
    Block * block = comm_block->block();

    FieldBlock * field_block = block->field_block();
    if ( ! field_block->ghosts_allocated() ) {
      ERROR("EnzoBoundary::enforce",
	    "Function called with ghosts not allocated");
    }

    int nx,ny,nz;
    field_block->size(&nx,&ny,&nz);
    

    double * x = new double [nx];
    double * y = new double [ny];
    double * z = new double [nz];

    block->field_cells(x,y,z);

    double xm,ym,zm;
    double xp,yp,zp;
    block -> lower(&xm,&ym,&zm);
    block -> upper(&xp,&yp,&zp);

    double t = comm_block->time();

    for (int index = 0; index < field_list_.size(); index++) {

      int index_field = field_descr->field_id(field_list_[index]);
      int gx,gy,gz;
      field_descr->ghosts(index_field,&gx,&gy,&gz);
      int ndx=nx+2*gx;
      int ndy=ny+2*gy;
      int ndz=nz+2*gz;
      void * array = field_block->field_values(index_field);
      precision_type precision = field_descr->precision(index_field);
      int ix0=0 ,iy0=0,iz0=0;
      nx=ndx, ny=ndy, nz=ndz;

      if (axis == axis_x) nx=gx;
      if (axis == axis_y) ny=gy;
      if (axis == axis_z) nz=gz;
      if (face == face_upper) {
	if (axis == axis_x) ix0 = ndx - gx;
	if (axis == axis_y) iy0 = ndy - gy;
	if (axis == axis_z) iz0 = ndz - gz;
      }

      double * v = new double [nx*ny*nz];

      value_->evaluate(v, t,
		       nx,nx,x+ix0,
		       ny,ny,y+iy0,
		       nz,nz,z+iz0);

      switch (precision) {
      case precision_single:
	copy_((float *)array,v,ndx,ndy,ndz,nx,ny,nz,ix0,iy0,iz0);
	break;
      case precision_double:
	copy_((double *)array,v,ndx,ndy,ndz,nx,ny,nz,ix0,iy0,iz0);
	break;
      case precision_extended80:
      case precision_extended96:
      case precision_quadruple:
	copy_((long double *)array,v,ndx,ndy,ndz,nx,ny,nz,ix0,iy0,iz0);
	break;
      }

      delete [] v;
    }
    delete [] x;
    delete [] y;
    delete [] z;
  }
}

//----------------------------------------------------------------------

template <class T>
void BoundaryValue::copy_(T * field, double * value,
			  int ndx, int ndy, int ndz,
			  int nx,  int ny,  int nz,
			  int ix0, int iy0, int iz0) const throw()
{
  for (int ix=ix0; ix<ix0+nx; ix++) {
    for (int iy=iy0; iy<iy0+ny; iy++) {
      for (int iz=iz0; iz<iz0+nz; iz++) {
	int iv = (ix-ix0) + nx*((iy-iy0) + ny*(iz-iz0));
	int ib = ix + ndx*(iy + ndy*(iz));
	field[ib] = (T) value[iv];
      }
    }
  }
}

//----------------------------------------------------------------------
