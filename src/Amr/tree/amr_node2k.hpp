#ifndef NODE2K_HPP
#define NODE2K_HPP

//345678901234567890123456789012345678901234567890123456789012345678901234567890

/** 
 *********************************************************************
 *
 * @file      amr_node2k.hpp
 * @brief     Node class for 2D k-trees
 * @author    James Bordner (jobordner@ucsd.edu)
 * @date      Tue Oct 27 12:32:07 PDT 2009
 * @bug       
 * @note      
 *
 * 
 *
 * $Id$
 *
 *********************************************************************
 */

#include <stdlib.h>

#include "cello.h"

enum face_type {
  XM = 0,
  XP = 1,
  YM = 2,
  YP = 3};

class Tree2K;

class Node2K {

/** 
 *********************************************************************
 *
 * @class     Node
 * @brief     Node class for K^2-trees
 * @ingroup   Amr
 *
 * Node class for K^2-trees 
 *
 *********************************************************************
 */

public:

  //-------------------------------------------------------------------
  // PUBLIC OPERATIONS
  //-------------------------------------------------------------------

  /// Create a new leaf node
  Node2K( int k,  int level_adjust = 0  );

  /// Delete a node and all descedents
  ~Node2K();

  /// return the specified child
  Node2K * child (int ix, int iy);

  /// return the specified neighbor
  Node2K * neighbor (face_type face);

  /// make the two nodes neighbors.  friend function since either can be NULL
  friend void make_neighbors 
  (Node2K * node_1, Node2K * node_2, face_type face_1);

  /// get the child's cousin
  Node2K * cousin (face_type face, int ix, int iy);

  /// return the parent
  Node2K * parent ();

  /// Refine if any elements in the array are true and recurse
  /// return the level
  int refine 
    (
     const int * level_array, 
     int ndx,  int ndy,
     int lowx, int upx,
     int lowy, int upy,
     int level, 
     int max_level,
     bool is_full = true
     );

  /// Perform a pass of trying to remove level-jumps 
  void balance_pass(bool & refined_tree, bool is_full = true);

  /// Perform a pass of trying to optimize uniformly-refined nodes
  void optimize_pass(bool & refined_tree);

  /// Fill the image region with values
  void fill_image
    (
     float * image,
     int ndx,  int ndy,
     int lowx, int upx,  
     int lowy, int upy,
     int level,
     int num_levels,
     int line_width
     );

  /// Return whether node has all children
  bool all_children () {
    for (int i=0; i<k_*k_; i++) {
	if (! child_[i]) return false;
    }
    return true;
  };

  /// Return whether node has any children
  bool any_children () { 

    for (int i=0; i<k_*k_; i++) {
	if (child_[i]) return true;
    }
    return false;

  };

  //-------------------------------------------------------------------
  // STATIC OPERATIONS
  //-------------------------------------------------------------------

  /// Return the number of nodes
  static int num_nodes() { return num_nodes_; };

private:

  //-------------------------------------------------------------------
  // PRIVATE OPERATIONS
  //-------------------------------------------------------------------

  /// Create child nodes
  void create_children_();

  /// Connect child nodes
  void update_children_();

  /// Delete children and their descendents
  void delete_children_();

  /// Update neighbors for a child
  void update_child_ (int ix, int iy);

  /// Create a child
  void create_child_ (int ix, int iy);

  /// Index into child[] for ix,iy, iz
  int index_(int ix, int iy) { return (ix + k_*iy); };

  /// Return index of opposite face
  int opposite_face_ (face_type face) { return int(face) ^ 1; };

  /// Return number of faces
  int num_faces_() { return 4; };

  /// Return maximum number of children
  int num_children_() { return k_*k_; };

  /// Return the level increment log2(k_)
  int level_increment_() { 
    switch (k_) {
    case 2:  return ( 1 ); break;
    case 4:  return ( 2 ); break;
    case 8:  return ( 3 ); break;
    case 16: return ( 4 ); break;
    default:
      fprintf (stderr,"Invalid k=%d for Node2K\n",k_);
      exit(1);
      break;
    }
  }

private:

  //-------------------------------------------------------------------
  // PRIVATE ATTRIBUTES
  //-------------------------------------------------------------------

  /// Number of cells per node axis, e.g. 2, 4, etc.
  char k_;

  /// Child nodes in edge_type x edge_type ordering
  Node2K ** child_;

  /// Neighbor nodes in edge_type ordering
  Node2K ** neighbor_;

  /// Parent node
  Node2K * parent_;

  /// Relative level for coalesced nodes
  int level_adjust_;

  //-------------------------------------------------------------------
  // STATIC ATTRIBUTES
  //-------------------------------------------------------------------

  /// Number of nodes allocated
  static int num_nodes_;

};

#endif /* NODE_K_HPP */
