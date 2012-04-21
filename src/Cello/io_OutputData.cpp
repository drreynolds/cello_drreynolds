// See LICENSE_CELLO file for license and copyright information

/// @file     io_OutputData.cpp
/// @author   James Bordner (jobordner@ucsd.edu)
/// @date     Thu Mar 17 11:14:18 PDT 2011
/// @brief    Implementation of the OutputData class

#include "cello.hpp"
#include "io.hpp"

//----------------------------------------------------------------------

OutputData::OutputData(const Factory * factory) throw ()
  : Output(factory)
{
}

//----------------------------------------------------------------------

OutputData::~OutputData() throw()
{
  close();
}

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

bool OutputData::is_open () throw()
{  return (file_ != 0); }

//----------------------------------------------------------------------

void OutputData::finalize () throw ()
{  Output::finalize(); }

//----------------------------------------------------------------------

void OutputData::write
(
 const Hierarchy  * hierarchy,
 const FieldDescr * field_descr
 ) throw()
{
  IoHierarchy io_hierarchy(hierarchy);

  Output::write_meta (&io_hierarchy);

  Output::write (hierarchy, field_descr);

}

//----------------------------------------------------------------------

void OutputData::write
(
 const Patch * patch,
 const FieldDescr * field_descr,
 int ixp0, int iyp0, int izp0
 ) throw()
{
  // Create file group for patch

// #ifdef CONFIG_USE_CHARM
//   DEBUG1(" patch_proxy = %p",patch); 
//   const Patch * patch_local = ((const CProxy_Patch *)patch)->ckLocal();
// #else
  const Patch * patch_local = patch;
// #endif

  DEBUG1("patch id = %d",patch_local->id());
  int ib = patch_local->id();
  char buffer[40];
  sprintf (buffer,"patch_%d",ib);
  file_->group_chdir(buffer);
  file_->group_create();

  // Read patch meta-data

  IoPatch io_patch(patch);

  Output::write_meta_group (&io_patch);

  // Also write the patches parallel Layout

  DEBUG0;
  const Layout * layout = patch_local->layout();
  IoLayout io_layout(layout);

  Output::write_meta_group (&io_layout);
  DEBUG0;

  // Call write(block) on contained blocks
  Output::write(patch,field_descr,ixp0,iyp0,izp0);
  DEBUG0;

}

//----------------------------------------------------------------------

void OutputData::write
( 
  const Block * block,
  const FieldDescr * field_descr,
  int ixp0, int iyp0, int izp0) throw()
{

  DEBUG("OutputData::write(block)");
  // Create file group for block

  char buffer[40];
  int ib = block->index();
  sprintf (buffer,"block_%d",ib);
  file_->group_chdir(buffer);
  file_->group_create();

  // Write block meta data

  io_block()->set_block(block);

  Output::write_meta_group (io_block());

  // Call write(block) on base Output object

  Output::write(block,field_descr,ixp0,iyp0,izp0);

  file_->group_close();
  file_->group_chdir("..");

}

//----------------------------------------------------------------------

void OutputData::write
( 
  const FieldBlock * field_block,
  const FieldDescr * field_descr,
  int field_index) throw()
{
  io_field_block()->set_field_descr(field_descr);
  io_field_block()->set_field_block(field_block);
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
