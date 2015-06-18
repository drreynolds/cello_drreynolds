// See LICENSE_CELLO file for license and copyright information

/// @file     mesh_Tree.hpp
/// @author   James Bordner (jobordner@ucsd.edu)
/// @date     2012-01-06
/// @brief    [\ref Mesh] Declaration of the Tree class for r^d-trees
///

#ifndef MESH_TREE_HPP
#define MESH_TREE_HPP

class Tree {

  /// @class    Tree
  /// @ingroup  Mesh
  /// @brief    [\ref Mesh] 

public: // interface

  /// Create a r-way tree in d-dimensions, i.e. Nodes have r^d children
  Tree(int d, int r) throw();

  /// Destructor
  ~Tree() throw();

  /// CHARM++ Pack / Unpack function
  inline void pup (PUP::er &p)
  {
    TRACEPUP;
    // NOTE: change this function whenever attributes change
    p | d_;
    p | r_;
    p | c_;
    bool up = p.isUnpacking();
    if (up) root_ = new Node;
    p | *root_;
    p | num_nodes_;
    p | max_level_;

  }

  /// Return the dimensionality of the tree
  int rank() const
  { return d_; }

  /// Return the refinement level of the tree
  int refinement() const
  { return r_; }

  /// Return the number of child nodes in each node (d**r)
  int num_children() const
  { return c_; }

  /// The number of allocated Nodes in the tree
  int num_nodes () const
  { return num_nodes_; }

  /// The maximum number of level of any Node ever in the tree
  /// (coarsened Nodes do not effect max_level
  int max_level () const
  { return max_level_; }

  /// Return the root Node of the tree
  Node * root_node() const
  { return root_; }

  /// Refine the given node
  void refine_node (const NodeTrace &);

  /// Coarsen the given node so that it has no descendents
  void coarsen_node (const NodeTrace &);

  /// Return the neighboring Node in the specified direction
  bool node_neighbor (const NodeTrace & node_trace, 
		      NodeTrace     * node_neighbor,
		      int ix, int iy=0, int iz=0) const;

  /// Return the parent node
  NodeTrace node_parent (const NodeTrace &) const;

  /// Return the specified child Node
  NodeTrace node_child (const NodeTrace &, int k) const;

  void index(int k, int * kx, int *ky, int *kz) const
  { index_(k,kx,ky,kz); }

  /// Balance the Tree
  void balance ();

  /// Coalesce nodes in the Tree
  void coalesce ();

private: // functions

  void index_(int k, int * kx, int *ky, int *kz) const;
  
private: // attributes

  /// Dimensionality of the Tree
  int d_;

  /// Refinement ratio of levels in the Tree
  int r_;

  /// Number of children per node
  int c_;

  /// Pointer to the root node
  Node * root_;

  /// Number of nodes in the tree
  int num_nodes_;

  /// Maximum level of any node in the Tree (will not reflect node coarsenings)
  int max_level_;

};

#endif /* MESH_TREE_HPP */

