// See LICENSE_CELLO file for license and copyright information

/// @file     Io_IoFieldBlock.cpp
/// @author   James Bordner (jobordner@ucsd.edu)
/// @date     2011-10-06
/// @brief    Implementation of the IoFieldBlock class

#include "io.hpp"

//----------------------------------------------------------------------

IoFieldBlock::IoFieldBlock() throw ()
  : Io(4,1),
    field_descr_(0),
    field_block_(0),
    field_index_(0)

{
  meta_name_.push_back("size");
  meta_name_.push_back("array_size");
  meta_name_.push_back("num_fields");
  meta_name_.push_back("ghosts_allocated");

  ASSERT2("IoFieldBlock::IoFieldBlock()",
	 "meta_name.size() [%d] !=  meta_count_ [%d]",
	  meta_name_.size(),meta_count(),
	  meta_name_.size()==meta_count());
}


//----------------------------------------------------------------------

void IoFieldBlock::meta_value
(int index,
 void ** buffer, std::string * name, enum scalar_type * type,
 int * n0, int * n1, int * n2, int * n3, int * n4) throw()
{
}

//----------------------------------------------------------------------

void IoFieldBlock::data_value
(int index,
 void ** buffer, std::string * name, enum scalar_type * type,
 int * n0, int * n1, int * n2, int * n3, int * n4) throw()
{
  if (buffer) (*buffer) = (void * )field_block_->field_values(field_index_);
  if (name)   (*name)   = field_descr_->field_name(field_index_);
  if (type) {

    precision_enum precision = field_descr_->precision(field_index_);
    if (precision == precision_default) precision = default_precision;

    switch (precision) {
    case precision_single: (*type) = scalar_type_float; break;
    case precision_double: (*type) = scalar_type_double; break;
    default:
      ERROR2 ("IoFieldBlock",
	      "Unsupported precision type %d for field %s",
	      precision, field_descr_->field_name(field_index_).c_str());
    }
  }
  int nbx,nby,nbz;
  field_block_->size(&nbx,&nby,&nbz);

  int ngx=0,ngy=0,ngz=0;

  // CURRENTLY MUST INCLUDE GHOSTS IN DATA OUTPUT
  field_descr_->ghosts(field_index_,&ngx,&ngy,&ngz);

   if (n0) (*n0) = nbx + 2*ngx;
   if (n1) (*n1) = nby + 2*ngy;
   if (n2) (*n2) = nbz + 2*ngz;

}

//----------------------------------------------------------------------
