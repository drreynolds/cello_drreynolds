// See LICENSE_CELLO file for license and copyright information

/// @file     io_OutputImage.cpp
/// @author   James Bordner (jobordner@ucsd.edu)
/// @date     Thu Mar 17 11:14:18 PDT 2011
/// @brief    Implementation of the OutputImage class

#include "cello.hpp"

#include "io.hpp"

//----------------------------------------------------------------------

OutputImage::OutputImage(const Factory * factory,
			 int process_count,
			 int nrows, int ncols) throw ()
  : Output(factory),
    data_(),
    axis_(axis_z),
    nrows_(nrows),
    ncols_(ncols),
    png_(0)

{
  // Override default Output::process_stride_: only root writes
  process_stride_ = process_count;

  map_r_.resize(2);
  map_g_.resize(2);
  map_b_.resize(2);
  map_a_.resize(2);

  map_r_[0] = 0.0;
  map_g_[0] = 0.0;
  map_b_[0] = 0.0;
  map_a_[0] = 1.0;

  map_r_[1] = 1.0;
  map_g_[1] = 1.0;
  map_b_[1] = 1.0;
  map_a_[1] = 1.0;
}

//----------------------------------------------------------------------

OutputImage::~OutputImage() throw ()
{
}

//----------------------------------------------------------------------

void OutputImage::set_colormap
(int n, double * map_r, double * map_g, double * map_b, double * map_a) throw()
{
  // Copy rbg lists

  map_r_.resize(n);
  map_g_.resize(n);
  map_b_.resize(n);
  map_a_.resize(n);

  for (int i=0; i<n; i++) {
    map_r_[i] = map_r[i];
    map_g_[i] = map_g[i];
    map_b_[i] = map_b[i];
    map_a_[i] = map_a ? map_a[i] : 1.0;
  }

}

//======================================================================

void OutputImage::init () throw()
{
  image_create_();
}

//----------------------------------------------------------------------

void OutputImage::open () throw()
{
  // Open file if writing a single block
  std::string file_name = expand_file_name();

  if (is_writer()) {
    // Create png object
    Monitor::instance()->print ("Output","writing image file %s", 
			      file_name.c_str());
    png_create_(file_name);
  }
}

//----------------------------------------------------------------------

void OutputImage::close () throw()
{
  if (is_writer()) image_write_();
  image_close_();
  png_close_();
}

//----------------------------------------------------------------------

void OutputImage::finalize () throw()
{
  Output::finalize();
}


//----------------------------------------------------------------------

void OutputImage::write_hierarchy
(
 const FieldDescr * field_descr,
 Hierarchy * hierarchy
 ) throw()
{
  ItPatch it_patch (hierarchy);
  while (Patch * patch = ++it_patch) {
    // NO OFFSET: ASSUMES ROOT PATCH
    write_patch (field_descr, patch,  0,0,0);
  }

}

//----------------------------------------------------------------------

void OutputImage::write_patch
(
 const FieldDescr * field_descr,
 Patch * patch,
 int ixp0,
 int iyp0,
 int izp0
 ) throw()
// @param field_descr  Field descriptor
// @param patch        Patch to output
// @param ixp0  offset of the patch relative to the parent patch along x-axis 
// @param iyp0  offset of the patch relative to the parent patch along y-axis
// @param izp0  offset of the patch relative to the parent patch along z-axis
{

  /// Call write_patch() on parent Output class
  Output::write_patch(field_descr,patch,ixp0,iyp0,izp0);

}

//----------------------------------------------------------------------

