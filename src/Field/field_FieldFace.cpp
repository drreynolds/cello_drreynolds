// See LICENSE_CELLO file for license and copyright information

/// @file     field_FieldFace.cpp
/// @author   James Bordner (jobordner@ucsd.edu)
/// @date     2011-04-12
/// @brief    Implementation of the FieldFace class

#include "cello.hpp"

#include "field.hpp"

FieldFace::FieldFace() throw()
  : array_()
{
}

//----------------------------------------------------------------------

FieldFace::~FieldFace() throw ()
{
  deallocate_();
}

//----------------------------------------------------------------------

FieldFace::FieldFace(const FieldFace & field_face) throw ()
  : array_()
/// @param     FieldFace  Object being copied
{
  array_ = field_face.array_;
}

//----------------------------------------------------------------------

FieldFace & FieldFace::operator= (const FieldFace & field_face) throw ()
/// @param     field_face  Source object of the assignment
///
/// @return    The target assigned object
{
  array_ = field_face.array_;
  return *this;
}

//======================================================================

void FieldFace::load
(
 const FieldDescr * field_descr,
 const FieldBlock * field_block,
 axis_enum          axis,
 face_enum          face,
 bool               full_x,
 bool               full_y
 ) throw()
{
  allocate_(field_descr,field_block,axis,full_x,full_y);

  size_t num_fields = field_descr->field_count();

  size_t index = 0;

  for (size_t field=0; field< num_fields; field++) {
  
    // Get precision
    precision_enum precision = field_descr->precision(field);

    // Get field values and face array
    const char * field_face = field_block->field_values(field);

    // Get chunk of array for this field
    char * array_face  = &array_[index];

    // Get field dimensions
    int nd3[3];
    field_block->field_size(field_descr,field,&nd3[0],&nd3[1],&nd3[2]);

    // Compute multipliers for index calculations
    int md3[3] = {1, nd3[0], nd3[0]*nd3[1]};

    // Get ghost depth
    int ng3[3];
    field_descr->ghosts(field,&ng3[0],&ng3[1],&ng3[2]);

    // Load values for the field
    switch (precision) {
    case precision_single:
      index += load_precision_
	( (float * )      (array_face),
	  (const float * )(field_face),
	  nd3,md3,ng3,axis,face,full_x,full_y);
      break;
    case precision_double:
      index += load_precision_
	((double * )      (array_face),
	 (const double * )(field_face),
	 nd3,md3,ng3,axis,face,full_x,full_y);
      break;
    case precision_quadruple:
      index += load_precision_
	((long double * )      (array_face),
	 (const long double * )(field_face),
	 nd3,md3,ng3,axis,face,full_x,full_y);
      break;
    default:
      ERROR("FieldFace::load", "Unsupported precision");
    }
  }

}

//----------------------------------------------------------------------

void FieldFace::store
(
 const FieldDescr * field_descr,
 FieldBlock *       field_block,
 axis_enum          axis,
 face_enum          face,
 bool               full_x,
 bool               full_y
 ) throw()
{

  int ibx,iby;

  size_t num_fields = field_descr->field_count();

  size_t index = 0;

  for (size_t field=0; field<num_fields; field++) {

    // Get precision
    precision_enum precision = field_descr->precision(field);

    // Get field values
    char * field_ghost = field_block->field_values(field);
    
    // Get chunk of array for this field
    const char * array_ghost  = &array_[index];

    // Get field dimensions
    int nd3[3];
    field_block->field_size(field_descr,field,&nd3[0],&nd3[1],&nd3[2]);

    // Compute multipliers for index calculations
    int md3[3] = {1, nd3[0], nd3[0]*nd3[1]};

    // Get ghost depth
    int ng3[3];
    field_descr->ghosts(field,&ng3[0],&ng3[1],&ng3[2]);

    // Compute permutation indices
    switch (precision) {
    case precision_single:
      index += store_precision_
	((float * )      (field_ghost),
	 (const float * )(array_ghost),
	 nd3,md3,ng3,axis,face,full_x,full_y);
      break;
    case precision_double:
      index += store_precision_
	((double * )      (field_ghost),
	 (const double * )(array_ghost),
	 nd3,md3,ng3,axis,face,full_x,full_y);
      break;
    case precision_quadruple:
      index += store_precision_
	((long double * )      (field_ghost),
	 (const long double * )(array_ghost),
	 nd3,md3,ng3,axis,face,full_x,full_y);
      break;
    default:
      ERROR("FieldFace::store", "Unsupported precision");
    }
  }

  deallocate_();
}

//----------------------------------------------------------------------

