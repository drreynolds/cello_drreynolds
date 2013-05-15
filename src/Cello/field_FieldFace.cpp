// See LICENSE_CELLO file for license and copyright information

/// @file     field_FieldFace.cpp
/// @author   James Bordner (jobordner@ucsd.edu)
/// @date     2011-04-12
/// @brief    Implementation of the FieldFace class

#include "cello.hpp"
#include "field.hpp"
#include "problem_Prolong.hpp"
#include "problem_Restrict.hpp"

//----------------------------------------------------------------------

FieldFace::FieldFace() throw()
  : field_block_(0),
    field_descr_(0),
    array_(),
    restrict_(0),
    prolong_(0)
{
  for (int i=0; i<3; i++) {
    ghost_[i] = false;
    face_[i] = 0;
    child_[i] = 0;
  }
}

//----------------------------------------------------------------------

FieldFace::FieldFace 
(
 FieldBlock * field_block,
 const FieldDescr * field_descr
 ) throw()
  : field_block_(field_block),
    field_descr_((FieldDescr*)field_descr),
    array_(),
    restrict_(0),
    prolong_(0)
{
  for (int i=0; i<3; i++) {
    ghost_[i] = false;
    face_[i] = 0;
    child_[i] = 0;
  }
}

//----------------------------------------------------------------------

FieldFace::~FieldFace() throw ()
{
  deallocate();
}

//----------------------------------------------------------------------

FieldFace::FieldFace(const FieldFace & field_face) throw ()
  : field_block_(field_face.field_block_),
    field_descr_(field_face.field_descr_),
    array_()
{
  copy_(field_face);
}

//----------------------------------------------------------------------

FieldFace & FieldFace::operator= (const FieldFace & field_face) throw ()
/// @param     field_face  Source object of the assignment
///
/// @return    The target assigned object
{
  copy_(field_face);
  return *this;
}

//----------------------------------------------------------------------

void FieldFace::copy_(const FieldFace & field_face)
{
  array_        = field_face.array_;
  
  for (int i=0; i<3; i++) {
    ghost_[i] = field_face.ghost_[i];
    face_[i] = field_face.face_[i];
    child_[i] = field_face.child_[i];
  }
  prolong_ =  field_face.prolong_;
  restrict_ =  field_face.restrict_;
}
//----------------------------------------------------------------------

#ifdef CONFIG_USE_CHARM
void FieldFace::pup (PUP::er &p)
{

  // NOTE: change this function whenever attributes change

  TRACEPUP;

  bool up = p.isUnpacking();

  if (up) field_descr_ = new FieldDescr;
  p | *field_descr_;
  if (up) field_block_ = new FieldBlock;
  p | *field_block_;
  p | array_;
  PUParray(p,face_,3);
  PUParray(p,ghost_,3);
  PUParray(p,child_,3);
  p | *restrict_;  // PUPable
  p | *prolong_;  // PUPable
}
#endif
//======================================================================

void FieldFace::load ( int * n, char ** array) throw()
{
  if (array_.size() == 0)  allocate ();

  size_t num_fields = field_descr_->field_count();

  size_t index_array = 0;

  for (size_t index_field=0; index_field< num_fields; index_field++) {
  
    precision_type precision = field_descr_->precision(index_field);

    const void * field_face = field_block_->field_values(index_field);

    void * array_face  = &array_[index_array];

    int nd3[3],ng3[3],im3[3],n3[3];

    field_block_->field_size(field_descr_,index_field,&nd3[0],&nd3[1],&nd3[2]);
    field_descr_->ghosts(index_field,&ng3[0],&ng3[1],&ng3[2]);

    load_loop_limits_ (im3,n3, nd3,ng3);

    if (restrict_) {

      // Restrict field to array

      int i0 = im3[0] + nd3[0]*(im3[1] + nd3[1]*im3[2]);

      int nc3[3] = { (n3[0]+1)/2, (n3[1]+1)/2,(n3[2]+1)/2 };

      restrict_->apply(precision, array_face,nc3,nc3, field_face,nd3,n3);

      index_array += nc3[0]*nc3[1]*nc3[2];

    } else {

      // Copy field to array
      switch (precision) {
      case precision_single:
	{
	  float *       array = (float *) array_face;
	  const float * field = (const float *) field_face;
	  index_array += load_ ( array,field, nd3,n3,im3);
	}
	break;
      case precision_double:
	{
	  double *       array = (double *)array_face;
	  const double * field = (const double *) field_face;
	  index_array += load_ ( array,field, nd3,n3,im3);
	}
	break;
      case precision_quadruple:
	{
	  long double *       array = (long double *)array_face;
	  const long double * field = (const long double *) field_face;
	  index_array += load_ ( array,field, nd3,n3,im3);
	}
	break;
      default:
	ERROR("FieldFace::load", "Unsupported precision");
	break;
      }

    }
  }

  *n = array_.size();
  *array = &array_[0];
}

