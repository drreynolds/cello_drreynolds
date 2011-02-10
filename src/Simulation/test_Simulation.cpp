// $Id$
// See LICENSE_CELLO file for license and copyright information

/// @file     test_Simulation.cpp
/// @author   James Bordner (jobordner@ucsd.edu)
/// @date     2010-05-06
/// @brief    Program implementing unit tests for the Simulation class
 
#include "test.hpp"

#include "simulation.hpp"
#include "enzo.hpp" /* Required for EnzoSimulation */

#include PARALLEL_CHARM_INCLUDE(test_Simulation.decl.h)

PARALLEL_MAIN_BEGIN
{

  PARALLEL_INIT;

  unit_init();

  unit_class ("Simulation");

  // Create simulation object

  Monitor * monitor = new Monitor;

  // Create the simulation

  unit_func("Simulation");

  // NOTE: Need concrete EnzoSimulation class since Simulation is 
  //       an abstract base class

  Simulation * simulation = new EnzoSimulation (monitor);

  unit_assert(true);

  // Initialize the simulation

  unit_func("initialize");

  FILE * fp = fopen ("input/test_Simulation.in","r");
  simulation->initialize(fp);

  unit_assert(true);

  // Test accessor functions

  unit_func("monitor");
  unit_assert (simulation->monitor() == monitor);

  unit_func("mesh");
  unit_assert (simulation->mesh() != NULL);

  unit_func("mesh");
  unit_assert (simulation->mesh() != NULL);
  
  unit_func("control");
  unit_assert (simulation->control() != NULL);

  unit_func("timestep");
  unit_assert (simulation->timestep() != NULL);

  unit_func("initial");
  unit_assert (simulation->initial() != NULL);

  unit_func("boundary");
  unit_assert (simulation->boundary() != NULL);

  unit_func("num_method");
  unit_assert (simulation->num_method() > 0);

  unit_func("method");
  for (int i=0; i<simulation->num_method(); i++) {
    unit_assert (simulation->method(i) != NULL);
  }

  // Run the simulation

  unit_func("run");
  simulation->run();
  unit_assert(false);
  
  // Load the simulation

  unit_func("read");
  simulation->read();

  // Save the simulation

  unit_func("write");
  simulation->write();
  unit_assert(false);

  // Finalize the simulation

  unit_func("finalize");
  simulation->finalize();
  unit_assert(false);

  delete simulation;
  fclose (fp);

  unit_finalize();


  PARALLEL_EXIT;

}
PARALLEL_MAIN_END

#include PARALLEL_CHARM_INCLUDE(test_Simulation.def.h)
