// See LICENSE_CELLO file for license and copyright information

/// @file     Io_IoPatch.cpp
/// @author   James Bordner (jobordner@ucsd.edu)
/// @date     2011-10-06
/// @brief    Implementation of the IoPatch class

#include "io.hpp"

//----------------------------------------------------------------------

IoPatch::IoPatch(const Patch * patch) throw ()
  : Io(5,0),
    patch_(patch)
{
  meta_name_.push_back("size");
  meta_name_.push_back("offset");
  meta_name_.push_back("blocking");
  meta_name_.push_back("lower");
  meta_name_.push_back("upper");

  ASSERT2("IoPatch::IoPatch()",
	 "meta_name.size() [%d] !=  meta_count_ [%d]",
	  meta_name_.size(),meta_count(),
	  meta_name_.size()==meta_count());
}


//----------------------------------------------------------------------

void IoPatch::meta_value
(int index,
 void ** buffer, std::string * name, enum scalar_type * type,
 int * nxd, int * nyd, int * nzd) throw()
{
  Io::meta_value(index,buffer,name,type,nxd,nyd,nzd);

  if (index == 0) {

    *buffer = (void *) patch_->size_;
    *type   = scalar_type_int;
    *nxd     = 3;
    
  } else if (index == 1) {

    *buffer = (void *) patch_->offset_;
    *type   = scalar_type_int;
    *nxd     = 3;

  } else if (index == 2) {

    *buffer = (void *) patch_->blocking_;
    *type   = scalar_type_int;
    *nxd     = 3;

  } else if (index == 3) {

    *buffer = (void *) patch_->lower_;
    *type   = scalar_type_double;
    *nxd     = 3;

  } else if (index == 4) {

    *buffer = (void *) patch_->upper_;
    *type   = scalar_type_double;
    *nxd     = 3;

  }
  
}

//----------------------------------------------------------------------

void IoPatch::data_value
(int index,
 void ** buffer, std::string * name, enum scalar_type * type,
 int * nxd, int * nyd, int * nzd,
 int * nx,  int * ny,  int * nz) throw()
{
}

//----------------------------------------------------------------------
