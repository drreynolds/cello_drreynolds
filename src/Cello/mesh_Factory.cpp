// See LICENSE_CELLO file for license and copyright information

/// @file     mesh_Factory.hpp
/// @author   James Bordner (jobordner@ucsd.edu)
/// @date     Tue Mar 15 15:29:56 PDT 2011
/// @brief    [\ref Mesh] Declaration of the Factory class

#include "mesh.hpp"

//----------------------------------------------------------------------

Hierarchy * Factory::create_hierarchy 
(
 int dimension, int refinement,
 int process_first, int process_last_plus) const throw ()
{
  return new Hierarchy 
    (this,dimension,refinement,process_first, process_last_plus); 
}

//----------------------------------------------------------------------

void Factory::pup (PUP::er &p)

{
  TRACEPUP;

  PUP::able::pup(p);

  // NOTE: change this function whenever attributes change
}

//----------------------------------------------------------------------

IoBlock * Factory::create_io_block () const throw()
{
  return new IoBlock;
}

//----------------------------------------------------------------------

IoFieldBlock * Factory::create_io_field_block () const throw()
{
  return new IoFieldBlock;

}

//----------------------------------------------------------------------

CProxy_CommBlock Factory::create_block_array
(
 int nbx, int nby, int nbz,
 int nx, int ny, int nz,
 int num_field_blocks,
 bool allocate,
 bool testing
 ) const throw()
{
  TRACE8("Factory::create_block_array(na(%d %d %d) n(%d %d %d num_field_blocks %d  allocate %d",
	 nbx,nby,nbz,nx,ny,nz,num_field_blocks,allocate);

  CProxy_CommBlock proxy_block;

  if (allocate) {

    CProxy_ArrayMap array_map  = CProxy_ArrayMap::ckNew(nbx,nby,nbz);
    CkArrayOptions opts;
    opts.setMap(array_map);
    proxy_block = CProxy_CommBlock::ckNew(opts);

    int count_adapt;
    bool initial;

    int    cycle = 0;
    double time  = 0.0;
    double dt    = 0.0;
    int num_face_level = 0;
    int * face_level = 0;

    for (int ix=0; ix<nbx; ix++) {
      for (int iy=0; iy<nby; iy++) {
	for (int iz=0; iz<nbz; iz++) {

	  Index index(ix,iy,iz);

	  proxy_block[index].insert 
	    (index,
	     nx,ny,nz,
	     num_field_blocks,
	     count_adapt = 0,
	     initial = true,
	     cycle,time,dt,
	     0,NULL,op_array_copy,
	     num_face_level, face_level,
	     testing);

	}
      }
    }

    proxy_block.doneInserting();

  } else {

    proxy_block = CProxy_CommBlock::ckNew();

  }

  TRACE1("Factory::create_block_array = %p",&proxy_block);
  return proxy_block;
}

//----------------------------------------------------------------------
CommBlock * Factory::create_block
(
 CProxy_CommBlock * block_array,
 Index index,
 int nx, int ny, int nz,
 int num_field_blocks,
 int count_adapt,
 bool initial,
 int cycle, double time, double dt,
 int narray, char * array, int op_array,
 int num_face_level, int * face_level,
 bool testing
 ) const throw()
{

  TRACE6("Factory::create_block(n(%d %d %d)  num_field_blocks %d  count_adatp %d  initial %d)",
	 nx,ny,nz,num_field_blocks,count_adapt,initial);

  (*block_array)[index].insert
    (
     index,
     nx,ny,nz,
     num_field_blocks,
     count_adapt,
     initial,
     cycle, time,dt,
     narray, array,op_array,
     num_face_level, face_level,
     testing);

  CommBlock * block = (*block_array)[index].ckLocal();

  ASSERT("Factory::create_block()","block is NULL",block != NULL);

  return block;

}

