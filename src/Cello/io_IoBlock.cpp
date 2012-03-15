// See LICENSE_CELLO file for license and copyright information

/// @file     Io_IoBlock.cpp
/// @author   James Bordner (jobordner@ucsd.edu)
/// @date     2011-10-06
/// @brief    Implementation of the IoBlock class

#include "io.hpp"

//----------------------------------------------------------------------

IoBlock::IoBlock() throw ()
  : Io(7,0),
    block_(0)
{
  meta_name_.push_back("size");
  meta_name_.push_back("lower");
  meta_name_.push_back("upper");
  meta_name_.push_back("cycle");
  meta_name_.push_back("time");
  meta_name_.push_back("dt");
  meta_name_.push_back("num_field_blocks");
  meta_name_.push_back("index");
}


//----------------------------------------------------------------------

void IoBlock::meta_value
(int index,
 void ** buffer, std::string * name, enum scalar_type * type,
 int * n0, int * n1, int * n2, int * n3, int * n4) throw()
{
  Io::meta_value(index,buffer,name,type,n0,n1,n2,n3,n4);

  if (index == 0) {

    *buffer = (void *) block_->size_;
    *type   = scalar_type_int;
    *n0     = 3;
    
  } else if (index == 1) {

    *buffer = (void *) block_->lower_;
    *type   = scalar_type_double;
    *n0     = 3;

  } else if (index == 2) {

    *buffer = (void *) block_->upper_;
    *type   = scalar_type_double;
    *n0     = 3;

  } else if (index == 3) {

    *buffer = (void *) & block_->cycle_;
    *type   = scalar_type_int;
    *n0     = 1;

  } else if (index == 4) {

    *buffer = (void *) & block_->time_;
    *type   = scalar_type_double;
    *n0     = 1;

  } else if (index == 5) {

    *buffer = (void *) & block_->dt_;
    *type   = scalar_type_double;
    *n0     = 1;

  } else if (index == 6) {

    *buffer = (void *) & block_->num_field_blocks_;
    *type   = scalar_type_int;
    *n0     = 3;

  } else if (index == 7) {

    *buffer = (void *) & block_->index_;
    *type   = scalar_type_int;
    *n0     = 3;

  }
}

//----------------------------------------------------------------------

void IoBlock::data_value
(int index,
 void ** buffer, std::string * name, enum scalar_type * type,
 int * n0, int * n1, int * n2, int * n3, int * n4) throw()
{
}

//----------------------------------------------------------------------