void OutputImage::write_block
(
 const FieldDescr * field_descr,
 Block * block,
 int ixp0,
 int iyp0,
 int izp0
) throw()
// @param field_descr  Field descriptor
// @param patch        Patch to output
// @param ixp0  offset of the parent patch relative to its parent along x-axis 
// @param iyp0  offset of the parent patch relative to its parent along y-axis 
// @param izp0  offset of the parent patch relative to its parent along z-axis 
{

  FieldBlock * field_block = block->field_block();
  
  int nxb,nyb,nzb;
  field_block->size(&nxb,&nyb,&nzb);

  int ix,iy,iz;
  block->index_patch(&ix,&iy,&iz);

  int ixb0 = ixp0 + ix*nxb;
  int iyb0 = iyp0 + iy*nyb;
  int izb0 = izp0 + iz*nzb;

  // Index of (single) field to write

  it_field_->first();

  int index = it_field_->value();

    // Get ghost depth

  int gx,gy,gz;
  field_descr->ghosts(index,&gx,&gy,&gz);

  // Get array dimensions

  int ndx,ndy,ndz;
  ndx = nxb + 2*gx;
  ndy = nyb + 2*gy;
  ndz = nzb + 2*gz;

  // Add block contribution to image

  char * field_unknowns = field_block->field_unknowns(field_descr,index);

  switch (field_descr->precision(index)) {

  case precision_single:

    image_reduce_ (((float *) field_unknowns),
		   ndx,ndy,ndz, 
		   nxb,nyb,nzb,
		   ixb0,iyb0,izb0,
		   axis_, reduce_sum);
    break;

  case precision_double:

    image_reduce_ (((double *) field_unknowns),
		   ndx,ndy,ndz, 
		   nxb,nyb,nzb,
		   ixb0,iyb0,izb0,
		   axis_, reduce_sum);
    break;

  default:

    WARNING1 ("OutputImage::write_block",
	     "Unsupported Field precision %d",
	      field_descr->precision(index));
    break;

  }

}

//----------------------------------------------------------------------

void OutputImage::write_field
  ( const FieldDescr * field_descr,
    FieldBlock * Fieldblock, int field_index) throw()
{
  WARNING("OutputImage::write_field()",
	  "This function should not be called");
}

//----------------------------------------------------------------------

void OutputImage::prepare_remote (int * n, char ** buffer) throw()
{

  int size = 0;
  int nx = nrows_;
  int ny = ncols_;

  // Determine buffer size

  size += nx*ny*sizeof(double);
  size += 2*sizeof(int);
  (*n) = size;

  // Allocate buffer (deallocated in cleanup_remote())
  (*buffer) = new char [ size ];

  union {
    char   * c;
    double * d;
    int    * i;
  } p ;

  p.c = (*buffer);

  *p.i++ = nx;
  *p.i++ = ny;

  for (int k=0; k<nx*ny; k++) *p.d++ = data_[k];
  
}

//----------------------------------------------------------------------

void OutputImage::update_remote  ( int n, char * buffer) throw()
{

  union {
    char   * c;
    double * d;
    int    * i;
  } p ;

  p.c = buffer;

  int nx = *p.i++;
  int ny = *p.i++;

  for (int k=0; k<nx*ny; k++) data_[k] += *p.d++;

}

//----------------------------------------------------------------------

void OutputImage::cleanup_remote  (int * n, char ** buffer) throw()
{
  delete [] (*buffer);
  (*buffer) = NULL;
}


//======================================================================

void OutputImage::png_create_ (std::string filename) throw()
{
  if (is_writer()) {
    const char * file_name = strdup(filename.c_str());
    png_ = new pngwriter(nrows_, ncols_,0,file_name);
    free ((void *)file_name);
  }
}

//----------------------------------------------------------------------

void OutputImage::png_close_ () throw()
{
  if (is_writer()) {
    png_->close();
    delete png_;
    png_ = 0;
  }
}

//----------------------------------------------------------------------

void OutputImage::image_create_ () throw()
{

  ASSERT("OutputImage::image_create_",
	 "image_ already created",
	 data_ == NULL);

  data_ = new double [nrows_*ncols_];

  for (int i=0; i<nrows_*ncols_; i++) data_[i] = 0.0;
}

//----------------------------------------------------------------------

