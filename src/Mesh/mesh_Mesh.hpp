// $Id$
// See LICENSE_CELLO file for license and copyright information

/// @file     mesh_Mesh.hpp
/// @author   James Bordner (jobordner@ucsd.edu)
/// @date     Tue Nov 10 15:38:40 PST 2009
/// @brief    [\ref Mesh] Declaration of the Mesh class

#ifndef MESH_MESH_HPP
#define MESH_MESH_HPP

/// MOVED TO cello.hpp
/// strict_auto_ptr class
// template<class T>
// class strict_auto_ptr : public std::auto_ptr<T> {
//  public:
//   strict_auto_ptr(T* p = NULL) throw() : std::auto_ptr<T>(p) { }
//  private:
//   strict_auto_ptr (const strict_auto_ptr&) throw();
//   void operator = ( const strict_auto_ptr&) throw();
// };

class Mesh {

  /// @class    Mesh
  /// @ingroup  Mesh
  /// @brief    [\ref Mesh] Adaptive mesh refinement hierarchy

public: // interface

  /// Initialize an Mesh object
  Mesh(DataDescr * data_descr) throw ();

  //----------------------------------------------------------------------

  /// Return dimension
  int dimension() throw ()
  { return dimension_; };

  /// Set dimension
  void set_dimension(int dimension) throw ()
  {dimension_ = dimension; };

  //

  /// Return domain lower extent
  void lower(double * nx, double * ny, double * nz) throw ()
  {
    *nx = lower_[0];
    *ny = lower_[1];
    *nz = lower_[2];
  }

  /// Set domain lower extent
  void set_lower(double nx, double ny, double nz) throw ()
  {
    lower_[0] = nx;
    lower_[1] = ny;
    lower_[2] = nz;
  };

  //

  /// Return domain upper extent
  void upper(double * nx, double * ny, double * nz) throw ()
  {
    *nx = upper_[0];
    *ny = upper_[1];
    *nz = upper_[2];
  }

  /// Set domain upper extent
  void set_upper(double nx, double ny, double nz) throw ()
  {
    upper_[0] = nx;
    upper_[1] = ny;
    upper_[2] = nz;
  };

  //
  
  /// Return max_level
  int max_level() throw ()
  { return max_level_; };

  /// Set max_level
  void set_max_level(int max_level) throw ()
  {max_level_ = max_level; };

  /// Return refinement factor
  int refine() throw ()
  {return refine_; };

  /// Set refinement factor
  void set_refine(int refine) throw ()
  {refine_ = refine; }; 

  /// Return root_size
  void root_size(int * nx, int * ny, int * nz) throw ()
  {
    *nx = root_size_[0];
    *ny = root_size_[1];
    *nz = root_size_[2];
  }

  /// Set root size
  void set_root_size(int nx, int ny, int nz) throw ()
  {
    root_size_[0] = nx;
    root_size_[1] = ny;
    root_size_[2] = nz;
  };

  /// Return min_patch_size
  int min_patch_size() throw ()
  {return min_patch_size_; };

  /// Set minimum patch size
  void set_min_patch_size(int min_patch_size) throw ()
  { min_patch_size_ = min_patch_size; };

  /// Return max_patch_size
  int max_patch_size() throw ()
  {return max_patch_size_; };

  /// Set maximum patch size
  void set_max_patch_size(int max_patch_size) throw ()
  { max_patch_size_ = max_patch_size; };

  /// Return min_block_size
  int min_block_size() throw ()
  {return min_block_size_; };

  /// Set minimum block size
  void set_min_block_size(int min_block_size) throw ()
  { min_block_size_ = min_block_size; };

  /// Return max_block_size
  int max_block_size() throw ()
  {return max_block_size_; };

  /// Set maximum block size
  void set_max_block_size(int max_block_size) throw ()
  { max_block_size_ = max_block_size; };

  /// Pointer to the root Patch
  Patch * root_patch() throw ()
  {return root_patch_; };

  /// Set pointer to the root Patch
  void set_root_patch(Patch * root_patch) throw ()
  { root_patch_ = root_patch; };
  

  /// Return whether to avoid level jumps
  bool balanced() throw ()
  {return balanced_; };

  /// Set whether to avoid level jumps
  void set_balanced(bool balanced) throw ()
  { balanced_ = balanced; };

  /// Return whether to backfill levels
  bool backfill() throw ()
  {return backfill_; };

  /// Set whether to backfill levels
  void set_backfill(bool backfill) throw ()
  { backfill_ = backfill; };

  /// Return whether to coalesce patches
  bool coalesce() throw ()
  {return coalesce_; };

  /// Set whether to coalesce patches
  void set_coalesce(bool coalesce) throw ()
  { coalesce_ = coalesce; };
  
private: // attributes

  /// Tree defining the MESH hierarchy topology
  //  strict_auto_ptr<TreeK> tree_;
  TreeK * tree_;


  /// Spacial dimensions of the Mesh: 1, 2, or 3

  int dimension_;

  /// Lower extent of the patch
  double lower_[3];

  /// Upper extent of the patch
  double upper_[3];


  /// Minimum allowed patch size
  /// Parameter Mesh::min_patch_size
  int min_patch_size_;

  /// Maximum allowed patch size
  /// Parameter Mesh::max_patch_size
  int max_patch_size_;

  /// Minimum allowed block size
  /// Parameter Mesh::min_block_size
  int min_block_size_;

  /// Maximum allowed block size
  /// Parameter Mesh::max_block_size
  int max_block_size_;

  /// Root grid size
  
  int root_size_[3];

  /// Local Patches

  Patch * root_patch_;

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

