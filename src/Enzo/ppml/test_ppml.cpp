// $Id$
// See LICENSE_ENZO file for license and copyright information

/// @file      test_ppml.cpp
/// @author    James Bordner (jobordner@ucsd.edu)
/// @todo      Replace image_dump() with Monitor::image()
/// @date      Fri Mar  7 17:11:14 PST 2008
/// @brief     Program implementing unit tests for hydrodynamics

#include "cello.hpp" 
#include "cello_hydro.h"

#include "string.h"
#include "test_ppml.h"

#include "test.hpp"
#include "enzo.hpp"
#include "parallel.hpp"
#include "performance.hpp"
#include "monitor.hpp"

#include "parallel.def"

#include PARALLEL_CHARM_INCLUDE(test_ppml.decl.h)

//----------------------------------------------------------------------

void print_usage(const char * name)
{
  PARALLEL_PRINTF ("usage: %s <ppml-blast|ppml-implosion3> [size] [cycles] [dump-frequency]\n",
	  name);
  exit(1);
}

//----------------------------------------------------------------------

PARALLEL_MAIN_BEGIN

{

  PARALLEL_INIT;

  unit_init();

  Monitor * monitor = new Monitor;

  // Check command line arguments

  if (PARALLEL_ARGC < 2) {
    print_usage(PARALLEL_ARGV[0]);
  }

  int argi = 0;

  // Parse command line arguments

  enum problem_ppml_type problem = problem_ppml_unknown;

  if (PARALLEL_ARGC > ++argi) {
    for (int i=0; i<num_problems; i++) {
      if (strcmp(PARALLEL_ARGV[argi],problem_name[i]) == 0) {
	problem = problem_ppml_type(i);
      }
    }
  }
  if (problem == 0) print_usage(PARALLEL_ARGV[0]);

  int size = problem_size[problem];
  if (PARALLEL_ARGC > ++argi) size = atoi(PARALLEL_ARGV[argi]);
  int cycle_stop = problem_cycles[problem];
  if (PARALLEL_ARGC > ++argi) cycle_stop = atoi(PARALLEL_ARGV[argi]);
  int dump_frequency = 10;
  if (PARALLEL_ARGC > ++argi) dump_frequency = atoi(PARALLEL_ARGV[argi]);

  PARALLEL_PRINTF ("problem = %s  size = %d  cycles = %d  dump_frequency = %d\n",
	  problem_name[problem], size, cycle_stop, dump_frequency);

  // Initialize for generic hydrodynamics

  EnzoDescr enzo;

  enzo.initialize_hydro ();

  // Initialize for specific problem type

  float  time_stop  = 2.5;

  switch (problem) {
  case problem_ppml_blast:
    enzo.initialize_ppml(size);
    break;
  case problem_ppml_implosion3:
    enzo.initialize_ppml_implosion3(size);
    break;
  default:
    print_usage(PARALLEL_ARGV[0]);
  }

  float dt;

  int   cycle;
  float time;

  Timer timer;
  timer.start();

  double lower = 0.125*size;
  double upper =   1.0*size;

  unit_class("enzo");
  unit_func("SolveMHDEquations");

  for (cycle = 0, time = 0.0;
       (cycle < cycle_stop) && (time < time_stop);
       ++cycle, time += dt) {

    dt =  MIN(enzo.ComputeTimeStep(), time_stop - time);

    enzo.SetExternalBoundaryValues();

    if (dump_frequency && (cycle % dump_frequency) == 0) {
      PARALLEL_PRINTF ("cycle = %6d seconds = %5.0f sim-time = %6f dt = %6f\n",
	      cycle,timer.value(),time,dt);
      fflush(stdout);
      enzo.image_dump(problem_name[problem],cycle,lower,upper,monitor);
    }

    enzo.SolveMHDEquations(NULL, cycle, dt);

  }
  unit_assert(cycle >= cycle_stop || time >= time_stop);

  PARALLEL_PRINTF ("%d %d %g\n",size+6,cycle_stop,timer.value());
  fflush(stdout);

  if (dump_frequency && (cycle % dump_frequency) == 0) {
    enzo.SetExternalBoundaryValues();
    enzo.image_dump(problem_name[problem],cycle,lower,upper,monitor);
  }

  delete monitor;

  unit_finalize();

  PARALLEL_EXIT;

}

PARALLEL_MAIN_END

#include PARALLEL_CHARM_INCLUDE(test_ppml.def.h)