void OutputImage::image_write_ (double min, double max) throw()
{

  // error check min <= max

  ASSERT2("OutputImage::image_write_",
	 "min %g is greater than max %g",
	 min,max, (min <= max));

  // simplified variable names

  int mx = nrows_;
  int my = ncols_;
  int m  = mx*my;

  // Scale by data if min == max (default)

  if (min == max) {
    min = std::numeric_limits<double>::max();
    max = std::numeric_limits<double>::min();
  }

  // Ensure min and max fully enclose data
  for (int i=0; i<m; i++) {
    min = MIN(min,data_[i]);
    max = MAX(max,data_[i]);
  }

  size_t n = map_r_.size();

  // loop over pixels (ix,iy)

  for (int ix = 0; ix<mx; ix++) {

    for (int iy = 0; iy<my; iy++) {

      int i = ix + mx*iy;

      double value = data_[i];

      double r=0.0,g=0.0,b=0.0,a=0.0;

      if (min <= value && value <= max) {

	// map v to lower colormap index
	size_t k = size_t((n - 1)*(value - min) / (max-min));

	// prevent k == map_.size()-1, which happens if value == max

	if (k > n - 2) k = n-2;

	// linear interpolate colormap values
	double lo = min +  k   *(max-min)/(n-1);
	double hi = min + (k+1)*(max-min)/(n-1);

	// should be in bounds, but force if not due to rounding error
	if (value < lo) value = lo;
	if (value > hi) value = hi;

	// interpolate colormap

	double ratio = (value - lo) / (hi-lo);

	r = (1-ratio)*map_r_[k] + ratio*map_r_[k+1];
	g = (1-ratio)*map_g_[k] + ratio*map_g_[k+1];
	b = (1-ratio)*map_b_[k] + ratio*map_b_[k+1];
	a = (1-ratio)*map_a_[k] + ratio*map_a_[k+1];
      }

      // Plot pixel
      png_->plot(ix+1, iy+1, 0.0, 0.0, 0.0);
      png_->plot_blend(ix+1, iy+1, a, r,g,b);
    }
  }      

}

//----------------------------------------------------------------------

void OutputImage::image_close_ () throw()
{
  ASSERT("OutputImage::image_create_",
	 "image_ already created",
	 data_ != NULL);
  delete [] data_;
  data_ = 0;
}

//----------------------------------------------------------------------

template<class T>
void OutputImage::image_reduce_
(T * array, 
 int nxd, int nyd, int nzd,
 int nx,  int ny,  int nz,
 int nx0, int ny0, int nz0,
 axis_enum   axis, 
 reduce_enum op_reduce) throw()
{
  // Array multipliers

  int nd3[3] = {1, nxd, nxd*nyd}; 

  // Array size

  int n[3]  = {nx,  ny,  nz};
  int n0[3] = {nx0, ny0, nz0};

  // Remap array axes to image axes axis_x,axis_y

  int axis_x = (axis+1) % 3;  // image x axis
  int axis_y = (axis+2) % 3;  // image y-axis

  // Array size permuted to match image

  int npx = n[axis_x];
  int npy = n[axis_y];
  int npz = n[axis];

  // Array start permuted to match image

  int npx0 = n0[axis_x];
  int npy0 = n0[axis_y];

  // Loop over array subsection

  // image x-axis

  for (int iax=0; iax<npx; iax++) {

    int iix = npx0 + iax;

    // image y-axis

    for (int iay=0; iay<npy; iay++) {
      
      int iiy = npy0 + iay;

      int index_image = iix + nrows_*iiy;

      if ( ! ( (iix < nrows_) &&
	       (iiy < ncols_)) ) {
	ERROR5 ("OutputImage::image_reduce_",
		"Invalid Access axis %d index(%d %d)  image(%d %d)\n",
		axis, iix, iiy, nrows_,ncols_);
      }

      double & pixel_value = data_ [index_image];

      double min = std::numeric_limits<double>::max();
      double max = std::numeric_limits<double>::min();

      // reduction axis

      // initialize reduction
      switch (op_reduce) {
      case reduce_min: 
	pixel_value = max;
	break;
      case reduce_max: 
	pixel_value = min;
	break;
      case reduce_avg: 
      case reduce_sum: 
      default:         
	pixel_value = 0; break;
      }

      // reduce along axis
      for (int iz=0; iz<npz; iz++) {
	
	int index_array = 
	  nd3[axis_x]*iax + 
	  nd3[axis_y]*iay + 
	  nd3[axis]*iz;

	// reduce along z axis

	switch (op_reduce) {
	case reduce_min: 
	  pixel_value = MIN(array[index_array],(T)(pixel_value)); 
	  break;
	case reduce_max: 
	  pixel_value = MAX(array[index_array],(T)(pixel_value)); 
	  break;
	case reduce_avg: 
	case reduce_sum: 
	  pixel_value += array[index_array]; break;
	default:
	  break;
	}
      }

      if (op_reduce == reduce_avg) pixel_value /= npz;

    }

  }
}

