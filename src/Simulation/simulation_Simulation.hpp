// See LICENSE_CELLO file for license and copyright information

#ifndef SIMULATION_SIMULATION_HPP
#define SIMULATION_SIMULATION_HPP

/// @file     simulation_Simulation.hpp
/// @author   James Bordner (jobordner@ucsd.edu)
/// @date     2009-11-10 16:14:57
/// @brief    Interface file for the Simulation class
/// @note     2010-12-17: code-wiki interface review
/// @todo     subclass Simulation for MPI and CHARM

class Boundary;
class Factory;
class FieldDescr;
class GroupProcess;
class Initial;
class Hierarchy;
class Method;
class Monitor;
class Output;
class Parameters;
class Performance;
class Stopping;
class Timestep;

#ifdef CONFIG_USE_CHARM
#include "mesh.decl.h"
#include "simulation.decl.h"
class Simulation : public CBase_Simulation {
#else
class Simulation {
#endif

  /// @class    Simulation
  /// @ingroup  Simulation
  /// @brief    [\ref Simulation] Class specifying a simulation to run
  ///
  /// @detailed A Simulation object encapsulates a single simulation,
  /// and Simulation objects are replicated across processes.  Simulations
  /// are defined as groups in CHARM++.

public: // interface

  /// Constructor for CHARM++

  /// Initialize the Simulation object
  Simulation
  ( const char *       parameter_file,
#ifdef CONFIG_USE_CHARM
    int                n,
    CProxy_BlockReduce proxy_block_reduce, 
#else
    GroupProcess *     group_process = 0,
#endif
    int                index = 0
    ) throw();

  //==================================================
  // CHARM
  //==================================================

#ifdef CONFIG_USE_CHARM
  /// Initialize an empty Simulation
  Simulation();

  /// Initialize a migrated Simulation
  Simulation (CkMigrateMessage *m);

  //==================================================

  /// Request all Hierarchy blocks to send output to main::p_output_close()
  //  void p_output(int index, int cycle, double time) throw();

  // Output
  void p_output (int cycle, double time, double dt, bool stopping) throw();

  // Monitor, test Stopping, update Boundary and ghost zones
  void refresh () throw();

  /// default reduction callback
  void p_done (CkReductionMsg * m)
  {    TRACE1 ("done(%g)\n",*((double *)m->getData()));  delete m; }

  //--------------------------------------------------
  // Output
  //--------------------------------------------------

  /// reset output index to 0
  void output_first() throw();

  /// Process the next output object if any, else proceed with simulation
  void output_next() throw();

  /// Reduce output, using p_output_write to send data to writing processes
  void p_output_reduce() throw();

  /// Receive data from non-writing process, write to disk, close, and
  /// proceed with next output
  void p_output_write (int n, char * buffer) throw();

  CProxy_BlockReduce proxy_block_reduce() 
  { return   proxy_block_reduce_; }

#else

  /// Perform scheduled output for this cycle_ and time_
  void scheduled_output();

#endif

  /// Destructor
  virtual ~Simulation() throw();

  //----------------------------------------------------------------------
  // ACCESSOR FUNCTIONS
  //----------------------------------------------------------------------

  /// Return the dimensionality of the Simulation
  int dimension() const throw()
  { return dimension_; }

  /// Return the Hierarchy
  Hierarchy * hierarchy() const throw()
  { return hierarchy_; }
  
  /// Return the Parameters
  Parameters * parameters() const throw()
  { return parameters_; }
  
  /// Return the field descriptor
  FieldDescr * field_descr() const throw()
  { return field_descr_; }

  /// Return the performance object
  Performance * performance() const throw()
  { return performance_; }

  /// Return the group process object
  GroupProcess * group_process() const throw()
  { return group_process_; }

  /// Return the monitor object
  Monitor * monitor() const throw()
  { return monitor_; }

  /// Return the stopping object, if any
  Stopping *  stopping() const throw()
  { return stopping_; }
  
  /// Return the time-stepping object, if any
  Timestep * timestep() const throw()
  { return timestep_; }

  /// Return the initialization object, if any
  Initial *  initial() const throw()
  { return initial_; }

  /// Return the boundary object, if any
  Boundary * boundary() const throw()
  { return boundary_; }

  /// Return the number of output objects
  size_t num_output() const throw()
  { return output_list_.size(); }

  /// Return the ith output object
  Output * output(int i) const throw()
  { return output_list_[i]; }

  /// Return the number of methods
  size_t num_method() const throw()
  { return method_list_.size(); }

  /// Return the ith method object
  Method * method(int i) const throw()
  { return method_list_[i]; }

  /// Return the current cycle number
  int cycle() const throw() 
  { return cycle_; };

