// $Id$
// See LICENSE_CELLO file for license and copyright information

/// @file     field_FieldBlock.cpp
/// @author   James Bordner (jobordner@ucsd.edu)
/// @date     Wed May 19 18:17:50 PDT 2010
/// @todo     Remove dependence of velocity field names "velocity_[xyz]"
/// @brief    Implementation of the FieldBlock class

#include "cello.hpp"

#include "field.hpp"

//----------------------------------------------------------------------

FieldBlock::FieldBlock
(
 const FieldDescr * field_descr,
 int nx, int ny, int nz
) throw()
  : field_descr_(field_descr),
    field_faces_(0),
    array_(0),
    field_values_(),
    ghosts_allocated_(false)
{
  ASSERT("FieldBlock","Input field_descr is NULL",field_descr != NULL);

  size_[0] = nx;
  size_[1] = ny;
  size_[2] = nz;
}

//----------------------------------------------------------------------

FieldBlock::~FieldBlock() throw()
{  
  deallocate_array();
}

//----------------------------------------------------------------------

FieldBlock::FieldBlock ( const FieldBlock & field_block ) throw ()
{  INCOMPLETE("FieldBlock::FieldBlock",""); }

//----------------------------------------------------------------------

FieldBlock & FieldBlock::operator= ( const FieldBlock & field_block ) throw ()
{  INCOMPLETE("FieldBlock::operator=","");
  return *this;
}

//----------------------------------------------------------------------

void FieldBlock::size( int * nx, int * ny, int * nz ) const throw()
{
  *nx = size_[0];
  *ny = size_[1];
  *nz = size_[2];
}

//----------------------------------------------------------------------

void * FieldBlock::field_values ( int id_field ) throw (std::out_of_range)
{
  return (unsigned(id_field) < field_values_.size()) ? 
    field_values_.at(id_field) : NULL;
}

//----------------------------------------------------------------------

void * FieldBlock::field_unknowns ( int id_field ) throw (std::out_of_range)
{
  char * field_unknowns = field_values_.at(id_field);

  if ( ghosts_allocated() ) {

    int gx,gy,gz;
    field_descr_->ghosts(id_field,&gx,&gy,&gz);

    bool cx,cy,cz;
    field_descr_->centering(id_field,&cx,&cy,&cz);

    int nx,ny,nz;
    size(&nx,&ny,&nz);

    nx += 2*gx + (cx?0:1);
    ny += 2*gy + (cy?0:1);
    nz += 2*gz + (cz?0:1);

    precision_enum precision = field_descr_->precision(id_field);
    int bytes_per_element = cello::precision_size (precision);

    field_unknowns += bytes_per_element * (gx + nx*(gy + ny*gz));
  } 

  return field_unknowns;
}

//----------------------------------------------------------------------

void FieldBlock::cell_width(DataBlock * data_block,
			    double * hx, double * hy, double * hz ) const throw ()
{
  double xm,xp,ym,yp,zm,zp;
  data_block->extent(&xm,&xp,&ym,&yp,&zm,&zp);

  *hx = (xp-xm) / size_[0];
  *hy = (yp-ym) / size_[1];
  *hz = (zp-zm) / size_[2];
}

//----------------------------------------------------------------------

FieldFaces * FieldBlock::field_faces() throw ()
{
  if (field_faces_ == NULL) {
    field_faces_ = new FieldFaces(this);
  }
  return field_faces_;
}

//----------------------------------------------------------------------

