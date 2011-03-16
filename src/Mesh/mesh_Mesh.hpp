// $Id$
// See LICENSE_CELLO file for license and copyright information

/// @file     mesh_Mesh.hpp
/// @author   James Bordner (jobordner@ucsd.edu)
/// @date     Tue Nov 10 15:38:40 PST 2009
/// @brief    [\ref Mesh] Declaration of the Mesh class

#ifndef MESH_MESH_HPP
#define MESH_MESH_HPP

class Factory;

class Mesh {

  /// @class    Mesh
  /// @ingroup  Mesh
  /// @brief    [\ref Mesh] Adaptive mesh refinement hierarchy

public: // interface

  /// Initialize an Mesh object
  Mesh(Factory * factory,
       GroupProcess * group_process,
       int nx,  int ny,  int nz,
       int nbx, int nby, int nbz) throw ();

  /// Delete the Mesh object
  virtual ~Mesh() throw ();

  //----------------------------------------------------------------------

  /// Return dimension
  int dimension() const throw ();

  /// Set dimension
  void set_dimension(int dimension) throw ();

  /// Return domain lower extent
  void lower(double * nx = 0, double * ny = 0, double * nz = 0) const throw ();

  /// Set domain lower extent
  void set_lower(double nx, double ny, double nz) throw ();

  /// Return domain upper extent
  void upper(double * nx = 0, double * ny = 0, double * nz = 0) const throw ();

  /// Set domain upper extent
  void set_upper(double nx, double ny, double nz) throw ();
  
  /// Return max_level
  int max_level() const throw ();

  /// Set max_level
  void set_max_level(int max_level) throw ();

  /// Return refinement factor
  int refine_factor() const throw ();

  /// Set refinement factor
  void set_refine_factor(int refine) throw ();

  /// Return root_size
  void root_size(int * nx, int * ny, int * nz) const throw ();

  /// Pointer to the root Patch
  Patch * root_patch() throw ();

  /// Return the total number of local patches
  size_t num_patches() const throw();

  /// Return the ith patch
  Patch * patch(size_t i) throw();

  /// Return the ith patch
  Patch * patch(size_t i) const throw();

  /// Insert the given Patch into the list of patches
  virtual void insert_patch(Patch *) throw();

  /// Return whether to avoid level jumps
  bool balanced() const throw ();

  /// Set whether to avoid level jumps
  void set_balanced(bool balanced) throw ();

  /// Return whether to backfill levels
  bool backfill() const throw ();

  /// Set whether to backfill levels
  void set_backfill(bool backfill) throw ();

  /// Return whether to coalesce patches
  bool coalesce() const throw ();

  /// Set whether to coalesce patches
  void set_coalesce(bool coalesce) throw ();

  /// Return the GroupProcess associated with the mesh
  GroupProcess * group() const throw();

  /// Return the factory object associated with the Mesh
  Factory * factory () const throw()
  { return factory_; }

protected: // attributes

  /// Factory for creating Simulations, Meshes, Patches and Blocks [abstract factory design pattern]
  Factory * factory_;

  /// Parallel Group for distributing the Mesh across processors
  GroupProcess * group_process_;

  /// List of local patchs

  std::vector<Patch *> patch_list_;

  /// Tree defining the MESH hierarchy topology
  //  strict_auto_ptr<TreeK> tree_;
  TreeK * tree_;

  /// Spacial dimensions of the Mesh: 1, 2, or 3

  int dimension_;

  /// Lower extent of the patch
  double lower_[3];

  /// Upper extent of the patch
  double upper_[3];

  /// Root grid size
  
  int root_size_[3];

  /// Refinement factor = 2, 4, etc.
  /// Parameter Mesh::refine
  int refine_;

  /// Maximum level for the hierarchy (0 = unigrid) assuming r=2
  /// Parameter Mesh::max_level
  int max_level_;

  /// Whether the tree is balanced or "full"
  /// Parameter Mesh::balanced
  bool balanced_;

  /// Whether to backfill for refine > 2 to regain r == 2 balance
  /// Parameter Mesh::backfill
  bool backfill_;

  /// Whether to coalesce small patches into one big one
  /// Parameter Mesh::coalesce
  bool coalesce_;
};

#endif /* MESH_MESH_HPP */

