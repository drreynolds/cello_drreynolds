/** 
 *********************************************************************
 *
 * @file      
 * @brief     
 * @author    
 * @date      
 * @ingroup
 * @bug       
 * @note      
 *
 *--------------------------------------------------------------------
 *
 * SYNOPSIS:
 *
 *    
 *
 * DESCRIPTION:
 *
 *    
 *
 * CLASSES:
 *
 *    
 *
 * FUCTIONS:
 *
 *    
 *
 * USAGE:
 *
 *    
 *
 * REVISION HISTORY:
 *
 *    
 *
 * COPYRIGHT: See the LICENSE_CELLO file in the project directory
 *
 *--------------------------------------------------------------------
 *
 * $Id$
 *
 *********************************************************************
 */

/** 
 *********************************************************************
 *
 * @file      test_performance.cpp
 * @brief     Program implementing unit tests for performance classes
 * @author    James Bordner
 * @date      Wed Apr 23 12:25:18 PDT 2008
 *
 * $Id$
 *
 *********************************************************************
 */
 
#include <stdio.h>
#include <stdlib.h>
#include <string>

#include "error.hpp"
#include "test.hpp"
#include "memory.hpp"
#include "performance.hpp"

int main(int argc, char ** argv)
{
  Timer timer;
  const double time_tolerance = 0.05;

  // Timer tests

  unit_class ("Timer");

  unit_open();

  printf ("Initial timer value = %24.16f\n",timer.value());

  timer.start();

  system("sleep 1");
  timer.stop();

  printf ("Initial timer value = %24.16f\n",timer.value());

  unit_assert((timer.value() - 1.0) < time_tolerance);

  timer.start();
  system("sleep 1");
  timer.stop();

  printf ("Initial timer value = %24.16f\n",timer.value());

  unit_assert((timer.value() - 2.0) < time_tolerance);

  unit_class ("Performance");

  unit_func("Performance");
  Performance * performance = new Performance
    ( num_attributes, num_counters, num_components, 3 );

  // Add attributes

  performance->new_attribute(attribute_timestep, "timestep", true);
  performance->new_attribute(attribute_level,    "level");
  
  // Add counters

  performance->new_counter(time_real,          "time_real");
  performance->new_counter(time_sim,           "time_sim");
  performance->new_counter(mem_curr_bytes,     "mem_curr_bytes");
  performance->new_counter(mem_high_bytes,     "mem_high_bytes");
  performance->new_counter(mem_new_count,      "mem_new_count");
  performance->new_counter(mem_delete_count,   "mem_delete_count");
  performance->new_counter(mem_new_bytes,      "mem_new_bytes");
  performance->new_counter(mem_delete_bytes,   "mem_delete_bytes");
  performance->new_counter(disk_read_bytes,    "disk_read_bytes");
  performance->new_counter(disk_write_bytes,   "disk_write_bytes");
  performance->new_counter(disk_read_time,     "disk_read_time");
  performance->new_counter(disk_write_time,    "disk_write_time");
  performance->new_counter(user_patch_count,   "user_patch_count");
  performance->new_counter(user_cell_count,    "user_cell_count");
  performance->new_counter(user_particle_count,"user_particle_count");

#ifdef CONFIG_USE_MPI
  performance->new_counter(comm_send_bytes,    "comm_send_bytes");
  performance->new_counter(comm_recv_bytes,    "comm_recv_bytes");
  performance->new_counter(comm_send_time,     "comm_send_time");
  performance->new_counter(comm_recv_time,     "comm_recv_time");
  performance->new_counter(comm_global_time,   "comm_global_time");
  performance->new_counter(comm_send_count,    "comm_send_count");
  performance->new_counter(comm_recv_count,    "comm_recv_count");
  performance->new_counter(comm_global_count,  "comm_global_count");
#endif /* CONFIG_USE_MPI */

  // Add groups

  performance->new_group(component_amr,"amr");
  performance->new_group(component_analysis,"analysis");
  performance->new_group(component_array,"array");
  performance->new_group(component_control,"control");
  performance->new_group(component_disk,"disk");
  performance->new_group(component_error,"error");
  performance->new_group(component_field,"field");
  performance->new_group(component_memory,"memory");
  performance->new_group(component_method,"method");
  performance->new_group(component_monitor,"monitor");
  performance->new_group(component_parallel,"parallel");
  performance->new_group(component_parameters,"parameters");
  performance->new_group(component_particles,"particles");
  performance->new_group(component_performance,"performance");
  performance->new_group(component_portal,"portal");
  performance->new_group(component_problem,"problem");
  performance->new_group(component_simulation,"simulation");
  
  // Add functions

  performance->new_region(1, "function_1");
  performance->new_region(2, "function_2");
  performance->new_region(3, "function_3");

  // Initialize counters that are non-zero at start

  unit_assert(true);

  unit_func("new_attribute");
  unit_assert (false); //FAILS

  unit_func("new_counter");
  unit_assert (false); //FAILS

  unit_func("new_group");
  unit_assert (false); //FAILS

  unit_func("new_region");
  unit_assert (false); //FAILS


  //--------------------------------------------------
  // Attributes
  //--------------------------------------------------

  unit_func("get_attribute");
  unit_assert (false); //FAILS

  unit_func("set_attribute");
  unit_assert (false); //FAILS

  unit_func("num_attributes");
  unit_assert (false); //FAILS

  //--------------------------------------------------
  // Groups
  //--------------------------------------------------

  unit_func ("begin_group");


  unit_func("get_group");
  unit_assert (false); //FAILS

  unit_func("set_group");
  unit_assert (false); //FAILS

  unit_func("num_groups");
  unit_assert (false); //FAILS

  unit_func("end_group");
  unit_assert (false); //FAILS

  //--------------------------------------------------
  // Regions
  //--------------------------------------------------


  unit_func("get_region");
  unit_assert (false); //FAILS

  unit_func("set_region");
  unit_assert (false); //FAILS

  unit_func("num_regions");
  unit_assert (false); //FAILS

  unit_func("start_region");
  unit_assert (false); //FAILS

  unit_func("stop_region");
  unit_assert (false); //FAILS

  //--------------------------------------------------
  // Counters
  //--------------------------------------------------

  unit_func("get_counter");
  unit_assert (false); //FAILS

  unit_func("set_counter");
  unit_assert (false); //FAILS

  unit_func("increment_counter");
  unit_assert (false); //FAILS

  unit_func("num_counters");
  unit_assert (false); //FAILS

  //--------------------------------------------------
  // Disk
  //--------------------------------------------------

  unit_func("flush");
  unit_assert (false); //FAILS

  unit_func("~Performance");
  unit_assert (false); //FAILS

  delete performance;

  Memory::print();

  unit_close();
}
