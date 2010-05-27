// $Id$
// See LICENSE_CELLO file for license and copyright information

#ifndef FIELD_BLOCK_HPP
#define FIELD_BLOCK_HPP

/// @file     field_FieldBlock.hpp
/// @author   James Bordner (jobordner@ucsd.edu)
/// @date     Mon Oct 12 14:38:21 PDT 2009
/// @todo     Add allocation and deallocation
/// @todo     Implement and test merge(),split()
/// @todo     Implement and test grow(),shrink()
/// @todo     Implement and test read(),write()
/// @brief    Fortran-style array class.

#include <stdexcept>

#include "field_FieldDescr.hpp"
#include "disk_DiskFile.hpp"

class FieldBlock {

  /// @class    FieldBlock
  /// @ingroup  Field
  /// @brief    Interface between field arrays and low-level (C/fortran) routines.
  /// 
  /// Defines up to a 4-D fortran-like array for storing 1 or more 3D
  /// arrays.  Axes can be permuted, including the index selecting the
  /// array for storing interleaved arrays.

public: // interface

  /// Create a new uninitialized FieldBlock object
  FieldBlock() throw();

  /// Deconstructor
  ~FieldBlock() throw();

  /// Copy constructor
  FieldBlock(const FieldBlock & field_block) throw ();

  /// Assignment operator
  FieldBlock & operator= (const FieldBlock & field_block) throw ();

  /// Return dimensions of fields on the block, assuming centered
  void dimensions(int * nx, int * ny, int * nz) const throw();

  /// Return array for the corresponding field, which may or may not contain ghosts
  void * field_values (int id_field) throw (std::out_of_range);

  /// Return lower and upper+1 index ranges (excluding ghosts)
  void index_range(int * lower_x, int * lower_y, int *lower_z, 
		   int * upper_x, int * upper_y, int *upper_z) const throw ();

  /// Return lower values of the block (excluding ghosts)
  void box_extent(double * lower_x, double * lower_y, double *lower_z, 
		  double * upper_x, double * upper_y, double *upper_z) const throw ();

  /// Return width of cells along each dimension
  void cell_width(double * hx, double * hy, double * hz) const throw ();

  /// Return the associated field descriptor
  FieldDescr * field_descr() throw ();

  /// Clear specified array(s) to specified value
  void clear(double value = 0.0, 
	     int id_field_first      = -1, 
	     int id_field_last_plus  = -1) throw();
 
  /// Return whether array is allocated or not
  bool array_allocated() const throw();

  /// Allocate storage for the field block
  void allocate_array() throw();

  /// Deallocate storage for the field block
  void deallocate_array() throw();

  /// Return whether ghost cells are allocated or not
  bool ghosts_allocated() const throw ();

  /// Allocate and clear ghost values if not already allocated
  void allocate_ghosts() throw ();

  /// Deallocate ghost values if allocated
  void deallocate_ghosts() throw ();

  /// Split a block into 2, 4, or 8 subblocks; does not delete self
  void split(bool split_x, bool split_y, bool split_z, 
	     FieldBlock ** field_blocks) throw ();

  /// Merge 2, 4, or 8 subblocks into a single block
  FieldBlock * merge(bool merge_x, bool merge_y, bool merge_z, 
		     FieldBlock ** field_blocks) throw ();
 
  /// Read a block from disk.  Create new FieldDescr if not supplied or different.
  /// return NULL iff no new field_descr is created
  FieldDescr * read(DiskFile * disk_file, FieldDescr * field_descr = 0) throw ();

  /// Write a block from disk, and optionally associated descriptor
  void write(DiskFile * disk_file, FieldDescr * field_descr = 0) const throw ();

  /// Set dimensions of the array block
  void set_dimensions(int nx, int ny, int nz) throw();

  /// Set array values for a given field
  void set_field_values (int id_field, void * values) throw();

  /// Set the associated field descriptor
  void set_field_descr(FieldDescr * field_descr) throw();

  /// Set the box extent
  void set_box_extent( double lower_x, double lower_y, double lower_z,
		       double upper_x, double upper_y, double upper_z ) throw();

private: // attributes

  /// Corresponding Field descriptor
  FieldDescr * field_descr_;

  /// Dimensions of fields on the block, assuming centered
  int dimensions_[3];

  /// Allocated array of field values
  void * array_;
  
  /// Pointers into values_ of the first element of each field
  std::vector<void *> field_values_;

  /// Extent of the box associated with the block
  /// WARNING: should not be used for deep AMR due to precision / range issues
  double box_lower_[3];
  double box_upper_[3];

  /// Whether ghost values are allocated or not (make [3] for directionally split?)
  bool ghosts_allocated_;

};   

#endif /* FIELD_BLOCK_HPP */
