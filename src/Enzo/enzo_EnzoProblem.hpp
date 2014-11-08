// See LICENSE_CELLO file for license and copyright information

/// @file     enzo_EnzoProblem.hpp
/// @author   James Bordner (jobordner@ucsd.edu)
/// @date     2012-03-03 14:43:11
/// @brief    [\ref Enzo] Declaration of the EnzoProblem class

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

  /// CHARM++ PUP::able declaration
  PUPable_decl(EnzoProblem);

  /// CHARM++ migration constructor
  EnzoProblem(CkMigrateMessage *m) : Problem (m) {}

  /// CHARM++ Pack / Unpack function
  void pup (PUP::er &p);

private: // functions

  /// Create named boundary conditions object
  virtual Boundary * create_boundary_ 
  (std::string type,
   int index,
   Config * config,
   Parameters * parameters
   ) throw ();

  /// Create named initialization object
  virtual Initial *  create_initial_ 
  (std::string type, 
   Config * config,
   Parameters * parameters,
   const FieldDescr *,
   const GroupProcess * group_process) throw ();

  /// Create named refine object
  virtual Refine * create_refine_ 
  (std::string type, 
   Config * config, 
   Parameters * parameters,
   const FieldDescr * field_descr,
   int index) throw ();

  /// Create named method object
  virtual Method * create_method_ 
  (std::string type, Config * config, const FieldDescr *) throw ();

  // /// Create named timestep object
  // virtual Timestep * create_timestep_
  // (std::string type, Config * config) throw ();

  /// Create named interpolation object
  virtual Prolong * create_prolong_
  (std::string type, Config * config) throw ();

  /// Create named restriction object
  virtual Restrict * create_restrict_
  (std::string type, Config * config) throw ();

};

#endif /* ENZO_ENZO_PROBLEM_HPP */

