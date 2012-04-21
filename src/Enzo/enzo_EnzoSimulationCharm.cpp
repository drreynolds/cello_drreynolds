// See LICENSE_CELLO file for license and copyright information

/// @file     enzo_EnzoSimulationCharm.cpp
/// @author   James Bordner (jobordner@ucsd.edu)
/// @date     2011-03-17
/// @brief    Implementation of EnzoSimulationCharm user-dependent class member functions

#ifdef CONFIG_USE_CHARM

#include "cello.hpp"

#include "enzo.hpp"

#include "simulation_charm.hpp"
#include "mesh_charm.hpp"

#include "simulation.hpp"


//----------------------------------------------------------------------

EnzoSimulationCharm::EnzoSimulationCharm
(
 const char         parameter_file[],
 int                n) throw ()
  : EnzoSimulation(parameter_file, n)
{

// #ifdef CONFIG_USE_PROJECTIONS
//   traceRegisterUserEvent("Compute",10);
// #endif

  initialize();

}

//----------------------------------------------------------------------

EnzoSimulationCharm::~EnzoSimulationCharm() throw()
{
}

//----------------------------------------------------------------------

void EnzoSimulationCharm::run() throw()
{
  c_initial();
}

//======================================================================

#endif /* CONFIG_USE_CHARM */
