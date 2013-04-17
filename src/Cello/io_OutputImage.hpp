// See LICENSE_CELLO file for license and copyright information

/// @file     io_OutputImage.hpp 
/// @author   James Bordner (jobordner@ucsd.edu) 
/// @date     Mon Mar 14 17:35:56 PDT 2011
/// @brief    [\ref Io] Declaration for the OutputImage class

#ifndef IO_OUTPUT_IMAGE_HPP
#define IO_OUTPUT_IMAGE_HPP

class Factory;
class FieldDescr;

class OutputImage : public Output {

  /// @class    OutputImage
  /// @ingroup  Io
  /// @brief [\ref Io] class for writing images

public: // functions

  /// Empty constructor for Charm++ pup()
  OutputImage() throw() {}

  /// Create an uninitialized OutputImage object
  OutputImage(int index,
	      const Factory * factory,
	      int process_count,
	      int nrows, int ncols) throw();

  /// OutputImage destructor: free allocated image data
  virtual ~OutputImage() throw();

#ifdef CONFIG_USE_CHARM

  /// Charm++ PUP::able declarations
  PUPable_decl(OutputImage);

  /// Charm++ PUP::able migration constructor
  OutputImage (CkMigrateMessage *m) : Output (m) {}

  /// CHARM++ Pack / Unpack function
  void pup (PUP::er &p);

#endif

  // Set the image colormap
  void set_colormap
  (int n, double * map_r, double * map_g, double * map_b, double * map_a=0) throw();

  // Set the axis for projecting
  void set_axis (axis_type axis) throw()
  { axis_ = axis; };


public: // virtual functions

  /// Prepare for accumulating block data
  virtual void init () throw();

  /// Open (or create) a file for IO
  virtual void open () throw();

  /// Close file for IO
  virtual void close () throw();

  /// Cleanup after output
  virtual void finalize () throw();

  /// Write block-related field data
  virtual void write_block
  ( const CommBlock * block,
    const FieldDescr * field_descr,
    int ixp0=0, int iyp0=0, int izp0=0) throw();

  /// Write fields
  virtual void write_field_block
  ( const FieldBlock * Fieldblock, 
    const FieldDescr * field_descr,
    int field_index) throw();

  /// Prepare local array with data to be sent to remote chare for processing
  virtual void prepare_remote (int * n, char ** buffer) throw();

  /// Accumulate and write data sent from a remote processes
  virtual void update_remote  ( int n, char * buffer) throw();

  /// Free local array if allocated; NOP if not
  virtual void cleanup_remote (int * n, char ** buffer) throw();

private: // functions

  /// Create the png file object
  void png_create_ (std::string filename) throw();

  /// Delete the png object
  void png_close_() throw();

  /// Create the image data object
  void image_create_ () throw();

  /// Generate PNG image, using given min and max for colormap
  void image_write_ (double min=0.0, double max=0.0) throw();

  /// Close the image data
  void image_close_ () throw();

   /// Generate a PNG image of an array
   template<class T>
   void image_reduce_
   ( T * array,
     int nxd, int nyd, int nzd,   // Array dimensions
     int nx,  int ny,  int nz,   // Array dimensions
     int nx0, int ny0, int nz0) throw();

private: // attributes

  /// Color map
  std::vector<double> map_r_;
  std::vector<double> map_g_;
  std::vector<double> map_b_;
  std::vector<double> map_a_;

  /// Current image
  double * data_;

  /// Reduction operation
  reduce_type op_reduce_;

  /// Axis along which to reduce
  axis_type axis_;

  /// Current image columns
  int nrows_;

  /// Current image rows
  int ncols_;

  /// Current pngwriter
  pngwriter * png_;


};

#endif /* IO_OUTPUT_IMAGE_HPP */
