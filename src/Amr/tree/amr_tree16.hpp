// $Id$
// See LICENSE_CELLO file for license and copyright information

#ifndef AMR_TREE16_HPP
#define AMR_TREE16_HPP

/// @file
/// @brief     
/// @author    
/// @date      
///
/// Detailed description of file amr_tree16.hpp


/** 
 *********************************************************************
 *
 * @file      
 * @brief     
 * @author    
 * @date      
 * @ingroup
 * @note      
 *
 *--------------------------------------------------------------------
 *
 * DESCRIPTION:
 *
 *    
 *
 * CLASSES:
 *
 *    
 *
 * FUCTIONS:
 *
 *    
 *
 * USAGE:
 *
 *    
 *
 * REVISION HISTORY:
 *
 *    
 *
 * COPYRIGHT: See the LICENSE_CELLO file in the project directory
 *
 *--------------------------------------------------------------------
 *
 * $Id$
 *
 *********************************************************************
 */


#define MAX_LEVELS 80

class Tree16 {

  /// @class    Foo
  /// @brief    Brief description of class Foo.
  /// @ingroup  Template

 public:

  // create a tree refined to the array non-zeros
  // assume array width and height are powers of 2
  Tree16();

  ~Tree16() { delete root_; };

  // Refine down to array
  void refine
    (const int * level_array, 
     int nd0, int nd1, 
     int max_level, 
     bool full_nodes = true
     );

  // Refine nodes to remove level jumps
  void balance(bool full_nodes = true);

  // Replace uniformly-refined patch with single node
  void optimize();

  // Create an image of levels
  float * create_image (int n, int line_width);

  // Return the number of levels
  int levels() { return levels_; }

 private:

  int levels_;
  Node16 * root_;

};

#endif
