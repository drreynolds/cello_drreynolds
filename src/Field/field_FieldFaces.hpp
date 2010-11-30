// $Id$
// See LICENSE_CELLO file for license and copyright information

#ifndef FIELD_FIELD_FACES_HPP
#define FIELD_FIELD_FACES_HPP

/// @file     field_FieldFaces.hpp
/// @author   James Bordner (jobordner@ucsd.edu)
/// @date     Thu Feb 25 16:20:17 PST 2010
/// @brief    Interface for the FieldFaces class

class FieldFaces {

  /// @class    FieldFaces
  /// @ingroup  Field
  /// @brief    Class for representing and operating on ghost zones

private:

  /// FieldFaces must be allocated with field_block argument

  FieldFaces() throw();

public: // interface

  /// Constructor
  FieldFaces(FieldBlock * field_block) throw();

  //----------------------------------------------------------------------
  // Big Three
  //----------------------------------------------------------------------

  /// Destructor
  ~FieldFaces() throw();

  /// Copy constructor
  FieldFaces(const FieldFaces & FieldFaces) throw();

  /// Assignment operator
  FieldFaces & operator= (const FieldFaces & FieldFaces) throw();

  //----------------------------------------------------------------------

  /// 	Initialize sending face zones to another block patch
  void send_init() throw();
  /// 	Begin sending face zones to another block patch
  void send_begin() throw();
  /// 	End sending face zones to another block patch
  void send_end() throw();
  /// 	Finalize sending face zones to another block patch
  void send_final() throw();
	
  /// 	Initialize receiving ghost zones from another block patch
  void recv_init() throw();
  /// 	Begin receiving ghost zones from another block patch
  void recv_begin() throw();
  /// 	End receiving ghost zones from another block patch
  void recv_end() throw();
  /// 	Finalize receiving ghost zones from another block patch
  void recv_final() throw();
	
  /// 	Initialize sending/receiving face/ghost zones from another block patch
  void sendrecv_init() throw();
  /// 	Begin sending/receiving face/ghost zones from another block patch
  void sendrecv_begin() throw();
  /// 	End sending/receiving face/ghost zones from another block patch
  void sendrecv_end() throw();
  /// 	Finalize sending/receiving face/ghost zones from another block patch
  void sendrecv_final() throw();


private: // functions

  
private: // attributes

  /// Pointer to the corresponding FieldBlock
  FieldBlock * field_block_;

  /// Allocated arrays [xm,xp,ym,yp,zm,zp] of ghost values (0 if none)
  char * ghost_[6];

  /// Allocated arrays [xm,xp,ym,yp,zm,zp] of inner-face values (0 if none)
  char * face_[6];

};

#endif /* FIELD_FIELD_FACES_HPP */
