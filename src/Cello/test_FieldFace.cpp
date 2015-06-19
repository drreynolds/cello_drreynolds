// See LICENSE_CELLO file for license and copyright information

/// @file     test_FieldFace.cpp
/// @author   James Bordner (jobordner@ucsd.edu)
/// @date     2008-02-20
/// @brief    Unit tests for the FieldFace class

#include "main.hpp" 
#include "test.hpp"

#include "data.hpp"

//----------------------------------------------------------------------

bool is_ghost(int ix,int iy,int iz,
	      int gx,int gy,int gz,
	      int mdx,int mdy,int mdz)
{
  return ( ! ((gx <= ix) && (ix < (mdx - gx)) && 
	      (gy <= iy) && (iy < (mdy - gy)) && 
	      (gz <= iz) && (iz < (mdz - gz))) );
}

//----------------------------------------------------------------------

double test_value (int ix, int iy, int iz,
		   int gx,int gy,int gz,
		   int mdx, int mdy, int mdz,
		   int ibx, int iby, int ibz,
		   int nbx, int nby, int nbz,
		   int index_field, 
		   int MD3, int ND3)
{
  int mx = (mdx-2*gx);
  int my = (mdy-2*gy);
  int mz = (mdz-2*gz);
  int NX = nbx*mx;
  int NY = nby*my;
  int NZ = nbz*mz;
  double xg = (ibx*mx + ix + NX) % NX;
  double yg = (iby*my + iy + NY) % NY;
  double zg = (ibz*mz + iz + NZ) % NZ;
  double value = xg + 13*(yg + 19*zg);
  return value;
}

//----------------------------------------------------------------------

double init_value (int ix, int iy, int iz,
		   int gx,int gy,int gz,
		   int mdx, int mdy, int mdz,
		   int ibx, int iby, int ibz,
		   int nbx, int nby, int nbz,
		   int index_field, 
		   int MD3, int ND3)
{
  double value = test_value (ix,iy,iz,gx,gy,gz,mdx,mdy,mdz,ibx,iby,ibz,nbx,nby,nbz,index_field,MD3,ND3);
  return is_ghost(ix,iy,iz,gx,gy,gz,mdx,mdy,mdz) ? -value : value;
}

//----------------------------------------------------------------------

template<class T>
void init_field(T * values,
		int ibx, int iby, int ibz,
		int nbx, int nby, int nbz,
		int index_field, 
		int mx, int my, int mz,
		int gx, int gy, int gz,
		int ND3)
{

  const int mdx = mx + 2*gx;
  const int mdy = my + 2*gy;
  const int mdz = mz + 2*gz;
  const int MD3 = mdx*mdy*mdz;

  for (int iz = 0; iz < mdz; iz++) {
    for (int iy = 0; iy < mdy; iy++) {
      for (int ix = 0; ix < mdx; ix++) {
	int i = ix + mdx * (iy + mdy * iz);
	values[i] = init_value(ix,iy,iz,
				     gx,gy,gz,
				     mdx,mdy,mdz,
				     ibx,iby,ibz,
				     nbx,nby,nbz,
				     index_field,
				     MD3,ND3);
      }
    }
  }
}

//----------------------------------------------------------------------

template<class T>
bool test_field(T * values,
		int ibx, int iby, int ibz,
		int nbx, int nby, int nbz,
		int index_field, 
		int mx, int my, int mz,
		int gx, int gy, int gz,
		int ND3)
{

  const int mdx = mx + 2*gx;
  const int mdy = my + 2*gy;
  const int mdz = mz + 2*gz;
  const int MD3 = mdx*mdy*mdz;

  bool result = true;
  for (int iz = 0; iz < mdz; iz++) {
    for (int iy = 0; iy < mdy; iy++) {
      for (int ix = 0; ix < mdx; ix++) {
	int i = ix + mdx * (iy + mdy * iz);
	double value = test_value(ix,iy,iz,
				  gx,gy,gz,
				  mdx,mdy,mdz,
				  ibx,iby,ibz,
				  nbx,nby,nbz,
				  index_field,
				  MD3,ND3);
	//	printf ("%p test\n",values+i);
	if (values[i] != value) result = false;
      }
    }
  }
  if (! result) {
    PARALLEL_PRINTF ("mismatch block   (%d %d %d)\n", ibx,iby,ibz);
    PARALLEL_PRINTF ("mismatch field    %d\n",        index_field);
    PARALLEL_PRINTF ("mismatch size    (%d %d %d)\n", mdx,mdy,mdz);
    for (int iz = 0; iz < mdz; iz++) {
      for (int iy = 0; iy < mdy; iy++) {
	for (int ix = 0; ix < mdx; ix++) {
	  int i = ix + mdx * (iy + mdy * iz);
	  double value = test_value(ix,iy,iz,
				    gx,gy,gz,
				    mdx,mdy,mdz,
				    ibx,iby,ibz,
				    nbx,nby,nbz,
				    index_field,
				    MD3,ND3);
	  printf ("%d %d %d  %07g %07g\n",ix,iy,iz,double(values[i]),value);
	}
      }
    }
  }

  return result;
}

