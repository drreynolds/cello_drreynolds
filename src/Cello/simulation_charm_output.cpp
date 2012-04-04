// See LICENSE_CELLO file for license and copyright information

/// @file     simulation_charm_output.cpp
/// @author   James Bordner (jobordner@ucsd.edu)
/// @date     2011-09-01
/// @brief    Functions implementing CHARM++ output-related functions
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

// (Called from BlockReduce::entry_prepare())

void Simulation::entry_output () throw()
{
  // reset output "loop" over output objects
  output_first();

  // process first output object, which continues with refresh() if done
  output_next();
}

//----------------------------------------------------------------------

void Simulation::output_first() throw()
{
  index_output_ = -1;
}

//----------------------------------------------------------------------

void Simulation::output_next() throw()
{
  // find next output

  Problem * problem = Simulation::problem();

  Output * output;

  do {
    // skip over output objects that are not scheduled this cycle
    ++index_output_;
    output = problem->output(index_output_);
  } while (output && ! output->is_scheduled(cycle_, time_));

  // assert ! output || output->is_scheduled(cycle_, time_)

  // output if any scheduled, else proceed with refresh

  if (output != NULL) {

    // Prepare for IO
    output->init();

    // Open files
    output->open();

    // Write hierarchy
    output->write_simulation(this);


  } else {

    charm_monitor();

  }
}

//----------------------------------------------------------------------

// Output::init()

//----------------------------------------------------------------------

void Block::entry_write (int index_output)
{
  Simulation * simulation = proxy_simulation.ckLocalBranch();

  FieldDescr * field_descr = simulation->field_descr();
  Output * output = simulation->problem()->output(index_output);

  output->write_block(this,field_descr,0,0,0);

  // Synchronize via main chare before writing
  Hierarchy * hierarchy = simulation->hierarchy();
  int num_blocks = hierarchy->patch(0)->num_blocks();
  simulation->proxy_block_reduce().entry_output_reduce (num_blocks);
}

//----------------------------------------------------------------------

void Block::entry_read ()
{
  INCOMPLETE("Block::entry_read");
}

//----------------------------------------------------------------------

void BlockReduce::entry_output_reduce(int count)
{
  if (++count_output_ >= count) {
    proxy_simulation.entry_output_reduce();
    count_output_ = 0;
  }
}

//----------------------------------------------------------------------

void Simulation::entry_output_reduce() throw()
{
  Output * output = Simulation::problem()->output(index_output_);

  output->end_write_patch();

  int ip       = CkMyPe();
  int ip_writer = output->process_writer();

  if (ip != ip_writer) {

    int n=1;  char * buffer = 0;

    // Copy / alias buffer array of data to send
    output->prepare_remote(&n,&buffer);

    // Remote call to receive data
    proxy_simulation[ip_writer].entry_output_write (n, buffer);

    // Close up file
    output->close();

    // Deallocate from prepare_remote()
    output->cleanup_remote(&n,&buffer);

    // Prepare for next output
    output->finalize();

    // Continue with next output object if any
    output_next();

  } else {

    proxy_simulation[ip].entry_output_write(0,0);

  }

}

//----------------------------------------------------------------------

void Simulation::entry_output_write (int n, char * buffer) throw()
{
  Output * output = Simulation::problem()->output(index_output_);

  if (n != 0) {
    output->update_remote(n, buffer);
  }

  // fan-in from writers
  int remaining = output->counter()->remaining();

  if (remaining == 0) {

    output->close();

    output->finalize();

    output_next();
  }

}

//======================================================================

#endif /* CONFIG_USE_CHARM */


