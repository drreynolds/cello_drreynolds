// $Id$
// See LICENSE_CELLO file for license and copyright information

/// @file     mesh_Patch.cpp
/// @author   James Bordner (jobordner@ucsd.edu)
/// @date     Thu Feb 25 16:20:17 PST 2010
/// @brief    Implementation of the Patch class

#include "parallel.hpp"

#include "mesh.hpp"

//----------------------------------------------------------------------

Patch::Patch() throw()
  : data_descr_(0),
    data_block_(),
    layout_(0)

{
  patch_size_[0] = 1;
  patch_size_[1] = 1;
  patch_size_[2] = 1;

  extents_[0] = 0.0;
  extents_[1] = 1.0;
  extents_[2] = 0.0;
  extents_[3] = 1.0;
  extents_[4] = 0.0;
  extents_[5] = 1.0;
}

//----------------------------------------------------------------------

Patch::~Patch() throw()
{
  deallocate();
}

//----------------------------------------------------------------------

Patch::Patch(const Patch & patch) throw()
  :  data_descr_ (patch.data_descr())
{
  deallocate();

  allocate();
}

//----------------------------------------------------------------------

Patch & Patch::operator= (const Patch & patch) throw()
{
  deallocate();
  data_descr_ = patch.data_descr();
  allocate();
}

//----------------------------------------------------------------------

void Patch::set_data_descr (DataDescr * data_descr) throw()
{
  data_descr_ = data_descr;
}

//----------------------------------------------------------------------

DataDescr * Patch::data_descr () const throw()
{
  return data_descr_;
}

//----------------------------------------------------------------------

void Patch::set_patch_size (int npx, int npy, int npz) throw()
{
  patch_size_[0] = npx;
  patch_size_[1] = npy;
  patch_size_[2] = npz;
}

//----------------------------------------------------------------------

void Patch::patch_size (int * npx, int * npy, int * npz) const throw()
{
  if (npx) *npx = patch_size_[0];
  if (npy) *npy = patch_size_[1];
  if (npz) *npz = patch_size_[2];
}

//----------------------------------------------------------------------

void Patch::set_layout (Layout * layout) throw()
{
  // WARNING: potential for dangling pointer
  layout_ = layout;
}

//----------------------------------------------------------------------

Layout * Patch::layout () const throw()
{
  return layout_;
}

//----------------------------------------------------------------------

void Patch::set_extents (double xm, double xp,
			 double ym, double yp,
			 double zm, double zp) throw()
{
  extents_[0] = xm;
  extents_[1] = xp;
  extents_[2] = ym;
  extents_[3] = yp;
  extents_[4] = zm;
  extents_[5] = zp;
}

//----------------------------------------------------------------------

void Patch::extents (double * xm, double * xp,
		     double * ym, double * yp,
		     double * zm, double * zp) const throw()
{
  if (xm) *xm = extents_[0];
  if (xp) *xp = extents_[1];
  if (ym) *ym = extents_[2];
  if (yp) *yp = extents_[3];
  if (zm) *zm = extents_[4];
  if (zp) *zp = extents_[5];
}

  
//----------------------------------------------------------------------

void Patch::allocate(int ip) throw()
{
  // determine process range [ip0, ip0+np)
  int ip0,np;
  layout_->process_range(&ip0,&np);

  // ensure data_block_[ip][ib] array is sufficiently long in dimension 1
  data_block_.resize(np);

  // determine local block count nb
  
  int nbl = layout_->local_count(ip);

  // create local blocks

  data_block_[ip].resize(nbl);

  for (int i=0; i<nbl; i++) {
  }
}

//----------------------------------------------------------------------

void Patch::deallocate(int ip) throw()
{
}

//----------------------------------------------------------------------

bool Patch::is_allocated(int ip) const throw() 
{
}

//----------------------------------------------------------------------

int Patch::block_count(int ip) const  throw()
{
}

//----------------------------------------------------------------------

DataBlock * Patch::block(int i, int ip) const throw()
{
}

//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

// Patch::Patch(DataDescr * data_descr,
// 	     int patch_size[3], 
// 	     int block_size,
// 	     double lower[3],
// 	     double upper[3]) 
//   : block_size_(block_size)
// {
//   patch_size_[0] = patch_size[0];
//   patch_size_[1] = patch_size[1];
//   patch_size_[2] = patch_size[2];

//   lower_[0] = lower[0];
//   lower_[1] = lower[1];
//   lower_[2] = lower[2];

//   upper_[0] = upper[0];
//   upper_[1] = upper[1];
//   upper_[2] = upper[2];

//   // Make sure patch_size is evenly divisible by block_size
//   bool is_valid[3];
//   is_valid[0] = patch_size[0] == (patch_size[0]/block_size_)*block_size_;
//   is_valid[1] = patch_size[1] == (patch_size[1]/block_size_)*block_size_;
//   is_valid[2] = patch_size[2] == (patch_size[2]/block_size_)*block_size_;

//   printf ("%d %d %d  %d\n",patch_size[0],patch_size[1],patch_size[2],
// 	  block_size_);
//   ASSERT("Patch::Patch","patch size must be evenly divisible by block size",
// 	 is_valid[0] && 
// 	 (is_valid[1] || patch_size[1] == 1) && 
// 	 (is_valid[2] || patch_size[2] == 1));
	 
//   block_count_[0] = MAX(1,patch_size[0] / block_size_);
//   block_count_[1] = MAX(1,patch_size[1] / block_size_);
//   block_count_[2] = MAX(1,patch_size[2] / block_size_);

//   // Create local data blocks

//   int nb = block_count_[0] * block_count_[1] * block_count_[2];

//   data_block_ = new DataBlock * [nb];

//   for (int i = 0; i < nb; i++) {
//     data_block_[i] = new DataBlock;
//   }

//   // Initialize local data blocks

//   double block_width[3];

//   block_width[0] = block_size_ * (upper_[0] - lower_[0]) / patch_size[0];
//   block_width[1] = block_size_ * (upper_[1] - lower_[1]) / patch_size[1];
//   block_width[2] = block_size_ * (upper_[2] - lower_[2]) / patch_size[2];

//   FieldDescr * field_descr = data_descr->field_descr();

//   for (int iz = 0; iz < block_count_[2]; iz++) {
//     double z = lower[2] + iz * block_width[2];
//     for (int iy = 0; iy < block_count_[1]; iy++) {
//       double y = lower[1] + iy * block_width[1];
//       for (int ix = 0; ix < block_count_[0]; ix++) {
// 	double x = lower[0] + iz * block_width[0];

// 	FieldBlock * field_block = data_block(ix,iy,iz)->field_block();

// 	field_block->set_field_descr(field_descr);

// 	field_block->set_size(block_size_,
// 			      block_size_,
// 			      block_size_);

// 	field_block->set_extent(x,x+block_width[0],
// 				y,y+block_width[1],
// 				z,z+block_width[2]);
//       }
//     }
//   }
// }

// //----------------------------------------------------------------------

// Patch::~Patch() throw ()
// {
//   for (int i=0; i<num_data_blocks(); i++) {
//     delete data_block_[i];
//   }
//   delete [] data_block_;
// }

// //----------------------------------------------------------------------

// Patch::Patch(const Patch & patch) throw ()
// /// @param     patch  Object being copied
// {
// }

// //----------------------------------------------------------------------

// Patch & Patch::operator= (const Patch & patch) throw ()
// /// @param     patch  Source object of the assignment
// /// @return    The target assigned object
// {
//   return *this;
// }

//======================================================================

