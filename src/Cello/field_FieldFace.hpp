// See LICENSE_CELLO file for license and copyright information

/// @file     field_FieldFace.hpp
/// @author   James Bordner (jobordner@ucsd.edu)
/// @date     2011-04-12
/// @brief    [\ref Field] Interface for the FieldFace class

#ifndef FIELD_FIELD_FACE_HPP
#define FIELD_FIELD_FACE_HPP

class FieldFace {

  /// @class    FieldFace
  /// @ingroup  Field
  /// @brief [\ref Field] Class for loading field faces and storing
  /// field ghosts zones

public: // interface

  /// Constructor of uninitialized FieldFace
  FieldFace() throw();

  /// Constructor of initialized FieldFace
  FieldFace (FieldBlock * field_block,
	     const FieldDescr * field_descr) throw();
     
  /// Destructor
  ~FieldFace() throw();

  /// Copy constructor
  FieldFace(const FieldFace & FieldFace) throw();

  /// Assignment operator
  FieldFace & operator= (const FieldFace & FieldFace) throw();

#ifdef CONFIG_USE_CHARM
  /// CHARM++ Pack / Unpack function
  inline void pup (PUP::er &p)
  {

    TRACEPUP;

    // NOTE: change this function whenever attributes change
    p | *field_block_;
    p | *field_descr_;
    p | array_;
    PUParray(p,face_,3);
    PUParray(p,ghost_,3);
  }
#endif
  //----------------------------------------------------------------------

  /// Set whether or not to include ghost zones along each axis
  inline void set_ghost (bool gx, bool gy = true, bool gz = true)
  {
    ghost_[0] = gx;
    ghost_[1] = gy;
    ghost_[2] = gz;
  }

  /// Get whether or not to include ghost zones along each axis
  inline void get_ghost (bool * gx, bool * gy = 0, bool * gz = 0)
  {
    if (gx) (*gx) = ghost_[0];
    if (gy) (*gy) = ghost_[1];
    if (gz) (*gz) = ghost_[2];
  }

  /// Set the face
  inline void set_face (int fx, int fy = 0, int fz = 0)
  {
    face_[0] = fx;
    face_[1] = fy;
    face_[2] = fz;
  }
  
  /// Get the face
  inline void get_face (int * fx, int * fy = 0, int * fz = 0)
  {
    if (fx) (*fx) = face_[0];
    if (fy) (*fy) = face_[1];
    if (fz) (*fz) = face_[2];
  }
  
  /// Load an array with the field face
  void load(int * n, char ** array) throw();


  /// Store the array values to the field's ghost data
  void store(int n, char * array) throw();

  /// Allocate array_ storage
  char * allocate() throw();

  /// Deallocate array_ storage
  void deallocate() throw();

  /// Return the size of the array
  size_t size() const throw() { return array_.size(); };

  /// Return a pointer to the array
  char * array () throw() 
  { return size() > 0 ?  &array_[0] : 0; };

private: // functions

  /// Compute loop limits for load_precision_ and store_precision_
  void loop_limits_
  (int *ix0, int *iy0, int *iz0,
   int *nx,  int *ny,  int *nz,
   int nd3[3], int ng3[3],
   bool load);


  /// Precision-agnostic function for loading field block face into
  /// the field_face array; returns number of bytes copied
  template<class T>
  size_t load_precision_ (T *       array_face, 
			  const T * field_face,
			  int       nd3[3], 
			  int       ng3[3]) throw();

  /// Precision-agnostic function for copying the field_face array into
  /// the field block ghosts; returns number of bytes copied
  template<class T>
  size_t store_precision_ (T *       field_ghosts, 
			   const T * array_ghosts, 
			   int       nd3[3], 
			   int       ng3[3]) throw();

private: // attributes

  /// field block for this face
  FieldBlock * field_block_;

  /// Field descriptor for the field block
  FieldDescr * field_descr_;

  /// Allocated array used for storing all ghosts and face
  std::vector<char> array_;

  /// Selects face: -1 0 1 for each axis (lower, ignore, upper)
  /// e.g. face_ = {1,0,-1) is upper-x, lower-z face
  /// face_ = {0,0,0} is entire FieldBlock
  int face_[3];

  /// Whether to include ghost zones along x,y,z axes
  bool ghost_[3];

};

#endif /* FIELD_FIELD_FACE_HPP */
