// See LICENSE_CELLO file for license and copyright information

/// @file     enzo_EnzoInitialImplosion2.cpp
/// @author   James Bordner (jobordner@ucsd.edu)
/// @date     Tue Jan  4 19:30:35 PST 2011
/// @brief    Implementation of Enzo 2D Implosion problem initialization

#include "cello.hpp"

#include "enzo.hpp"

//----------------------------------------------------------------------

EnzoInitialImplosion2::EnzoInitialImplosion2 () throw ()
  : Initial()
{}

//----------------------------------------------------------------------

void EnzoInitialImplosion2::compute 
(
 const FieldDescr * field_descr,
 Block * block
 ) throw()

{

  EnzoBlock * enzo_block = static_cast<EnzoBlock*> (block);

  ASSERT("EnzoInitialImplosion2",
	 "Expecting an EnzoBlock",
	 enzo_block != NULL);

  FieldBlock * field_block = enzo_block->field_block();

  ASSERT("EnzoInitialImplosion2",
	 "Insufficient number of fields",
	 field_descr->field_count() >= 4);

  WARNING("EnzoInitialImplosion2::compute",
	  "hard-coded field index ordering");

  int index_density         = 0;
  int index_velocity_x      = 1;
  int index_velocity_y      = 2;
  int index_total_energy    = 3;
 
  enzo_float *  d = (enzo_float *) field_block->field_values(index_density);
  enzo_float * vx = (enzo_float *) field_block->field_values(index_velocity_x);
  enzo_float * vy = (enzo_float *) field_block->field_values(index_velocity_y);
  enzo_float * te = (enzo_float *) field_block->field_values(index_total_energy);

  // Block size (excluding ghosts)
  int nx,ny;
  field_block->size(&nx,&ny);

  // Cell widths
  double hx,hy;
  field_block->cell_width(enzo_block,&hx,&hy);

  // Ghost depths
  int gx,gy;
  field_descr->ghosts(index_density,&gx,&gy);

  // Left edges
  double x0, y0;
  block->lower(&x0,&y0);

  // WARNING("EnzoInitialImplosion2",
  // 		  "Assumes same ghost zone depth for all fields");

  int ngx = nx + 2*gx;
  int ngy = ny + 2*gy;

  for (int iy=gy; iy<ny+gy; iy++) {
    double y = y0 + (iy - gy + 0.5)*hy;
    for (int ix=gy; ix<nx+gx; ix++) {
      double x = x0 + (ix - gx + 0.5)*hx;
      int i = INDEX(ix,iy,0,ngx,ngy);
      if (x + y < 0.1517) {
	d[i]  = 0.125;
	vx[i] = 0.0;
	vy[i] = 0.0;
	te[i] = 0.14 / ((enzo::Gamma - 1.0) * d[i]);
      } else {
	d[i]  = 1.0;
	vx[i] = 0.0;
	vy[i] = 0.0;
	te[i] = 1.0 / ((enzo::Gamma - 1.0) * d[i]);
      }
    }
  }

}