  /// Return the current time
  double time() const throw() 
  { return time_; };

  /// Return the current dt (stored from main)
  double dt() const throw() 
  { return dt_; };

  /// Return the currint stopping criteria (stored from main reduction)
  bool stop() const throw() 
  { return stop_; };

  /// Return the Simulation index
  size_t index() const throw() 
  { return index_; };

  void update_cycle(int cycle, int time, double dt, double stop) {
    cycle_ = cycle;
    time_  = time;
    dt_    = dt;
    stop_  = stop;
  };

  /// Return whether to update all faces at once or axis-by-axis
  bool temp_update_all() const throw() { return temp_update_all_; };

  /// Return whether to include orthogonal ghosts in updates
  bool temp_update_full() const throw() { return temp_update_full_; };

  void monitor_output() const {
    monitor_-> print("Simulation", "cycle %04d time %15.12f dt %15.12g", 
		     cycle_,time_,dt_);
  }

public: // virtual functions

  /// initialize the Simulation given a parameter file
  virtual void initialize() throw();

  /// Finalize the Simulation after running it
  virtual void finalize() throw();

  /// Run the simulation
  virtual void run() throw();

  /// Load a Simulation from disk
  virtual void read() throw();

  /// Write a Simulation state to disk
  virtual void write() const throw();

  /// Return a Hierarchy factory object
  virtual const Factory & factory () const throw();

protected: // functions

  /// Initialize global simulation parameters
  void initialize_simulation_ () throw();

  /// Initialize the hierarchy object
  void initialize_hierarchy_ () throw();

  /// Initialize the data object
  void initialize_data_ () throw();


  /// Initialize the stopping object
  void initialize_stopping_ () throw();

  /// Initialize the timestep object
  void initialize_timestep_() throw();

  /// Initialize the initial conditions object
  void initialize_initial_ () throw();

  /// Initialize the boundary conditions object
  void initialize_boundary_() throw();

  /// Initialize the output objects
  void initialize_output_  () throw();

  /// Initialize the method objects
  void initialize_method_  () throw();

  /// Initialize parallelism-related parameters
  void initialize_parallel_  () throw();

  void deallocate_() throw();

protected: // abstract virtual functions

  /// Create named stopping object
  virtual Stopping * 
  create_stopping_ (std::string name) throw ();

  /// Create named timestep object
  virtual Timestep * 
  create_timestep_ (std::string name) throw ();

  /// Create named initialization object
  virtual Initial * 
  create_initial_ (std::string name) throw ();

  /// Create named boundary object
  virtual Boundary * 
  create_boundary_ (std::string name) throw ();

  /// Create named output object
  virtual Output * 
  create_output_ (std::string name) throw ();

  /// Create named method object
  virtual Method * 
  create_method_ (std::string name) throw ();

protected: // attributes

  //----------------------------------------------------------------------
  // SIMULATION PARAMETERS
  //----------------------------------------------------------------------

  /// Factory, created on first call to factory()
  mutable Factory * factory_;

  /// Parameters associated with this simulation
  Parameters * parameters_;

  /// Parameter file name
  std::string parameter_file_;

  /// Parallel group for the simulation
  GroupProcess * group_process_;

  /// Dimension or rank of the simulation
  int  dimension_; 

  /// Current cycle
  int cycle_;

  /// Current time
  double time_;

  /// Current timestep
  double dt_;

  /// Current stopping criteria
  bool stop_;

  /// [TEMPORARY: DEBUGGING] Whether to update all faces at once or axis-by-axis
  bool temp_update_all_;

  /// [TEMPORARY: DEBUGGING] Whether to include orthogonal ghosts in updates
  bool temp_update_full_;

  //----------------------------------------------------------------------
  // SIMULATION COMPONENTS
  //----------------------------------------------------------------------

  /// Index of this simulation in an ensemble
  size_t index_;

  /// Performance object
  Performance * performance_;

  /// Monitor object
  Monitor * monitor_;

  /// AMR hierarchy
  Hierarchy * hierarchy_;
  
  /// Field descriptor
  FieldDescr * field_descr_;

  /// Stopping criteria
  Stopping * stopping_;

  /// Time-step computation
  Timestep * timestep_;

  /// Initial conditions object
  Initial * initial_;

  /// Boundary conditions object
  Boundary * boundary_;

#ifdef CONFIG_USE_CHARM

  /// CHARM proxy for global reduction operations on blocks
  CProxy_BlockReduce  proxy_block_reduce_;

  /// Index of currently active output object
  size_t index_output_;

#endif

  /// Output objects
  std::vector<Output *> output_list_;

  /// List of method objects
  std::vector<Method *> method_list_;

};

#endif /* SIMULATION_SIMULATION_HPP */

