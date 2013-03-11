// See LICENSE_CELLO file for license and copyright information

/// @file     method_Timestep.hpp 
/// @author   James Bordner (jobordner@ucsd.edu) 
/// @date     Mon Jul 13 11:11:47 PDT 2009 
/// @brief    [\ref Method] Declaration for the Timestep component

#ifndef METHOD_TIMESTEP_HPP
#define METHOD_TIMESTEP_HPP

#ifdef CONFIG_USE_CHARM
class Timestep : public PUP::able 
#else
class Timestep 
#endif
{


  /// @class    Timestep
  /// @ingroup  Method
  /// @brief    [\ref Method] Encapsulate the computation of a timestep

public: // interface

  /// Create a new Timestep
  Timestep() throw() {};

  /// Destructor
  virtual ~Timestep() throw() {}

#ifdef CONFIG_USE_CHARM

  /// Charm++ PUP::able declarations
  PUPable_abstract(Timestep);

  /// CHARM++ Pack / Unpack function
  void pup (PUP::er &p) 
  { TRACEPUP; PUP::able::pup(p); }

#endif

public: // virtual functions

  /// Evaluate the timestep for the block

  virtual double evaluate 
  ( const FieldDescr * field_descr, CommBlock * block ) throw() = 0; 

};

#endif /* METHOD_TIMESTEP_HPP */
