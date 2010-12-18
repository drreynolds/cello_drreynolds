// $Id$
// See LICENSE_CELLO file for license and copyright information

#ifndef SIMULATION_SIMULATION_HPP
#define SIMULATION_SIMULATION_HPP

/// @file     simulation_Simulation.hpp
/// @author   James Bordner (jobordner@ucsd.edu)
/// @date     2009-11-10 16:14:57
/// @brief    Interface file for the Simulation class
/// @note     2010-12-17: code-wiki interface review

class Simulation {

  /// @class    Simulation
  /// @ingroup  Simulation
  /// @brief    Class specifying a simulation to run

public: // interface

  /// Initialize the Simulation object
  Simulation(Global * global);

  //----------------------------------------------------------------------
  // Big Three
  //----------------------------------------------------------------------

  /// Destructor
  ~Simulation() throw();

  /// Copy constructor
  Simulation(const Simulation & simulation) throw();

  /// Assignment operator
  Simulation & operator= (const Simulation & simulation) throw();

  //----------------------------------------------------------------------

  /// initialize the Simulation given a parameter file
  void initialize(std::string parameter_file) throw();

  /// Finalize the Simulation after running it
  void finalize() throw();

  /// Run the simulation
  void run() throw();

  /// Load a Simulation from disk
  void read() throw();

  /// Write a Simulation state to disk
  void write() throw();

protected: // functions

  /// Initialize parameters
  void initialize_parameters_ (Parameters * parameters) throw();

  /// Initialize the mesh component
  void initialize_mesh_       (Parameters * parameters) throw();

  /// Initialize the data component
  void initialize_data_descr_ (Parameters * parameters) throw();

  /// Initialize the control component
  void initialize_control_    (Parameters * parameters) throw();

  /// Initialize the timestep component
  void initialize_timestep_   (Parameters * parameters) throw();

  /// Initialize the initialization method component
  void initialize_initial_    (Parameters * parameters) throw();

  /// Initialize the method components
  void initialize_methods_    (Parameters * parameters) throw();


  /// Add the named initialization method
  MethodInitial * add_initial_ (std::string method_name) throw();

  /// Add a named hyperbolic numerical method
  MethodHyperbolic * add_method_ (std::string method_name) throw();

  /// Set the named control method
  MethodControl * set_control_ (std::string control_name) throw();

  /// Set the named timestep method
  MethodTimestep * set_timestep_ (std::string timestep_name) throw();
  
protected: // abstract virtual functions

  /// Create named initialization method.
  virtual MethodInitial * 
  create_initial_ (std::string name_initial) throw () = 0;

  /// Create named hyperbolic method.
  virtual MethodHyperbolic * 
  create_method_ (std::string name_hyperbolic) throw () = 0;

  /// Create named timestep method.
  virtual MethodTimestep * 
  create_timestep_ (std::string name_timestep) throw () = 0;

  /// Create named control method.
  virtual MethodControl * 
  create_control_ (std::string name_control) throw () = 0;

protected: // attributes

  //----------------------------------------------------------------------
  // PARAMETERS
  //----------------------------------------------------------------------

  /// Dimension or rank of the simulation.
  int    dimension_; 

  /// Lower and upper domain extents
  double extent_[6];

  //----------------------------------------------------------------------
  // SIMULATION COMPONENTS
  //----------------------------------------------------------------------

  /// "global" data, including parameters, monitor, error, parallel, etc.
  Global * global_;

  /// AMR mesh
  Mesh * mesh_;
  
  /// Data descriptor
  DataDescr * data_descr_;


  /// List of initialization routines
  std::vector<MethodInitial *> initialize_list_;

  /// List of numerical methods to apply at each timestep
  std::vector<MethodHyperbolic *> method_list_;

  /// Method for time-step computation
  MethodTimestep * timestep_;

  /// Method for overall control of the simulation
  MethodControl * control_;


};

#endif /* SIMULATION_SIMULATION_HPP */

