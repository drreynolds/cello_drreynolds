// $Id$
// See LICENSE_CELLO file for license and copyright information

#ifndef CELLO_HPP
#define CELLO_HPP

/// @file     cello.hpp
/// @author   James Bordner (jobordner@ucsd.edu)
/// @date     Thu Nov 11 17:08:38 PST 2010
/// @brief    Include Cello global configuration settings

#include "cello_config.def"

#include "cello_macros.hpp"
#include "cello_precision.hpp"

/*********************************************************************
 * PERFORMANCE DECLARATIONS
 **********************************************************************/

/* Performance attributes */

enum attribute_enum {
  attribute_undefined, // 0 is an undefined attribute
  attribute_timestep,  // Simulation timesteps [monotonic]
  attribute_level,     // AMR hierarchy level
  attribute_component, // software component [memory]
  attribute_function,  // code function
  num_attributes = attribute_function // Number of attribute types
};

/* Performance counters */

enum counter_enum {
  counter_undefined, // 0 is an undefined counter
#ifdef CONFIG_USE_MPI
  counter_comm_send_bytes,     // Amount of data sent from this thread
  counter_comm_recv_bytes,     // Amount of data sent from this thread
  counter_comm_send_time,      // Time spent sending data
  counter_comm_recv_time,      // Time spent receiving data
  counter_comm_global_time,    // Time spent in collective communication
  counter_comm_send_count,     // Number of sends
  counter_comm_recv_count,     // Number of receives
  counter_comm_global_count,   // Number of barriers/reductions
#endif /* CONFIG_USE_MPI */

#ifdef CONFIG_USE_PAPI
  counter_time_user,           // CPU time in user code of region
  counter_time_sys,            // CPU time in system of region
  counter_cpu_flop_count,      // Number of floating point operations
  mem_count,           // Number of memory accesses
#endif /* CONFIG_USE_PAPI */
  counter_time_real,           // Wallclock time of region
  counter_time_sim,            // Simulation time
  counter_mem_curr_bytes,      // Current number of bytes allocated
  counter_mem_high_bytes,      // Maximum number of bytes allocated
  counter_mem_new_count,       // Number of calls to allocate memory
  counter_mem_delete_count,    // Number of calls to deallocate memory
  counter_mem_new_bytes,       // Number of bytes allocated
  counter_mem_delete_bytes,    // Number of bytes deallocated
  counter_disk_read_bytes,     // Number of bytes read from disk
  counter_disk_write_bytes,    // Number of bytes written to disk
  counter_disk_read_time,      // Time spent reading from disk
  counter_disk_write_time,     // Time spent writing to disk
  counter_user_patch_count,    // Number of grid patches in each level
  counter_user_cell_count,     // Number of grid cells in each level
  counter_user_particle_count, // Number of particles
  num_counters = counter_user_particle_count
};

/* Performance functions */

enum function_enum {
  function_undefined, // 0 is an undefined function
  num_functions = function_undefined
};

/*********************************************************************
 * PROBLEM DECLARATIONS
 **********************************************************************/

/// @enum     boundary_enum
/// @brief    External boundary condition types
enum boundary_enum {
  boundary_undefined,   // 0 is an undefined boundary
  boundary_reflecting,  // 
  boundary_outflow,  // 
  boundary_inflow,  // 
  boundary_periodic,  // 
  boundary_dirichlet, //
  boundary_neumann,
  num_boundaries = boundary_neumann // Number of attribute types
};

/// @enum     face_enum
/// @brief    Cell faces [lower|upper][x|y|z]
enum face_enum {
  face_lower_x = 0,
  face_upper_x = 1,
  face_lower_y = 2,
  face_upper_y = 3,
  face_lower_z = 4,
  face_upper_z = 5,
  face_all};

/*********************************************************************
 * COMPONENTS
 **********************************************************************/

enum component_enum {
  // !!! EDIT component_enum AND component_name TOGETHER !!!
  component_undefined,
  component_enzop,
  component_first = component_enzop,
  component_disk,
  component_distribute,
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
  component_schedule,
  component_simulation,
  component_task,
  num_components = component_task
};

extern const char * component_name [];

#endif /* CELLO_HPP */
