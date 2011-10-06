// See LICENSE_CELLO file for license and copyright information

/// @file     io_OutputData.cpp
/// @author   James Bordner (jobordner@ucsd.edu)
/// @date     Thu Mar 17 11:14:18 PDT 2011
/// @todo     Move write_hierarchy() call to write_patch() etc. to parent
///           Output::write_hierarchy()
/// @brief    Implementation of the OutputData class

#include "cello.hpp"

#include "io.hpp"

//----------------------------------------------------------------------

OutputData::OutputData(Simulation * simulation) throw ()
  : Output(simulation)
{
}

//----------------------------------------------------------------------

OutputData::~OutputData() throw ()
{
}

//======================================================================

void OutputData::init () throw()
{
}

//----------------------------------------------------------------------

void OutputData::open () throw()
{
  std::string file_name = expand_file_name();

  Monitor::instance()->print ("Output","writing data file %s", 
			      file_name.c_str());

  delete file_;

  file_ = new FileHdf5 (".",file_name);

  file_->file_create();
}

//----------------------------------------------------------------------

void OutputData::close () throw()
{
  file_->file_close();

  delete file_;
  file_ = 0;
}

//----------------------------------------------------------------------

void OutputData::finalize () throw ()
{
  Output::finalize();
}

//----------------------------------------------------------------------

void OutputData::write_hierarchy 
(
 const FieldDescr * field_descr,
 Hierarchy * hierarchy
 ) throw()
{

  // Loop over metadata items in Hierarchy

  IoHierarchy io_hierarchy(hierarchy);

  for (int i=0; i<io_hierarchy.meta_count(); i++) {

    const char * name;
    scalar_type type;
    void * buffer;
    int i0,i1,i2,i3,i4;

    // Get ith Hierarchy metadata
    io_hierarchy.meta_value(i,& buffer, &name, &type, &i0,&i1,&i2,&i3,&i4);

    // Write ith Hierarchy metadata
    file_->file_write_meta(buffer,name,type,i0,i1,i2,i3,i4);
  }

  // Call write_patch() on component patches
  Output::write_hierarchy (field_descr,hierarchy);

}

//----------------------------------------------------------------------

void OutputData::write_patch 
(
 const FieldDescr * field_descr,
 Patch * patch,
 int ixp0, int iyp0, int izp0
 ) throw()
{

  // Create file group for patch

  char buffer[40];
  int ib = patch->index();
  sprintf (buffer,"/patch_%d",ib);
  file_->group_create(buffer);

  // Loop over metadata items in Hierarchy

  IoPatch io_patch(patch);

  for (int i=0; i<io_patch.meta_count(); i++) {

    void * buffer;
    const char * name;
    scalar_type type;
    int i0,i1,i2,i3,i4;

    // Get ith Patch metadata
    io_patch.meta_value(i,& buffer, &name, &type, &i0,&i1,&i2,&i3,&i4);

    // Write ith Patch metadata
    file_->group_write_meta(buffer,name,type,i0,i1,i2,i3,i4);
  }

  // Call write_block() on component blocks

  Output::write_patch(field_descr,patch,ixp0,iyp0,izp0);

  file_->group_close();

}

//----------------------------------------------------------------------

void OutputData::write_block ( const FieldDescr * field_descr,
  Block * block,
  int ixp0, int iyp0, int izp0) throw()
{
  // Create file group for block

  char buffer[40];
  int ib = block->index();
  sprintf (buffer,"/block_%d",ib);
  file_->group_create(buffer);

  // Write block meta data

  IoBlock io_block(block);

  for (int i=0; i<io_block.meta_count(); i++) {

    void * buffer;
    const char * name;
    scalar_type type;
    int i0,i1,i2,i3,i4;

    // Get ith Block metadata
    io_block.meta_value(i,& buffer, &name, &type, &i0,&i1,&i2,&i3,&i4);

    // Write ith Block metadata
    file_->group_write_meta(buffer,name,type,i0,i1,i2,i3,i4);
  }

  file_->group_close();

}

//----------------------------------------------------------------------

void OutputData::prepare_remote (int * n, char ** buffer) throw()
{
  TRACE("OutputData::prepare_remote ()");
}

//----------------------------------------------------------------------

void OutputData::update_remote  ( int n, char * buffer) throw()
{
  TRACE("OutputData::update_remote  ()");
}

//----------------------------------------------------------------------


void OutputData::cleanup_remote (int * n, char ** buffer) throw()
{
  TRACE("OutputData::cleanup_remote ()");
}

//======================================================================
