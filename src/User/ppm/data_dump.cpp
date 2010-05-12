// $Id$
// See LICENSE_ENZO file for license and copyright information

/// @file      data_dump.cpp
/// @author    James Bordner (jobordner@ucsd.edu)
/// @date      Sun Aug 30 14:16:29 PDT 2009
/// @brief     Write BaryonField's to disk

#include "cello_hydro.h"

#include <string>
#include <hdf5.h>

#include "disk.hpp"

void data_dump(const char * file_root, int cycle)
{ 

  int nx = GridDimension[0];
  int ny = GridDimension[1];
  int nz = GridDimension[2];

  Hdf5 hdf5;

  // Open hdf5 file dump for cycle
  char filename[80];

  sprintf (filename,"%s-%06d.hdf5",file_root,cycle);
  hdf5.file_open(filename,"w");
  hdf5.dataset_open_write ("density",nx,ny,nz);
  hdf5.write(BaryonField[field_density]);
  hdf5.dataset_close ();
  hdf5.file_close();
}
