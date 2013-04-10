// See LICENSE_CELLO file for license and copyright information

/// @file     simulation_SimulationMpi.hpp
/// @author   James Bordner (jobordner@ucsd.edu)
/// @date     2010-05-11
/// @brief    [\ref Simulation] Declaration of the SimulationMpi class

#ifndef SIMULATION_SIMULATION_MPI_HPP
#define SIMULATION_SIMULATION_MPI_HPP

#if defined(CONFIG_USE_MPI) || ! defined(CONFIG_USE_CHARM)

class CommBlock;
#ifdef REMOVE_PATCH
#else /* REMOVE_PATCH */
class Patch;
#endif /* REMOVE_PATCH */


class SimulationMpi : public Simulation {

  /// @class    SimulationMpi
  /// @ingroup  Simulation
  /// @brief    [\ref Simulation] Simulation class for MPI

public: // functions

  /// Constructor
  SimulationMpi
  ( const char * parameter_file,
    const GroupProcess * group_process) throw();

  /// Destructor
  ~SimulationMpi() throw();

  /// Initialize the MPI Simulation
  virtual void initialize() throw();
  
  /// Run the simulation
  virtual void run() throw();

  /// Perform scheduled output for this cycle_ and time_
  void scheduled_output();

protected:

  void update_boundary_ (CommBlock * block, bool boundary[3][2]) throw();
#ifdef REMOVE_PATCH
  void refresh_ghost_   (CommBlock * block, bool boundary[3][2]) throw();
#else /* REMOVE_PATCH */
  void refresh_ghost_   (CommBlock * block, Patch * patch, bool boundary[3][2]) throw();
#endif /* REMOVE_PATCH */

  
};

#endif /* ! CONFIG_USE_CHARM */

#endif /* SIMULATION_SIMULATION_MPI_HPP */

