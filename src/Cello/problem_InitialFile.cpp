// See LICENSE_CELLO file for license and copyright information

/// @file     method_InitialFile.cpp
/// @author   James Bordner (jobordner@ucsd.edu)
/// @date     2011-02-16
/// @brief    Implementation of the InitialFile class

#include "cello.hpp"

#include "problem.hpp"

//----------------------------------------------------------------------

InitialFile::InitialFile
(Parameters * parameters,
 GroupProcess * group_process,
 int cycle, double time) throw ()
  : Initial (cycle,time),
    parameters_(parameters),
    group_process_(group_process),
    input_(0)
{
  TRACE("InitialFile::InitialFile");
}

//----------------------------------------------------------------------

InitialFile::~InitialFile() throw()
{
  delete input_; input_ = 0;
}

//----------------------------------------------------------------------

void InitialFile::enforce
(
 const Hierarchy  * hierarchy,
 const FieldDescr * field_descr,
 Block            * block
 ) throw()
{
  if (! input_) input_ = new InputData(hierarchy->factory());

  if (! input_->is_open() ) {

    input_->is_scheduled (cycle_,time_);

    std::string              file_name = "";
    std::vector<std::string> file_args;

    get_filename_(&file_name,&file_args);

    input_->open();

  }

  INCOMPLETE("InitialFile::enforce");


  Patch * patch = input_->read_patch(0,field_descr,0,0,0);

  File * file = input_->file();
  int num_blocks = file->group_count();
  for (int i = 0; i<num_blocks; i++) {
    //    Block * block = input_->read_block(0,
    TRACE2("  block(%d) = %s",i,file->group_name(i).c_str());
  }
}

//----------------------------------------------------------------------

void InitialFile::get_filename_
(
 std::string * file_name,
 std::vector<std::string> * file_args
 ) throw()
{
  // parameter: Initial : name

  parameters_->group_set(0,"Initial");

  if (parameters_->type("name") == parameter_string) {

    *file_name = parameters_->value_string("name","");
     
  } else if (parameters_->type("name") == parameter_list) {

    int list_length = parameters_->list_length("name");

    *file_name = parameters_->list_value_string(0,"name","");

    for (int index = 1; index<list_length; index++) {
      file_args->push_back(parameters_->list_value_string(index,"name",""));
    }

  } else {

    ERROR1("InitialFile::enforce",
	   "Bad type %d for 'Initial : name' parameter",
	   parameters_->type("name"));

  }

  TRACE("Setting file_name,file_args");

  input_->set_filename (*file_name, *file_args);

}
