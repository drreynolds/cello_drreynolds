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
  full_[0] = true;
  full_[1] = true;
  full_[2] = true;
}

FieldFace::FieldFace 
(
 const FieldBlock * field_block,
 const FieldDescr * field_descr,
 int  fx, int  fy, int  fz,
 bool gx, bool gy, bool gz
) throw()
  : array_()
{
  full_[0] = true;
  full_[1] = true;
  full_[2] = true;

  set_full(gx,gy,gz);
  load(field_descr,field_block,fx,fy,fz);
}

//----------------------------------------------------------------------

#ifdef CONFIG_USE_CHARM
FieldFace::FieldFace(int n, char * array) throw()
  : array_()
{
  full_[0] = true;
  full_[1] = true;
  full_[2] = true;

  array_.resize(n);
  for (int i=0; i<n; i++) array_[i] = array[i]; 
};
#endif

//----------------------------------------------------------------------

FieldFace::~FieldFace() throw ()
{
  deallocate();
}

//----------------------------------------------------------------------

FieldFace::FieldFace(const FieldFace & field_face) throw ()
  : array_()
/// @param     FieldFace  Object being copied
{
  array_        = field_face.array_;
  full_[axis_x] = field_face.full_[axis_x];
  full_[axis_y] = field_face.full_[axis_y];
  full_[axis_z] = field_face.full_[axis_z];
}

//----------------------------------------------------------------------

FieldFace & FieldFace::operator= (const FieldFace & field_face) throw ()
/// @param     field_face  Source object of the assignment
///
/// @return    The target assigned object
{
  array_ = field_face.array_;
  full_[axis_x] = field_face.full_[axis_x];
  full_[axis_y] = field_face.full_[axis_y];
  full_[axis_z] = field_face.full_[axis_z];
  return *this;
}

//======================================================================

void FieldFace::load
(
 const FieldDescr * field_descr,
 const FieldBlock * field_block,
 int                fx,
 int                fy,
 int                fz
) throw()
{
  if (array_.size() == 0) {
    allocate (field_descr,field_block,fx,fy,fz);
  }

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

    // Get ghost depth
    int ng3[3];
    field_descr->ghosts(field,&ng3[0],&ng3[1],&ng3[2]);

    // Load values for the field
    switch (precision) {
    case precision_single:
      index += load_precision_
	( (float * )      (array_face),
	  (const float * )(field_face),
	  nd3,ng3,fx,fy,fz);
      break;
    case precision_double:
      index += load_precision_
	((double * )      (array_face),
	 (const double * )(field_face),
	 nd3,ng3,fx,fy,fz);
      break;
    case precision_quadruple:
      index += load_precision_
	((long double * )      (array_face),
	 (const long double * )(field_face),
	 nd3,ng3,fx,fy,fz);
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
 int                fx,
 int                fy,
 int                fz
) throw()
{

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

    // Get ghost depth
    int ng3[3];
    field_descr->ghosts(field,&ng3[0],&ng3[1],&ng3[2]);

    // Compute permutation indices
    switch (precision) {
    case precision_single:
      index += store_precision_
	((float * )      (field_ghost),
	 (const float * )(array_ghost),
	 nd3,ng3,fx,fy,fz);
      break;
    case precision_double:
      index += store_precision_
	((double * )      (field_ghost),
	 (const double * )(array_ghost),
	 nd3,ng3,fx,fy,fz);
      break;
    case precision_quadruple:
      index += store_precision_
	((long double * )      (field_ghost),
	 (const long double * )(array_ghost),
	 nd3,ng3,fx,fy,fz);
      break;
    default:
      ERROR("FieldFace::store", "Unsupported precision");
    }
  }

  deallocate();
}

//----------------------------------------------------------------------

void FieldFace::allocate
(
 const FieldDescr * field_descr,
 const FieldBlock * field_block,
 int                fx,
 int                fy,
 int                fz
 ) throw()
{

  size_t num_fields = field_descr->field_count();

  int array_size = 0;

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

    // Adjust field_bytes if not storing ghost zones

    int n_old = nd3[0]*nd3[1]*nd3[2];

    if (! full_[0]) nd3[0] -= 2*ng3[0];
    if (! full_[1]) nd3[1] -= 2*ng3[1];
    if (! full_[2]) nd3[2] -= 2*ng3[2];

    int n_new = nd3[0]*nd3[1]*nd3[2];

    field_bytes /= n_old;
    field_bytes *= n_new;

    // Get size of the corner, edge, or face

    int face_bytes = field_bytes;
    if (fx) face_bytes = (face_bytes * ng3[0]) / nd3[0];
    if (fy) face_bytes = (face_bytes * ng3[1]) / nd3[1];
    if (fz) face_bytes = (face_bytes * ng3[2]) / nd3[2];

    face_bytes += 
      field_block->adjust_alignment_(face_bytes,bytes_per_element);

    // Increment array size

    array_size += face_bytes;

  }

  // Allocate the array

  array_.resize(array_size);
  for (int i=0; i<array_size; i++) array_[i] = 0;

}

//----------------------------------------------------------------------

void FieldFace::deallocate() throw()
{
  array_.clear();
}

//----------------------------------------------------------------------

