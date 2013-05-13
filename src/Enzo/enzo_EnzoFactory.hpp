// See LICENSE_CELLO file for license and copyright information

/// @file     enzo_EnzoFactory.hpp
/// @author   James Bordner (jobordner@ucsd.edu)
/// @date     Tue Mar 15 15:29:56 PDT 2011
/// @brief    [\ref Enzo] Declaration of the EnzoFactory class

#ifndef ENZO_ENZO_FACTORY_HPP
#define ENZO_ENZO_FACTORY_HPP

class EnzoFactory : public Factory {

  /// @class    EnzoFactory
  /// @ingroup  Enzo
  /// @brief [\ref Enzo] Abstract class for creating concrete Hierarchy,
  /// Patch, and CommBlock objects

public: // interface

  /// CHARM++ constructor
  EnzoFactory() throw() 
  : Factory()
  { TRACE ("EnzoFactory::EnzoFactory()"); }

#ifdef CONFIG_USE_CHARM

  PUPable_decl(EnzoFactory);

  EnzoFactory(CkMigrateMessage *m) : Factory (m) {}

  /// CHARM++ Pack / Unpack function
  virtual void pup (PUP::er &p);

#endif

  /// Create the Input / Output accessor object for EnzoBlock
  virtual IoBlock * create_io_block () const throw();

#ifdef CONFIG_USE_CHARM
  /// Create a new CHARM++ CommBlock array [abstract factory design pattern]
  virtual CProxy_CommBlock create_block_array
  (int nbx, int nby, int nbz,
   int nx, int ny, int nz,
   int num_field_blocks,
   bool allocate,
   bool testing=false) const throw();
#endif

  /// Create a new CommBlock  [abstract factory design pattern]
  virtual CommBlock * create_block
  (
#ifdef CONFIG_USE_CHARM
   CProxy_CommBlock * block_array,
#else
   Simulation * simulation,
#endif /* CONFIG_USE_CHARM */
   Index index,
   int nx, int ny, int nz,
   int num_field_blocks,
   int count_adapt,
   bool initial,
   bool testing=false) const throw();

};

#endif /* ENZO_ENZO_FACTORY_HPP */

