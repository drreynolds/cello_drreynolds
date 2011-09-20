// See LICENSE_CELLO file for license and copyright information

/// @file     simulation_charm_output.cpp
/// @author   James Bordner (jobordner@ucsd.edu)
/// @date     2011-09-01
/// @brief    Functions implementing CHARM++ output-related functions
/// @todo      Move dt / cycle update to separate function from p_output()
///
/// This file contains member functions for various CHARM++ chares and
/// classes used for Output in a CHARM++ simulation.  Functions are
/// listed in roughly the order of flow-of-control.

#ifdef CONFIG_USE_CHARM

#include "simulation.hpp"
#include "mesh.hpp"

#include "simulation_charm.hpp"
#include "mesh_charm.hpp"

//----------------------------------------------------------------------

// (Called from BlockReduce::p_prepare())

void Simulation::p_output 
( int cycle, double time, double dt, bool stop ) throw()
{

  TRACE("Simulation::p_output()");

  // Update Simulation cycle and time from reduction to main
  
  cycle_ = cycle;
  time_  = time;
  dt_    = dt;
  stop_  = stop;

  // reset output "loop" over output objects
  output_first();

  // process first output object, which continues with refresh() if done
  output_next();
}

//----------------------------------------------------------------------

void Simulation::output_first() throw()
{
  TRACE("Simulation::output_first()");
  index_output_ = 0;
}

//----------------------------------------------------------------------

void Simulation::output_next() throw()
{

  TRACE("Simulation::output_next");

  // find next output

  while (index_output_ < num_output() && 
	 ! output(index_output_)->is_scheduled(cycle_, time_))
    ++index_output_;

  // output if any scheduled, else proceed with refresh

  if (index_output_ < num_output()) {

    // Prepare for IO
    output(index_output_)->init();

    // Open files
    output(index_output_)->open();

    // Loop over Patches in the Hierarchy
    ItPatch it_patch(hierarchy_);
    Patch * patch;
    while (( patch = ++it_patch )) {
      // If Patch is local, call output on block chare array
      if (patch->blocks_allocated()) {
	patch->block_array().p_output (index_output_);
      }
    }

  } else {

    refresh();

  }
}

//----------------------------------------------------------------------

// Output::init()

//----------------------------------------------------------------------

void Block::p_output (int index_output)
{

  TRACE("Block::p_output");

  Simulation * simulation = proxy_simulation.ckLocalBranch();

  FieldDescr * field_descr = simulation->field_descr();
  simulation->output(index_output)->write(field_descr,this);

  // Synchronize via main chare before writing
  int num_blocks = simulation->hierarchy()->patch(0)->num_blocks();
  simulation->proxy_block_reduce().p_output_reduce (num_blocks);
}

//----------------------------------------------------------------------

// Output::write(block)

//----------------------------------------------------------------------

void BlockReduce::p_output_reduce(int count)
{
  TRACE("BlockReduce::p_output_reduce");

  if (++count_output_ >= count) {
    INCOMPLETE("BlockReduce::p_output_reduce()");
    proxy_simulation.p_output_reduce();
    count_output_ = 0;
  }
}

//----------------------------------------------------------------------

void Simulation::p_output_reduce() throw()
{
  TRACE("Simulation::p_output_reduce()");

  int ip       = CkMyPe();
  int ip_stride = ip - (ip % output(index_output_)->process_stride());

  // Even self calls this to avoid hanging if case np == 1
  char buffer[20];
  sprintf(buffer,"%02d > %02d send",ip,ip_stride);
  if (ip != ip_stride) {
    TRACE2("%d -> %d calling p_output_write()\n",ip,ip_stride);
    proxy_simulation[ip_stride].p_output_write (strlen(buffer),buffer);
    output_next();
  } else {
    TRACE2("%d -> %d calling p_output_write()\n",ip,ip_stride);
    proxy_simulation[ip].p_output_write(0,0);
  }

}

//----------------------------------------------------------------------

void Simulation::p_output_write (int n, char * buffer) throw()
{
  TRACE("Simulation::p_output_write()");

  Output * output = Simulation::output(index_output_);
  int ip       = CkMyPe();
  int ip_stride = ip - (ip % output->process_stride());
  TRACE4("%d %d  %d  %d\n",ip,ip_stride,CkMyPe(),output->process_stride());

  int count = output->counter();

  if (count == 0) {
    TRACE("Simulation::p_output_write: initialize");
  }
  if (n == 0) {
    TRACE("Simulation::p_output_write: process reduce remote");
  } else {
    TRACE("Simulation::p_output_write: process reduce local");
  }
  
  if (count == output->process_stride()) {

    // write to disk
    TRACE("Simulation::p_output_write: finalize");

    // close
    output->close();

    // next


    output_next();
  }


}

//======================================================================

#endif /* CONFIG_USE_CHARM */