void FieldFace::loop_limits
(
 int *ix0,
 int *iy0,
 int *iz0,
 int *nx,
 int *ny,
 int *nz,
 int nd3[3],
 int ng3[3],
 int  fx,
 int  fy,
 int  fz,
 bool load
 )
{
  // offset (ix0,iy0,iz0)

  if (fx == 0) {
    (*ix0) = (full_[0]) ? 0 : ng3[0];
  } else {
    if (load) {
      if (fx == -1) (*ix0) = ng3[0];   
      if (fx == +1) (*ix0) = nd3[0] - 2*ng3[0];
    } else {
      if (fx == -1) (*ix0) = 0;
      if (fx == +1) (*ix0) = nd3[0] - ng3[0];
    }
  }
  if (fy == 0) {
    (*iy0) = (full_[1]) ? 0 : ng3[1];
  } else {
    if (load) {
      if (fy == -1) (*iy0) = ng3[1];
      if (fy == +1) (*iy0) = nd3[1] - 2*ng3[1];
    } else {
      if (fy == -1) (*iy0) = 0;
      if (fy == +1) (*iy0) = nd3[1] - ng3[1];
    }
  }

  if (fz == 0) {
    (*iz0) = (full_[2]) ? 0 : ng3[2];
  } else {
    if (load) {
      if (fz == -1) (*iz0) = ng3[2];
      if (fz == +1) (*iz0) = nd3[2] - 2*ng3[2];
    } else {
      if (fz == -1) (*iz0) = 0;
      if (fz == +1) (*iz0) = nd3[2] - ng3[2];
    }
  }


  if (fx == 0) {
    (*nx) = (full_[0]) ? nd3[0] : nd3[0]-2*ng3[0];
  } else {
    (*nx) = ng3[0]; 
  }

  if (fy == 0) {
    (*ny) = (full_[1]) ? nd3[1] : nd3[1]-2*ng3[1];
  } else {
    (*ny) = ng3[1]; 
  }

  if (fz == 0) {
    (*nz) = (full_[2]) ? nd3[2] : nd3[2]-2*ng3[2];
  } else {
    (*nz) = ng3[2]; 
  }

}

//----------------------------------------------------------------------

template<class T>
size_t FieldFace::load_precision_
(
 T *       array, 
 const T * field_face,
 int       nd3[3], 
 int       ng3[3],
 int       fx,
 int       fy,
 int       fz
) throw()
{
  int ix0,iy0,iz0;
  int nx,ny,nz;
  bool load;

  // Loop limits

  loop_limits(&ix0,&iy0,&iz0,&nx,&ny,&nz, nd3,ng3, fx,fy,fz,load=true);

  // TRACE3 (" load fx,fy,fz = %d %d %d",fx,fy,fz);
  // TRACE3 (" load nd3      = %d %d %d",nd3[0],nd3[1],nd3[2]); 
  // TRACE3 (" load ng3      = %d %d %d",ng3[0],ng3[1],ng3[2]); 
  // TRACE3 (" load i0       = %d %d %d",ix0,iy0,iz0);
  // TRACE3 (" load n        = %d %d %d",nx,ny,nz);
  for (int iz=0; iz <nz; iz++)  {
    int kz = iz+iz0;
    for (int iy=0; iy < ny; iy++) {
      int ky = iy+iy0;
      for (int ix=0; ix < nx; ix++) {
	int kx = ix+ix0;
	int index_array = ix +  nx   *(iy +  ny    * iz);
	int index_field = kx + nd3[0]*(ky + nd3[1] * kz);
	array[index_array] = field_face[index_field];
      }
    }
  }

  return (sizeof(T) * nx * ny * nz);
}

//----------------------------------------------------------------------

template<class T>
size_t FieldFace::store_precision_
(
 T *       field_ghost,
 const T * array,
 int       nd3[3],
 int       ng3[3],
 int       fx,
 int       fy,
 int       fz
 ) throw()
{

  int ix0,iy0,iz0;
  int nx,ny,nz;
  bool load;

  // Loop limits

  loop_limits(&ix0,&iy0,&iz0,&nx,&ny,&nz, nd3,ng3, fx,fy,fz,load=false);

  // TRACE3 (" store fx,fy,fz = %d %d %d",fx,fy,fz);
  // TRACE3 (" store nd3      = %d %d %d",nd3[0],nd3[1],nd3[2]); 
  // TRACE3 (" store ng3      = %d %d %d",ng3[0],ng3[1],ng3[2]); 
  // TRACE3 (" store i0       = %d %d %d",ix0,iy0,iz0);
  // TRACE3 (" store n        = %d %d %d",nx,ny,nz);
  for (int iz=0; iz <nz; iz++)  {
    int kz = iz+iz0;
    for (int iy=0; iy < ny; iy++) {
      int ky = iy+iy0;
      for (int ix=0; ix < nx; ix++) {
	int kx = ix+ix0;
	int index_array = ix +  nx   *(iy +  ny    * iz);
	int index_field = kx + nd3[0]*(ky + nd3[1] * kz);
	field_ghost[index_field] = array[index_array];
      }
    }
  }

  return (sizeof(T) * nx * ny * nz);
}
