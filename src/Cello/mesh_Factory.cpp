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
 bool allocate
 ) const throw()
{

  CProxy_CommBlock proxy_block;

  if (allocate) {

#ifdef    PREPARE_AMR

    proxy_block = CProxy_CommBlock::ckNew();

    for (int ix=0; ix<nbx; ix++) {
      for (int iy=0; iy<nby; iy++) {
	for (int iz=0; iz<nbz; iz++) {
	  Index index(ix,iy,iz);
	  index.set_array(ix,iy,iz);
	  index.set_level(0);

	  proxy_block[index].insert 
	    (ix,iy,iz,
	     nbx,nby,nbz,
	     nx,ny,nz,
	     xm,ym,zm, 
	     xb,yb,zb, 
	     num_field_blocks);

	}
      }
    }

    proxy_block.doneInserting();

#else  /* PREPARE_AMR */

    proxy_block = CProxy_CommBlock::ckNew
      (
       nbx,nby,nbz,
       nx,ny,nz,
       xm,ym,zm, 
       xb,yb,zb, 
       num_field_blocks,
       nbx,nby,nbz);
    
#endif /* PREPARE_AMR */

  } else {

    proxy_block = CProxy_CommBlock::ckNew();

  }

  TRACE1("Factor::create_block_array = %p",&proxy_block);
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
 int num_field_blocks
 ) const throw()
{
#if defined (CONFIG_USE_CHARM) && ! defined (PREPARE_AMR)

  CProxy_CommBlock block_array = CProxy_CommBlock::ckNew
    (nbx,nby,nbz,
     nx,ny,nz,
     xm,ym,zm, 
     xb,yb,zb, 
     num_field_blocks,
     nbx,nby,nbz);

  return block_array(ibx,iby,ibz).ckLocal();

#else

  // CProxy_CommBlock proxy_block_reduce = 
  //   CProxy_CommBlock::ckNew()
  return new CommBlock 
    (ibx,iby,ibz, 
     nbx,nby,nbz,
     nx,ny,nz,
     xm,ym,zm, 
     xb,yb,zb, 
     num_field_blocks);

#endif
}

