// See LICENSE_CELLO file for license and copyright information

/// @file     enzo_EnzoBoundary.hpp 
/// @author   James Bordner (jobordner@ucsd.edu) 
/// @date     Mon Mar 14 12:02:24 PDT 2011
/// @brief    [\ref Enzo] Declaration for the EnzoBoundary component

#ifndef ENZO_ENZO_BOUNDARY_HPP
#define ENZO_ENZO_BOUNDARY_HPP

//----------------------------------------------------------------------

/// @enum     boundary_type_enum
/// @brief    External boundary condition types
enum boundary_enum {
  boundary_type_undefined,   // 0 is an undefined boundary
  boundary_type_reflecting,
  boundary_type_outflow,
  boundary_type_inflow,
  boundary_type_periodic,
  boundary_type_file, // boundary conditions to be read from a file
  boundary_type_code  // boundary conditions to be computed from code
};
typedef int boundary_type;

//----------------------------------------------------------------------

class EnzoBoundary : public Boundary {

  /// @class    EnzoBoundary
  /// @ingroup  Enzo
  /// @brief    [\ref Enzo] Encapsulate enforcement of boundary conditions

public: // interface

  /// Create a new EnzoBoundary
  EnzoBoundary(boundary_type boundary_type) throw();

  /// Charm++ PUP::able declarations
  PUPable_decl(EnzoBoundary);
  
  /// Charm++ PUP::able migration constructor
  EnzoBoundary (CkMigrateMessage *m) {}

  /// CHARM++ Pack / Unpack function
  void pup (PUP::er &p);
  
public: // virtual functions

  /// Enforce boundary conditions on block for a subet of faces
  virtual void enforce ( const FieldDescr * field_descr,
			 CommBlock * block,
			 face_enum face = face_all,
			 axis_enum axis = axis_all) const throw(); 

  /// Whether boundary conditions are periodic (handled by ghost refresh)
  virtual bool is_periodic() const throw()
  { return boundary_type_ == boundary_type_periodic; };

protected: // functions

  //--------------------------------------------------

  /// Enforce reflecting boundary conditions on a boundary face
  void enforce_reflecting_
  ( const FieldDescr * field_descr,
    FieldBlock * field_block,
    face_enum face, 
    axis_enum axis) const throw();

  /// Template for reflecting boundary conditions on different precisions
  template<class T>
  void enforce_reflecting_precision_
  ( face_enum face,
    axis_enum axis,
    T * array,
    int nx,int ny,int nz,
    int gx,int gy,int gz,
    bool vx,bool vy,bool vz) const throw();

  //--------------------------------------------------

  /// Enforce outflow boundary conditions on a boundary face
  void enforce_outflow_
  ( const FieldDescr * field_descr,
    FieldBlock * field_block, 
    face_enum face, 
    axis_enum axis) const throw();

  /// Template for outflow boundary conditions on different precisions
  template<class T>
  void enforce_outflow_precision_
  ( face_enum face,
    axis_enum axis,
    T * array,
    int nx,int ny,int nz,
    int gx,int gy,int gz) const throw();

  //--------------------------------------------------

  /// Enforce inflow boundary conditions on a boundary face
  void enforce_inflow_
  ( const FieldDescr * field_descr,
    FieldBlock * field_block, 
    face_enum face, 
    axis_enum axis) const throw();

protected: // attributes

  // Type of boundary conditions
  boundary_type boundary_type_;

};

#endif /* ENZO_ENZO_BOUNDARY_HPP */
