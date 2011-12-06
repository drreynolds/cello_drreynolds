// See LICENSE_CELLO file for license and copyright information

/// @file     method_Initial.hpp 
/// @author   James Bordner (jobordner@ucsd.edu) 
/// @date     Mon Jul 13 11:11:47 PDT 2009 
/// @brief    [\ref Method] Declaration for the Initial component

#ifndef METHOD_INITIAL_HPP
#define METHOD_INITIAL_HPP

class Initial {

  /// @class    Initial
  /// @ingroup  Method
  /// @brief    [\ref Method] Encapsulate an initial conditions generator

public: // interface

  /// Create a new Initial
  Initial() throw()
  {};

  /// Destructor
  virtual ~Initial() throw()
  {}

public: // virtual functions

  /// Enforce initial conditions on the given Block
  virtual void enforce (const Hierarchy * hierarchy,
			const FieldDescr * field_descr,
			Block * block) throw() = 0;

protected: // attributes

};

#endif /* METHOD_INITIAL_HPP */
