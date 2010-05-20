// $Id$
// See LICENSE_CELLO file for license and copyright information

/// @file     test_Hdf5.cpp
/// @author   James Bordner (jobordner@ucsd.edu)
/// @date     Thu Feb 21 16:47:35 PST 2008
/// @brief    Program implementing unit tests for the Hdf5 class
 
#include <stdio.h>
#include <string>
#include <hdf5.h>

#include "error.hpp"
#include "test.hpp"
#include "disk.hpp"

int main(int argc, char ** argv)
{
  unit_init();

  int nx = 100;
  int ny = 50;
  Scalar * a = new Scalar[nx*ny];

  for (int iy=0; iy<ny; iy++) {
    for (int ix=0; ix<nx; ix++) {
      int i = ix + nx*(iy);
      a[i] = ix*3 + iy*5;
    }
  }

  unit_class ("Hdf5");

  unit_func("file_open");

  Hdf5 hdf5;

  int mx,my,mz;
  hdf5.file_open("file_open_test.hdf5","w");
  mx = nx;
  my = ny;
  mz = 1;

  hdf5.dataset_open_write ("dataset",mx,my,mz);
  hdf5.write(a);
  hdf5.dataset_close ();
  hdf5.file_close();


  hdf5.file_open("file_open_test.hdf5","r");
  hdf5.dataset_open_read ("dataset",&mx,&my,&mz);

  Scalar * b = new Scalar[nx*ny];
  
  hdf5.read(b);
  hdf5.dataset_close ();
  hdf5.file_close();

  bool passed = true;
  for (int iy=0; iy<ny && passed; iy++) {
    for (int ix=0; ix<nx && passed; ix++) {
      int i = ix + nx*(iy);
      if (a[i] != b[i]) passed = false;
    }
  }

  unit_func("read,write");
  unit_assert(passed);

  unit_finalize();

}
