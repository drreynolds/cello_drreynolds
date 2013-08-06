// See LICENSE_CELLO file for license and copyright information

/// @file     mesh_Hierarchy.cpp
/// @author   James Bordner (jobordner@ucsd.edu)
/// @date     Tue Sep 21 16:12:22 PDT 2010
/// @brief    Implementation of the Hierarchy class

#include "cello.hpp"

#include "mesh.hpp"

#include "charm_mesh.hpp"

//----------------------------------------------------------------------

Hierarchy::Hierarchy 
(
#ifndef CONFIG_USE_CHARM
 Simulation * simulation,
#endif
 const Factory * factory,
 int dimension, int refinement,
 int process_first, int process_last_plus
 ) throw ()
  :
#ifndef CONFIG_USE_CHARM
  simulation_(simulation),
#endif
  factory_((Factory *)factory),
  dimension_(dimension),
  refinement_(refinement),
  group_process_(GroupProcess::create(process_first,process_last_plus)),
  layout_(0)
{
  TRACE("Hierarchy::Hierarchy()");
  // Initialize extents
  for (int i=0; i<3; i++) {
    lower_[i] = 0.0;
    upper_[i] = 1.0;
    root_size_[i] = 1;
    blocking_[i] = 0;
  }
}

//----------------------------------------------------------------------

Hierarchy::~Hierarchy() throw()
{
# ifdef CONFIG_USE_CHARM

  deallocate_blocks();
  delete group_process_; group_process_ = 0;

# else

  for (size_t i=0; i<block_.size(); i++) {
    delete block_[i];
    block_[i] = 0;
  }

# endif
}

//----------------------------------------------------------------------

#ifdef CONFIG_USE_CHARM
  /// CHARM++ Pack / Unpack function
void Hierarchy::pup (PUP::er &p)
{
    
  TRACEPUP;
  // NOTE: change this function whenever attributes change

  bool up = p.isUnpacking();

  p | *factory_;
  p | dimension_;
  p | refinement_;
  p | num_blocks_;
  if (p.isUnpacking()) block_array_ = new CProxy_CommBlock;
  p | *block_array_;
  p | block_exists_;
  p | block_sync_;
  PUParray(p,root_size_,3);
  PUParray(p,lower_,3);
  PUParray(p,upper_,3);

  if (up) group_process_ = GroupProcess::create();
  p | *layout_;
  PUParray(p,blocking_,3);

}
#endif

//----------------------------------------------------------------------

void Hierarchy::set_lower(double x, double y, double z) throw ()
{
  lower_[0] = x;
  lower_[1] = y;
  lower_[2] = z;
}

//----------------------------------------------------------------------

void Hierarchy::set_upper(double x, double y, double z) throw ()
{
  upper_[0] = x;
  upper_[1] = y;
  upper_[2] = z;
}

//----------------------------------------------------------------------

void Hierarchy::set_root_size(int nx, int ny, int nz) throw ()
{
  TRACE3("Hierarchy::set_root_size(%d %d %d)",nx,ny,nz);

  root_size_[0] = nx;
  root_size_[1] = ny;
  root_size_[2] = nz;

}

//----------------------------------------------------------------------

void Hierarchy::set_blocking(int nx, int ny, int nz) throw ()
{
  TRACE3("Hierarchy::set_blocking(%d %d %d)",nx,ny,nz);

  blocking_[0] = nx;
  blocking_[1] = ny;
  blocking_[2] = nz;
  if (!layout_) {
    layout_ = new Layout (blocking_[0],blocking_[1],blocking_[2]);
    layout_->set_process_range(0,group_process_->size());
  }


}

//----------------------------------------------------------------------

int Hierarchy::dimension() const throw ()
{ 
  return dimension_; 
}

//----------------------------------------------------------------------

void Hierarchy::root_size(int * nx, int * ny, int * nz) const throw ()
{
  if (nx) *nx = root_size_[0];
  if (ny) *ny = root_size_[1];
  if (nz) *nz = root_size_[2];
}

//----------------------------------------------------------------------

void Hierarchy::lower(double * x, double * y, double * z) const throw ()
{
  if (x) *x = lower_[0];
  if (y) *y = lower_[1];
  if (z) *z = lower_[2];
}
//----------------------------------------------------------------------

void Hierarchy::upper(double * x, double * y, double * z) const throw ()
{
  if (x) *x = upper_[0];
  if (y) *y = upper_[1];
  if (z) *z = upper_[2];
}

//----------------------------------------------------------------------

void Hierarchy::blocking (int * nbx, int * nby, int * nbz) const throw()
{
  if (nbx) (*nbx) = blocking_[0];
  if (nby) (*nby) = blocking_[1];
  if (nbz) (*nbz) = blocking_[2];
}

