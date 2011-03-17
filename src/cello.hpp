// $Id$
// See LICENSE_CELLO file for license and copyright information

#ifndef CELLO_HPP
#define CELLO_HPP

/// @file    cello.hpp
/// @author  James Bordner (jobordner@ucsd.edu)
/// @date    Thu Nov 11 17:08:38 PST 2010
/// @todo    Need face_axis_enum?
/// @todo    Move boundary enum to Boundary class after b.c. removed from Field
/// @brief   Include Cello global configuration settings

#include "cello_config.def"
#include "cello_macros.hpp"
#include "cello_precision.hpp"

/*********************************************************************
 * PROBLEM DECLARATIONS
 **********************************************************************/


/// @enum     face_axis_enum
/// @brief    Face [lower|upper][x|y|z]
enum face_axis_enum {
  face_lower_axis_x = 0,
  face_upper_axis_x = 1,
  face_lower_axis_y = 2,
  face_upper_axis_y = 3,
  face_lower_axis_z = 4,
  face_upper_axis_z = 5,
  face_axis_all
};

/// @enum     face_enum
/// @brief    Face [lower|upper]
enum face_enum {
  face_lower = 0,
  face_upper = 1,
  face_all
};

/// @enum     axis_enum
/// @brief    Axis [x|y|z]
enum axis_enum {
  axis_x = 0,
  axis_y = 1,
  axis_z = 2,
  axis_all
};


/*********************************************************************
 * COMPONENTS
 **********************************************************************/

enum component_enum {
  // !!! EDIT component_enum AND component_name TOGETHER !!!
  component_undefined,
  component_enzop,
  component_first = component_enzop,
  component_disk,
  component_error,
  component_field,
  component_memory,
  component_mesh,
  component_method,
  component_monitor,
  component_parallel,
  component_parameters,
  component_particles,
  component_performance,
  component_portal,
  component_simulation,
  num_components = component_simulation
};

extern const char * component_name [];

#endif /* CELLO_HPP */
