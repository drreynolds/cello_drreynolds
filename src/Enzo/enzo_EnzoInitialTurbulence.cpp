// See LICENSE_CELLO file for license and copyright information

/// @file     enzo_EnzoInitialTurbulence.cpp
/// @author   James Bordner (jobordner@ucsd.edu)
/// @date     Wed Jul 23 00:30:49 UTC 2014
/// @brief    Implementation of Enzo 2D Implosion problem initialization

#include "cello.hpp"

#include "enzo.hpp"

//----------------------------------------------------------------------

EnzoInitialTurbulence::EnzoInitialTurbulence 
(int init_cycle, double init_time, 
 double density_initial,
 double pressure_initial,
 double temperature_initial,
 double gamma) throw ()
  : Initial(init_cycle, init_time),
    density_initial_(density_initial),
    pressure_initial_(pressure_initial),
    temperature_initial_(temperature_initial),
    gamma_(gamma)
{ }

//----------------------------------------------------------------------

void EnzoInitialTurbulence::pup (PUP::er &p)
{
  // NOTE: update whenever attributes change

  TRACEPUP;

  Initial::pup(p);

  p | density_initial_;
  p | pressure_initial_;
  p | temperature_initial_;
  p | gamma_;

}

//----------------------------------------------------------------------

void EnzoInitialTurbulence::enforce_block 
(
 CommBlock * comm_block,
 const FieldDescr * field_descr,
 const Hierarchy  * hierarchy
 ) throw()

{

  if (!comm_block->is_leaf()) return;

  //  INCOMPLETE("EnzoInitialTurbulence::enforce_block()");

  ASSERT("EnzoInitialTurbulence",
	 "CommBlock does not exist",
	 comm_block != NULL);

  FieldBlock * field_block = comm_block->block()->field_block();

  enzo_float *  d = (enzo_float *) field_block->values("density");
  enzo_float *  p = (enzo_float *) field_block->values("pressure");
  enzo_float *  t = (enzo_float *) field_block->values("temperature");
  enzo_float * a3[3] = { (enzo_float *) field_block->values("driving_x"),
			 (enzo_float *) field_block->values("driving_y"),
			 (enzo_float *) field_block->values("driving_z") };
  enzo_float * v3[3] = { (enzo_float *) field_block->values("velocity_x"),
			 (enzo_float *) field_block->values("velocity_y"),
			 (enzo_float *) field_block->values("velocity_z") };

  enzo_float * te = (enzo_float *) field_block->values("total_energy");

  int rank = comm_block->simulation()->rank();

  ASSERT("EnzoInitializeTurbulence::enforce_block()",
	 "Missing Field 'density'", d);
  ASSERT("EnzoInitializeTurbulence::enforce_block()",
	 "Missing Field 'pressure'",p);
  ASSERT("EnzoInitializeTurbulence::enforce_block()",
	 "Missing Field 'temperature'",t);
  if (rank >= 1)
    ASSERT("EnzoInitializeTurbulence::enforce_block()",
	   "Missing Field 'driving_x'", a3[0]);
  if (rank >= 2)
    ASSERT("EnzoInitializeTurbulence::enforce_block()",
	   "Missing Field 'driving_y'",  a3[1]);
  if (rank >= 3)
    ASSERT("EnzoInitializeTurbulence::enforce_block()",
	   "Missing Field 'driving_z'",  a3[2]);
  if (rank >= 1)
    ASSERT("EnzoInitializeTurbulence::enforce_block()",
	   "Missing Field 'velocity_x'", v3[0]);
  if (rank >= 2)
    ASSERT("EnzoInitializeTurbulence::enforce_block()",
	   "Missing Field 'velocity_y'", v3[1]);
  if (rank >= 3)
    ASSERT("EnzoInitializeTurbulence::enforce_block()",
	   "Missing Field 'velocity_z'",  v3[2]);
  ASSERT("EnzoInitializeTurbulence::enforce_block()",
	 "Missing Field 'total_energy'",  te);

  double xm,ym,zm;
  comm_block->block()->lower(&xm,&ym,&zm);

  double xp,yp,zp;
  comm_block->block()->upper(&xp,&yp,&zp);

  double hx,hy,hz;
  field_block->cell_width(xm,xp,&hx,
			  ym,yp,&hy,
			  zm,zp,&hz);

  int nx,ny,nz;
  field_block->size(&nx,&ny,&nz);
  int gx,gy,gz;
  field_descr->ghosts(0,&gx,&gy,&gz);

  int ndx = nx + 2*gx;
  int ndy = ny + 2*gy;
  int ndz = nz + 2*gz;

  // initialize driving fields using turboinit

  int Nx,Ny,Nz;
  comm_block->simulation()->hierarchy()->root_size (&Nx, &Ny, &Nz);

  // assumes cubical domain

  ASSERT3 ("EnzoInitialTurbulence::enforce_block()",
	   "Root grid mesh dimensions %d %d %d must be equal or 1",
	   Nx,Ny,Nz,
	   (Ny==1) || 
	   ((Nz==1) && (Nx == Ny)) ||
	   (Nx == Ny && Ny == Nz));

  // scale by level
  for (int i=0; i<comm_block->level(); i++) Nx  *= 2;

  // compute offsets
  Index index = comm_block->index();

  int ix,iy,iz;
  index.array(&ix,&iy,&iz);

  int bx,by,bz;
  index.tree(&bx,&by,&bz);

  int o3[3] = { ix * nx, iy * ny, iz * nz };

  int level = index.level();

  unsigned mask = 1 << (INDEX_MAX_TREE_BITS - 1);

  for (int i=0; i<level; i++) {
    bool mx = (mask & bx);
    bool my = (mask & by);
    bool mz = (mask & bz);
    o3[0] = 2*o3[0] + (mx ? nx : 0);
    o3[1] = 2*o3[1] + (my ? ny : 0);
    o3[2] = 2*o3[2] + (mz ? nz : 0);
    mask = mask >> 1;
  }

  FORTRAN_NAME(turboinit)
    (&rank, &Nx, 
     (enzo_float *)v3[0],
     (enzo_float *)v3[1],
     (enzo_float *)v3[2],
     &ndx,&ndy,&ndz,
     &o3[0],&o3[1],&o3[2]);

  for (int i=0; i<ndx*ndy*ndz; i++) {
    a3[0][i] = v3[0][i];
    a3[1][i] = v3[1][i];
    a3[2][i] = v3[2][i];
  }

  for (int iz=0; iz<nz+2*gz; iz++) {
    for (int iy=0; iy<ny+2*gy; iy++) {
      for (int ix=0; ix<nx+2*gx; ix++) {
	int i = ix + ndx*(iy + ndy*iz);
	d[i]  = density_initial_;
	if (pressure_initial_) {
	  p[i] = pressure_initial_;
	  te[i] = pressure_initial_ / (gamma_ - 1) / d[i];
	}
	if (temperature_initial_) {
	  te[i] = temperature_initial_ / (gamma_ - 1);
	}
	for (int id=0; id<rank; id++) {
	  te[i] += 0.5*v3[id][i]*v3[id][i];
	}
      }
    }
  }
  if (temperature_initial_) {
    EnzoMethodPressure method_pressure(gamma_);
    method_pressure.compute(comm_block);
  }
}