void FieldBlock::clear
(
 float value,
 int id_field_first,
 int id_field_last) throw()
{
  if ( array_allocated() ) {
    if (id_field_first == -1) {
      id_field_first = 0;
      id_field_last  = field_descr_->field_count() - 1;
    } else if (id_field_last == -1) {
      id_field_last  = id_field_first;
    }
    for (int id_field = id_field_first;
	 id_field <= id_field_last;
	 id_field++) {
      int nx,ny,nz;
      field_size_(id_field,&nx,&ny,&nz);
      precision_enum precision = field_descr_->precision(id_field);
      switch (precision) {
      case precision_single:
	for (int i=0; i<nx*ny*nz; i++) {
	  ((float *)field_values_[id_field])[i] = (float) value;
	}
	break;
      case precision_double:
	for (int i=0; i<nx*ny*nz; i++) {
	  ((double *)field_values_[id_field])[i] = (double) value;
	}
	break;
      case precision_quadruple:
	for (int i=0; i<nx*ny*nz; i++) {
	  ((long double *)field_values_[id_field])[i] = (long double) value;
	}
	break;
      default:
	char buffer[80];
	sprintf (buffer,"Clear called with unsupported precision %s" , 
		 cello::precision_name[precision]);
	ERROR("FieldBlock::clear", buffer);
      }

    }
  } else {
    ERROR("FieldBlock::clear",
		  "Called clear with unallocated arrays");
  }
}

//----------------------------------------------------------------------

void FieldBlock::allocate_array() throw()
{
  if (! (size_[0] > 0 &&
	 size_[1] > 0 &&
	 size_[2] > 0) ) {
    ERROR ("FieldBlock::allocate_array",
		   "Allocate called with zero field size");
  }

  if ( ! array_allocated() ) {
    
    int padding   = field_descr_->padding();
    int alignment = field_descr_->alignment();

    int array_size = 0;

    for (int id_field=0; id_field<field_descr_->field_count(); id_field++) {

      // Increment array_size, including padding and alignment adjustment

      int nx,ny,nz;       // not needed

      int size = field_size_(id_field, &nx,&ny,&nz);

      array_size += adjust_padding_   (size,padding);
      array_size += adjust_alignment_ (size,alignment);

    }

    // Adjust for possible initial misalignment

    array_size += alignment - 1;

    // Allocate the array

    array_ = new char [array_size];

    // Initialize field_begin

    char * field_begin = array_ + align_padding_(array_,alignment);

    int field_offset = 0;

    field_values_.reserve(field_descr_->field_count());

    for (int id_field=0; id_field<field_descr_->field_count(); id_field++) {

      field_values_.push_back(field_begin + field_offset);

      // Increment array_size, including padding and alignment adjustment

      int nx,ny,nz;       // not needed

      int size = field_size_(id_field,&nx,&ny,&nz);

      field_offset += adjust_padding_  (size,padding);
      field_offset += adjust_alignment_(size,alignment);
    }

    // check if array_size is too big or too small

    if ( ! ( 0 <= (array_size - field_offset)
	     &&   (array_size - field_offset) < alignment)) {
      ERROR ("FieldBlock::allocate_array",
		     "Code error: array size was computed incorrectly");
    };

  } else {
    ERROR ("FieldBlock::allocate_array",
		   "Allocate called with array already allocated");
  }
}

//----------------------------------------------------------------------

void FieldBlock::deallocate_array () throw()
{
  if ( array_allocated() ) {

    delete [] array_;
    array_ = 0;
    field_values_.clear();

  }
}

//----------------------------------------------------------------------

bool FieldBlock::array_allocated() const throw()
{
  return array_ != 0;
}

//----------------------------------------------------------------------
	
bool FieldBlock::ghosts_allocated() const throw ()
{
  return ghosts_allocated_;
}

//----------------------------------------------------------------------

void FieldBlock::allocate_ghosts() throw ()
{
  if (! ghosts_allocated() ) {

    std::vector<char *> old_field_values;
    char *              old_array;

    old_array = array_;
    array_ = 0;

    backup_array_ (old_field_values);

    ghosts_allocated_ = true;

    allocate_array();

    restore_array_ (old_field_values);

    delete [] old_array;

  } else {
    WARNING("FieldBlock::allocate_ghosts",
		    "Allocate called with ghosts already allocated");
  }
}

//----------------------------------------------------------------------

void FieldBlock::deallocate_ghosts() throw ()
{
  if ( ghosts_allocated() ) {

    std::vector<char *> old_field_values;
    char *              old_array;

    old_array = array_;
    array_ = 0;

    backup_array_ (old_field_values);

    ghosts_allocated_ = false;

    allocate_array();

    restore_array_ (old_field_values);

    delete [] old_array;

  } else {
    WARNING("FieldBlock::deallocate_ghosts",
		    "Function called with ghosts not allocated");
  }
}

