// See LICENSE_CELLO file for license and copyright information

/// @file     simulation_Simulation.hpp
/// @author   James Bordner (jobordner@ucsd.edu)
/// @date     2009-11-10 16:14:57
/// @brief    Interface file for the Simulation class
/// @note     2010-12-17: code-wiki interface review

#ifndef SIMULATION_SIMULATION_HPP
#define SIMULATION_SIMULATION_HPP

class Factory;
class FieldDescr;
class GroupProcess;
class Hierarchy;
class Monitor;
class Parameters;
class Performance;
class Problem;

#ifdef CONFIG_USE_CHARM
#  include "mesh.decl.h"
#  include "simulation.decl.h"
#endif

class Simulation 
#ifdef CONFIG_USE_CHARM
   : public CBase_Simulation 
#endif
{
  /// @class    Simulation
  /// @ingroup  Simulation
  /// @brief    [\ref Simulation] Class specifying a simulation to run
  ///
  /// @detailed A Simulation object encapsulates a single simulation,
  /// and Simulation objects are replicated across processes.  Simulations
  /// are defined as groups in CHARM++.

public: // interface

  /// Simulation constructor


  Simulation
  ( const char *       parameter_file,
#ifdef CONFIG_USE_CHARM
    int                n,
    CProxy_BlockReduce proxy_block_reduce,
#endif
    GroupProcess *     group_process = 0
    ) throw();

  //==================================================
  // CHARM
  //==================================================

#ifdef CONFIG_USE_CHARM

  /// Initialize an empty Simulation
  Simulation();

  /// Initialize a migrated Simulation
  Simulation (CkMigrateMessage *m);

#endif

  //==================================================

#ifdef CONFIG_USE_CHARM

  /// Request all Hierarchy blocks to send output to main::entry_output_close()
  //  void entry_output(int index, int cycle, double time) throw();

  // Output
  void entry_output () throw();

  // Monitor, test Stopping, update Boundary and ghost zones
  void refresh () throw();

  /// default reduction callback
  void entry_done (CkReductionMsg * m)
  {  delete m; }

  //--------------------------------------------------
  // Output
  //--------------------------------------------------

  /// reset output index to 0
  void output_first() throw();

  /// Process the next output object if any, else proceed with simulation
  void output_next() throw();

  /// Reduce output, using entry_output_write to send data to writing processes
  void entry_output_reduce() throw();

  /// Receive data from non-writing process, write to disk, close, and
  /// proceed with next output
  void entry_output_write (int n, char * buffer) throw();

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

  /// Return the Problem container object
  Problem *  problem() const throw()
  { return problem_; }

  // /// Return the dimensionality of the Simulation
  // int dimension() const throw()
  // { return dimension_; }

  /// Return the Hierarchy
  Hierarchy * hierarchy() const throw()
  { return hierarchy_; }
  
  /// Return the Parameters
  Parameters * parameters() const throw()
  { return parameters_; }
  
  /// Return the field descriptor
  FieldDescr * field_descr() const throw()
  { return field_descr_; }

  /// Return the performance object associated with each cycle
  Performance * performance_cycle() const throw()
  { return performance_cycle_; }

  /// Return the performance object associated with the entire simulation
  Performance * performance_simulation() const throw()
  { return performance_simulation_; }

  /// Return the group process object
  GroupProcess * group_process() const throw()
  { return group_process_; }

  /// Return the monitor object
  Monitor * monitor() const throw()
  { return monitor_; }

  /// Return the current cycle number
  int cycle() const throw() 
  { return cycle_; };

  /// Return the current time
  double time() const throw() 
  { return time_; };

  /// Return the current dt (stored from main)
  double dt() const throw() 
  { return dt_; };

  /// Return the current stopping criteria (stored from main reduction)
  bool stop() const throw() 
  { return stop_; };

  /// Output Simulation information
  void monitor_output() const ;

  /// Output Performance information
  void performance_output(Performance * performance, 
			  const char * region = "") const ;

public: // virtual functions

  /// Update Simulation cycle, time, timestep, and stopping criteria
  virtual void update_cycle(int cycle, double time, double dt, double stop) ;

  /// initialize the Simulation given a parameter file
  virtual void initialize() throw();

  /// Finalize the Simulation after running it
  virtual void finalize() throw();

  /// Run the simulation
  virtual void run() throw()
  { ERROR ("Simulation::run","Implictly abstract function called"); }

  /// Return a Hierarchy factory object
  virtual const Factory * factory () const throw();

protected: // functions

  /// Initialize the Problem object
  void initialize_problem_ () throw();

  /// Initialize global simulation parameters
  void initialize_simulation_ () throw();

  /// Initialize the hierarchy object
  void initialize_hierarchy_ () throw();

  /// Initialize the data object
  void initialize_data_descr_ () throw();

  /// Initialize parallelism-related parameters
  void initialize_parallel_  () throw();

  void deallocate_() throw();

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

  /// Whether the group_process_ object was allocated inside Simulation
  bool is_group_process_new_;

#ifdef CONFIG_USE_CHARM

  /// CHARM proxy for global reduction operations on blocks
  CProxy_BlockReduce  proxy_block_reduce_;

  /// Index of currently active output object
  int index_output_;

#endif

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

  //----------------------------------------------------------------------
  // SIMULATION COMPONENTS
  //----------------------------------------------------------------------

  /// Problem container object
  Problem * problem_;

  /// Simulation Performance object
  Performance * performance_simulation_;

  /// Cycle Performance object
  Performance * performance_cycle_;

  /// Monitor object
  Monitor * monitor_;

  /// AMR hierarchy
  Hierarchy * hierarchy_;
  
  /// Field descriptor
  FieldDescr * field_descr_;

};

#endif /* SIMULATION_SIMULATION_HPP */

