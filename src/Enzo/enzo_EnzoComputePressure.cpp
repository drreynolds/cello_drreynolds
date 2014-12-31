// See LICENSE_CELLO file for license and copyright information

/// @file     enzo_EnzoComputePressure.cpp
/// @author   James Bordner (jobordner@ucsd.edu)
/// @date     2014-10-27 22:37:41
/// @brief    Implements the EnzoComputePressure class

#include "cello.hpp"

#include "enzo.hpp"

//----------------------------------------------------------------------

EnzoComputePressure::EnzoComputePressure (double gamma,
					  int comoving_coordinates)
  : Compute(),
    gamma_(gamma),
    comoving_coordinates_(comoving_coordinates)
{
}

//----------------------------------------------------------------------

void EnzoComputePressure::pup (PUP::er &p)
{

  // NOTE: change this function whenever attributes change

  TRACEPUP;

  Compute::pup(p);

  p | gamma_;
  p | comoving_coordinates_;

}

//----------------------------------------------------------------------

void EnzoComputePressure::compute ( CommBlock * comm_block) throw()
{
  Field field = comm_block->block()->field();
  if (field.precision(0) == precision_single) {
    compute_<float>(comm_block);
  } else if (field.precision(0) == precision_double) {
    compute_<double>(comm_block);
  }
}

//----------------------------------------------------------------------

template <typename T>
void EnzoComputePressure::compute_(CommBlock * comm_block)
{

  if (!comm_block->is_leaf()) return;

  EnzoBlock * enzo_block = static_cast<EnzoBlock*> (comm_block);

  Field field = enzo_block->block()->field();

  T * p = (T*) field.values("pressure");
  T * d = (T*) field.values("density");
  T * v3[3] = 
    { (T*) field.values("velocity_x"),
      (T*) field.values("velocity_y"),
      (T*) field.values("velocity_z")  };

  T * te = (T*) field.values("total_energy");

  int nx,ny,nz;
  field.size(&nx,&ny,&nz);

  int gx,gy,gz;
  field.ghosts (0,&gx,&gy,&gz);
  const int rank = comm_block->rank();
  if (rank < 1) gx = 0;
  if (rank < 2) gy = 0;
  if (rank < 3) gz = 0;

  int m = (nx+2*gx) * (ny+2*gy) * (nz+2*gz);
  T gm1 = gamma_ - 1.0;
  for (int i=0; i<m; i++) {
    T e= te[i];
    if (rank >= 1) e -= 0.5*v3[0][i]*v3[0][i];
    if (rank >= 2) e -= 0.5*v3[1][i]*v3[1][i];
    if (rank >= 3) e -= 0.5*v3[2][i]*v3[2][i];
    p[i] = gm1 * d[i] * e;
  }
}

