// $Id$
// See LICENSE_CELLO file for license and copyright information

/// @file      schedule_Schedule.cpp
/// @author    James Bordner (jobordner@ucsd.edu)
/// @date      2009-11-10
/// @brief     Implementation of the Schedule class

#include "parameters.hpp"
#include "simulation.hpp"
#include "monitor.hpp"
#include "schedule.hpp"
 
//====================================================================
// PUBLIC FUNCTIONS
//====================================================================

Schedule::Schedule()
  : simulation_(NULL)
{
}

//----------------------------------------------------------------------

void Schedule::initialize_simulation()
{
  if (simulation_ != 0) {
    ERROR_MESSAGE("Schedule::initialize_simulation()",
		  "Attempting to re-initialize an existing simulation");
  }

  simulation_ = new Simulation;

  simulation_ -> initialize();
}

//----------------------------------------------------------------------

void Schedule::execute_simulation()
{
  INCOMPLETE_MESSAGE("Schedule::execute_simulation","");
}

//----------------------------------------------------------------------

void Schedule::terminate_simulation()
{
  INCOMPLETE_MESSAGE("Schedule::terminate_simulation","");
}
