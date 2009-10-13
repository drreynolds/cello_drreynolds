//345678901234567890123456789012345678901234567890123456789012345678901234567890

/*
 * ENZO: THE NEXT GENERATION
 *
 * A parallel astrophysics and cosmology application
 *
 * Copyright (C) 2008 James Bordner
 * Copyright (C) 2008 Laboratory for Computational Astrophysics
 * Copyright (C) 2008 Regents of the University of California
 *
 * See CELLO_LICENSE in the main directory for full license agreement
 *
 */

/** 
 *********************************************************************
 *
 * @file      test_array.cpp
 * @brief     Program implementing unit tests for the ArraySerial class
 * @author    James Bordner
 * @date      Thu Feb 21 16:04:03 PST 2008
 * 
 * $Id$
 * 
 *********************************************************************
 */
 
#include <stdio.h>

#include "test.hpp"
#include "error.hpp"
#include "array.hpp"

main()
{

  unit_open();

  unit_class ("ArraySerial");

  //----------------------------------------------------------------------
  // test single array WITH resize: length, size, clear, values, access
  //----------------------------------------------------------------------

  {
    ArraySerial a;

    int n0=10,n1=15,n2=20;
    a.resize(n0,n1,n2);
    int n = n0*n1*n2;
    int m0,m1,m2;
    a.size(&m0,&m1,&m2);
    int m = a.length();
    unit_func("length");
    unit_assert(n == m);
    unit_func("size");
    unit_assert(m0==n0);
    unit_assert(m1==n1);
    unit_assert(m2==n2);

    Scalar * av = a.values();

    // set values
    int i0,i1,i2;
    for (i0=0; i0<n0; i0++) {
      for (i1=0; i1<n1; i1++) {
	for (i2=0; i2<n2; i2++) {
	  int i = i0 + n0*(i1 + n1*i2);
	  av [ i ] = 100+i;
	}
      }
    }    

    // test values

    bool passed = true;
    for (i0=0; i0<n0; i0++) {
      for (i1=0; i1<n1; i1++) {
	for (i2=0; i2<n2; i2++) {
	  int i = i0 + n0*(i1 + n1*i2);
	  if (av[i]!=a(i0,i1,i2) && passed) {
	    passed = false;
	    printf ("av[%d] = %g  a(%d,%d,%d) = %g\n",
		    i,av[i],i0,i1,i2,a(i0,i1,i2));
	  }
	}
      }
    }    
    unit_func("operator()");
    unit_assert(passed);

    // clear() to 0

    a.clear();
    passed = true;
    for (i0=0; i0<n0; i0++) {
      for (i1=0; i1<n1; i1++) {
	for (i2=0; i2<n2; i2++) {
	  int i = i0 + n0*(i1 + n1*i2);
	  if (av[i]!=0.0 && passed) {
	    passed = false;
	    printf ("av[%d] = %g\n",i,av[i]);
	  }
	}
      }
    }    
    unit_func("clear");
    unit_assert(passed);

  }
  //----------------------------------------------------------------------
  // test single array WITHOUT resize: length, size, clear, values, access
  //----------------------------------------------------------------------

  {

    int n0=10,n1=15,n2=20;
    
    ArraySerial a;
    a.resize(n0,n1,n2);
 
    int n = n0*n1*n2;
    int m0,m1,m2;
    a.size(&m0,&m1,&m2);
    int m = a.length();
    unit_func("length");
    unit_assert(n == m);
    unit_func("size");
    unit_assert(m0==n0);
    unit_assert(m1==n1);
    unit_assert(m2==n2);

    Scalar * av = a.values();

    // set values
    int i0,i1,i2;
    for (i0=0; i0<n0; i0++) {
      for (i1=0; i1<n1; i1++) {
	for (i2=0; i2<n2; i2++) {
	  int i = i0 + n0*(i1 + n1*i2);
	  av [ i ] = 100+i;
	}
      }
    }    

    // test values

    bool passed = true;
    for (i0=0; i0<n0; i0++) {
      for (i1=0; i1<n1; i1++) {
	for (i2=0; i2<n2; i2++) {
	  int i = i0 + n0*(i1 + n1*i2);
	  if (av[i]!=a(i0,i1,i2) && passed) {
	    passed = false;
	    printf ("av[%d] = %g  a(%d,%d,%d) = %g\n",
		    i,av[i],i0,i1,i2,a(i0,i1,i2));
	  }
	}
      }
    }    
    unit_func("operator()");
    unit_assert(passed);

    // Multiple arrays: copy
    ArraySerial b;
    b = a;

    Scalar * bv = b.values();
    
    // compare values

    passed = true;
    for (i0=0; i0<n0; i0++) {
      for (i1=0; i1<n1; i1++) {
	for (i2=0; i2<n2; i2++) {
	  int i = i0 + n0*(i1 + n1*i2);
	  if (bv[i]!=av[i] && passed) {
	    passed = false;
	    printf ("bv[%d] = %g  av[%d] = %g\n",i,bv[i],i,av[i]);
	  }
	}
      }
    }    
    unit_func("operator =");
    unit_assert(passed);

    // clear() to non-0

    a.clear(1.5);
    passed = true;
    for (i0=0; i0<n0; i0++) {
      for (i1=0; i1<n1; i1++) {
	for (i2=0; i2<n2; i2++) {
	  int i = i0 + n0*(i1 + n1*i2);
	  if (av[i]!=1.5 && passed) {
	    passed = false;
	    printf ("av[%d] = %g\n",i,av[i]);
	  }
	}
      }
    }    
    unit_func("clear");
    unit_assert(passed);

  }

  unit_class ("Block");

  {

    int n0=10,n1=15,n2=20;
    
    ArraySerial a;
    a.resize(n0,n1,n2);

    int m0,m1,m2;
    a.size(&m0,&m1,&m2);
    Block b (a.values(),m0,m0,m1,m1,m2,m2);

    int nx,ny,nz;
    b.n(&nx,&ny,&nz);

    Scalar * bv = b.array();
    for (int iz=0; iz<nz; iz++) {
      for (int iy=0; iy<ny; iy++) {
	for (int ix=0; ix<nx; ix++) {
	  int i = ix + nx*(iy + ny*iz);
	  bv[i] = 1.0;
	}
      }
    }

  }
  
  unit_close();

}
