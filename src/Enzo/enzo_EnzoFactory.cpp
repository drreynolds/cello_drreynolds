// See LICENSE_CELLO file for license and copyright information

/// @file     enzo_EnzoFactory.hpp
/// @author   James Bordner (jobordner@ucsd.edu)
/// @date     Tue Mar 15 15:29:56 PDT 2011
/// @brief    [\ref Enzo] Declaration of the EnzoFactory class

#include "enzo.hpp"

#include "charm_enzo.hpp"

//----------------------------------------------------------------------

#ifdef CONFIG_USE_CHARM

void EnzoFactory::pup (PUP::er &p)
{
  // NOTE: change this function whenever attributes change

  TRACE0;
  TRACEPUP;

  Factory::pup(p);

}

#endif

//----------------------------------------------------------------------

IoBlock * EnzoFactory::create_io_block () const throw()
{
  return new IoEnzoBlock;
}

//----------------------------------------------------------------------

#ifdef CONFIG_USE_CHARM
CProxy_Block EnzoFactory::create_block_array
(
 int nbx, int nby, int nbz,
 int nx, int ny, int nz,
 double xm, double ym, double zm,
 double hx, double hy, double hz,
 CProxy_Patch proxy_patch,
 int patch_id,
 int patch_rank,
 int num_field_blocks,
 bool allocate
 ) const throw()
{
  if (allocate) {
    return CProxy_EnzoBlock::ckNew
      (
       nbx,nby,nbz,
       nx,ny,nz,
       xm,ym,zm, 
       hx,hy,hz, 
       proxy_patch,
       patch_id,
       patch_rank,
       num_field_blocks,
       nbx,nby,nbz);
  } else {
    return CProxy_EnzoBlock::ckNew();
  }
}

#else

//----------------------------------------------------------------------

Block * EnzoFactory::create_block
(
 int ibx, int iby, int ibz,
 int nbx, int nby, int nbz,
 int nx, int ny, int nz,
 double xm, double ym, double zm,
 double hx, double hy, double hz,
 int patch_id,
 int patch_rank,
 int num_field_blocks
 ) const throw()
{
#ifdef CONFIG_USE_CHARM
    CProxy_Block block_array = CProxy_EnzoBlock::ckNew
    (nbx,nby,nbz,
     nx,ny,nz,
     xm,ym,zm, 
     xb,yb,zb, 
     proxy_patch,
     patch_id,
     patch_rank,
     num_field_blocks,
     nbx,nby,nbz);
  return block_array(ibx,iby,ibz).ckLocal();
#else
  return new EnzoBlock 
    (
     ibx,iby,ibz, 
     nbx,nby,nbz,
     nx,ny,nz,
     xm,ym,zm, 
     hx,hy,hz, 
     patch_id,
     patch_rank,
     num_field_blocks);
#endif
}

#endif