//----------------------------------------------------------------------
void FieldBlock::refresh_ghosts() throw()
{
  INCOMPLETE("FieldBlock::refresh_ghosts","");
  if ( ghosts_allocated() ) {
  } else {
    WARNING("FieldBlock::refresh_ghosts",
		    "Function called with ghosts not allocated");
  }
}

//----------------------------------------------------------------------
void FieldBlock::enforce_boundary
(
 boundary_enum boundary,
 face_enum     face,
 axis_enum     axis 
 ) throw()
{
  // WARNING: recursive
  if ( ghosts_allocated() ) {
    if (face == face_all) {
      enforce_boundary(boundary,face_lower,axis);
      enforce_boundary(boundary,face_upper,axis);
    } else if (axis == axis_all) {
      enforce_boundary(boundary,face,axis_x);
      enforce_boundary(boundary,face,axis_y);
      enforce_boundary(boundary,face,axis_z);
    } else {
      switch (boundary) {
      case boundary_reflecting:
	enforce_boundary_reflecting_(face,axis);
	break;
      case boundary_outflow:
	enforce_boundary_outflow_(face,axis);
	break;
      case boundary_inflow:
	enforce_boundary_inflow_(face,axis);
	break;
      case boundary_periodic:
	enforce_boundary_periodic_(face,axis);
	break;
      default:
	ERROR("FieldBlock::enforce_boundary",
	      "Undefined boundary type");
	break;
      }
    }
  } else {
    ERROR("FieldBlock::enforce_boundary",
	  "Function called with ghosts not allocated");
  }
}

//----------------------------------------------------------------------
void FieldBlock::enforce_boundary_reflecting_
(
 face_enum face,
 axis_enum axis
 ) throw()
{
  int nx,ny,nz;
  int gx,gy,gz;
  size(&nx,&ny,&nz);
  for (int field = 0; field < field_descr_->field_count(); field++) {
    field_descr_->ghosts(field,&gx,&gy,&gz);
    void * array = field_values(field);
    bool vx = field_descr_->field_name(field) == "velocity_x";
    bool vy = field_descr_->field_name(field) == "velocity_y";
    bool vz = field_descr_->field_name(field) == "velocity_z";
    precision_enum precision = field_descr_->precision(field);
    switch (precision) {
    case precision_single:
      enforce_boundary_reflecting_precision_(face,axis,
					     (float *)array,
					     nx,ny,nz,
					     gx,gy,gz,
					     vx,vy,vz);
      break;
    case precision_double:
      enforce_boundary_reflecting_precision_(face,axis,
					     (double *)array,
					     nx,ny,nz,
					     gx,gy,gz,
					     vx,vy,vz);
      break;
    case precision_extended80:
    case precision_extended96:
    case precision_quadruple:
      enforce_boundary_reflecting_precision_(face,axis,
					     (long double *)array,
					     nx,ny,nz,
					     gx,gy,gz,
					     vx,vy,vz);
      break;
    case precision_half:
    default:
      break;
    }
  }
}

