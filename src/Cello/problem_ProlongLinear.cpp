// See LICENSE_CELLO file for license and copyright information

/// @file     field_ProlongLinear.cpp
/// @author   James Bordner (jobordner@ucsd.edu)
/// @date     2013-05-09
/// @brief    Implentation of default linear prolongation

#include "problem.hpp"

//----------------------------------------------------------------------

ProlongLinear::ProlongLinear() throw()
  : Prolong ()
{
  TRACE("ProlongLinear::ProlongLinear");
}

//----------------------------------------------------------------------

void ProlongLinear::apply 
( precision_type precision,
  void *       values_f, int nd3_f[3], int n3_f[3],
  const void * values_c, int nd3_c[3], int n3_c[3])
{
  switch (precision)  {

  case precision_single:

    apply_( (float *)       values_f, nd3_f, n3_f,
	    (const float *) values_c, nd3_c, n3_c);

    break;

  case precision_double:

    apply_( (double *)       values_f, nd3_f, n3_f,
	    (const double *) values_c, nd3_c, n3_c);

    break;

  default:

    ERROR1 ("ProlongLinear::apply()",
	    "Unknown precision %d",
	    precision);
      }
}

//----------------------------------------------------------------------

template <class T>
void ProlongLinear::apply_
( T *       values_f, int nd3_f[3], int n3_f[3],
  const T * values_c, int nd3_c[3], int n3_c[3])
{
  int dx_c = 1;
  int dy_c = nd3_c[0];
  int dz_c = nd3_c[1];

  const double c1[4] = { 5.0*0.25, 3.0*0.25, 1.0*0.25, -1.0*0.25};
  const double c2[4] = {-1.0*0.25, 1.0*0.25, 3.0*0.25,  5.0*0.25};

	  
  if (n3_f[1]==1) {

    ASSERT ("ProlongLinear::apply_",
	    "fine array must be twice the size of the coarse array",
	    n3_f[0]==n3_c[0]*2);

    ASSERT ("ProlongLinear::apply_",
	    "fine grid array sizes must be divisible by 4",
	    n3_f[0] % 4 == 0);

    for (int ix0=0; ix0<n3_f[0]; ix0+=4) {
      int i_c = ix0/2;
      for (int ix=ix0; ix<ix0+4; ix++) {
	int icx = ix-ix0;
	int i_f = ix;
	values_f[i_f] = ( c1[icx]*values_c[i_c] + 
			  c2[icx]*values_c[i_c+dx_c]);
      }
    }

  } else if (n3_f[2] == 1) {

    ASSERT ("ProlongLinear::apply_",
	    "fine array must be twice the size of the coarse array",
	    n3_f[1]==n3_c[1]*2);

    ASSERT ("ProlongLinear::apply_",
	    "fine grid array sizes must be divisible by 4",
	    n3_f[1] % 4 == 0);

    for (int ix0=0; ix0<n3_f[0]; ix0+=4) {
      for (int iy0=0; iy0<n3_f[1]; iy0+=4) {

	int i_c = ix0/2 + nd3_c[1]*(iy0/2);

	for (int ix=ix0; ix<ix0+4; ix++) {
	  int icx = ix-ix0;
	  for (int iy=iy0; iy<iy0+4; iy++) {
	    int icy = iy-iy0;

	    int i_f = ix + nd3_f[0]*iy;

	    values_f[i_f] = 
	      ( c1[icx]*c1[icy]*values_c[i_c] +
		c2[icx]*c1[icy]*values_c[i_c+dx_c] +
		c1[icx]*c2[icy]*values_c[i_c     +dy_c] +
		c2[icx]*c2[icy]*values_c[i_c+dx_c+dy_c]);
	  }
	}
      }
    }
  } else {

    ASSERT ("ProlongLinear::apply_",
	    "fine array must be twice the size of the coarse array",
	    n3_f[2]==n3_c[2]*2);

    ASSERT ("ProlongLinear::apply_",
	    "fine grid array sizes must be divisible by 4",
	    n3_f[2] % 4 == 0);

    for (int ix0=0; ix0<n3_f[0]; ix0+=4) {
      for (int iy0=0; iy0<n3_f[1]; iy0+=4) {
	for (int iz0=0; iz0<n3_f[2]; iz0+=4) {

	  int i_c = ix0/2 + nd3_c[0]*(iy0/2 + nd3_c[1]*iz0/2);

	  for (int ix=ix0; ix<ix0+4; ix++) {
	    int icx = ix-ix0;
	    for (int iy=iy0; iy<iy0+4; iy++) {
	      int icy = iy-iy0;
	      for (int iz=iz0; iz<iz0+4; iz++) {
		int icz = iz-iz0;

		int i_f = ix + nd3_f[0]*(iy + nd3_f[1]*iz);

		values_f[i_f] = 
		  ( c1[icx]*c1[icy]*c1[icz]*values_c[i_c] +
		    c2[icx]*c1[icy]*c1[icz]*values_c[i_c+dx_c] +
		    c1[icx]*c2[icy]*c1[icz]*values_c[i_c     +dy_c] +
		    c2[icx]*c2[icy]*c1[icz]*values_c[i_c+dx_c+dy_c] +
		    c1[icx]*c1[icy]*c2[icz]*values_c[i_c          +dz_c] +
		    c2[icx]*c1[icy]*c2[icz]*values_c[i_c+dx_c     +dz_c] +
		    c1[icx]*c2[icy]*c2[icz]*values_c[i_c     +dy_c+dz_c] +
		    c2[icx]*c2[icy]*c2[icz]*values_c[i_c+dx_c+dy_c+dz_c]);
	      }
	    }
	  }
	}
      }
    }
  }
}

//======================================================================

