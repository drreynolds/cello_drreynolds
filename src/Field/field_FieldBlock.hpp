// See LICENSE_CELLO file for license and copyright information

#ifndef FIELD_FIELD_BLOCK_HPP
#define FIELD_FIELD_BLOCK_HPP

/// @file     field_FieldBlock.hpp
/// @author   James Bordner (jobordner@ucsd.edu)
/// @date     Mon Oct 12 14:38:21 PDT 2009
/// @todo     Implement write()
/// @todo Re-evaluate read(), since current FieldBlock constructor
/// requires size: add FieldBlock(FILE*) constructor? add factory
/// design pattern to create FieldBlocks read from a file?
/// @todo     Clean allocate_array() and allocate_ghosts() usage
/// @brief    [\ref Field] Fortran-style array class.

class Block;
class FieldDescr;

/// @enum  file_content_type
/// @brief Argument for read() and write() to specify what to read or write

enum file_content_type {
  file_content_header,
  file_content_data,
  file_content_all
};

class FieldBlock {

  /// @class    FieldBlock
  /// @ingroup  Field
  /// @brief [\ref Field] Interface between field arrays and low-level
  /// (C/fortran) routines.
  /// 
  /// Defines up to a 4-D fortran-like array for storing 1 or more 3D
  /// arrays.  Axes can be permuted, including the index selecting the
  /// array for storing interleaved arrays.

  friend class FieldFace;

public: // interface

  /// Create a new uninitialized FieldBlock object
  FieldBlock(int nx=0, int ny=1, int nz=1) throw();

  /// Deconstructor
  ~FieldBlock() throw();

  /// Copy constructor
  FieldBlock(const FieldBlock & field_block) throw ();

  /// Assignment operator
  FieldBlock & operator= (const FieldBlock & field_block) throw ();

  /// Return size of fields on the block, assuming centered
  void size(int * nx, int * ny, int * nz) const throw();

  /// Return array for the corresponding field, which may or may not
  /// contain ghosts depending on if they're allocated
  char * field_values (int id_field) throw (std::out_of_range);

  /// Return array for the corresponding field, which may or may not
  /// contain ghosts depending on if they're allocated
  const char * field_values (int id_field) const throw (std::out_of_range);

  /// Return array for the corresponding field, which does not contain
  /// ghosts whether they're allocated or not
  char * field_unknowns ( const FieldDescr * field_descr,
			  int id_field) throw (std::out_of_range);

  const char * field_unknowns ( const FieldDescr * field_descr,
				int id_field) const throw (std::out_of_range);

  /// Return raw pointer to the array of all fields.  Const since
  /// otherwise dangerous due to varying field sizes, precisions,
  /// padding and alignment
  const char * array ()  const throw () 
  { return array_; };

  /// Return width of cells along each dimension
  void cell_width(Block * block,
		  double * hx, double * hy, double * hz) const throw ();

  // /// Return the associated field faces object
  // FieldFaces * field_faces(const FieldDescr * field_descr) throw ();

  /// Clear specified array(s) to specified value
  void clear ( const FieldDescr * field_descr,
	       float value = 0.0, 
	       int id_field_first = -1, 
	       int id_field_last  = -1) throw();
 
  /// Return whether array is allocated or not
  bool array_allocated() const throw();

  /// Allocate storage for the field block
  void allocate_array(const FieldDescr * field_descr) throw();

  /// Deallocate storage for the field block
  void deallocate_array() throw();

  /// Return whether ghost cells are allocated or not
  bool ghosts_allocated() const throw ();

  /// Allocate and clear ghost values if not already allocated
  void allocate_ghosts(const FieldDescr * field_descr) throw ();

  /// Deallocate ghost values if allocated
  void deallocate_ghosts(const FieldDescr * field_descr) throw ();

  /// Refresh ghost zones on an internal face
  void refresh_ghosts(const FieldDescr * field_descr) throw();

  /// Split a block into 2, 4, or 8 subblocks; does not delete self
  void split(bool split_x, bool split_y, bool split_z, 
	     FieldBlock ** field_blocks) throw ();

  /// Merge 2, 4, or 8 subblocks into a single block
  FieldBlock * merge(bool merge_x, bool merge_y, bool merge_z, 
		     FieldBlock ** field_blocks) throw ();
 
  /// Set array values for a given field
  void set_field_values (int id_field, char * values) throw();

  /// Return the number of elements (nx,ny,nz) along each axis, and total
  /// number of bytes n
  int field_size (const FieldDescr * field_descr, int id_field, 
		  int *nx, int *ny, int *nz) const throw();

  //----------------------------------------------------------------------

  /// Print basic field characteristics for debugging
  void print (const FieldDescr * field_descr,
	      const char * message,
	      double lower[3], double upper[3]) const throw();

  /// Write a block to disk as a png image
  void image (const FieldDescr * field_descr,
	      const char * prefix,
	      int cycle, int ibx, int iby, int ibz) const throw();

  //----------------------------------------------------------------------
  // I/O
  //----------------------------------------------------------------------

  /// Open a file for the FieldBlock
  void open (File * file, const char * filename, const char * mode) const throw();

  /// Close a file for the FieldBlock
  void close (File * file) const throw();

  /// Read "metadata" or field data associated with the FieldBlock
  void read (File * file, file_content_type file_content) throw ();

  /// Write "metadata" or field data associated with the FieldBlock
  void write(File * file, file_content_type file_content) const throw ();

private: // functions

  /// Given field size and padding, compute offset to start of the next field
  int adjust_padding_ (int size, int padding) const throw();

  /// Given field size and alignment, compute offset to start of the next field
  int adjust_alignment_ (int size, int alignment) const throw();

  /// Given array start and alignment, return first address that is
  /// aligned
  int align_padding_ (char * start, int alignment) const throw();

  /// Move (not copy) array_ to array and field_values_ to
  /// field_values
  void backup_array_  ( const FieldDescr * field_descr,
			std::vector<char *> & field_values );

  /// Move (not copy) array to array_ and field_values to
  /// field_values_
  void restore_array_ ( const FieldDescr * field_descr,
			std::vector<char *> & field_values )
    throw (std::out_of_range);

  //----------------------------------------------------------------------

private: // attributes

  // /// Corresponding Field faces
  // FieldFaces * field_faces_;

  /// Size of fields on the block, assuming centered
  int size_[3];

  /// Allocated array of field values
  char * array_;
  
  /// Pointers into values_ of the first element of each field
  std::vector<char *> field_values_;

  /// Whether ghost values are allocated or not (make [3] for
  /// directionally split?)
  bool ghosts_allocated_;

};   

#endif /* FIELD_FIELD_BLOCK_HPP */