//----------------------------------------------------------------------

void init_fields
(
 FieldDescr * field_descr,
 FieldData * field_data[],
 int nbx,int nby, int nbz,
 int mx, int my, int mz)
{
  //--------------------------------------------------
  // Initialize the field blocks in 4 x 4 x 4 array
  //--------------------------------------------------

  const int ND3 = nbx*nby*nbz;

  for (int ibz = 0; ibz < nbz; ibz++) {
    for (int iby = 0; iby < nby; iby++) {
      for (int ibx = 0; ibx < nbx; ibx++) {

	int index_data = ibx + nbx * (iby + nby * ibz);

	// Create the FaceData object

	field_data[index_data] = new FieldData (field_descr, mx, my, mz);

	FieldData * data = field_data[index_data];

	// Allocate field data including ghosts

	data->allocate_permanent(true);

	// Initialize fields

	int gx, gy, gz;

	// field 0
	field_descr->ghost_depth(0, &gx, &gy, &gz);
	float * v1 = (float *) (data->values(0));
	init_field(v1,ibx,iby,ibz,nbx,nby,nbz,0,mx,my,mz,gx,gy,gz,ND3);

	// field 1
	field_descr->ghost_depth(1, &gx, &gy, &gz);
	double * v2 = (double *) (data->values(1));
	init_field(v2,ibx,iby,ibz,nbx,nby,nbz,1,mx,my,mz,gx,gy,gz,ND3);

	// field 2
	field_descr->ghost_depth(2, &gx, &gy, &gz);
	long double * v3 = (long double *) (data->values(2));
	init_field(v3,ibx,iby,ibz,nbx,nby,nbz,2,mx,my,mz,gx,gy,gz,ND3);
 
      }
    }
  }
}

//----------------------------------------------------------------------

bool test_fields
(
 FieldDescr * field_descr,
 FieldData * field_data[],
 int nbx,int nby, int nbz,
 int mx, int my, int mz)
{
  const int ND3 = nbx*nby*nbz;

  bool result = true;

  for (int ibz = 0; ibz < nbz; ibz++) {
    for (int iby = 0; iby < nby; iby++) {
      for (int ibx = 0; ibx < nbx; ibx++) {

	int index_data = ibx + nbx * (iby + nby * ibz);

	FieldData * data = field_data[index_data];

	int gx, gy, gz;

	bool test_result;

	// field 0
	field_descr->ghost_depth(0, &gx, &gy, &gz);
	float * v1 = (float *) (data->values(0));
	test_result = test_field(v1,ibx,iby,ibz,nbx,nby,nbz,0,mx,my,mz,gx,gy,gz,ND3);
	unit_assert(test_result);  // @@@@
	result = result && test_result;
	printf ("gx,gy,gz = %d %d %d\n",gx,gy,gz);

	// field 1
	field_descr->ghost_depth(1, &gx, &gy, &gz);
	double * v2 = (double *) (data->values(1));
	test_result = test_field(v2,ibx,iby,ibz,nbx,nby,nbz,1,mx,my,mz,gx,gy,gz,ND3);
	unit_assert(test_result);  // @@@@
	result = result && test_result;

	// field 2
	field_descr->ghost_depth(2, &gx, &gy, &gz);
	long double * v3 = (long double *) (data->values(2));
	test_result = test_field(v3,ibx,iby,ibz,nbx,nby,nbz,2,mx,my,mz,gx,gy,gz,ND3);
	unit_assert(test_result);  // @@@@
	result = result && test_result;
 
      }
    }
  }
  return result;
}