template<class T>
void FieldBlock::enforce_boundary_reflecting_precision_
(face_enum face, axis_enum axis,
 T * array,
 int nx,int ny,int nz,
 int gx,int gy,int gz,
 bool vx,bool vy,bool vz)
{
  int mx = nx + 2*gx;
  int my = ny + 2*gy;
  int mz = nz + 2*gz;

  int ix,iy,iz,ig;
  T sign;

  if (face == face_lower && axis == axis_x) {
    if (nx > 1) {
      ix = gx;
      sign = vx ? -1.0 : 1.0;
      for (ig=0; ig<gx; ig++) {
	for (iy=0; iy<my; iy++) {
	  for (iz=0; iz<mz; iz++) {
	    int i_internal = INDEX(ix+ig,  iy,iz,mx,my);
	    int i_external = INDEX(ix-ig-1,iy,iz,mx,my);
	    array[i_external] = sign*array[i_internal];
	  }
	}
      }
    }
  } else if (face == face_upper && axis == axis_x) {
    if (nx > 1) {
      ix = nx+gx-1;
      sign = vx ? -1.0 : 1.0;
      for (ig=0; ig<gx; ig++) {
	for (iy=0; iy<my; iy++) {
	  for (iz=0; iz<mz; iz++) {
	    int i_internal = INDEX(ix-ig,  iy,iz,mx,my);
	    int i_external = INDEX(ix+ig+1,iy,iz,mx,my);
	    array[i_external] = sign*array[i_internal];
	  }
	}
      }
    }
  } else if (face == face_lower && axis == axis_y) {
    if (ny > 1) {
      iy = gy;
      sign = vy ? -1.0 : 1.0;
      for (ig=0; ig<gy; ig++) {
	for (ix=0; ix<mx; ix++) {
	  for (iz=0; iz<mz; iz++) {
	    int i_internal = INDEX(ix,iy+ig,  iz,mx,my);
	    int i_external = INDEX(ix,iy-ig-1,iz,mx,my);
	    array[i_external] = sign*array[i_internal];
	  }
	}
      }
    }
  } else if (face == face_upper && axis == axis_y) {
    if (ny > 1) {
      iy = ny+gy-1;
      sign = vy ? -1.0 : 1.0;
      for (ig=0; ig<gy; ig++) {
	for (ix=0; ix<mx; ix++) {
	  for (iz=0; iz<mz; iz++) {
	    int i_internal = INDEX(ix,iy-ig,  iz,mx,my);
	    int i_external = INDEX(ix,iy+ig+1,iz,mx,my);
	    array[i_external] = sign*array[i_internal];
	  }
	}
      }
    }
  } else if (face == face_lower && axis == axis_z) {
    if (nz > 1) {
      iz = gz;
      sign = vz ? -1.0 : 1.0;
      for (ig=0; ig<gz; ig++) {
	for (ix=0; ix<mx; ix++) {
	  for (iy=0; iy<my; iy++) {
	    int i_internal = INDEX(ix,iy,iz+ig,  mx,my);
	    int i_external = INDEX(ix,iy,iz-ig-1,mx,my);
	    array[i_external] = sign*array[i_internal];
	  }
	}
      }
    }
  } else if (face == face_upper && axis == axis_z) {
    if (nz > 1) {
      iz = nz+gz-1;
      sign = vz ? -1.0 : 1.0;
      for (ig=0; ig<gz; ig++) {
	for (ix=0; ix<mx; ix++) {
	  for (iy=0; iy<my; iy++) {
	    int i_internal = INDEX(ix,iy,iz-ig,  mx,my);
	    int i_external = INDEX(ix,iy,iz+ig+1,mx,my);
	    array[i_external] = sign*array[i_internal];
	  }
	}
      }
    }
  } else {
    ERROR("FieldBlock::enforce_boundary_reflecting_precision_",
	  "Cannot be called with face_all");
  }
}

//----------------------------------------------------------------------
void FieldBlock::enforce_boundary_outflow_
(
 face_enum face,
 axis_enum axis
 ) throw()
{
  INCOMPLETE("FieldBlock::enforce_boundary_outflow","");
}

//----------------------------------------------------------------------
void FieldBlock::enforce_boundary_inflow_
(
 face_enum face,
 axis_enum axis
 ) throw()
{
  INCOMPLETE("FieldBlock::enforce_boundary_inflow","");
}

//----------------------------------------------------------------------
void FieldBlock::enforce_boundary_periodic_
(
 face_enum face,
 axis_enum axis
 ) throw()
{
  INCOMPLETE("FieldBlock::enforce_boundary_periodic","");
}

//----------------------------------------------------------------------

void FieldBlock::split
(
 bool split_x, bool split_y, bool split_z,
 FieldBlock ** new_field_blocks ) throw ()
{
}

//----------------------------------------------------------------------

FieldBlock * FieldBlock::merge
(
 bool merge_x, bool merge_y, bool merge_z,
 FieldBlock ** field_blocks ) throw ()
{
  FieldBlock * new_field_block = 0;
  return new_field_block;
}

//----------------------------------------------------------------------
	
