// $Id$
// See LICENSE_CELLO file for license and copyright information

#ifndef SIMULATION_HPP
#define SIMULATION_HPP

/// @file     simulation.hpp
/// @author   James Bordner (jobordner@ucsd.edu)
/// @date     2009-11-10 16:14:57
/// @brief    Interface file for the Simulation class

#include <vector>
#include "cello.h"
#include "parameters.hpp"
#include "amr.hpp"
#include "field.hpp"
#include "method.hpp"

class Simulation {

  /// @class    Simulation
  /// @ingroup  Simulation
  /// @brief    Class specifying a simulation to run

public: // interface

  /// Initialize the Simulation object
  Simulation();

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

  /// Initialize from parameters
  void initialize ();

  /// Return the dimension 1 <= d <= 3, of the simulation
  int dimension() 
  {
    ASSERT ("Simulation::dimension",
	    "lower_ and upper_ vectors are different sizes",
	    lower_.size() == upper_.size()); 
    return lower_.size(); 
  };

  /// Return extents
  void domain_extents (int lower[], int upper[])
  {
    lower[0] = lower_[0];
    lower[1] = lower_[1];
    lower[2] = lower_[2];
    upper[0] = upper_[0];
    upper[1] = upper_[1];
    upper[2] = upper_[2];
  };

  /// Return the number of fields
  int num_fields () { return fields_.size(); };

  /// Return the ith field
  Field * field (int i) { return fields_[i]; };

  /// Return the number of methods
  int num_methods () { return methods_.size(); };

  /// Return the ith method
  Method * method (int i) { return methods_[i]; };

  /// Return the Amr object
  Amr * amr () { return amr_; };

private: // attributes

  /// Domain extents
  std::vector<double> lower_;
  std::vector<double> upper_;

  /// AMR grid
  Amr * amr_;

  /// Fields
  std::vector<Field *> fields_;

  /// Methods
  std::vector<Method *> methods_;

};

#endif /* SIMULATION_HPP */

