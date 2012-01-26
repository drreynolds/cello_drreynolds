// See LICENSE_CELLO file for license and copyright information

/// @file     io_OutputRestart.hpp 
/// @author   James Bordner (jobordner@ucsd.edu) 
/// @date     2011-09-26
/// @brief    [\ref Io] Declaration for the OutputRestart class

#ifndef IO_OUTPUT_RESTART_HPP
#define IO_OUTPUT_RESTART_HPP

class ItField;

class OutputRestart : public Output {

  /// @class    OutputRestart
  /// @ingroup  Io
  /// @brief [\ref Io] class for reading and writing Restart files

public: // functions

  /// Create an uninitialized OutputRestart object
  OutputRestart(const Factory * factory) throw();

  /// OutputRestart destructor
  virtual ~OutputRestart() throw();

public: // virtual functions

  /// Prepare for accumulating block data
  virtual void init () throw();

  /// Open (or create) a file for IO
  virtual void open () throw();

  /// Close file for IO
  virtual void close () throw();

  /// Write hierarchy-related field data
  virtual void write_hierarchy
  ( const FieldDescr * field_descr,
    Hierarchy * hierarchy) throw();

  /// Write patch-related field data; may be called by write_hierarchy
  virtual void write_patch
  ( const FieldDescr * field_descr,
    Patch * patch,
    int ixp0=0, int iyp0=0, int izp0=0) throw();

  /// Write block-related field data; may be called by write_patch
  virtual void write_block
  ( const FieldDescr * field_descr,
    Block * block,
    int ixp0=0, int iyp0=0, int izp0=0) throw();

  /// Prepare local array with data to be sent to remote chare for processing
  virtual void prepare_remote (int * n, char ** buffer) throw();

  /// Accumulate and write data sent from a remote processes
  virtual void update_remote  ( int n, char * buffer) throw();

  /// Free local array if allocated; NOP if not
  virtual void cleanup_remote (int * n, char ** buffer) throw();

private: // functions

  /// Create the png file object
  void png_create_ (std::string filename,
		    int image_size_x,  int image_size_y) throw();

  /// Delete the png object
  void png_close_() throw();

  /// Create the image data object
  void image_create_ (int image_size_x,  int image_size_y) throw();

  /// Generate PNG image, using given min and max for colormap
  void image_write_ (double min, double max) throw();

  /// Close the image data
  void image_close_ () throw();

   /// Generate a PNG image of an array
   template<class T>
   void image_reduce_
   ( T * array,
     int nxd, int nyd, int nzd,   // Array dimensions
     int nx,  int ny,  int nz,   // Array dimensions
     int nx0, int ny0, int nz0,  // Array offset into image
     axis_enum   axis,           // Axis along which to project
     reduce_enum op_reduce) throw();

private: // attributes

  /// Color map
  std::vector<double> map_r_;
  std::vector<double> map_g_;
  std::vector<double> map_b_;

  /// Current image
  double * image_;

  /// Current image size
  int image_size_x_;
  int image_size_y_;

  /// Current pngwriter
  pngwriter * png_;


};

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

   // Remap array axes to image axes iax,iay

   int iax = (axis+1) % 3;  // image x axis
   int iay = (axis+2) % 3;  // image y-axis
   int iaz = axis;          // reduction axis

   // Array size permuted to match image

   int npx = n[iax];
   int npy = n[iay];
   int npz = n[iaz];

   // Array start permuted to match image

   int npx0 = n0[iax];
   int npy0 = n0[iay];

   // Loop over array subsection

   // image x-axis

   double min=std::numeric_limits<double>::max();
   double max=std::numeric_limits<double>::min();

   for (int index_array_x=0; index_array_x<npx; index_array_x++) {

     int index_image_x = npx0 + index_array_x;

     // image y-axis

     for (int index_array_y=0; index_array_y<npy; index_array_y++) {
      
       int index_image_y = npy0 + index_array_y;

       int index_image = index_image_x + image_size_x_*index_image_y;

       if ( ! ( ( index_image_x < image_size_x_) &&
		 (index_image_y < image_size_y_)) ) {
	 ERROR5 ("OutputImage::image_reduce_",
		 "Invalid Access axis %d index(%d %d)  image(%d %d)\n",
		 axis, index_image_x, index_image_y, image_size_x_,image_size_y_);
       }

       double & pixel_value = image_ [index_image];

       // reduction axis

       // initialize reduction
       switch (op_reduce) {
       case reduce_min: 
	 pixel_value = std::numeric_limits<double>::max();
	 break;
       case reduce_max: 
	 pixel_value = std::numeric_limits<double>::min();
	 break;
       case reduce_avg: 
       case reduce_sum: 
       default:         
	 pixel_value = 0; break;
       }

       // reduce along axis
       for (int iz=0; iz<npz; iz++) {
	
	 int index_array = 
	   nd3[iax]*index_array_x + 
	   nd3[iay]*index_array_y + 
	   nd3[iaz]*iz;

	 // reduce along iaz axis

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
       min=MIN(pixel_value,min);
       max=MAX(pixel_value,max);

     }
   }
 }

#endif /* IO_OUTPUT_IMAGE_HPP */