//======================================================================
PARALLEL_MAIN_BEGIN
{

  PARALLEL_INIT;

  unit_init(0,1);

  unit_class("FieldFace");

  //--------------------------------------------------
  // Initialize the global field descriptor object field_descr
  //--------------------------------------------------

  FieldDescr * field_descr = new FieldDescr;

  // insert fields

  field_descr->insert_permanent("field_1");
  field_descr->insert_permanent("field_2");
  field_descr->insert_permanent("field_3");

  // initialize field precisions

  field_descr->set_precision(0, precision_single);
  field_descr->set_precision(1, precision_double);
  field_descr->set_precision(2, precision_quadruple);

  // initialize field ghost zone depths

  field_descr->set_ghost_depth(0, 1,1,1);
  field_descr->set_ghost_depth(1, 1,2,3);
  field_descr->set_ghost_depth(2, 3,2,1);


  int nbx=2, nby=3, nbz=4;
  FieldData * field_data[nbx*nby*nbz];

  int mx=5, my=6, mz=7;

  init_fields(field_descr,field_data,nbx,nby,nbz,mx,my,mz);

  //----------------------------------------------------------------------
  // Refresh ghosts
  //----------------------------------------------------------------------

  for (int ia = 0; ia<3; ++ia) {

    for (int ibz = 0; ibz < nbz; ibz++) {
      for (int iby = 0; iby < nby; iby++) {
	for (int ibx = 0; ibx < nbx; ibx++) {
	  axis_enum axis = (axis_enum)(ia);
	  
	  int index_lower = ibx + nbx * (iby + nby * ibz);
	  FieldData * data_lower = field_data[index_lower];

	  int index_upper = 0;
	  if (axis==0) index_upper = ((ibx+1)%nbx) + nbx * (  iby        + nby * ibz);
	  if (axis==1) index_upper =   ibx        + nbx * (((iby+1)%nby) + nby * ibz);
	  if (axis==2) index_upper =   ibx        + nbx * (  iby        + nby * ((ibz+1)%nbz));

	  FieldData * data_upper = field_data[index_upper];

	  int ixm=0,iym=0,izm=0;
	  int ixp=0,iyp=0,izp=0;

	  if (axis==axis_x) {ixm=-1; ixp=+1; }
	  if (axis==axis_y) {iym=-1; iyp=+1; }
	  if (axis==axis_z) {izm=-1; izp=+1; }

	  FieldFace face_lower (data_lower);
	  FieldFace face_upper (data_upper);
	  face_lower.set_ghost(true,true,true);
	  face_upper.set_ghost(true,true,true);

	  int n;
	  char * array;


	  face_lower.set_face(ixp,iyp,izp);
	  face_upper.set_face(ixm,iym,izm);

	  face_lower.load (&n, &array);
	  face_upper.store (n,array);

	  face_upper.load (&n, &array);
	  face_lower.store (n,array);

	  // NEW
	  //
	  //  block    face   block
	  //   lower   lower   upper
	  //  +---++    ++    ++    ++---+
	  //  |   ||    ||    ||    ||   |
	  //  +---++    ++    ++    ++---+
	  // 1    xx >> xx
	  // 2          xx    >>    xx
	  // 3                yy << yy
	  // 4    yy    <<    yy
	  //
	  //  array_lower = face_lower.load(ixp,iyp,izp)
	  //  face_upper.store (ixm,iym,izm,array_lower);


	  // OLD
	  //
	  //  block    face   block
	  //   lower           upper
	  //  +---++    ++    ++---+
	  //  |   ||    ||    ||   |
	  //  +---++    ++    ++---+
	  // 1    xx >> xx
	  // 2          xx >> xx 
	  // 3          yy << yy
	  // 4    yy << yy

	  // face.allocate   (field_descr,data_lower,ixm,iym,izm);

	  // face.load       (field_descr, data_lower, ixp,iyp,izp);
	  // face.store      (field_descr, data_upper, ixm,iym,izm);

	  // face.load       (field_descr, data_upper, ixm,iym,izm);
	  // face.store      (field_descr, data_lower, ixp,iyp,izp);

	  // face.deallocate ();

	}
      }
    }
  }

  unit_func("load/copy/store");
  unit_assert(test_fields(field_descr,field_data,nbx,nby,nbz,mx,my,mz)); // @@@

  //----------------------------------------------------------------------	
  // clean up
  //----------------------------------------------------------------------	

  for (int ibz = 0; ibz < nbz; ibz++) {
    for (int iby = 0; iby < nby; iby++) {
      for (int ibx = 1; ibx < nbx; ibx++) {
	int index_data = ibx + nbx * (iby + nby * ibz);
	delete field_data[index_data];
	field_data[index_data] = 0;
      }
    }
  }

  delete field_descr;

  unit_finalize();
  //----------------------------------------------------------------------

  exit_();
}

PARALLEL_MAIN_END

