// See LICENSE_CELLO file for license and copyright information

/// @file     enzo_EnzoProblem.cpp
/// @author   James Bordner (jobordner@ucsd.edu)
/// @date     2012-03-03
/// @brief    Implementation of EnzoProblem class
///
/// 

#include "enzo.hpp"

//----------------------------------------------------------------------

EnzoProblem::EnzoProblem() throw ()
  : Problem()
{
}

//----------------------------------------------------------------------

EnzoProblem::~EnzoProblem() throw ()
{
}

//----------------------------------------------------------------------

#ifdef CONFIG_USE_CHARM

void EnzoProblem::pup (PUP::er &p)
{
  // NOTE: change this function whenever attributes change

  TRACEPUP;

  Problem::pup(p);
}

#endif

//======================================================================

Boundary * EnzoProblem::create_boundary_
(
 std::string  name,
 Config * config
 ) throw ()
/// @param name   Name of boundary condition to use
{

  Boundary * boundary = 0;

  if (       name == "reflecting") { 
    boundary = new EnzoBoundary (boundary_type_reflecting);
  } else if (name == "outflow") {
    boundary = new EnzoBoundary (boundary_type_outflow);
  } else if (name == "inflow") {
    boundary = new EnzoBoundary (boundary_type_inflow);
  } else if (name == "periodic") {
    boundary = new EnzoBoundary (boundary_type_periodic);
  } else {
    boundary = Problem::create_boundary_(name,config);
  }
	     
  return boundary;
}

//----------------------------------------------------------------------

Timestep * EnzoProblem::create_timestep_
(
 std::string  name,
 Parameters * parameters
 ) throw ()
/// @param name   Name of the timestep method to create
{
  Timestep * timestep = 0;

  if (name == "ppml") {
    timestep = new EnzoTimestepPpml;
  } else if (name == "ppm") {
    timestep = new EnzoTimestep;
  } else {
    timestep = Problem::create_timestep_(name,parameters);
  }

  return timestep;
}

//----------------------------------------------------------------------

Initial * EnzoProblem::create_initial_ 
(
 std::string  name,
 Parameters * parameters,
 const GroupProcess * group_process
 ) throw ()
{
  
  //--------------------------------------------------
  // parameter: Initial : cycle
  // parameter: Initial : time
  //--------------------------------------------------

  int    init_cycle  = parameters->value_integer ("Initial:cycle",0);
  double init_time   = parameters->value_float   ("Initial:time",0.0);

  Initial * initial = 0;

  if (name == "implosion_2d") {
    initial = new EnzoInitialImplosion2(init_cycle,init_time);
  } else {
    initial = Problem::create_initial_(name,parameters,group_process);
  }

  return initial;

}

//----------------------------------------------------------------------

Method * EnzoProblem::create_method_ ( std::string  name ) throw ()
/// @param name   Name of the method to create
{

  Method * method = 0;

  TRACE1("EnzoProblem::create_method %s",name.c_str());
  if (name == "ppm") {
    method = new EnzoMethodPpm ();
  } else if (name == "ppml") {
    method = new EnzoMethodPpml();
  } else {
    method = Problem::create_method_(name);
  }

  return method;
}


