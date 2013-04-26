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
#include "comm.hpp"

#include "charm_simulation.hpp"
#include "charm_mesh.hpp"

//----------------------------------------------------------------------

void CommBlock::p_output(CkReductionMsg * msg)
{

  TRACE("CommBlock::p_output()");
  double * min_reduce = (double * )msg->getData();

  double dt_forest   = min_reduce[0];
  bool   stop_forest = min_reduce[1] == 1.0 ? true : false;
  set_dt   (dt_forest);
  TRACE2("CommBlock::p_output(): dt=%f  stop=%d",dt_forest,stop_forest);

  delete msg;

  Simulation * simulation = proxy_simulation.ckLocalBranch();

  simulation->update_state(cycle_,time_,dt_forest,stop_forest);

  // Wait for all blocks to check in before calling Simulation::p_output()
  // for next output

  TRACE("CommBlock::p_output() calling SimulationCharm::p_output");
  SimulationCharm * simulation_charm = proxy_simulation.ckLocalBranch();
  simulation_charm->p_output();
}

//----------------------------------------------------------------------

void SimulationCharm::p_output ()
{
  TRACE("SimulationCharm::p_output");
  TRACE2 ("block_sync: %d/%d",block_sync_.index(),block_sync_.stop());
  if (block_sync_.done()) {
    CkCallback callback (CkIndex_SimulationCharm::c_output(), thisProxy);
    contribute(0,0,CkReduction::concat,callback);
  }
}

//----------------------------------------------------------------------

void SimulationCharm::c_output()
{
  TRACE("OUTPUT SimulationCharm::c_output()");
  problem()->output_reset();
  problem()->output_next(this);
}

//----------------------------------------------------------------------

void Problem::output_next(Simulation * simulation) throw()
{
  TRACE("OUTPUT Problem::output_next()");
  int cycle   = simulation->cycle();
  double time = simulation->time();

  Output * output;

  // skip over unscheduled outputs

  do {

    ++index_output_;
    output = this->output(index_output_);

  } while (output && ! output->is_scheduled(cycle, time));

  // output if any scheduled, else proceed with refresh

  if (output != NULL) {

    output->init();
    output->open();
    output->write_simulation(simulation);

  } else {

    simulation->monitor_output();

  }
}


//----------------------------------------------------------------------

void CommBlock::p_write (int index_output)
{
  TRACE("OUTPUT CommBlock::p_write()");
  Simulation * simulation = proxy_simulation.ckLocalBranch();

  FieldDescr * field_descr = simulation->field_descr();
  Output * output = simulation->problem()->output(index_output);

  output->write_block(this,field_descr);

  SimulationCharm * simulation_charm  = proxy_simulation.ckLocalBranch();
  simulation_charm->s_write();
}

//----------------------------------------------------------------------

void SimulationCharm::s_write()
{
  TRACE("SimulationCharm::s_write()");
  TRACE2 ("block_sync: %d/%d",block_sync_.index(),block_sync_.stop());
  if (block_sync_.done()) {
    CkCallback callback (CkIndex_SimulationCharm::c_write(), thisProxy);
    contribute(0,0,CkReduction::concat,callback);

  }

}

//----------------------------------------------------------------------

void SimulationCharm::c_write()
{
  problem()->output_wait(this);
}

//----------------------------------------------------------------------

void Problem::output_wait(Simulation * simulation) throw()
{
  TRACE("OUTPUT Problem::output_wait()");
  Output * output = this->output(index_output_);

  int ip       = CkMyPe();
  TRACE1 ("output = %p",output);
  int ip_writer = output->process_writer();

  if (ip == ip_writer) {

    proxy_simulation[ip].p_output_write(0,0);

  } else {

    int n=1;  char * buffer = 0;

    // Copy / alias buffer array of data to send
    output->prepare_remote(&n,&buffer);

    // Remote call to receive data

    proxy_simulation[ip_writer].p_output_write (n, buffer);

    // Close up file

    output->close();

    // Deallocate from prepare_remote()
    output->cleanup_remote(&n,&buffer);

    // Prepare for next output
    output->finalize();

    // Continue with next output object if any
    output_next(simulation);

  }

}

//----------------------------------------------------------------------

void SimulationCharm::p_output_write (int n, char * buffer)
{
  TRACE("OUTPUT SimulationCharm::p_output_write()");
  problem()->output_write(this,n,buffer);
}

//----------------------------------------------------------------------

void Problem::output_write 
(
 Simulation * simulation,
 int n, char * buffer
) throw()
{
  Output * output = this->output(index_output_);
  TRACE2("OUTPUT Problem::output_write() %d %p",n,output);

  if (n != 0) {
    output->update_remote(n, buffer);
  }

  if (output->sync()->done()) {

    output->close();

    output->finalize();

    output_next(simulation);
  }

}

//----------------------------------------------------------------------

void SimulationCharm::monitor_output()
{
  TRACE("Simulation::monitor_output()");
  Simulation::monitor_output();

  c_compute();
}
//======================================================================

#endif /* CONFIG_USE_CHARM */


