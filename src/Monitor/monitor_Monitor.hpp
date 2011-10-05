// See LICENSE_CELLO file for license and copyright information

//----------------------------------------------------------------------
/// @file     monitor_Monitor.hpp
/// @author   James Bordner (jobordner@ucsd.edu)
/// @date     2009-10-05
/// @brief    [\ref Monitor] Declaration of the Monitor class
//----------------------------------------------------------------------
#ifndef MONITOR_MONITOR_HPP
#define MONITOR_MONITOR_HPP

//----------------------------------------------------------------------
/// @def    MONITOR_LENGTH 255
/// @brief  Maximum length of monitor text output

#define MONITOR_LENGTH 255
   
//----------------------------------------------------------------------
class Timer; 

class Monitor {

  /// @class    Monitor
  /// @ingroup  Monitor
  /// @todo     Make calling image() easier
  /// @todo     Add configuration parameters, e.g. monitoring level, date/time output, format, etc.
  /// @todo     Add calling component to print
  /// @brief    [\ref Monitor] User monitoring of simulation execution status
  ///
  /// The Monitor component is used to communicate information about
  /// the running simulation to the user. Information can be output in
  /// several forms, including text files, HTML files, plots, or other
  /// (generally small) image files. Information is assumed to be from
  /// a correctly-running simulation: anomalous errors or warnings are
  /// output by the Error component.

  //----------------------------------------------------------------------

private:

  friend class Simulation;

  /// Private constructor of the Monitor object [singleton design pattern]
  Monitor();

  /// Private destructor  of the Monitor object [singleton design pattern]
  ~Monitor();

//----------------------------------------------------------------------

public: // interface

  /// Return an instance of a Monitor object
  static Monitor * instance()
  { 
    return & instance_;
    // if ( instance_ == NULL )
    //   instance_ = new Monitor;

    // return instance_;
  };

  /// Set whether the monitor is active for text output.  Useful for
  /// parallel, e.g. "monitor->set_active(parallel->is_root())"
  void set_active(bool active) { active_ = active; };

  /// Return whether monitoring is active
  bool is_active() const throw () { return active_; };

  /// Print the Cello header 
  void header () const;

  /// Print a message to stdout
  void print (const char * buffer, ...) const;

  /// Write a message to file
  void write (FILE * fp, const char * buffer, ...) const;

  /// Generate a PNG image of an array
  void image_open (std::string filename, 
		   int image_size_x,  int image_size_y);

   /// Generate a PNG image of an array
   template<class T>
   void image_reduce 
   ( T * array,
     int nxd, int nyd, int nzd,   // Array dimensions
     int nx,  int ny,  int nz,   // Array dimensions
     int nx0, int ny0, int nz0,  // Array offset into image
     axis_enum   axis,           // Axis along which to project
     reduce_enum op_reduce);


  /// Generate PNG image, using given min and max for colormap
  void image_close (double min, double max);

  /// Generate a PNG image of an array
  template<class T>
  void image 
  ( std::string name, 
    int mx, int my,             // image size
    T * array,
    int nxd, int nyd, int nzd,   // Array dimensions
    int nx,  int ny,  int nz,   // Array size
    int nx0, int ny0, int nz0,  // Array offset into image
    axis_enum   axis,           // Axis along which to project
    reduce_enum op_reduce,      // Reduction operation along axis
    double min, double max     // Limits for color map
    );

  void image_set_map 
  (int n, double * map_r, double * map_g, double * map_b) throw();

  //----------------------------------------------------------------------

private: // functions


  //----------------------------------------------------------------------

private: // attributes

  /// Timer for keeping track of time for output
  Timer * timer_; 

  /// Whether monitoring is activated.  Used for e.g. ip != 0.
  bool active_;

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

  //----------------------------------------------------------------------

private: // static attributes

  /// Single instance of the Monitor object [singleton design pattern]
  // static Monitor * instance_;
  static Monitor instance_;

};

//----------------------------------------------------------------------

 template<class T>
 void Monitor::image_reduce
 (T * array, 
  int nxd, int nyd, int nzd,
  int nx,  int ny,  int nz,
  int nx0, int ny0, int nz0,
  axis_enum   axis, 
  reduce_enum op_reduce)
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
  
   for (int index_array_x=0; index_array_x<npx; index_array_x++) {

     int index_image_x = npx0 + index_array_x;

     // image y-axis

     for (int index_array_y=0; index_array_y<npy; index_array_y++) {
      
       int index_image_y = npy0 + index_array_y;

       int index_image = index_image_x + image_size_x_*index_image_y;

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
     }
   }
 }

 //----------------------------------------------------------------------

//----------------------------------------------------------------------

template<class T>
void Monitor::image
(std::string filename, 
 int mx, int my,
 T * array, 
 int nxd,  int nyd,  int nzd,
 int nx,   int ny,   int nz,
 int nx0, int ny0,   int nz0,
 axis_enum axis, reduce_enum op_reduce,
 double min, double max)

/**
*********************************************************************
*
* @param  filename     File name
* @param  mx,my        Size of the image
* @param  array        Array of values to plot
* @param  nxd,nyd,nzd  Dimension of the array
* @param  nx,ny,nz     Size of the array
* @param  nx0,ny0,nz0  Starting index of the array in the image
* @param  axis         Which axis to reduce
* @param  op_reduce    Reduction operator
* @param  min,max      Bounds for color map values
*
* Plot an array as a png file
*
*********************************************************************
*/
{

  // n array size
  // m image size

  // k colormap index

  // Open the image

  image_open(filename,mx,my);

  image_reduce(array,
	       nxd,nyd,nzd,
	       nx,ny,nz,
	       nx0,ny0,nz0,
	       axis,op_reduce);

  // close the image

  image_close(min,max);
}

#endif /* MONITOR_MONITOR_HPP */