void FieldFace::allocate_
(
 const FieldDescr * field_descr,
 const FieldBlock * field_block,
 axis_enum          axis,
 bool               full_x,
 bool               full_y
 ) throw()
{

  size_t num_fields = field_descr->field_count();

  int array_size = 0;

  int iax=(axis+1) % 3;
  int iay=(axis+2) % 3;

  for (size_t index_field = 0; index_field < num_fields; index_field++) {

    // Need element size for alignment adjust below

    precision_enum precision = field_descr->precision(index_field);
    int bytes_per_element = cello::sizeof_precision (precision);

    // Get field block dimensions nd3[]
    // Get field_size, which includes ghosts and precision adjustments

    int nd3[3];
    int field_bytes = field_block->field_size 
      (field_descr,index_field, &nd3[0], &nd3[1], &nd3[2]);

    // Get ghost depth

    int ng3[3];
    field_descr->ghosts(index_field,&ng3[0],&ng3[1],&ng3[2]);

    // Adjust if not storing ghost zones

    
    int n_old = nd3[0]*nd3[1]*nd3[2];

    if (! full_x) nd3[iax] -= 2*ng3[iax];
    if (! full_y) nd3[iay] -= 2*ng3[iay];

    int n_new = nd3[0]*nd3[1]*nd3[2];

    field_bytes /= n_old;
    field_bytes *= n_new;

    // Get size of one face for given axis

    int face_bytes = field_bytes * ng3[axis] / nd3 [axis];

    face_bytes += 
      field_block->adjust_alignment_(face_bytes,bytes_per_element);

    // four arrays of this size: lower and upper, ghosts and face

    array_size += face_bytes;

  }

  // Allocate the array

  array_.resize(array_size);
  for (int i=0; i<array_size; i++) array_[i] = 0;

}

//----------------------------------------------------------------------

void FieldFace::deallocate_() throw()
{
  array_.clear();
}

//----------------------------------------------------------------------

template<class T>
size_t FieldFace::load_precision_
(
 T *       array, 
 const T * field_face,
 int       nd3[3], 
 int       md3[3], 
 int       ng3[3],
 axis_enum axis, 
 face_enum face,
 bool      full_x,
 bool      full_y
) throw()
{
  int iax=(axis+1) % 3;
  int iay=(axis+2) % 3;

  int iz0 = (face == face_lower) ? ng3[axis] : nd3[axis]-2*ng3[axis];

  // Loop limits

  int ix0,nx,iy0,ny;

  ix0 = full_x ? 0 : ng3[iax];
  iy0 = full_y ? 0 : ng3[iay];
  nx  = full_x ? nd3[iax] : nd3[iax] - 2*ng3[iax];
  ny  = full_y ? nd3[iay] : nd3[iay] - 2*ng3[iay];

  for (int iz=0; iz <ng3[axis]; iz++)  { // 0 <= iz < ng3[axis]
    for (int iy=0; iy < ny; iy++) {
      for (int ix=0; ix < nx; ix++) {
	int index_array  = iz + ng3[axis]*(ix + nx*iy);
	int index_field_face = 
	  (ix0+ix)*md3[iax] + 
	  (iy0+iy)*md3[iay] + 
	  (iz0+iz)*md3[axis];
	array[index_array] = field_face[index_field_face];
      }
    }
  }

  return (sizeof(T) * nx * ny * ng3[axis]);
}

//----------------------------------------------------------------------

template<class T>
size_t FieldFace::store_precision_
(
 T *       field_ghost,
 const T * array,
 int       nd3[3],
 int       md3[3],
 int       ng3[3],
 axis_enum axis,
 face_enum face,
 bool      full_x,
 bool      full_y
 ) throw()
{
  int iax=(axis+1) % 3;
  int iay=(axis+2) % 3;

  int iz0 = (face == face_lower) ? 0 : nd3[axis]-ng3[axis];

  // Loop limits

  int ix0,nx,iy0,ny;
  ix0 = full_x ? 0 : ng3[iax];
  iy0 = full_y ? 0 : ng3[iay];
  nx  = full_x ? nd3[iax] : nd3[iax] - 2*ng3[iax];
  ny  = full_y ? nd3[iay] : nd3[iay] - 2*ng3[iay];

  for (int iz=0; iz < ng3[axis]; iz++)  { // 0 <= iz < ng3[axis]
    for (int iy=0; iy < ny; iy++) {
      for (int ix=0; ix < nx; ix++) {
	int index_field_ghost = 
	  (ix0+ix)*md3[iax] + 
	  (iy0+iy)*md3[iay] + 
	  (iz0+iz)*md3[axis];
	int index_array  = iz + ng3[axis]*(ix + nx*iy);
	field_ghost[index_field_ghost] = array[index_array];
      }
    }
  }

  return (sizeof(T) * nx * ny * ng3[axis]);
}

