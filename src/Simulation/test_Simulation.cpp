// $Id$
// See LICENSE_CELLO file for license and copyright information

/// @file     test_Simulation.cpp
/// @author   James Bordner (jobordner@ucsd.edu)
/// @date     2010-05-06
/// @todo     Add create_simulation
/// @brief    Program implementing unit tests for the Simulation class
 
#include "test.hpp"

#include "simulation.hpp"
#include "enzo.hpp" /* Required for EnzoSimulationSerial */

#include PARALLEL_CHARM_INCLUDE(test.decl.h)

PARALLEL_MAIN_BEGIN
{

  PARALLEL_INIT;

  GroupProcess * group_process = GroupProcess::create();

  unit_init();

  unit_class("Simulation");
  // Create the simulation

  unit_func("Simulation");

  // NOTE: Need concrete EnzoSimulationSerial class since Simulation is 
  //       an abstract base class

#if defined(CONFIG_USE_CHARM)
  Simulation * simulation = 
    new EnzoSimulationCharm("input/test_Simulation.in",group_process);
#else
  Simulation * simulation = 
    new EnzoSimulationMpi("input/test_Simulation.in",group_process);
#endif


  unit_assert(simulation != 0);

  // Initialize the simulation

  unit_func("initialize");

  simulation->initialize();

  unit_assert(true);

  // Test accessor functions

  unit_func("mesh");
  unit_assert (simulation->mesh() != NULL);

  unit_func("stopping");
  unit_assert (simulation->stopping() != NULL);

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
  unit_assert(unit_incomplete);
  
  // Load the simulation

  unit_func("read");
  simulation->read();

  // Save the simulation

  unit_func("write");
  simulation->write();
  unit_assert(unit_incomplete);

  // Finalize the simulation

  unit_func("finalize");
  simulation->finalize();
  unit_assert(unit_incomplete);

  delete simulation;

  unit_finalize();


  PARALLEL_EXIT;

}
PARALLEL_MAIN_END

#include PARALLEL_CHARM_INCLUDE(test_Simulation.def.h)
