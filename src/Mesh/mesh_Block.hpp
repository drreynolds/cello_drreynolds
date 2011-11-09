// See LICENSE_CELLO file for license and copyright information

/// @file     mesh_Block.hpp
/// @author   James Bordner (jobordner@ucsd.edu)
/// @date     Fri Apr  2 14:09:42 PDT 2010
/// @todo     Move FieldBlock members//attributes to Block when possible
/// @todo     Move size_ to Patch
/// @todo     Move lower_ to Patch, compute from index
/// @todo     Move upper_ to Patch, compute from index
/// @todo     Replace stored data with function calls using Patch pointer
/// @brief    [\ref Mesh] Declaration of the Block class

#ifndef MESH_BLOCK_HPP
#define MESH_BLOCK_HPP

class FieldDescr;
class FieldBlock;
class Patch;

#ifdef CONFIG_USE_CHARM
#include "mesh.decl.h"
class Block : public CBase_Block
#else
class Block
#endif
{
  /// @class    Block
  /// @ingroup  Mesh
  /// @brief    [\ref Mesh] Basic serial block of mesh data

  friend class IoBlock;

public: // interface

  /// create a Block with the given block count, lower PATCH extent, block
  /// size, and number of field blocks
  Block
  (
   int ibx, int iby, int ibz,
   int nbx, int nby, int nbz,
   int nx, int ny, int nz,
   double xmp, double ymp, double zmp,
   double xb, double yb, double zb,
   int num_field_blocks) throw();

#ifdef CONFIG_USE_CHARM

  /// For CHARM Block arrays
  Block
  (
   int nbx, int nby, int nbz,
   int nx, int ny, int nz,
   double xmp, double ymp, double zmp,
   double xb, double yb, double zb,
   int num_field_blocks) throw();

  /// Initialize an empty Block
  Block() { };

  /// Initialize a migrated Block
  Block (CkMigrateMessage *m) { };

  /// Initialize block for the simulation.
  void p_initial();

  /// Apply the numerical methods on the block
  void p_compute(double dt, int axis_set);

  /// Refresh ghost zones and apply boundary conditions
  void p_refresh(int cycle, double time, double dt, int axis_set);

  /// Refresh a FieldFace
  void p_refresh_face(int n, char buffer[], int axis, int face, int axis_set);

  /// Contribute block data to ith output object in the simulation
  void p_write (int index_output);

  //--------------------------------------------------

  /// Output, Monitor, Stopping [reduction], and Timestep [reduction]
  void prepare();

  /// Implementation of refresh
  void refresh(int axis_set);

  /// Boundary and Method
  void compute(int axis_set);

  //==================================================

#else /* ! CONFIG_USE_CHARM */

  /// Refresh ghost data
  void refresh_ghosts(const FieldDescr * field_descr,
		      const Patch * patch,
		      face_enum face,
		      axis_enum axis,
		      int index_field_set = 0) throw();
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

//----------------------------------------------------------------------

  /// Return domain lower extent
  inline void lower(double * x, 
		    double * y = 0,
		    double * z = 0) const throw ()
  {
    if (x) *x = lower_[0];
    if (y) *y = lower_[1];
    if (z) *z = lower_[2];
  }

//----------------------------------------------------------------------

  /// Return domain upper extent
  inline void upper(double * x,
		    double * y = 0,
		    double * z = 0) const throw ()
  {
    if (x) *x = upper_[0];
    if (y) *y = upper_[1];
    if (z) *z = upper_[2];
  }

  /// Return the position of this Block in the containing Patch 
  void index_patch (int * ibx = 0, int * iby = 0, int * ibz = 0) const throw();

  /// Return the index of this Block in the containing Patch 
  int index () const throw();

  /// Return the size the containing Patch
  void size_patch (int * nx, int * ny, int * nz) const throw();

  /// Return the neighboring block in the given direction
  /// [tested in test_Patch]
  Block * neighbor (axis_enum axis, face_enum face) const throw();

  /// Return the current cycle number
  int cycle() const throw() { return cycle_; };

  /// Return the current time
  double time() const throw() { return time_; };
 
  /// Return which block faces lie along the given lower[] and upper[] boundaries
  void is_on_boundary (double lower[3], double upper[3],
		       bool boundary[3][2]) throw();

public: // virtual functions

  virtual void allocate (FieldDescr * field_descr) throw();

  /// Set Block's cycle
  virtual void set_cycle (int cycle) throw()
  { cycle_ = cycle;}

  /// Set Block's time
  virtual void set_time (double time) throw()
  { time_  = time; }

  /// Initialize Block
  virtual void initialize () throw()
  {}

protected: // functions

  /// Allocate and copy in attributes from give Block
  void copy_(const Block & block) throw();

protected: // attributes

  /// Array of field blocks
  std::vector<FieldBlock *> field_block_;

#ifndef CONFIG_USE_CHARM
  /// Index into Patch [redundant with CHARM thisIndex.x .y .z]
  int index_[3];
#endif

  /// Size of Patch
  int size_[3];

  /// Lower extent of the box associated with the block [computable from Patch]
  double lower_[3];

  /// Upper extent of the box associated with the block [computable from Patch]
  double upper_[3];

  //--------------------------------------------------

#ifdef CONFIG_USE_CHARM

  /// Number of field blocks: used for CHARM++ PUP::er
  int num_field_blocks_;

  /// Counter when refreshing 6 faces at once
  int count_refresh_face_;

  /// Counter when refreshing x-, y-, and z-axes sequentially
  int count_refresh_face_x_;
  int count_refresh_face_y_;
  int count_refresh_face_z_;

#endif

  //--------------------------------------------------

  /// Current cycle number
  int cycle_;

  /// Current time
  double time_;

  /// Current timestep
  double dt_;

#ifdef CONFIG_USE_CHARM

public: // CHARM++ PUPer

  /// Pack / unpack the Block in a CHARM++ program
  void pup(PUP::er &p)
  {
    TRACE1("Block::pup() %s", p.isUnpacking() ? "unpacking":"packing" );

    p | num_field_blocks_;

    // allocate field_block_[] vector first if unpacking
    if (p.isUnpacking()) {
      field_block_.resize(num_field_blocks_);
    }

    for (int i=0; i<num_field_blocks_; i++) {
      p | *field_block_[i];
    }

#ifndef CONFIG_USE_CHARM
    // (never called: included for completeness)
    PUParray(p,index_,3);
#endif

    PUParray(p,size_,3);

    PUParray(p,lower_,3);

    PUParray(p,upper_,3);

#ifdef CONFIG_USE_CHARM
    p | count_refresh_face_;
    p | count_refresh_face_x_;
    p | count_refresh_face_y_;
    p | count_refresh_face_z_;
#endif

    p | cycle_;
    p | time_;
    p | dt_;
  }

#endif


};

#endif /* MESH_BLOCK_HPP */

