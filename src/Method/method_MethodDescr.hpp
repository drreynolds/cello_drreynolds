// $Id$
// See LICENSE_CELLO file for license and copyright information

#ifndef METHOD_METHOD_DESCR_HPP
#define METHOD_METHOD_DESCR_HPP

/// @file     method_MethodDescr.hpp
/// @author   James Bordner (jobordner@ucsd.edu)
/// @date     2010-05-11
/// @todo     consolidate different method types into one Method* list
/// @todo     add accessor functions
/// @brief    [\ref Method] Declaration of the MethodDescr class

class MethodDescr {

  /// @class    MethodDescr
  /// @ingroup  Method
  /// @brief    [\ref Method] Top-level container for user-implemented numerical components

public: // interface

  /// Constructor
  MethodDescr(Error   * error,
	      Monitor * monitor) throw()
    : method_control_(0),
      method_timestep_(0),
      method_hyperbolic_(0),
      error_(error),
      monitor_(monitor)
  {

    // Set "default" method control and timestepping routines

  };

  /// Destructor
  ~MethodDescr() throw()
  {
    delete method_control_;
    delete method_timestep_;
    for (size_t i=0; i<method_hyperbolic_.size(); i++) {
      delete method_hyperbolic_[i];
    }
  }


protected: // functions

  /// APPLICATION INHERITENCE OVERRIDE: Create named control method.
  virtual Control * create_method_control_ (std::string name_method_control) = 0;

  /// APPLICATION INHERITENCE OVERRIDE: Create named timestep method.
  virtual Timestep * create_method_timestep_ (std::string name_method_timestep) = 0;

  /// APPLICATION INHERITENCE OVERRIDE: Create named hyperbolic method.
  virtual MethodHyperbolic * create_method_hyperbolic_ (std::string name_method_hyperbolic) = 0;

protected: // attributes

  Control *                       method_control_;
  Timestep *                      method_timestep_;
  std::vector<MethodHyperbolic *> method_hyperbolic_;

  Error *                         error_;
  Monitor *                       monitor_;
};

#endif /* METHOD_METHOD_DESCR_HPP */

