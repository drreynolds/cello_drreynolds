// $Id$
// See LICENSE_CELLO file for license and copyright information

#ifndef METHOD_HPP
#define METHOD_HPP

/// @file     method.hpp
/// @author   James Bordner (jobordner@ucsd.edu)
/// @date     2010-04-02
/// @brief    Include file for the \ref Method component

#include <vector>
#include <string>

#include "cello.hpp"

#include "error.hpp"
#include "mesh.hpp"

/// @enum argument_enum
/// @brief type of Method argument
enum argument_enum {
  argument_unknown,
  argument_field,
  argument_particle
};

/// @enum access_enum
/// @brief access restrictions for Method argument
enum access_enum {
  access_unknown,
  access_read,
  access_write,
  access_read_write
};

#include "method_MethodDescr.hpp"

#include "method_Control.hpp"
#include "method_Timestep.hpp"
#include "method_Initial.hpp"

#include "method_MethodHyperbolic.hpp"


#endif /* METHOD_HPP */