void FieldBlock::read (File * file) throw ()
{
  INCOMPLETE("FieldBlock::read","");
}

//----------------------------------------------------------------------

void FieldBlock::write (File * file) const throw ()
{
  INCOMPLETE("FieldBlock::write","");
}

//----------------------------------------------------------------------

void FieldBlock::set_field_values 
(
 int    id_field, 
 char * field_values) throw()
{
  INCOMPLETE("FieldBlock::set_field_values","");
}

//======================================================================


int FieldBlock::adjust_padding_
(
 int size, 
 int padding) const throw ()
{
  return size + padding;
}

//----------------------------------------------------------------------

int FieldBlock::adjust_alignment_
(
 int size, 
 int alignment) const throw ()
{
  return (alignment - (size % alignment)) % alignment;
}

//----------------------------------------------------------------------

int FieldBlock::field_size_ 
(
 int id_field,
 int * nx,
 int * ny,
 int * nz
 ) const throw()
{

  // Adjust memory usage due to ghosts if needed

  int  gx,gy,gz;
  if ( ghosts_allocated() ) {
    field_descr_->ghosts(id_field,&gx,&gy,&gz);
  } else {
    gx = gy = gz =0;
  }

  // Adjust memory usage due to field centering if needed

  bool cx,cy,cz;
  field_descr_->centering(id_field,&cx,&cy,&cz);

  // Compute array size

  *nx = size_[0] + (1-cx) + 2*gx;
  *ny = size_[1] + (1-cy) + 2*gy;
  *nz = size_[2] + (1-cz) + 2*gz;

  // Return array size in bytes

  precision_enum precision = field_descr_->precision(id_field);
  int bytes_per_element = cello::precision_size (precision);
  return (*nx) * (*ny) * (*nz) * bytes_per_element;
}

//----------------------------------------------------------------------

void FieldBlock::backup_array_ 
( std::vector<char *> & old_field_values )
{
  // save old field_values_

  for (int i=0; i<field_descr_->field_count(); i++) {
    old_field_values.push_back(field_values_[i]);
  }
  field_values_.clear();

}

//----------------------------------------------------------------------

void FieldBlock::restore_array_ 
( std::vector<char *> & field_values_from) throw (std::out_of_range)
{
  // copy values
  for (int id_field=0; 
       id_field < field_descr_->field_count();
       id_field++) {

    // get "to" field size

    int nx2,ny2,nz2;
    field_size_(id_field, &nx2,&ny2,&nz2);

    // get "from" field size

    ghosts_allocated_ = ! ghosts_allocated_;

    int nx1,ny1,nz1;
    field_size_(id_field, &nx1,&ny1,&nz1);

    ghosts_allocated_ = ! ghosts_allocated_;

    // determine offsets to unknowns if ghosts allocated

    int offset1 = (nx1-nx2)/2 + nx1* ( (ny1-ny2)/2 + ny1 * (nz1-nz2)/2 );
    offset1 = MAX (offset1, 0);

    int offset2 = (nx2-nx1)/2 + nx2* ( (ny2-ny1)/2 + ny2 * (nz2-nz1)/2 );
    offset2 = MAX (offset2, 0);

    // determine unknowns size

    int nx = MIN(nx1,nx2);
    int ny = MIN(ny1,ny2);
    int nz = MIN(nz1,nz2);

    // adjust for precision

    int precision_size = 
      cello::precision_size(field_descr_->precision(id_field));

    offset1 *= precision_size;
    offset2 *= precision_size;

    // determine array start

    char * array1 = field_values_from.at(id_field) + offset1;
    char * array2 = field_values_.at(id_field)     + offset2;

    // copy values

    for (int iz=0; iz<nz; iz++) {
      for (int iy=0; iy<ny; iy++) {
	for (int ix=0; ix<nx; ix++) {
	  for (int ip=0; ip<precision_size; ip++) {
	    int i1 = ip + precision_size*(ix + nx1*(iy + ny1*iz));
	    int i2 = ip + precision_size*(ix + nx2*(iy + ny2*iz));
	    array2[i2] = array1[i1];
	  }
	}
      }
    }
  }

  field_values_from.clear();
}
