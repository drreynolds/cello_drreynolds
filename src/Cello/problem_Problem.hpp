// See LICENSE_CELLO file for license and copyright information

/// @file     problem_Problem.hpp
/// @author   James Bordner (jobordner@ucsd.edu)
/// @date     2012-03-02
/// @brief    [\ref Problem] Declaration of the Problem class
///
/// This class is used as a container for classes that define the
/// problem being solved.  These classes include the following:
///
///    Boundary:    Boundary conditions
///    Initial:     Initial conditions
///    Method:      List of numerical methods
///    Output:      List of output functions
///    Refinement:  How the mesh hierarchy is to be refined
///    Stopping:    Stopping criteria
///    Timestep:    Timestepping control

#ifndef PROBLEM_PROBLEM_HPP
#define PROBLEM_PROBLEM_HPP

class Boundary;
class Initial;
class Method;
class Output;
class Stopping;
class Timestep;

class Problem {

  /// @class    Problem
  /// @ingroup  Problem
  /// @brief    [\ref Problem] 

public: // interface

  /// Constructor
  Problem() throw();

  /// Destructor
  ~Problem() throw();

  /// Copy constructor
  Problem(const Problem & problem) throw();

  /// Assignment operator
  Problem & operator= (const Problem & problem) throw();

  /// Return the boundary object
  Boundary * boundary() const throw()  { return boundary_; }

  /// Return the initialization object
  Initial *  initial() const throw()   { return initial_; }

  /// Return the ith method object
  Method * method(int i) const throw() { return method_list_[i]; }

  /// Return the ith output object
  Output * output(int i) const throw() { return output_list_[i]; }

  /// Return the stopping object
  Stopping *  stopping() const throw() { return stopping_; }

  /// Return the timestep control object
  Timestep * timestep() const throw()  { return timestep_; }

  /// Initialize the boundary conditions object
  void initialize_boundary() throw();

private: // functions

  /// Initialize the initial conditions object
  void initialize_initial_ () throw();

  /// Initialize the method objects
  void initialize_method_  () throw();

  /// Initialize the output objects
  void initialize_output_  () throw();

  /// Initialize the stopping object
  void initialize_stopping_ () throw();

  /// Initialize the timestep object
  void initialize_timestep_() throw();


  /// Create named boundary object
  virtual Boundary * create_boundary_ (std::string name) throw ();

  /// Create named initialization object
  virtual Initial *  create_initial_ (std::string name) throw ();

  /// Create named method object
  virtual Method *   create_method_ (std::string name) throw ();

  /// Create named output object
  virtual Output *   create_output_ (std::string name) throw ();

  /// Create named stopping object
  virtual Stopping * create_stopping_ (std::string name) throw ();

  /// Create named timestep object
  virtual Timestep * create_timestep_ (std::string name) throw ();


  void deallocate_() throw();

private: // attributes

  /// Boundary conditions object
  Boundary * boundary_;

  /// Initial conditions object
  Initial * initial_;

  /// List of method objects
  std::vector<Method *> method_list_;

  /// Output objects
  std::vector<Output *> output_list_;

  /// Stopping criteria
  Stopping * stopping_;

  /// Time-step computation
  Timestep * timestep_;

};

#endif /* PROBLEM_PROBLEM_HPP */

