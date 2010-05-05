// $Id$
// See LICENSE_CELLO file for license and copyright information

/// @file     
/// @author   James Bordner (jobordner@ucsd.edu)
/// @date     
/// @brief    

#include "cello.h"

#include "error.hpp"
#include "test.hpp"
#include "array.hpp"

int main()
{

  unit_class ("Array");

  //----------------------------------------------------------------------
  // test single array WITH resize: length, size, clear, values, access
  //----------------------------------------------------------------------

  {
    Array a;

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
	  if (av[i] != a(i0,i1,i2) && passed) {
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
    Array b;
    b = a;

    Scalar * bv = b.values();
    
    // compare values

    passed = true;
    for (i0=0; i0<n0; i0++) {
      for (i1=0; i1<n1; i1++) {
	for (i2=0; i2<n2; i2++) {
	  int i = i0 + n0*(i1 + n1*i2);
	  if (bv[i] != av[i] && passed) {
	    passed = false;
	    printf ("bv[%d] = %g  av[%d] = %g\n",i,bv[i],i,av[i]);
	  }
	}
      }
    }    
    unit_func("operator =");
    unit_assert(passed);

    // clear() to 0

    a.clear();

    passed = true;
    for (i0=0; i0<n0; i0++) {
      for (i1=0; i1<n1; i1++) {
	for (i2=0; i2<n2; i2++) {
	  int i = i0 + n0*(i1 + n1*i2);
	  if (av[i] != 0.0 && passed) {
	    passed = false;
	    printf ("av[%d] = %g\n",i,av[i]);
	  }
	}
      }
    }    
    unit_func("clear");
    unit_assert(passed);

    // clear() to non-0

    a.clear(1.5);
    passed = true;
    for (i0=0; i0<n0; i0++) {
      for (i1=0; i1<n1; i1++) {
	for (i2=0; i2<n2; i2++) {
	  int i = i0 + n0*(i1 + n1*i2);
	  if (av[i] != 1.5 && passed) {
	    passed = false;
	    printf ("av[%d] = %g\n",i,av[i]);
	  }
	}
      }
    }    
    unit_func("clear");
    unit_assert(passed);

  }
  
}
