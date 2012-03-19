// See LICENSE_CELLO file for license and copyright information

/// @file     Io_IoHierarchy.cpp
/// @author   James Bordner (jobordner@ucsd.edu)
/// @date     2011-10-06
/// @brief    Implementation of the IoHierarchy class

#include "io.hpp"

//----------------------------------------------------------------------

IoHierarchy::IoHierarchy(const Hierarchy * hierarchy) throw ()
  : Io(3,0),
    hierarchy_(hierarchy)
{
  meta_name_.push_back("lower");
  meta_name_.push_back("upper");
  meta_name_.push_back("patch_count");

  ASSERT2("IoHierarchy::IoHierarchy()",
	 "meta_name.size() [%d] !=  meta_count_ [%d]",
	  meta_name_.size(),meta_count(),
	  meta_name_.size()==meta_count());
}


//----------------------------------------------------------------------

void IoHierarchy::meta_value
(int index,
 void ** buffer, std::string * name, enum scalar_type * type,
 int * nxd, int * nyd, int * nzd) throw()
{
  Io::meta_value(index,buffer,name,type,nxd,nyd,nzd);

  if (index == 0) {

    *buffer = (void *) hierarchy_->lower_;
    *type   = scalar_type_double;
    *nxd     = 3;
    
  } else if (index == 1) {

    *buffer = (void *) hierarchy_->upper_;
    *type   = scalar_type_double;
    *nxd     = 3;

  } else if (index == 2) {

    *buffer = (void *) & hierarchy_->patch_count_;
    *type   = scalar_type_int;
  }
  
}

//----------------------------------------------------------------------

void IoHierarchy::data_value
(int index,
 void ** buffer, std::string * name, enum scalar_type * type,
 int * nxd, int * nyd, int * nzd,
 int * nx,  int * ny,  int * nz) throw()
{
}

//----------------------------------------------------------------------
