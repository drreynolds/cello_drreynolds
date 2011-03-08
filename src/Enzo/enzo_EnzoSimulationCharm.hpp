// $Id: enzo_EnzoSimulationCharm.hpp 2068 2011-03-07 20:41:58Z bordner $
// See LICENSE_CELLO file for license and copyright information

#ifndef ENZO_ENZO_SIMULATION_CHARM_HPP
#define ENZO_ENZO_SIMULATION_CHARM_HPP

/// @file     enzo_EnzoSimulationCharm.hpp
/// @author   James Bordner (jobordner@ucsd.edu)
/// @date     2010-05-11
/// @brief    [\ref Enzo] Declaration of the EnzoSimulationCharm class

class EnzoSimulationCharm : public Simulation {

  /// @class    EnzoSimulationCharm
  /// @ingroup  Enzo
  /// @brief    [\ref Enzo] Simulation class for Enzo

public: // interface

  /// Constructor
  EnzoSimulationCharm(Parameters * parameters) throw();

  /// Destructor
  ~EnzoSimulationCharm() throw();

  /// Override Simulation initialize
  virtual void initialize(FILE * parameter_file) throw ();

  /// Finalize the Simulation after running it
  virtual void finalize() throw();

  /// Run the simulation
  virtual void run() throw();

  /// Load a Simulation from disk
  virtual void read() throw();

  /// Write a Simulation state to disk
  virtual void write() throw();


public: // functions

  /// Return the Enzo object created in EnzoSimulationCharm's constructor
  EnzoBlock * enzo() throw ()
  { return enzo_; };

protected: // virtual functions

  /// Create named stopping object
  Stopping * create_stopping_ (std::string name) throw ();

  /// Create named timestep object
  Timestep * create_timestep_ (std::string name) throw ();

  /// Create named initial conditions object
  Initial * create_initial_ (std::string name) throw ();

  /// Create named boundary conditions object
  Boundary * create_boundary_ (std::string name) throw ();

  /// Create named method object
  Method * create_method_ (std::string name) throw ();

private: // functions

  /// Initialize a block before advancing a timestep
  void block_start_(Block * block) throw ();

  /// Finalize a block after advancing a timestep
  void block_stop_(Block * block) throw ();

  /// Output data
  void output_images_( Block * block,
		       const char * file_format,
		       int cycle,
		       int cycle_skip=1) throw ();

  void deallocate_() throw();

private: // attributes

  EnzoBlock * enzo_;

};

#endif /* ENZO_ENZO_SIMULATION_CHARM_HPP */

