#ifndef NODE4_HPP
#define NODE4_HPP

//345678901234567890123456789012345678901234567890123456789012345678901234567890

/** 
 *********************************************************************
 *
 * @file      node4.hpp
 * @brief     
 * @author    James Bordner (jobordner@ucsd.edu)
 * @date      
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

#include "node.h"

class Tree4;

class Node4 {

/** 
 *********************************************************************
 *
 * @class     Node4
 * @brief     
 * @ingroup   GROUP
 *
 * 
 *
 *********************************************************************
 */

public:

  //-------------------------------------------------------------------
  // PUBLIC OPERATIONS
  //-------------------------------------------------------------------

  // Create a new leaf node
  Node4( int level_adjust = 0 );

  // Delete a node and all descedents
  ~Node4();

  // return the num'th child
  Node4 * child (corner_type corner);

  // return the num'th neighbor
  Node4 * neighbor (face_type face);

  // make the two nodes neighbors.  friend function since either can be NULL
  friend void make_neighbors (Node4 * node_1, face_type face_1, Node4 * node_2);

  // get the child's cousin
  Node4 * cousin (face_type face, corner_type corner);

  // return the parent
  Node4 * parent ();

  // Refine if any elements in the array are true and recurse
  // return the level
  int refine 
    (
     const int * level_array, 
     int nd0,  int nd1,
     int low0, int up0,  
     int low1, int up1,
     int level, 
     int max_level,
     bool is_full = true
     );

  // Perform a pass of trying to remove level-jumps 
  void normalize_pass(bool & refined_tree, bool is_full = true);

  // Perform a pass of trying to optimize uniformly-refined nodes
  void optimize_pass(bool & refined_tree);

  // Fill the image region with values
  void fill_image
    (
     float * image,
     int nd0,  int nd1,
     int low0, int up0,  
     int low1, int up1,
     int level,
     int num_levels,
     int line_width
     );

  // Return whether node has all children
  bool all_children () {
    return 
      ((child_[0]) &&
       (child_[1]) &&
       (child_[2]) &&
       (child_[3]));
  };

  // Return whether node has any children
  bool any_children () { 
    return 
      ((child_[0]) ||
       (child_[1]) ||
       (child_[2]) ||
       (child_[3]));
  };

  //-------------------------------------------------------------------
  // STATIC OPERATIONS
  //-------------------------------------------------------------------

  static int num_nodes() { return num_nodes_; };

private:

  //-------------------------------------------------------------------
  // PRIVATE OPERATIONS
  //-------------------------------------------------------------------

  /// 
  void create_children_();
  void update_children_();
  void delete_children_();

  void update_child_ (corner_type corner);
  void create_child_ (corner_type corner);

private:

  //-------------------------------------------------------------------
  // PRIVATE ATTRIBUTES
  //-------------------------------------------------------------------

  /// 
  Node4 * child_[4];    // column-ordering
  Node4 * neighbor_[4]; // Right up left down
  Node4 * parent_;
  int level_adjust_;      // scale for optimizing uniformly refined nodes

  //-------------------------------------------------------------------
  // STATIC ATTRIBUTES
  //-------------------------------------------------------------------

  static int num_nodes_;


};

#endif /* NODE4_HPP */