//----------------------------------------------------------------------

void FieldFace::print 
(
 const FieldDescr * field_descr,
 const FieldBlock * field_block,
 axis_enum          axis,
 face_enum          face,
 const char *       message
 ) const throw()
{
  size_t num_fields = field_descr->field_count();

  size_t index = 0;

  int iax=(axis+1) % 3;
  int iay=(axis+2) % 3;

  for (size_t index_field=0; index_field<num_fields; index_field++) {

    // Get precision
    precision_enum precision = field_descr->precision(index_field);

    // Get chunk of array for this field
    const char * values  = &array_[index];

    // Get field dimensions
    int nd3[3];
    field_block->field_size(field_descr,index_field,&nd3[0],&nd3[1],&nd3[2]);

    // Get ghost depth
    int ng3[3];
    field_descr->ghosts(index_field,&ng3[0],&ng3[1],&ng3[2]);

    // Compute permutation indices
    switch (precision) {
    case precision_single:
      {
	float min = std::numeric_limits<float>::max();
	float max = std::numeric_limits<float>::min();
	float sum = 0.0;

	for (int iz = 0; iz <ng3[axis]; iz++)  { // 0 <= iz < ng3[axis]
	  for (int iy=0; iy < nd3[iay]; iy++) {
	    for (int ix=0; ix < nd3[iax]; ix++) {
	      int index = iz + ng3[axis]*(ix + nd3[iax]*iy);
	      float value = ((float *)(values))[index];
	      min = MIN(min,value);
	      max = MAX(max,value);
	      sum += value;
	    }
	  }
	}
	PARALLEL_PRINTF
	  ("%s FieldFace[%p,%s] axis %d face %d (x%d y%d g%d) [%g %g %g]\n",
	   message ? message : "",this,
	   field_descr->field_name(index_field).c_str(),
	   axis,face,
	   nd3[iax],nd3[iay],ng3[axis],min,sum/(nd3[iax]*nd3[iay]*ng3[axis]),max);
	index += sizeof(float) * (nd3[iax] * nd3[iay]) * ng3[axis];
    }
    break;
  case precision_double:
    {
      double min = std::numeric_limits<double>::max();
      double max = std::numeric_limits<double>::min();
      double sum = 0.0;

      for (int iz = 0; iz <ng3[axis]; iz++)  { // 0 <= iz < ng3[axis]
	for (int iy=0; iy < nd3[iay]; iy++) {
	  for (int ix=0; ix < nd3[iax]; ix++) {
	    int index = iz + ng3[axis]*(ix + nd3[iax]*iy);
	    double value = ((double *)(values))[index];
	    min = MIN(min,value);
	    max = MAX(max,value);
	    sum += value;
	  }
	}
      }
      PARALLEL_PRINTF
	("%s FieldFace[%p,%s] axis %d face %d (x%d y%d g%d) [%lg %lg %lg]\n",
	 message ? message : "",this,
	 field_descr->field_name(index_field).c_str(),
	 axis,face,
	 nd3[iax],nd3[iay],ng3[axis],min,sum/(nd3[iax]*nd3[iay]*ng3[axis]),max);
      index += sizeof(double) * (nd3[iax] * nd3[iay]) * ng3[axis];
  }
  break;
 case precision_quadruple:
   {
     double min = std::numeric_limits<double>::max();
     double max = std::numeric_limits<double>::min();
     double sum = 0.0;

     for (int iz = 0; iz <ng3[axis]; iz++)  { // 0 <= iz < ng3[axis]
       for (int iy=0; iy < nd3[iay]; iy++) {
	 for (int ix=0; ix < nd3[iax]; ix++) {
	   int index = iz + ng3[axis]*(ix + nd3[iax]*iy);
	   long double value = ((long double *)(values))[index];
	   min = MIN(min,(double)value);
	   max = MAX(max,(double)value);
	   sum += value;
	 }
       }
     }
     PARALLEL_PRINTF
       ("%s FieldFace[%p,%s] axis %d face %d (x%d y%d g%d) [%lg %lg %lg]\n",
	message ? message : "",this,
	field_descr->field_name(index_field).c_str(),
	axis,face,
	nd3[iax],nd3[iay],ng3[axis],min,sum/(nd3[iax]*nd3[iay]*ng3[axis]),max);
     index += sizeof(long double) * (nd3[iax] * nd3[iay]) * ng3[axis];
}
break;
    default:
      break;
    }
  }
}
