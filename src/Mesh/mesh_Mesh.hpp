// $Id$
// See LICENSE_CELLO file for license and copyright information

/// @file     mesh_Mesh.hpp
/// @author   James Bordner (jobordner@ucsd.edu)
/// @date     Tue Nov 10 15:38:40 PST 2009
/// @brief    Declaration of the Mesh class

#ifndef MESH_MESH_HPP
#define MESH_MESH_HPP

/// strict_auto_ptr class
template<class T>
class strict_auto_ptr : public std::auto_ptr<T> {
 public:
  strict_auto_ptr(T* p = NULL) throw() : std::auto_ptr<T>(p) { }
 private:
  strict_auto_ptr (const strict_auto_ptr&) throw();
  void operator = ( const strict_auto_ptr&) throw();
};

class Mesh {

  /// @class    Mesh
  /// @ingroup  Mesh
  /// @brief    Adaptive mesh refinement hierarchy

public: // interface

  /// Initialize an Mesh object
  Mesh() :
    tree_(0),
    dimension_(0),
    max_level_(0),
    refine_(2),
    root_size_(),
    min_patch_size_(4),
    max_patch_size_(-1),
    balanced_(true),
    backfill_(true),
    coalesce_(true)
  {};

  /// Return dimension
  int dimension() 
  { return dimension_; };

  /// Set dimension
  void set_dimension(int dimension) 
  {dimension_ = dimension; };

  /// Return max_level
  int max_level() 
  { return max_level_; };

  /// Set max_level
  void set_max_level(int max_level) 
  {max_level_ = max_level; };

  /// Return refinement factor
  int refine() 
  {return refine_; };

  /// Set refinement factor
  void set_refine(int refine) 
  {refine_ = refine; }; 

  /// Return root_size
  std::vector<int> root_size()
  { return root_size_; };

  /// Set root size
  void set_root_size(std::vector<int> root_size) 
  { root_size_ = root_size; };

  /// Return min_patch_size
  int min_patch_size()
  {return min_patch_size_; };

  /// Set minimum patch size
  void set_min_patch_size(int min_patch_size)
  { min_patch_size_ = min_patch_size; };

  /// Return max_patch_size
  int max_patch_size()
  {return max_patch_size_; };

  /// Set maximum patch size
  void set_max_patch_size(int max_patch_size)
  { max_patch_size_ = max_patch_size; };

  /// Return whether to avoid level jumps
  bool balanced()
  {return balanced_; };

  /// Set whether to avoid level jumps
  void set_balanced(bool balanced)
  { balanced_ = balanced; };

  /// Return whether to backfill levels
  bool backfill()
  {return backfill_; };

  /// Set whether to backfill levels
  void set_backfill(bool backfill)
  { backfill_ = backfill; };

  /// Return whether to coalesce patches
  bool coalesce()
  {return coalesce_; };

  /// Set whether to coalesce patches
  void set_coalesce(bool coalesce)
  { coalesce_ = coalesce; };
  
private: // attributes

  /// Tree defining the MESH hierarchy topology
  strict_auto_ptr<TreeK> tree_;

  /// Spacial dimensions of the Mesh: 1, 2, or 3
  /// Determined from Domain::extent
  int dimension_;

  /// Maximum level for the hierarchy (0 = unigrid) assuming r=2
  /// Parameter Mesh::max_level
  int max_level_;

  /// Refinement factor = 2, 4, etc.
  /// Parameter Mesh::refine
  int refine_;

  /// Size of root-level mesh.  Move to Field?
  /// Parameter Mesh::root_size
  std::vector<int> root_size_; 

  /// Minimum allowed patch size (Move to Field?)
  /// Parameter Mesh::min_patch_size
  int min_patch_size_;

  /// Maximum allowed patch size (Move to Field?)
  /// Parameter Mesh::max_patch_size
  int max_patch_size_;

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