//----------------------------------------------------------------------

void FieldFace::store (int n, char * array) throw()
{

  size_t num_fields = field_descr_->field_count();

  size_t index_array = 0;

  for (size_t index_field=0; index_field<num_fields; index_field++) {

    precision_type precision = field_descr_->precision(index_field);

    char * field_ghost = field_block_->field_values(index_field);
    
    char * array_ghost  = array + index_array;

    int nd3[3],ng3[3],im3[3],n3[3];

    field_block_->field_size(field_descr_,index_field,&nd3[0],&nd3[1],&nd3[2]);
    field_descr_->ghosts(index_field,&ng3[0],&ng3[1],&ng3[2]);

    store_loop_limits_ (im3,n3, nd3,ng3);

    if (prolong_) {

      // Prolong array to field

      TRACE3 ("test padding %d %d %d",ng3[0],ng3[1],ng3[2]);
      bool need_padding = (ng3[0]%2==1) || (ng3[1]%2==1) || (ng3[2]%2==1);

      ASSERT("FieldFace::store()",
	     "Odd ghost zones not implemented yet: prolong needs padding",
	     ! need_padding);

      int i0 = im3[0] + nd3[0]*(im3[1] + nd3[1]*im3[2]);

      int nc3[3] = { (n3[0]+1)/2, (n3[1]+1)/2,(n3[2]+1)/2 };

      TRACE6 ("store prolong ghost  fine %d %d %d   n %d %d %d",
	      nd3[0],nd3[1],nd3[2],n3[0],n3[1],n3[2]);
      TRACE6 ("store prolong ghost  coarse %d %d %d   n %d %d %d",
	      nc3[0],nc3[1],nc3[2],nc3[0],nc3[1],nc3[2]);
      prolong_->apply(precision, field_ghost,nd3,n3, array_ghost,nc3,nc3);

      index_array += nc3[0]*nc3[1]*nc3[2];

    } else {

      // Copyarray to field

      switch (precision) {
      case precision_single:
	{
	  float *       field = (float *)field_ghost;
	  const float * array = (const float *)array_ghost;
	  index_array += store_ (field, array, nd3,n3,im3);
	}
	break;
      case precision_double:
	{
	  double *       field = (double *)field_ghost;
	  const double * array = (const double *)array_ghost;
	  index_array += store_ (field, array, nd3,n3,im3);
	}
	break;
      case precision_quadruple:
	{
	  long double *       field = (long double *)field_ghost;
	  const long double * array = (const long double *)array_ghost;
	  index_array += store_ (field, array, nd3,n3,im3);
	}
	break;
      default:
	ERROR("FieldFace::store", "Unsupported precision");
	break;
      }
    }
  }

  deallocate();
}

//----------------------------------------------------------------------

char * FieldFace::allocate () throw()
{
  size_t num_fields = field_descr_->field_count();

  int array_size = 0;

  for (size_t index_field = 0; index_field < num_fields; index_field++) {

    // Need element size for alignment adjust below

    precision_type precision = field_descr_->precision(index_field);
    int bytes_per_element = cello::sizeof_precision (precision);


    int nd3[3];
    int field_bytes = field_block_->field_size 
      (field_descr_,index_field, &nd3[0], &nd3[1], &nd3[2]);

    int ng3[3];
    field_descr_->ghosts(index_field,&ng3[0],&ng3[1],&ng3[2]);

    int n_old = nd3[0]*nd3[1]*nd3[2];

    if (! ghost_[0]) nd3[0] -= 2*ng3[0];
    if (! ghost_[1]) nd3[1] -= 2*ng3[1];
    if (! ghost_[2]) nd3[2] -= 2*ng3[2];

    int n_new = nd3[0]*nd3[1]*nd3[2];

    field_bytes /= n_old;
    field_bytes *= n_new;

    int face_bytes = field_bytes;
    if (face_[0]) face_bytes = (face_bytes * ng3[0]) / nd3[0];
    if (face_[1]) face_bytes = (face_bytes * ng3[1]) / nd3[1];
    if (face_[2]) face_bytes = (face_bytes * ng3[2]) / nd3[2];

    face_bytes += 
      field_block_->adjust_alignment_(face_bytes,bytes_per_element);

    array_size += face_bytes;

  }


  array_.resize(array_size);

  for (int i=0; i<array_size; i++) array_[i] = 0;

  return &array_[0];
}

