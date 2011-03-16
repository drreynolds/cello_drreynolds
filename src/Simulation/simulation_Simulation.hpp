// $Id$
// See LICENSE_CELLO file for license and copyright information

#ifndef SIMULATION_SIMULATION_HPP
#define SIMULATION_SIMULATION_HPP

/// @file     simulation_Simulation.hpp
/// @author   James Bordner (jobordner@ucsd.edu)
/// @date     2009-11-10 16:14:57
/// @brief    Interface file for the Simulation class
/// @todo     Remove unnecessary Parameters * from function parameters
/// @note     2010-12-17: code-wiki interface review

class Simulation {

  /// @class    Simulation
  /// @ingroup  Simulation
  /// @brief    [\ref Simulation] Class specifying a simulation to run

public: // interface

  /// Initialize the Simulation object
  Simulation(Factory * factory,
	     Parameters * parameters,
	     GroupProcess * group_process);

  //----------------------------------------------------------------------
  // Big Three
  //----------------------------------------------------------------------

  /// Destructor
  virtual ~Simulation() throw();

  /// Copy constructor
  Simulation(const Simulation & simulation) throw();

  /// Assignment operator
  Simulation & operator= (const Simulation & simulation) throw();

  //----------------------------------------------------------------------
  // ACCESSOR FUNCTIONS
  //----------------------------------------------------------------------

  /// Return the dimensionality of the Simulation
  int dimension() const throw();

  /// Return the lower domain extents
  void lower (double * xm, double * ym = 0, double * zm = 0) const throw();

  /// Return the upper domain extents
  void upper (double * xp, double * yp = 0, double * zp = 0) const throw();


  /// Return the Mesh
  Mesh * mesh() const throw();
  
  /// Return the field descriptor
  FieldDescr * field_descr() const throw();

  /// Return the stopping object, if any
  Stopping *  stopping() const throw();
  
  /// Return the time-stepping object, if any
  Timestep * timestep() const throw();

  /// Return the initialization object, if any
  Initial *  initial() const throw();

  /// Return the boundary object, if any
  Boundary * boundary() const throw();

  /// Return the number of method methods
  int num_method() const throw();

  /// Return the ith method
  Method * method(int i) const throw();

  /// Return the factory object
  Factory * factory () const throw();

public: // virtual functions

  /// initialize the Simulation given a parameter file
  virtual void initialize() throw();

  /// Finalize the Simulation after running it
  virtual void finalize() throw();

  /// Run the simulation
  virtual void run() throw() = 0;

  /// Load a Simulation from disk
  virtual void read() throw() = 0;

  /// Write a Simulation state to disk
  virtual void write() const throw() = 0;

protected: // functions

  /// Initialize global simulation parameters
  void initialize_simulation_ () throw();

  /// Initialize the mesh object
  void initialize_mesh_ () throw();

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

  /// Initialize the method objects
  void initialize_method_  () throw();


  void deallocate_() throw();

protected: // abstract virtual functions

  /// Create named stopping object
  virtual Stopping * 
  create_stopping_ (std::string name) throw () = 0;

  /// Create named timestep object
  virtual Timestep * 
  create_timestep_ (std::string name) throw () = 0;

  /// Create named initialization object
  virtual Initial * 
  create_initial_ (std::string name) throw () = 0;

  /// Create named boundary object
  virtual Boundary * 
  create_boundary_ (std::string name) throw () = 0;

  /// Create named method object
  virtual Method * 
  create_method_ (std::string name) throw () = 0;

protected: // attributes

  //----------------------------------------------------------------------
  // SIMULATION PARAMETERS
  //----------------------------------------------------------------------

  /// Factory for creating Simulations, Meshes, Patches and Blocks 
  /// [abstract factory design pattern]
  Factory * factory_; 

  /// Dimension or rank of the simulation
  int  dimension_; 

  /// Lower domain extents
  double lower_[3];

  /// Upper domain extents
  double upper_[3];

  //----------------------------------------------------------------------
  // SIMULATION COMPONENTS
  //----------------------------------------------------------------------

  /// Parallel group for the simulation
  GroupProcess * group_process_;

  /// Parameters associated with this simulation
  Parameters * parameters_;

  /// AMR mesh
  Mesh * mesh_;
  
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

  /// List of method objects
  std::vector<Method *> method_list_;

};

#endif /* SIMULATION_SIMULATION_HPP */

