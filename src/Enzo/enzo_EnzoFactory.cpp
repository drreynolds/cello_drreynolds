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
CProxy_CommBlock EnzoFactory::create_block_array
(
 int nbx, int nby, int nbz,
 int nx, int ny, int nz,
 double xm, double ym, double zm,
 double hx, double hy, double hz,
#ifdef REMOVE_PATCH
#else /* REMOVE_PATCH */
 CProxy_Patch proxy_patch,
#endif /* REMOVE_PATCH */
 int num_field_blocks,
 bool allocate
 ) const throw()
{
  CProxy_EnzoBlock enzo_block_array;
  if (allocate) {
    enzo_block_array = CProxy_EnzoBlock::ckNew
      (
       nbx,nby,nbz,
       nx,ny,nz,
       xm,ym,zm, 
       hx,hy,hz, 
#ifdef REMOVE_PATCH
#else /* REMOVE_PATCH */
       proxy_patch,
#endif /* REMOVE_PATCH */
       num_field_blocks,
       nbx,nby,nbz);
  } else {
    enzo_block_array = CProxy_EnzoBlock::ckNew();
  }
  TRACE1("EnzoFactory::create_block_array = %p",&enzo_block_array);
  return enzo_block_array;
}

#else

//----------------------------------------------------------------------

CommBlock * EnzoFactory::create_block
(
 int ibx, int iby, int ibz,
 int nbx, int nby, int nbz,
 int nx, int ny, int nz,
 double xm, double ym, double zm,
 double hx, double hy, double hz,
 int num_field_blocks
 ) const throw()
{
#ifdef CONFIG_USE_CHARM
    CProxy_CommBlock block_array = CProxy_EnzoBlock::ckNew
    (nbx,nby,nbz,
     nx,ny,nz,
     xm,ym,zm, 
     xb,yb,zb, 
#ifdef REMOVE_PATCH
#else /* REMOVE_PATCH */
     proxy_patch,
#endif /* REMOVE_PATCH */
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
     num_field_blocks);
#endif
}

#endif


