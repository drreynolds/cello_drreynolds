// See LICENSE_CELLO file for license and copyright information

/// @file     mesh_Refine.cpp
/// @author   James Bordner (jobordner@ucsd.edu)
/// @date     2014-08-18
/// @brief Implementation of the Refine base class for mesh refinement
///        criteria

#include "mesh.hpp"

//----------------------------------------------------------------------

void * Refine::initialize_output_(FieldData * field_data)
{
  void * output = 0;
  const bool do_output = output_ != "";

  if (do_output) {
    const int id_output = field_data->field_id(output_);
    output = field_data->values(id_output);
    int nx,ny,nz;
    field_data->size(&nx,&ny,&nz);
    int gx,gy,gz;
    field_data->ghost_depth(id_output, &gx,&gy,&gz);
    int nxd = nx + 2*gx;
    int nyd = ny + 2*gy;
    int nzd = nz + 2*gz;
    precision_type precision = field_data->precision(id_output);
    if (precision == precision_single)
      for (int i=0; i<nxd*nyd*nzd; i++) ((float*)output)[i] = -1;
    else if (precision == precision_double) {
      for (int i=0; i<nxd*nyd*nzd; i++) ((double*)output)[i] = -1;
    }
  }
  return output;
}
