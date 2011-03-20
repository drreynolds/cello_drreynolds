// $Id$
// See LICENSE_CELLO file for license and copyright information

#ifndef ENZO_ENZO_SIMULATION_MPI_HPP
#define ENZO_ENZO_SIMULATION_MPI_HPP

/// @file     enzo_EnzoSimulationMpi.hpp
/// @author   James Bordner (jobordner@ucsd.edu)
/// @date     2010-05-11
/// @brief    [\ref Enzo] Declaration of the EnzoSimulationMpi class

class EnzoSimulationMpi : public EnzoSimulation {

  /// @class    EnzoSimulationMpi
  /// @ingroup  Enzo
  /// @brief    [\ref Enzo] Simulation class for MPI Enzo-P

public: // functions

  /// Constructor
  EnzoSimulationMpi
  (
   Parameters * parameters,
   GroupProcess *
   ) throw();

  /// Destructor
  ~EnzoSimulationMpi() throw();

  /// Run the simulation
  virtual void run() throw();

};

#endif /* ENZO_ENZO_SIMULATION_MPI_HPP */

