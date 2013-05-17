// See LICENSE_CELLO file for license and copyright information

/// @file     mesh_Factory.hpp
/// @author   James Bordner (jobordner@ucsd.edu)
/// @date     Tue Mar 15 15:29:56 PDT 2011
/// @brief    [\ref Mesh] Declaration of the Factory class

#ifndef MESH_FACTORY_HPP
#define MESH_FACTORY_HPP

class GroupProcess;
class Hierarchy;
class IoBlock;
class IoFieldBlock;

class Factory
#ifdef CONFIG_USE_CHARM
  : public PUP::able 
#endif
{
  /// @class    Factory
  /// @ingroup  Mesh 
  /// @brief [\ref Mesh] Abstract class for creating concrete Hierarchy,
  /// Patch, and CommBlock objects

public: // interface

#ifdef CONFIG_USE_CHARM
  Factory() throw() : PUP::able()
  { TRACE("Factory::Factory()"); }
#endif
 
  /// Destructor (must be present to avoid possible vtable link errors)
  virtual ~Factory() throw() { }

#ifdef CONFIG_USE_CHARM

  /// CHARM++ function for determining the specific class in the class hierarchy
  PUPable_decl(Factory);

  /// CHARM++ migration constructor for PUP::able

  Factory (CkMigrateMessage *m) : PUP::able(m) 
  { TRACE("Factory::Factory(CkMigrateMessage*)"); }

  /// CHARM++ Pack / Unpack function
  virtual void pup (PUP::er &p);

#endif


  /// Create a new Hierarchy [abstract factory design pattern]
  virtual Hierarchy * create_hierarchy 
  (
#ifndef CONFIG_USE_CHARM
   Simulation * simulation,
#endif
   int dimension, int refinement,
   int process_first, int process_last_plus) const throw ();

  /// Create an Input / Output accessor object for CommBlock
  virtual IoBlock * create_io_block ( ) const throw();

  /// Create an Input / Output accessor object for a FieldBlock
  virtual IoFieldBlock * create_io_field_block ( ) const throw();

#ifdef CONFIG_USE_CHARM

  /// Create a new CHARM++ CommBlock array
  virtual CProxy_CommBlock create_block_array
  (
   int nbx, int nby, int nbz,
   int nx, int ny, int nz,
   int num_field_blocks,
   bool allocate,
   bool testing = false) const throw();

#endif

  /// Create a new CommBlock
  virtual CommBlock * create_block
  (
#ifdef CONFIG_USE_CHARM
   CProxy_CommBlock * block_array,
#else
   Simulation * simulation,
#endif
   Index index,
   int nx, int ny, int nz,
   int num_field_blocks,
   int count_adapt,
   bool initial,
   int narray,
   char * array,
   bool testing = false) const throw();

};

#endif /* MESH_FACTORY_HPP */

