// See LICENSE_CELLO file for license and copyright information

//----------------------------------------------------------------------

/// @file      enzo-p.cpp
/// @author    James Bordner (jobordner@ucsd.edu)
/// @date      Mon Oct  5 15:10:56 PDT 2009
/// @todo      support multiple input files
/// @brief     Cello main

//----------------------------------------------------------------------

#define CHARM_ENZO
#include "main.hpp"

#include "test.hpp"

#include "enzo.hpp"

#include "enzo_charm.hpp"

//----------------------------------------------------------------------

#define MAX_OUTPUT 10 /* Maximum number of outputs going on at a time */

#ifdef CONFIG_USE_CHARM
extern CProxy_Simulation proxy_simulation;
#endif

PARALLEL_MAIN_BEGIN
{

  // initialize parallel

  PARALLEL_INIT;

  // Create global parallel process group object
  GroupProcess * group_process = GroupProcess::create();

  // initialize unit testing

  int ip = group_process->rank();
  int np = group_process->size();

  unit_init(ip,np);

 #ifdef CONFIG_USE_CHARM
   monitor_ = Monitor::instance();
 #else
  Monitor * monitor_ = Monitor::instance();
 #endif

  monitor_->set_active (group_process->is_root());
  monitor_->header();
  monitor_->print ("BEGIN ENZO-P");

  // open parameter file, calling usage() if invalid

  if (PARALLEL_ARGC != 2) {
    // Print usage if wrong number of arguments
    char buffer [ERROR_LENGTH];
    sprintf (buffer,
	     "\nUsage: %s %s <parameter-file>\n\n", 
	     PARALLEL_RUN,PARALLEL_ARGV[0]);
    ERROR("main",buffer);
  }

  // Read in parameters

  char * parameter_file = PARALLEL_ARGV[1];

  //--------------------------------------------------

#ifdef CONFIG_USE_CHARM

  proxy_main     = thishandle;

  // If using CHARM, create the EnzoSimulationCharm groups

  CProxy_BlockReduce proxy_block_reduce = 
    CProxy_BlockReduce::ckNew();

  proxy_simulation = CProxy_EnzoSimulationCharm::ckNew
    (parameter_file, strlen(parameter_file)+1, proxy_block_reduce, 0);

  //--------------------------------------------------

#else /* ! CONFIG_USE_CHARM */

  Simulation * simulation = 
    new EnzoSimulationMpi (parameter_file,group_process, 0);

  ASSERT ("main()","Failed to create Simulation object",simulation != 0);

  // Initialize the simulation

  simulation->initialize();

  // Run the simulation

  simulation->run();

  // Test results: DUPLICATE CODE IN src/main.cpp !!!

  Parameters * parameters = simulation->parameters();

  int    cycle_final = parameters->value_integer("Testing:cycle_final",0);

  if (cycle_final != 0) {
    unit_assert (simulation->cycle()==cycle_final);
    monitor_->print ("cycle:  %d",simulation->cycle());
  }

  double time_final  = parameters->value_integer("Testing:time_final",0.0);

  if (time_final != 0.0) {
    unit_assert (simulation->time()==time_final);
    monitor_->print ("time:  %g",simulation->time());
  }

  // Delete the simulation

  delete simulation;
      
  // display footer text

  Monitor::instance()->print ("END ENZO-P");

  // clean up

  delete group_process;

  // finalize unit testing

  unit_finalize();

  // exit

  PARALLEL_EXIT;

#endif
  //--------------------------------------------------
}

PARALLEL_MAIN_END


//======================================================================
#ifdef CONFIG_USE_CHARM
#  include "enzo.def.h"
#endif
//======================================================================
