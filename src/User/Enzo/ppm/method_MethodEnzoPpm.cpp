// $Id: method_MethodEnzoPpm.cpp 1262 2010-03-03 15:44:05Z bordner $
// See LICENSE_ENZO file for license and copyright information

/// @file     method_MethodEnzoPpm.cpp
/// @author   James Bordner (jobordner@ucsd.edu)
/// @date     Fri Apr  2 17:05:23 PDT 2010
/// @brief    Implements the MethodEnzoPpm class

#include "method_MethodEnzoPpm.hpp"
#include "error.hpp"

void MethodEnzoPpm::initialize(std::string method_name) throw()
{
  // Register method name

  method_name_ = method_name;

  // Specify arguments

  add_argument_(argument_type_field, "density",        access_type_read_write);
  add_argument_(argument_type_field, "energy_total",   access_type_read_write);
  add_argument_(argument_type_field, "energy_internal",access_type_read_write);
  add_argument_(argument_type_field, "velocity_x",     access_type_read_write);
  add_argument_(argument_type_field, "velocity_y",     access_type_read_write);
  add_argument_(argument_type_field, "velocity_z",     access_type_read_write);

  // Initialize from parameters

  
}

void MethodEnzoPpm::apply() throw()
{
  INCOMPLETE_MESSAGE("MethodEnzoPpm::apply","");
}
