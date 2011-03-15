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
  : field_block_(field_block),
    ghosts_(0),
    faces_(0),
    array_(0)
{
  allocate_();
}

//----------------------------------------------------------------------

FieldFaces::~FieldFaces() throw ()
{
  deallocate_();
}

//----------------------------------------------------------------------

FieldFaces::FieldFaces(const FieldFaces & field_faces) throw ()
  : field_block_(field_faces.field_block_),
    ghosts_(0),
    faces_(0),
    array_(0)
/// @param     FieldFaces  Object being copied
{

  INCOMPLETE("FieldFaces::FieldFaces(FieldFaces &)");

  // Reallocate storage for new FieldBlock

  deallocate_();

  field_block_ = field_faces.field_block_;

  allocate_();

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

void FieldFaces::load() throw()
{
  INCOMPLETE("FieldFaces::load()");
}

//----------------------------------------------------------------------

void FieldFaces::save() throw()
{
  INCOMPLETE("FieldFaces::save()");
}

//----------------------------------------------------------------------

void FieldFaces::allocate_() throw()
{
  const FieldDescr * field_descr = field_block_->field_descr();

  int num_fields = field_descr->field_count();

  faces_.reserve (6*num_fields);
  ghosts_.reserve(6*num_fields);

  // Determine array_ size

  int array_size = 0;

  for (int id_field = 0; id_field < num_fields; id_field++) {

    // Need element size for alignment adjust below

    precision_enum precision = field_descr->precision(id_field);
    int bytes_per_element = cello::sizeof_precision (precision);

    // Get field block dimensions n3[]
    // Get field_size, which includes ghosts and precision adjustments

    int n3[3];
    int field_size = field_block_->field_size_(id_field,&n3[0],&n3[1],&n3[2]);

    // Get ghost depth

    int g3[3];
    field_descr->ghosts(id_field,&g3[0],&g3[1],&g3[2]);

    for (int axis=0; axis<3; axis++) {

      // Get size of one face for given axis

      int face_size = g3[axis] * field_size / n3[axis];

      face_size += field_block_->adjust_alignment_(face_size,bytes_per_element);

      // 4: lower, upper, ghost, faces

      array_size += 4 * face_size;

    }
  }

  array_ = new char [ array_size ];

}

//----------------------------------------------------------------------

void FieldFaces::deallocate_() throw()
{
  delete [] array_;
}

//----------------------------------------------------------------------