size_t Hierarchy::num_blocks(int * nbx, 
			     int * nby,
			     int * nbz) const throw()
{ 
  if (nbx) *nbx = blocking_[0];
  if (nby) *nby = blocking_[1];
  if (nbz) *nbz = blocking_[2];

  return blocking_[0]*blocking_[1]*blocking_[2];
}

//----------------------------------------------------------------------

void Hierarchy::deallocate_blocks() throw()
{

#ifdef CONFIG_USE_CHARM

  if (block_exists_) {
    block_array_->ckDestroy();
    delete block_array_; block_array_ = 0;
    block_exists_ = false;
  }

#else

  for (size_t i=0; i<block_.size(); i++) {
    delete block_[i];
    block_[i] = 0;
  }

#endif
}

//----------------------------------------------------------------------

Layout * Hierarchy::layout () throw()
{
  return layout_;
}

//----------------------------------------------------------------------

const Layout * Hierarchy::layout () const throw()
{
  return layout_;
}

//----------------------------------------------------------------------

void Hierarchy::create_forest
(
 FieldDescr   * field_descr,
 bool allocate_blocks,
 bool allocate_data,
 bool testing,
 int process_first, int process_last_plus) throw()
{

  if (allocate_blocks) {
#ifdef CONFIG_USE_CHARM
    allocate_array_(allocate_data,testing);
#else  /* CONFIG_USE_CHARM */
    allocate_array_(allocate_data,testing,field_descr);
#endif  /* CONFIG_USE_CHARM */
  }
}

//----------------------------------------------------------------------

#ifdef CONFIG_USE_CHARM
#else  /* CONFIG_USE_CHARM */
size_t Hierarchy::num_local_blocks() const  throw()
{
  int rank = group_process_->rank();
  return layout_->local_count(rank);
}
#endif  /* CONFIG_USE_CHARM */

//----------------------------------------------------------------------

#ifdef CONFIG_USE_CHARM
#else  /* CONFIG_USE_CHARM */
CommBlock * Hierarchy::local_block(size_t i) const throw()
{
  return (i < block_.size()) ? block_[i] : 0;

}
#endif  /* CONFIG_USE_CHARM */

//----------------------------------------------------------------------

void Hierarchy::allocate_array_
(
 bool allocate_data,
 bool testing,
 const FieldDescr * field_descr
) throw()
  // NOTE: field_descr only needed for MPI; may be null for CHARM++
{

#ifdef CONFIG_USE_CHARM


#else /* CONFIG_USE_CHARM */

  // determine local block count nb
  int nb = num_local_blocks();

  // create local blocks
  block_.resize(nb);

#endif /* CONFIG_USE_CHARM */

  // Get number of blocks in the forest

  int nbx,nby,nbz;
  layout_->block_count (&nbx, &nby, &nbz);

  // determine block size
  int mbx = root_size_[0] / nbx;
  int mby = root_size_[1] / nby;
  int mbz = root_size_[2] / nbz;

  // Check that blocks evenly subdivide forest
  if (! ((nbx*mbx == root_size_[0]) &&
	 (nby*mby == root_size_[1]) &&
	 (nbz*mbz == root_size_[2]))) {

    ERROR6("Forest::allocate_array_()",  
	   "CommBlocks must evenly subdivide forest: "
	   "forest size = (%d %d %d)  block count = (%d %d %d)",
	   root_size_[0],root_size_[1],root_size_[2],
	   nbx,nby,nbz);
      
  }

  // CREATE AND INITIALIZE NEW DATA BLOCKS

  int num_field_blocks = 1;

#ifdef CONFIG_USE_CHARM

  TRACE("Allocating block_array_");

  block_array_ = new CProxy_CommBlock;

  (*block_array_) = factory_->create_block_array
    (nbx,nby,nbz,
     mbx,mby,mbz,
     num_field_blocks,
     allocate_data,
     testing);
    
  block_exists_ = allocate_data;
  block_sync_.stop() = nbx*nby*nbz;

#else /* CONFIG_USE_CHARM */

  int ip = group_process_->rank();

  for (int ib=0; ib<nb; ib++) {

    // Get index of block ib in the forest

    int ibx,iby,ibz;

    layout_->block_indices (ip,ib, &ibx, &iby, &ibz);

    // create a new data block

    int level;

    Index index (ibx,iby,ibz);

    int count_adapt;
    bool initial;
    int narray = 0;
    char * array = 0;
    int op_array = op_array_copy;
    int cycle = 0;
    double time = 0.0;
    double dt = 0.0;
    int num_face_level = 0;
    int * face_level = 0;

    CommBlock * comm_block = factory_->create_block 
      (
       simulation_,
       index,
       mbx,mby,mbz,
       num_field_blocks,
       count_adapt = 0,
       initial = true,
       cycle, time, dt,
       narray, array, op_array,
       num_face_level, face_level,
       testing);

    // Store the data block in the block array
    block_[ib] = comm_block;

  }
#endif /* CONFIG_USE_CHARM */
}


