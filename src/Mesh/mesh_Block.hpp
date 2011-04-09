// $Id$
// See LICENSE_CELLO file for license and copyright information

/// @file     mesh_Block.hpp
/// @author   James Bordner (jobordner@ucsd.edu)
/// @date     Fri Apr  2 14:09:42 PDT 2010
/// @todo     Move FieldBlock members//attributes to Block when possible
/// @todo     Replace stored data with function calls using Patch pointer
/// @brief    [\ref Mesh] Declaration of the Block class

#ifndef MESH_BLOCK_HPP
#define MESH_BLOCK_HPP

class FieldDescr;
class FieldBlock;
class Patch;

#ifdef CONFIG_USE_CHARM
#include "enzo.decl.h"
class Block : public CBase_Block {
#else
class Block {
#endif

  /// @class    Block
  /// @ingroup  Mesh
  /// @brief    [\ref Mesh] Basic serial block of mesh data

public: // interface

#ifdef CONFIG_USE_CHARM
  /// Initialize the Block chare array
  Block
  ( int nx, int ny, int nz,
    double xm, double ym, double zm,
    double hx, double hy, double hz,
    int num_field_blocks) throw();
#else
  /// Initialize the Block object
  Block
  ( int ix, int iy, int iz,
    int nx, int ny, int nz,
    double xm, double ym, double zm,
    double hx, double hy, double hz,
    int num_field_blocks) throw();
#endif

#ifdef CONFIG_USE_CHARM
  /// Initialize an empty Block
  Block() {TRACE("Oops")};

  /// Initialize a migrated Block
  Block (CkMigrateMessage *m) {TRACE("Oops")};
#endif

  //----------------------------------------------------------------------
  // Big Three
  //----------------------------------------------------------------------

  /// Destructor
  virtual ~Block() throw();

  /// Copy constructor
  Block(const Block & block) throw();

  /// Assignment operator
  Block & operator= (const Block & block) throw();

  //----------------------------------------------------------------------

  // /// Set domain lower extent
  // void set_lower(double x, double y, double z) throw ();

  // /// Set domain upper extent
  // void set_upper(double x, double y, double z) throw ();

  //----------------------------------------------------------------------

  /// Return the ith Field block
  const FieldBlock * field_block (int i=0) const throw();

  /// Return the ith Field block
  FieldBlock * field_block (int i=0) throw();

  /// Return domain lower extent
  void lower(double * x = 0, double * y = 0, double * z = 0) const throw ();

  /// Return domain upper extent
  void upper(double * x = 0,  double * y = 0, double * z = 0) const throw ();

  /// Return the index of this Block in the containing Patch 
  /// [tested in test_Patch]
  void index_patch (int * ix, int * iy, int * iz) const throw();

  /// Return the neighboring block in the given direction
  /// [tested in test_Patch]
  Block * neighbor (axis_enum axis, face_enum face) const throw();

  /// Refresh ghost data
  void refresh_ghosts(const FieldDescr * field_descr,
		      int index_field_set = 0) throw();

protected: // functions

  /// Allocate and copy in attributes from give Block
  void copy_(const Block & block) throw();

protected: // attributes

  /// Array of field blocks
  std::vector<FieldBlock *> field_block_;

  /// Index into Patch
  int index_[3];

  /// Extent of the box associated with the block
  /// WARNING: should not be used for deep AMR due to precision /
  /// range issues
  double lower_[3];
  double upper_[3];

};

#endif /* MESH_BLOCK_HPP */

