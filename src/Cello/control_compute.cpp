// See LICENSE_CELLO file for license and copyright information

/// @file     control_compute.cpp
/// @author   James Bordner (jobordner@ucsd.edu)
/// @date     2011-09-01
/// @brief    Functions implementing CHARM++ compute-related functions
/// @ingroup  Control

#include "simulation.hpp"
#include "mesh.hpp"
#include "comm.hpp"
#include "control.hpp"

#include "charm_simulation.hpp"
#include "charm_mesh.hpp"

//======================================================================

void CommBlock::compute_begin_ ()
{
  TRACE("CommBlock::compute_begin()");
  simulation()->set_phase(phase_compute);

  refresh_phase_ = phase_compute_continue;
  refresh_sync_  = "contribute";

  index_method_ = 0;
  index_refresh_ = method()->index_refresh(0);

  compute_next_();
}

//----------------------------------------------------------------------

void CommBlock::compute_next_ ()
{
  Method * method = simulation()->problem()->method(index_method_);

  TRACE2 ("CommBlock::compute_next() method = %d %p\n",
	  index_method_,method);

  if (method) {

    control_next(phase_refresh_enter,"neighbor");

  } else {

    compute_end_();

  }
}

//----------------------------------------------------------------------

void CommBlock::compute_continue_ ()
{

#ifdef CONFIG_USE_PROJECTIONS
  //  double time_start = CmiWallTimer();
#endif

  Method * method = simulation()->problem()->method(index_method_);

  TRACE2 ("CommBlock::compute_continue() method = %d %p\n",
	  index_method_,method);
    
  // Apply the method to the CommBlock
  method -> compute (this);

}

//----------------------------------------------------------------------

void CommBlock::compute_stop ()
{

  index_method_++;

  compute_next_();

  //  control_sync (phase_compute_exit,"none",true,__FILE__,__LINE__);

}

//----------------------------------------------------------------------

void CommBlock::compute_end_ ()
{
  TRACE1 ("CommBlock::compute_end() method = %d\n",
	  index_method_);

#ifdef CONFIG_USE_PROJECTIONS
  //  traceUserBracketEvent(10,time_start, CmiWallTimer());
#endif

  set_cycle (cycle_ + 1);
  set_time  (time_  + dt_);
  simulation()->set_cycle(cycle_);
  simulation()->set_time(time_);

  control_next();
  TRACE ("END   PHASE COMPUTE");
}

//----------------------------------------------------------------------



