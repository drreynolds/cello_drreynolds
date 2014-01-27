// See LICENSE_CELLO file for license and copyright information


/// @file     simulation_SimulationCharm.hpp
/// @author   James Bordner (jobordner@ucsd.edu)
/// @date     2011-03-17
/// @brief    [\ref Simulation] Declaration of the SimulationCharm class

#ifndef SIMULATION_SIMULATION_CHARM_HPP
#define SIMULATION_SIMULATION_CHARM_HPP

class SimulationCharm : public Simulation
			    
{

  /// @class    SimulationCharm
  /// @ingroup  Simulation
  /// @brief    [\ref Simulation] Simulation class for CHARM++

public: // functions

  /// CHARM++ Constructor
  SimulationCharm
  ( const char parameter_file[], int n) throw();

  /// Destructor
  ~SimulationCharm() throw();

  /// CHARM++ Constructor
  SimulationCharm() {}

  /// CHARM++ Migration constructor
  SimulationCharm(CkMigrateMessage*m)
    : Simulation(m)
  {}

  /// CHARM++ Pack / Unpack function
  inline void pup (PUP::er &p)
  {
    TRACEPUP;
    // NOTE: change this function whenever attributes change
    Simulation::pup(p);
    p | block_sync_;
  }

  /// Initialize the Charm++ Simulation
  virtual void initialize() throw();

  // /// Run the simulation
  // virtual void run() throw();

  /// Add a new CommBlock to this local branch
  inline void insert_block() 
  {
#ifdef CELLO_DEBUG
    PARALLEL_PRINTF ("%d: ++block_sync_ %d\n",  CkMyPe(),block_sync_.stop());
#endif
    ++block_sync_;
  }

  /// Remove a CommBlock from this local branch
  inline void delete_block() 
  {
#ifdef CELLO_DEBUG
    PARALLEL_PRINTF ("%d: --block_sync_ %d\n",  CkMyPe(),block_sync_.stop());
#endif
    --block_sync_; 
  }

  inline int block_count() const
  { 
#ifdef CELLO_DEBUG
    PARALLEL_PRINTF ("%d: block_sync_ %d\n",  CkMyPe(),block_sync_.stop());
#endif
    return block_sync_.stop(); 
  }

  /// Call initialize()
  // void p_initialize_begin();

  /// Wait for all Hierarchy to be initialized before creating any CommBlocks
  void r_initialize_forest();

  /// Wait for all local patches to be created before calling run
  void r_initialize_end();

  /// Call output on Problem list of Output objects
  void p_output ();
  void r_output ();

  /// Reduce output, using p_output_write to send data to writing processes
  void s_write();
  /// Continue on to Problem::output_wait()
  void r_write();

  /// Receive data from non-writing process, write to disk, close, and
  /// proceed with next output
  void p_output_write (int n, char * buffer);

  /// Stopping criteria and computation
  void compute ();

  /// Output Performance information to stdout (root process data only)
  virtual void performance_output();

  /// Reduction for performance data
  void r_performance_reduce (CkReductionMsg * msg);

  /// Updated Simulation function to call compute()
  void monitor_output();

protected: // attributes

  Sync block_sync_;

};

#endif /* SIMULATION_SIMULATION_CHARM_HPP */
