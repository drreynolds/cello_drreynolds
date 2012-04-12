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
class Patch;

class Factory {

  /// @class    Factory
  /// @ingroup  Mesh 
  /// @brief [\ref Mesh] Abstract class for creating concrete Hierarchy,
  /// Patch, and Block objects

public: // interface

  Factory() throw()
  {}
 
  /// Destructor (must be present to avoid possible vtable link errors)
  virtual ~Factory() throw()
  {}

  /// Create a new Hierarchy [abstract factory design pattern]
  virtual Hierarchy * create_hierarchy (int dimension, int refinement) const throw ();

  /// Create a new Patch [abstract factory design pattern]
#ifdef CONFIG_USE_CHARM
  virtual CProxy_Patch create_patch 
#else
  virtual Patch *      create_patch 
#endif
  (
   const FieldDescr * field_descr,
   int nx,   int ny,  int nz,
   int nx0,  int ny0, int nz0,
   int nbx,  int nby, int nbz,
   double xm, double ym, double zm,
   double xp, double yp, double zp,
   bool allocate_blocks = true,
   int process_first=0, int process_last_plus=-1
   ) const throw();

  /// Create an Input / Output accessor object for Block
  virtual IoBlock * create_io_block ( ) const throw();

  /// Create an Input / Output accessor object for a FieldBlock
  virtual IoFieldBlock * create_io_field_block ( ) const throw();

#ifdef CONFIG_USE_CHARM

  /// Create a new CHARM++ Block array
  virtual CProxy_Block create_block_array
  (int nbx, int nby, int nbz,
   int nx, int ny, int nz,
   double xm, double ym, double zm,
   double hx, double hy, double hz,
   CProxy_Patch proxy_patch,
   int num_field_blocks = 1,
   bool allocate = true) const throw();

  /// Pack / unpack the Factory in a CHARM++ program
  void pup(PUP::er &p) {};

#endif

  /// Create a new Block
  virtual Block * create_block
  (int ibx, int iby, int ibz,
   int nbx, int nby, int nbz,
   int nx, int ny, int nz,
   double xm, double ym, double zm,
   double xb, double yb, double zb,
#ifdef CONFIG_USE_CHARM
   CProxy_Patch proxy_patch,
#endif
   int num_field_blocks = 1) const throw();

};

#endif /* MESH_FACTORY_HPP */

