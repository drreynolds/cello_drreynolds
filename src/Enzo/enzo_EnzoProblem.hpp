// See LICENSE_CELLO file for license and copyright information

/// @file     enzo_EnzoProblem.hpp
/// @author   James Bordner (jobordner@ucsd.edu)
/// @date     yyyy-mm-dd
/// @brief    [\ref Enzo] Declaration of the EnzoProblem class
///

#ifndef ENZO_ENZO_PROBLEM_HPP
#define ENZO_ENZO_PROBLEM_HPP

class EnzoProblem : public Problem {

  /// @class    EnzoProblem
  /// @ingroup  Enzo
  /// @brief    [\ref Enzo] 

public: // interface

  /// Constructor
  EnzoProblem() throw();

  /// Destructor
  ~EnzoProblem() throw();

private: // functions

  /// Create named boundary conditions object
  virtual Boundary * create_boundary_ (std::string name) throw ();

  /// Create named initialization object
  virtual Initial *  create_initial_ 
  (std::string name, int init_cycle, double init_time) throw ();

  /// Create named stopping object
  virtual Stopping * create_stopping_ 
  (std::string name, int stop_cycle, double stop_time) throw ();

  /// Create named method object
  virtual Method *   create_method_ 
  (std::string name, Parameters * parameters) throw ();

  /// Create named output object
  virtual Output *   create_output_ 
  (std::string, GroupProcess *, Hierarchy *, const Factory * ) throw ();

  /// Create named timestep object
  virtual Timestep * create_timestep_ (std::string name) throw ();

};

#endif /* ENZO_ENZO_PROBLEM_HPP */

