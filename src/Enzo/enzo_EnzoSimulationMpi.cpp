// See LICENSE_CELLO file for license and copyright information

/// @file     enzo_EnzoSimulationMpi.cpp
/// @author   James Bordner (jobordner@ucsd.edu)
/// @todo     Create specific class for interfacing Cello code with User code
/// @todo     Move timestep reductions into Timestep object
/// @todo     Move all block initialization code into block constructor
/// @date     Tue May 11 18:06:50 PDT 2010
/// @brief    Implementation of EnzoSimulationMpi user-dependent class member functions

#ifndef CONFIG_USE_CHARM

#include "cello.hpp"

#include "enzo.hpp"

//----------------------------------------------------------------------

EnzoSimulationMpi::EnzoSimulationMpi
(
 const char * parameter_file,
 GroupProcess * group_process,
 int index ) throw ()
  : EnzoSimulation(parameter_file,group_process, index)
{
}

//----------------------------------------------------------------------

EnzoSimulationMpi::~EnzoSimulationMpi() throw()
{
}

//----------------------------------------------------------------------

void EnzoSimulationMpi::run() throw()
{
  
  Monitor * monitor = Monitor::instance();

  Performance performance;

  performance.start();
  
  //--------------------------------------------------
  // INITIALIZE FIELDS
  //--------------------------------------------------

  ItPatch it_patch(hierarchy_);
  Patch * patch;

  while ((patch = ++it_patch)) {

    ItBlock it_block(patch);
    Block * block;

    while ((block = ++it_block)) {

      initial_->compute(field_descr_,block);

      EnzoBlock * enzo_block = static_cast <EnzoBlock*> (block);

      enzo_block->initialize(cycle_, time_);

    }
  }

  // Perform any scheduled output

  output();
  
  //--------------------------------------------------
  // INITIAL STOPPING CRITERIA TEST
  //--------------------------------------------------

  int stop_hierarchy = true;

  while ((patch = ++it_patch)) {

    int    stop_patch  = true;

    ItBlock it_block(patch);
    Block * block;

    while ((block = ++it_block)) {

      EnzoBlock * enzo_block = static_cast <EnzoBlock*> (block);

      int & cycle_block = enzo_block->CycleNumber;
      double time_block =  enzo_block->Time();

      int stop_block = stopping_->complete(cycle_block,time_block);

      stop_patch = stop_patch && stop_block;

    }

    stop_hierarchy = stop_hierarchy && stop_patch;

  }

  //======================================================================
  // BEGIN MAIN LOOP
  //======================================================================

  while (! stop_hierarchy) {

    monitor->print("[Simulation %d] cycle %04d time %15.12f", index_, cycle_,time_);

    //--------------------------------------------------
    // Determine timestep
    //--------------------------------------------------

    double dt_hierarchy = std::numeric_limits<double>::max();

    // Accumulate Patch-local timesteps

    while ((patch = ++it_patch)) {

      double dt_patch = std::numeric_limits<double>::max();

      ItBlock it_block(patch);
      Block * block;

      // Accumulate Block-local timesteps

      while ((block = ++it_block)) {

	// Accumulate Block-local dt

	double dt_block = timestep_->compute(field_descr_,block);

	// Reduce timestep to coincide with scheduled output if needed

	double time_block = static_cast <EnzoBlock*> (block)->Time();

	for (size_t i=0; i<output_list_.size(); i++) {
	  Schedule * schedule = output_list_[i]->schedule();
	  dt_block = schedule->update_timestep(time_block,dt_block);
	}

	// Reduce timestep to coincide with end of simulation if needed

	dt_block = MIN (dt_block, (stopping_->stop_time() - time_block));

	// Update patch-level timestep

	dt_patch = MIN(dt_patch,dt_block);

      } // ( block = ++it_block )

      // Update hierarchy-level timestep

      dt_hierarchy = MIN(dt_hierarchy, dt_patch);

    } // ( patch = ++it_patch )

    ASSERT("EnzoSimulation::run", "dt == 0", dt_hierarchy != 0.0);

    //--------------------------------------------------
    // Apply the methods
    //--------------------------------------------------

    stop_hierarchy = true;

    while ((patch = ++it_patch)) {

      int stop_patch = true;

      ItBlock it_block(patch);
      Block * block;

      while ((block = ++it_block)) {

	// Enforce boundary conditions

	boundary_->enforce(field_descr_,block);

	// Refresh ghosts

	block->refresh_ghosts(field_descr_);

	EnzoBlock * enzo_block = static_cast <EnzoBlock*> (block);

	int        & cycle_block    = enzo_block->CycleNumber;
	enzo_float   time_block     = enzo_block->Time();
	enzo_float & dt_block       = enzo_block->dt;
	enzo_float & old_time_block = enzo_block->OldTime;

	// UNIFORM TIMESTEP OVER ALL BLOCKS IN HIERARCHY

	dt_block = dt_hierarchy;

	// Loop through methods

	for (size_t i = 0; i < method_list_.size(); i++) {

	  Method * method = method_list_[i];

	  method -> compute_block (block,time_block,dt_block);

	}

	// Update enzo_block values

	old_time_block  = time_block;
	time_block     += dt_block;
	cycle_block    += 1;

	// Global cycle and time reduction
	
	int stop_block = stopping_->complete(cycle_block,time_block);
	
	// Update stopping criteria for patch

	stop_patch = stop_patch && stop_block;

      } // (block = ++it_block)

      // Update stopping criteria for hierarchy

      stop_hierarchy = stop_hierarchy && stop_patch;

    } // (patch = ++it_patch)

    cycle_ ++;
    time_ += dt_hierarchy;

    // Perform any scheduled output

    for (size_t i=0; i<output_list_.size(); i++) {
      Output * output = output_list_[i];
      output->scheduled_write(field_descr_, hierarchy_,cycle_,time_);
    }

  } // while (! stop_hierarchy)

  //======================================================================
  // END MAIN LOOP
  //======================================================================

  monitor->print("[Simulation %d] cycle %04d time %15.12f", 
		 index_, cycle_, time_);

  performance.stop();

  performance.print(monitor_);

}

#endif /* ! CONFIG_USE_CHARM */
