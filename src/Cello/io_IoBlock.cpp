// See LICENSE_CELLO file for license and copyright information

/// @file     io_IoBlock.cpp
/// @author   James Bordner (jobordner@ucsd.edu)
/// @date     2011-10-06
/// @brief    Implementation of the IoBlock class

#include "io.hpp"

//----------------------------------------------------------------------

IoBlock::IoBlock() throw ()
  : Io(),
    block_(0)
{
  meta_name_.push_back("num_field_data");
  meta_name_.push_back("index");
  meta_name_.push_back("lower");
  meta_name_.push_back("upper");
  meta_name_.push_back("cycle");
  meta_name_.push_back("time");
  meta_name_.push_back("dt");

}


//----------------------------------------------------------------------

void IoBlock::meta_value
(int index,
 void ** buffer, std::string * name, scalar_type * type,
 int * nxd, int * nyd, int * nzd) throw()
{
  Io::meta_value(index,buffer,name,type,nxd,nyd,nzd);

  int count = 0;

  if (index == count++) {
    *buffer = (void *) & block_->data()->num_field_data_;
    *type   = scalar_type_int;
  } else if (index == count++) {
    *buffer = (void *) & block_->index_;
    *type   = scalar_type_int;
    *nxd     = 3;
  } else if (index == count++) {
    *buffer = (void *) block_->data()->lower_;
    *type   = scalar_type_double;
    *nxd     = 3;
  } else if (index == count++) {
    *buffer = (void *) block_->data()->upper_;
    *type   = scalar_type_double;
    *nxd     = 3;
  } else if (index == count++) {
    *buffer = (void *) & block_->cycle_;
    *type   = scalar_type_int;
  } else if (index == count++) {
    *buffer = (void *) & block_->time_;
    *type   = scalar_type_double;
  } else if (index == count++) {
    *buffer = (void *) & block_->dt_;
    *type   = scalar_type_double;
  }
}

//----------------------------------------------------------------------

void IoBlock::data_value
(int index,
 void ** buffer, std::string * name, scalar_type * type,
 int * nxd, int * nyd, int * nzd,
 int * nx,  int * ny,  int * nz) throw()
{
}

//----------------------------------------------------------------------
