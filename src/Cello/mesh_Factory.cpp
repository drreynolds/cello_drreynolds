// See LICENSE_CELLO file for license and copyright information

/// @file     mesh_Factory.hpp
/// @author   James Bordner (jobordner@ucsd.edu)
/// @date     Tue Mar 15 15:29:56 PDT 2011
/// @brief    [\ref Mesh] Declaration of the Factory class

#include "mesh.hpp"

//----------------------------------------------------------------------

Hierarchy * Factory::create_hierarchy (int dimension, int refinement,
				       int process_first, int process_last_plus) const throw ()
{
  return new Hierarchy (this,dimension,refinement,process_first, process_last_plus); 
}

//----------------------------------------------------------------------

#ifdef CONFIG_USE_CHARM

void Factory::pup (PUP::er &p)

{
  TRACEPUP;

  PUP::able::pup(p);

  // NOTE: change this function whenever attributes change
}

#endif

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
#ifdef CONFIG_USE_CHARM

CProxy_CommBlock Factory::create_block_array
(
 int nbx, int nby, int nbz,
 int nx, int ny, int nz,
 double xm, double ym, double zm,
 double xb, double yb, double zb,
 int num_field_blocks,
 bool allocate,
 bool testing
 ) const throw()
{

  CProxy_CommBlock proxy_block;

  TRACE("Factory::create_block_array");
  if (allocate) {

    CProxy_ArrayMap array_map  = CProxy_ArrayMap::ckNew(nbx,nby,nbz);
    CkArrayOptions opts;
    opts.setMap(array_map);
    proxy_block = CProxy_CommBlock::ckNew(opts);

    int level;

    for (int ix=0; ix<nbx; ix++) {
      for (int iy=0; iy<nby; iy++) {
	for (int iz=0; iz<nbz; iz++) {

	  Index index(ix,iy,iz);
	  index.set_level(0);
	  index.clear();
	  TRACE3("Inserting %d %d %d",ix,iy,iz);
	  proxy_block[index].insert 
	    (ix,iy,iz,
	     nbx,nby,nbz,
	     nx,ny,nz,
	     level = 0,
	     xm,ym,zm, 
	     xb,yb,zb, 
	     num_field_blocks,
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

#endif

//----------------------------------------------------------------------
CommBlock * Factory::create_block
(
 int ibx, int iby, int ibz,
 int nbx, int nby, int nbz,
 int nx, int ny, int nz,
 double xm, double ym, double zm,
 double xb, double yb, double zb,
 int num_field_blocks,
 bool testing
 ) const throw()
{
#ifdef CONFIG_USE_CHARM

  CProxy_CommBlock proxy_block = CProxy_CommBlock::ckNew();
  Index index(0,0,0);
  index.set_level(0);
  index.clear();

  int level;

  proxy_block[index].insert
    (0,0,0,
     nbx,nby,nbz,
     nx,ny,nz,
     level=0,
     xm,ym,zm, 
     xb,yb,zb, 
     num_field_blocks,
     testing);

  proxy_block.doneInserting();

  CommBlock * block = proxy_block[index].ckLocal();
  //  ASSERT("Factory::create_block()","block is NULL",block != NULL);

  return block;

#else /* CONFIG_USE_CHARM */

  return new CommBlock 
    (ibx,iby,ibz, 
     nbx,nby,nbz,
     nx,ny,nz,
     level=0,
     xm,ym,zm, 
     xb,yb,zb, 
     num_field_blocks,
     testing);

#endif /* CONFIG_USE_CHARM */
}

