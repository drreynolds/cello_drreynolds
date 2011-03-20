// $Id$
// See LICENSE_CELLO file for license and copyright information

/// @file     field_FieldFaces.cpp
/// @author   James Bordner (jobordner@ucsd.edu)
/// @date     Thu Feb 25 16:20:17 PST 2010
/// @brief    Brief description of file field_FieldFaces.cpp
///
/// Detailed description of file field_FieldFaces.cpp

#include "cello.hpp"

#include "field.hpp"

FieldFaces::FieldFaces(FieldBlock * field_block) throw ()
  : array_(),
    index_ghosts_(),
    index_faces_()
{
  allocate_(field_block);
}

//----------------------------------------------------------------------

FieldFaces::~FieldFaces() throw ()
{
  deallocate_();
}

//----------------------------------------------------------------------

FieldFaces::FieldFaces(const FieldFaces & field_faces) throw ()
  : array_(),
    index_ghosts_(),
    index_faces_()
/// @param     FieldFaces  Object being copied
{
  UNTESTED("FieldFaces");
  array_        = field_faces.array_;
  index_ghosts_ = field_faces.index_ghosts_;
  index_faces_  = field_faces.index_faces_;

}

//----------------------------------------------------------------------

FieldFaces & FieldFaces::operator= (const FieldFaces & field_faces) throw ()
/// @param     field_faces  Source object of the assignment
///
/// @return    The target assigned object
{
  INCOMPLETE("FieldFaces::operator =");
  return *this;
}

//======================================================================

void FieldFaces::load
(
 const FieldBlock * field_block,
 int field,
 axis_enum axis,
 face_enum face
 ) throw()
{
  const FieldDescr * field_descr = field_block->field_descr();

  UNTESTED("FieldFaces::load");
  if (field == -1) {
    int num_fields = field_descr->field_count();
    // WARNING: recursive
    for (int i=0; i<num_fields; i++) {
      load (field_block,i,axis,face);
    }
  } else {
    // Get precision
    precision_enum precision = field_descr->precision(field);
    // Get field values and face array
    const char * field_values = field_block->field_values(field);
    char * face_values  = &array_[index_faces_[index_(field,axis,face)]];
    // Get field (and face) dimensions
    int n[3];
    field_block->field_size(field,&n[0],&n[1],&n[2]);
    // Compute multipliers for index calculations
    int nd[3] = {1, n[0], n[0]*n[1]};
    // Get ghost depth
    int ng[3];
    field_descr->ghosts(field,&ng[0],&ng[1],&ng[2]);
    // Compute permutation indices
    int iax=(axis+1) % 3;
    int iay=(axis+2) % 3;
    switch (precision) {
    case precision_single:
      for (int iz = 0; iz <ng[axis]; iz++)  {
	for (int iy=0; iy < n[iay]; iy++) {
	  for (int ix=0; ix < n[iax]; ix++) {
	    int index_field = ix*nd[iax] + iy*nd[iay] + (iz+ng[axis])*nd[axis];
	    int index_face  = iz + ng[axis]*(ix + n[iax]*iy);
	    ((float *)(face_values))[index_face] = 
	      ((float *) (field_values))[index_field];
	  }
	}
      }
      break;
    case precision_double:
      for (int iz = 0; iz <ng[axis]; iz++)  {
	for (int iy=0; iy < n[iay]; iy++) {
	  for (int ix=0; ix < n[iax]; ix++) {
	    int index_field = ix*nd[iax] + iy*nd[iay] + (iz+ng[axis])*nd[axis];
	    int index_face  = iz + ng[axis]*(ix + n[iax]*iy);
	    ((double *)(face_values))[index_face] = 
	      ((double *) (field_values))[index_field];
	  }
	}
      }
      break;
    case precision_quadruple:
      for (int iz = 0; iz <ng[axis]; iz++)  {
	for (int iy=0; iy < n[iay]; iy++) {
	  for (int ix=0; ix < n[iax]; ix++) {
	    int index_field = ix*nd[iax] + iy*nd[iay] + (iz+ng[axis])*nd[axis];
	    int index_face  = iz + ng[axis]*(ix + n[iax]*iy);
	    ((long double *)(face_values))[index_face] = 
	      ((long double *) (field_values))[index_field];
	  }
	}
      }
      break;
    default:
      ERROR("FieldFaces::load", "Unsupported precision");
    }
  }
  
}

//----------------------------------------------------------------------

void FieldFaces::copy
(
 const FieldFaces * field_faces,
 int field,
 axis_enum axis,
 face_enum face
 ) throw()
{
  INCOMPLETE("FieldFaces::copy");
}

//----------------------------------------------------------------------

void FieldFaces::store
(
 FieldBlock * field_block,
 int field,
 axis_enum axis,
 face_enum face
 ) throw()
{
  INCOMPLETE("FieldFaces::store");
}

//----------------------------------------------------------------------

void FieldFaces::allocate_(FieldBlock * field_block) throw()
{
  UNTESTED("FieldFaces::allocate_");

  const FieldDescr * field_descr = field_block->field_descr();

  size_t num_fields = field_descr->field_count();

  index_faces_.reserve (num_fields*3*2);
  index_ghosts_.reserve(num_fields*3*2);

  // Determine array_ size

  int array_size = 0;

  // Determine array_ size

  int array_index = 0;
  for (size_t field = 0; field < num_fields; field++) {

    // Need element size for alignment adjust below

    precision_enum precision = field_descr->precision(field);
    int bytes_per_element = cello::sizeof_precision (precision);

    // Get field block dimensions n3[]
    // Get field_size, which includes ghosts and precision adjustments

    int axis_length[3];
    int field_bytes = field_block->field_size (field, 
					      &axis_length[0], 
					      &axis_length[1], 
					      &axis_length[2]);

    // Get ghost depth

    int ghost_count[3];
    field_descr->ghosts(field,
			&ghost_count[0],
			&ghost_count[1],
			&ghost_count[2]);

    for (int axis=0; axis<3; axis++) {

      // Get size of one face for given axis

      int face_bytes = ghost_count[axis] * field_bytes / axis_length [axis];

      face_bytes += field_block->adjust_alignment_(face_bytes,bytes_per_element);

      // 4: (lower, upper) x (ghost, faces)

      array_size += 4 * face_bytes;

      // Initialize the array_ offsets

      int index_lower = index_(field,axis,face_lower);

      index_faces_ [index_lower] = array_index;  array_index += face_bytes;
      index_ghosts_[index_lower] = array_index;  array_index += face_bytes;

      int index_upper = index_(field,axis,face_upper);

      index_faces_ [index_upper] = array_index;  array_index += face_bytes;
      index_ghosts_[index_upper] = array_index;  array_index += face_bytes;

    }
  }

  // Allocate the array
  array_.reserve(array_size);

}

//----------------------------------------------------------------------

void FieldFaces::deallocate_() throw()
{
  array_.clear();
  index_ghosts_.clear();
  index_faces_.clear();
}

//----------------------------------------------------------------------