//----------------------------------------------------------------------

void FieldFace::deallocate() throw()
{  array_.clear(); }

//----------------------------------------------------------------------

template<class T>
size_t FieldFace::load_
( T * array_face, const T * field_face, 
  int nd3[3], int n3[3],int im3[3] ) throw()
{


  for (int iz=0; iz <n3[2]; iz++)  {
    int kz = iz+im3[2];
    for (int iy=0; iy < n3[1]; iy++) {
      int ky = iy+im3[1];
      for (int ix=0; ix < n3[0]; ix++) {
	int kx = ix+im3[0];
	int index_array = ix +   n3[0]*(iy +   n3[1] * iz);
	int index_field = kx + nd3[0]*(ky + nd3[1] * kz);
	array_face[index_array] = field_face[index_field];
      }
    }
  }

  return (sizeof(T) * n3[0] * n3[1] * n3[2]);

}

//----------------------------------------------------------------------

template<class T>
    size_t FieldFace::store_
( T * field_ghost, const T * array, int nd3[3], int n3[3],int im3[3] ) throw()
{
  for (int iz=0; iz <n3[2]; iz++)  {
    int kz = iz+im3[2];
    for (int iy=0; iy < n3[1]; iy++) {
      int ky = iy+im3[1];
      for (int ix=0; ix < n3[0]; ix++) {
	int kx = ix+im3[0];
	int index_array = ix +   n3[0]*(iy +   n3[1] * iz);
	int index_field = kx + nd3[0]*(ky + nd3[1] * kz);
	field_ghost[index_field] = array[index_array];
      }
    }
  }

  return (sizeof(T) * n3[0] * n3[1] * n3[2]);
}

//----------------------------------------------------------------------

void FieldFace::load_loop_limits_
( int im3[3],int n3[3], int nd3[3], int ng3[3])
{
  // NOTES: 4:p12

  for (int axis=0; axis<3; axis++) {

    // starting index im3[axis]

    if (face_[axis] == 0) {

      if (ghost_[axis]) {

	im3[axis] =  0;
	n3[axis] = nd3[axis];

      } else {

	im3[axis] = ng3[axis];
	n3[axis]  = nd3[axis] - 2*ng3[axis];

      }

      if ( prolong_ ) {
	// adjust for child offset
	n3[axis]  /= 2;;
	im3[axis] += child_[axis] * n3[axis];
      }
    }

    if (face_[axis] == -1 || face_[axis] == 1) {

	if (face_[axis] == -1) im3[axis] = ng3[axis];   
	if (face_[axis] == +1) im3[axis] = nd3[axis] - 2*ng3[axis];

      if (restrict_) { // 2*g ghost depth

	if (face_[axis] == 1) im3[axis] -= ng3[axis];
	n3[axis] = 2*ng3[axis];

      }	else if (prolong_) { // g/2 ghost depth

	if (face_[axis] == 1) im3[axis] += ng3[axis]/2;
	n3[axis] = (ng3[axis]+1)/2;

      } else {

	n3[axis] = ng3[axis];

      }
    }

  }
}

//----------------------------------------------------------------------

void FieldFace::store_loop_limits_
( int im3[3],int n3[3], int nd3[3], int ng3[3])
{
  // NOTES: 4:p12

  for (int axis=0; axis<3; axis++) {

    if (face_[axis] == 0) {

      if (ghost_[axis]) {

	im3[axis] =  0;
	n3[axis] = nd3[axis];

      } else {

	im3[axis] = ng3[axis];
	n3[axis]  = nd3[axis] - 2*ng3[axis];

      }

      if ( restrict_ ) {
	n3[axis]  /= 2;;
	im3[axis] += child_[axis] * n3[axis];
      }
    }

    if (face_[axis] == -1 || face_[axis] == 1) {

      if (face_[axis] == -1) im3[axis] = 0;
      if (face_[axis] == +1) im3[axis] = nd3[axis] - ng3[axis];

      n3[axis] = ng3[axis];

    }

  }
}

