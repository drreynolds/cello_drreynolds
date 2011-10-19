// See LICENSE_CELLO file for license and copyright information

/// @file     method_Method.hpp 
/// @author   James Bordner (jobordner@ucsd.edu) 
/// @date     Mon Jul 13 11:11:47 PDT 2009 
/// @brief    [\ref Method] Declaration for the Method class

#ifndef METHOD_METHOD_HPP
#define METHOD_METHOD_HPP

class Method {

  /// @class    Method
  /// @ingroup  Method
  /// @brief    [\ref Method] Interface to an application method / analysis / visualization function.

public: // interface

  /// Create a new Method
  Method(Parameters * parameters) throw()
  {};

public: // virtual functions

  /// Apply the method to advance a block one timestep 

  virtual void compute_block( FieldDescr * field_descr,
			      Block * block,
			      double t, double dt ) throw() = 0; 

protected: // functions


protected: // attributes

};

#endif /* METHOD_METHOD_HPP */
