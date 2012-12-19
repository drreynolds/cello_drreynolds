// See LICENSE_CELLO file for license and copyright information

/// @file     mesh_Factory.hpp
/// @author   James Bordner (jobordner@ucsd.edu)
/// @date     Tue Mar 15 15:29:56 PDT 2011
/// @brief    [\ref Mesh] Declaration of the Factory class

#include "mesh.hpp"

#ifdef CONFIG_USE_CHARM
extern CProxy_SimulationCharm  proxy_simulation;
#endif

//----------------------------------------------------------------------
Hierarchy * Factory::create_hierarchy (int dimension, int refinement) const throw ()
{
  return new Hierarchy (this,dimension,refinement); 
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
#ifdef CONFIG_USE_CHARM
CProxy_Patch * 
#else
Patch * 
#endif
Factory::create_patch 
(
 const FieldDescr * field_descr,
 int nx,   int ny,  int nz,
 int nx0,  int ny0, int nz0,
 int nbx,  int nby, int nbz,
 double xm, double ym, double zm,
 double xp, double yp, double zp,
 int id,
 bool allocate_blocks,
 int process_first, int process_last_plus
 ) const throw()
{
#ifdef CONFIG_USE_CHARM
  CProxy_Patch * proxy_patch = new CProxy_Patch;
  *proxy_patch = CProxy_Patch::ckNew
    (nx,ny,nz,
     nx0,ny0,nz0,
     nbx,nby,nbz,
     xm,ym,zm,
     xp,yp,zp,
     id,
     allocate_blocks,
     process_first, process_last_plus);
  TRACE1("proxy_patch = %p",proxy_patch);
  return proxy_patch;
#else
  DEBUG1("ID = %d",id);
  return new Patch
    (this,
     field_descr,
     nx,ny,nz,
     nx0,ny0,nz0,
     nbx,nby,nbz,
     xm,ym,zm,
     xp,yp,zp,
     id,
     allocate_blocks,
     process_first, process_last_plus);
#endif
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
#ifdef CONFIG_USE_CHARM

CProxy_Block Factory::create_block_array
(
 int nbx, int nby, int nbz,
 int nx, int ny, int nz,
 double xm, double ym, double zm,
 double xb, double yb, double zb,
 CProxy_Patch proxy_patch,
 int patch_id,
 int patch_rank,
 int num_field_blocks,
 bool allocate
 ) const throw()
{
  DEBUG1("ID = %d",patch_id);
  if (allocate) {
    CProxy_Block * block = new CProxy_Block;
    *block = CProxy_Block::ckNew
      (
       nbx,nby,nbz,
       nx,ny,nz,
       xm,ym,zm, 
       xb,yb,zb, 
       proxy_patch,
       num_field_blocks,
       patch_id,
       patch_rank,
       nbx,nby,nbz);
    DEBUG1 ("block = %p",block);
    return *block;
  } else {
    return CProxy_Block::ckNew();
  }
}

#endif

//----------------------------------------------------------------------
Block * Factory::create_block
(
 int ibx, int iby, int ibz,
 int nbx, int nby, int nbz,
 int nx, int ny, int nz,
 double xm, double ym, double zm,
 double xb, double yb, double zb,
#ifdef CONFIG_USE_CHARM
 CProxy_Patch proxy_patch,
#endif 
 int patch_id,
 int patch_rank,
 int num_field_blocks
 ) const throw()
{
#ifdef CONFIG_USE_CHARM
  DEBUG1("ID = %d",patch_id);
    CProxy_Block block = CProxy_Block::ckNew
    (nbx,nby,nbz,
     nx,ny,nz,
     xm,ym,zm, 
     xb,yb,zb, 
     proxy_patch,
     patch_id,
     patch_rank,
     num_field_blocks,
     nbx,nby,nbz);
  return block(ibx,iby,ibz).ckLocal();
#else
  DEBUG1("ID = %d",patch_id);
  return new Block 
    (ibx,iby,ibz, 
     nbx,nby,nbz,
     nx,ny,nz,
     xm,ym,zm, 
     xb,yb,zb, 
     patch_id,
     patch_rank,
     num_field_blocks);
#endif
}

