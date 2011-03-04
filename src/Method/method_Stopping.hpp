// $Id$
// See LICENSE_CELLO file for license and copyright information

#ifndef METHOD_STOPPING_HPP
#define METHOD_STOPPING_HPP

/// @file     method_Stopping.hpp
/// @author   James Bordner (jobordner@ucsd.edu)
/// @date     2010-05-11
/// @brief    [\ref Method] Declaration of the Stopping class

class Stopping {

  /// @class    Stopping
  /// @ingroup  Method
  /// @brief    [\ref Method] Encapsulate stopping criteria

public: // interface

  /// Constructor
  Stopping() throw() {};

  /// Return whether the simulation is done
  virtual bool complete () throw() = 0;

protected:

};

#endif /* METHOD_STOPPING_HPP */

