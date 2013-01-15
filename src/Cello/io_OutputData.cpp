// See LICENSE_CELLO file for license and copyright information

/// @file     io_OutputData.cpp
/// @author   James Bordner (jobordner@ucsd.edu)
/// @date     Thu Mar 17 11:14:18 PDT 2011
/// @brief    Implementation of the OutputData class

#include "cello.hpp"
#include "io.hpp"

//----------------------------------------------------------------------

OutputData::OutputData
(
 int index,
 const Factory * factory,
 Config * config
) throw ()
  : Output(index,factory)
{
  // Set process stride, with default = 1

  int stride = config->output_stride[index_];

  process_stride_ = stride == 0 ? 1 : stride;

}

//----------------------------------------------------------------------

OutputData::~OutputData() throw()
{
  close();
}

//----------------------------------------------------------------------

#ifdef CONFIG_USE_CHARM

void OutputData::pup (PUP::er &p)
{

  TRACEPUP;

  // NOTE: change this function whenever attributes change

  Output::pup(p);

  // this function intentionally left blank
}

#endif

//======================================================================

void OutputData::open () throw()
{
  std::string file_name = expand_file_name_(&file_name_,&file_args_);

  Monitor::instance()->print 
    ("Output","writing data file %s", file_name.c_str());

  close();

  file_ = new FileHdf5 (".",file_name);

  file_->file_create();
}

//----------------------------------------------------------------------

void OutputData::close () throw()
{
  if (file_) file_->file_close();
  delete file_;  file_ = 0;
}

//----------------------------------------------------------------------

void OutputData::finalize () throw ()
{  Output::finalize(); }

//----------------------------------------------------------------------

void OutputData::write_hierarchy
(
 const Hierarchy  * hierarchy,
 const FieldDescr * field_descr
 ) throw()
{
  IoHierarchy io_hierarchy(hierarchy);

  write_meta (&io_hierarchy);

  write_hierarchy_(hierarchy, field_descr);

}

//----------------------------------------------------------------------

void OutputData::write_patch
(
 const Patch * patch,
 const FieldDescr * field_descr,
 int ixp0, int iyp0, int izp0
 ) throw()
{
  // Create file group for patch

  file_->group_chdir("/"+patch->name());
  file_->group_create();

  // Read patch meta-data

  IoPatch io_patch(patch);

  write_meta_group (&io_patch);

  // Also write the patches parallel Layout

  const Layout * layout = patch->layout();
  IoLayout io_layout(layout);

  write_meta_group (&io_layout);

  // Call write(block) on contained blocks
  write_patch_(patch,field_descr,ixp0,iyp0,izp0);

}

//----------------------------------------------------------------------

void OutputData::write_block
( 
  const CommBlock * block,
  const FieldDescr * field_descr,
  int ixp0, int iyp0, int izp0) throw()
{

  // Create file group for block

  std::string group_name = "/" + block->patch_name() + "/" + block->name();
  DEBUG1 ("block name = %s",group_name.c_str());
  file_->group_chdir(group_name);
  file_->group_create();

  // Write block meta data

  io_block()->set_block((CommBlock *)block);

  write_meta_group (io_block());

  // Call write(block) on base Output object

  write_block_(block,field_descr,ixp0,iyp0,izp0);

  file_->group_close();

}

//----------------------------------------------------------------------

void OutputData::write_field_block
( 
  const FieldBlock * field_block,
  const FieldDescr * field_descr,
  int field_index) throw()
{
  io_field_block()->set_field_descr((FieldDescr*)field_descr);
  io_field_block()->set_field_block((FieldBlock*)field_block);
  io_field_block()->set_field_index(field_index);

  for (size_t i=0; i<io_field_block()->data_count(); i++) {

    void * buffer;
    std::string name;
    scalar_type type;
    int nxd,nyd,nzd;  // Array dimension
    int nx,ny,nz;     // Array size

    // Get ith FieldBlock data
    io_field_block()->data_value(i, &buffer, &name, &type, 
				 &nxd,&nyd,&nzd,
				 &nx, &ny, &nz);

    // Write ith FieldBlock data

    file_->data_create(name.c_str(),type,nxd,nyd,nzd,nx,ny,nz);
    file_->data_write(buffer);
    file_->data_close();
  }

}

//======================================================================
