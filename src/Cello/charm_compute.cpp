// See LICENSE_CELLO file for license and copyright information

/// @file     simulation_charm_compute.cpp
/// @author   James Bordner (jobordner@ucsd.edu)
/// @date     2011-09-01
/// @brief    Functions implementing CHARM++ compute-related functions

#ifdef CONFIG_USE_CHARM

#include "simulation.hpp"
#include "mesh.hpp"
#include "comm.hpp"

#include "charm_simulation.hpp"
#include "charm_mesh.hpp"

//======================================================================


void SimulationCharm::c_compute()
{
  
  TRACE("SimulationCharm::c_compute()");
  if (cycle_ > 0 ) performance()->stop_region (perf_cycle);
  printf ("TRACE c_compute()\n");
  if (stop_) {
    
    performance_write();

    proxy_main.p_exit(CkNumPes());

  } else {

    performance()->start_region (perf_cycle);

    if (hierarchy()->group_process()->is_root()) 
      hierarchy()->block_array()->p_compute(cycle_,time_,dt_);
  }
}

//----------------------------------------------------------------------

void CommBlock::p_compute (int cycle, double time, double dt)
{
  TRACE_DEBUG("p_compute");
  TRACE ("BEGIN PHASE COMPUTE");
  // set_cycle(cycle);
  // set_time(time);
  // set_dt(dt);

  TRACE3 ("CommBlock::p_compute() cycle %d time %f dt %f",cycle,time,dt);
  Performance * performance = simulation()->performance();
  if (! performance->is_region_active(perf_compute)) {
    performance->start_region(perf_compute);
  }

#ifdef CONFIG_USE_PROJECTIONS
  double time_start = CmiWallTimer();
#endif

  if (is_leaf()) {

#ifdef CELLO_TRACE
    index_.print("p_compute");
#endif    
    FieldDescr * field_descr = simulation()->field_descr();
    int index_method = 0;
    while (Method * method = simulation()->problem()->method(index_method++)) {

      method -> compute_block (field_descr,this);

    }
  }

#ifdef CONFIG_USE_PROJECTIONS
  traceUserBracketEvent(10,time_start, CmiWallTimer());
#endif

  // Update CommBlock cycle and time to Simulation time and cycle


  set_cycle (cycle_ + 1);
  TRACE1("Calling set_time (%f)",time_+dt_);
  set_time  (time_  + dt_);
  
  
  //  simulation()->performance()->stop_region(perf_compute);

  TRACE ("CommBlock::compute() calling p_adapt(0)");

  TRACE ("END   PHASE COMPUTE");

  //  Performance * performance = simulation()->performance();
  if (performance->is_region_active(perf_compute))
    performance->stop_region(perf_compute);

  next_phase_ = phase_adapt;

  //  p_adapt_begin();
  
  p_refresh_begin();
}

// //----------------------------------------------------------------------

// void CommBlock::compute()
// {
//   TRACE ("CommBlock::compute()");

//   Simulation * simulation = proxy_simulation.ckLocalBranch();


// }

//----------------------------------------------------------------------

#endif /* CONFIG_USE_CHARM */


