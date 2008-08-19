/** 
 *********************************************************************
 *
 * @file      test_hdf5.cpp
 * @brief     Program implementing unit tests for the Hdf5 class
 * @author    James Bordner
 * @date      Thu Feb 21 16:47:35 PST 2008
 *
 * $Id$
 *
 *********************************************************************
 */
 
#include <stdio.h>
#include <string>

#include "hdf5.h"

#include "test_unit_.hpp"
#include "data_scalar_.hpp"
#include "data_array_.hpp"
#include "hdf5.hpp"

main()
{
  Array A (100,100);

  Scalar * a = A.values();
  int n0,n1;
  A.size(&n0,&n1);

  for (int i0=0; i0<100; i0++) {
    for (int i1=0; i1<100; i1++) {
      int i = i0 + n0*(i1);
      a[i] = i0*3 + i1*5;
    }
  }

  unit_class ("Hdf5");
  unit_open();
  unit_class_size(Hdf5);

  unit_func("file_open");

  Hdf5 hdf5;

  hdf5.file_open("file_open_test.hdf5","w");
  hdf5.dataset_open ("dataset",A);
  hdf5.write(A);
  hdf5.dataset_close ();
  hdf5.file_close();

  Array B;

  hdf5.file_open("file_open_test.hdf5","r");
  hdf5.dataset_open ("dataset",B);
  hdf5.read(B);
  hdf5.dataset_close ();
  hdf5.file_close();

  Scalar * b = B.values();

  bool passed = true;
  for (int i0=0; i0<100; i0++) {
    for (int i1=0; i1<100; i1++) {
      int i = i0 + n0*(i1);
      if (a[i] != B(i0,i1)) passed = false;
      if (b[i] != A(i0,i1)) passed = false;
    }
  }

  unit_func("read,write");
  unit_assert(passed);

  unit_close();
}
