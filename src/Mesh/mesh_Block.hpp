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
class Block : public CBase_Block
#else
class Block
#endif
{
  /// @class    Block
  /// @ingroup  Mesh
  /// @brief    [\ref Mesh] Basic serial block of mesh data

public: // interface

  /// create a Block with the given block count, lower PATCH extent, block
  /// size, and number of field blocks
  Block
  (
#ifndef CONFIG_USE_CHARM
   int ibx, int iby, int ibz,
#endif
   int nbx, int nby, int nbz,
   int nx, int ny, int nz,
   double xmp, double ymp, double zmp,
   double xb, double yb, double zb,
   int num_field_blocks) throw();

#ifdef CONFIG_USE_CHARM
  /// Initialize an empty Block
  Block() {TRACE("Block()")};

  /// Initialize a migrated Block
  Block (CkMigrateMessage *m) {TRACE("Block(msg)")};

  //==================================================

  /// Initialize block for the simulation.
  void p_initial();

  /// Refresh ghost zones and apply boundary conditions
  void p_refresh(double dt);

  /// Refresh a FieldFace
  void p_refresh_face(int n, char buffer[], int axis, int face);

  //--------------------------------------------------

  /// Contribute block data to ith output object in the simulation
  void p_output_accum (int index_output);

  //--------------------------------------------------

  /// Output, Monitor, Stopping [reduction], and Timestep [reduction]
  void prepare();

  /// Boundary and Method
  void compute();
  //==================================================

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

  /// Return the size the containing Patch
  void size_patch (int * nx, int * ny, int * nz) const throw();

  /// Return the neighboring block in the given direction
  /// [tested in test_Patch]
  Block * neighbor (axis_enum axis, face_enum face) const throw();

  /// Refresh ghost data
  void refresh_ghosts(const FieldDescr * field_descr,
		      int index_field_set = 0) throw();

  /// Return the current cycle number
  int cycle() const throw() { return cycle_; };

  /// Return the current time
  double time() const throw() { return time_; };

  /// Call application [i.e. Enzo] specific initialization
  virtual void initialize (int cycle_start, double time_start) throw()
  { 
    time_ = time_start;
    cycle_ = cycle_start;
  }

protected: // functions

  /// Allocate and copy in attributes from give Block
  void copy_(const Block & block) throw();

protected: // attributes

  /// Array of field blocks
  std::vector<FieldBlock *> field_block_;

  /// Index into Patch [redundant with CHARM thisIndex.x .y .z]
  int index_[3];

  /// Size of Patch
  int size_[3];

  /// Lower extent of the box associated with the block [computable from Patch]
  double lower_[3];

  /// Upper extent of the box associated with the block [computable from Patch]
  double upper_[3];

  //--------------------------------------------------

#ifdef CONFIG_USE_CHARM
  int count_refresh_face_;
#endif

  //--------------------------------------------------

  /// Current cycle number
  int cycle_;

  /// Current time
  double time_;

  /// Current timestep
  double dt_;

};

#endif /* MESH_BLOCK_HPP */

