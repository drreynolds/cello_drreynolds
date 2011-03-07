// $Id$
// See LICENSE_CELLO file for license and copyright information

/// @file     mesh_Mesh.cpp
/// @author   James Bordner (jobordner@ucsd.edu)
/// @date     Tue Sep 21 16:12:22 PDT 2010
/// @brief    Brief description of file mesh_Mesh.cpp

#include "cello.hpp"

#include "mesh.hpp"

//----------------------------------------------------------------------

Mesh::Mesh
(
 int nx, int ny, int nz,
 int nbx, int nby, int nbz
 ) throw ()
  : tree_(0),
    dimension_(0),
    refine_(0),
    max_level_(0),
    balanced_(0),
    backfill_(0),
    coalesce_(0)

{
  // create the root patch
  patch_.resize(1);
  patch_[0] = new Patch(nx,ny,nz,nbx,nby,nbz);

  // Initialize extents
  for (int i=0; i<3; i++) {
    lower_[i] = 0.0;
    upper_[i] = 1.0;
  }

#ifdef CONFIG_USE_MPI
  // For MPI, use Cello MPI communicator and associated group
  mpi_comm_ = MPI_COMM_CELLO;
  MPI_Comm_group (mpi_comm_, &mpi_group_);
#endif

}

//----------------------------------------------------------------------

Mesh::~Mesh() throw()
{
  for (size_t i=0; i<patch_.size(); i++) {
    delete patch_[i];
    patch_[i] = 0;
  }
  delete tree_;
}

//----------------------------------------------------------------------

size_t Mesh::num_patches() const  throw()
{
  return patch_.size();
}

//----------------------------------------------------------------------

Patch * Mesh::patch(size_t i) const throw()
{
  return ( patch_.size()-1 >= i )? patch_[i] : 0;
}

