// $Id$
// See LICENSE_CELLO file for license and copyright information

#ifndef SIMULATION_SIMULATION_HPP
#define SIMULATION_SIMULATION_HPP

/// @file     simulation_Simulation.hpp
/// @author   James Bordner (jobordner@ucsd.edu)
/// @date     2009-11-10 16:14:57
/// @brief    Interface file for the Simulation class

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
  ~Simulation() throw()
  {
    INCOMPLETE_MESSAGE("Simulation::~Simulation","");
  }

  /// Copy constructor
  Simulation(const Simulation & classname) throw()
  {
    INCOMPLETE_MESSAGE("Simulation::Simulation","");
  }

  /// Assignment operator
  Simulation & operator= (const Simulation & classname) throw()
  {
    INCOMPLETE_MESSAGE("Simulation::operator =","");
    return *this;
  }

  /// Initialize the Simulation's data fields

  void initialize() throw();

  /// Advance the simulation a specified amount

  void advance(double stop_time, int stop_cycle)  throw();

  /// Return the dimension 1 <= d <= 3, of the simulation
  int dimension()  throw()
  {
    ASSERT ("Simulation::dimension",
	    "lower_ and upper_ vectors are different sizes",
	    lower_.size() == upper_.size()); 
    return lower_.size(); 
  };

  /// Return extents.  Assumes lower[] and upper[] are allocated to at least dimension()
  int domain (int lower[], int upper[]) throw()
  {
    if (dimension() >= 1) {
      lower[0] = lower_[0];
      upper[0] = upper_[0];
    }
    if (dimension() >= 2) {
      lower[1] = lower_[1];
      upper[1] = upper_[1];
    }
    if (dimension() >= 3) {
      lower[2] = lower_[2];
      upper[2] = upper_[2];
    }
    return dimension();
  }

  /// Return the Global object
  Global * global ()  throw() 
  { return global_; };

  /// Return the Mesh object
  Mesh * mesh ()  throw() 
  { return mesh_; };

  /// Return the Schedule object
  Schedule * schedule ()  throw() 
  { return schedule_; };

  /// Return the User code descriptor
  UserDescr * user_descr () throw() 
  { return user_descr_; };

  /// Return the Data code descriptor
  DataDescr * data_descr () throw() 
  { return data_descr_; };

private: // attributes

  /// Domain extents

  std::vector<double> lower_;
  std::vector<double> upper_;

  /// "global" data, including parameters, monitor, error, parallel, etc.

  Global * global_;

  /// AMR mesh
  Mesh * mesh_;

  /// Parallel task scheduler
  Schedule * schedule_;

  /// Method descriptor
  UserDescr * user_descr_;

  /// Data descriptions
  DataDescr * data_descr_;

};

#endif /* SIMULATION_SIMULATION_HPP */

