// $Id$
// See LICENSE_CELLO file for license and copyright information

/// @file     method_MethodTimestep.hpp 
/// @author   James Bordner (jobordner@ucsd.edu) 
/// @date     Mon Jul 13 11:11:47 PDT 2009 
/// @brief    Declaration for the MethodTimestep component

#ifndef METHOD_METHOD_TIMESTEP_HPP
#define METHOD_METHOD_TIMESTEP_HPP


class MethodTimestep {

  /// @class    MethodTimestep
  /// @ingroup  Method
  /// @brief    Encapsulate determination of timestep

public: // interface

  /// Create a new MethodTimestep
  MethodTimestep() throw()
  {};

public: // virtual functions

  /// Perform any method-specific initialization

  virtual void initialize (DataDescr * data_descr) throw() {} ;

  /// Perform any timestep-specific finalizations steps, e.g. to
  /// deallocate any dynamically-allocated memory

  virtual void finalize (DataDescr * data_descr) throw(){};

  /// Initialize variables that may change for each block.  Called once per
  /// block per timestep.

  virtual void initialize_block (DataBlock * data_block) throw(){};

  /// Finalize after a timestep, e.g. to deallocate any
  /// dynamically-allocated variables

  virtual void finalize_block (DataBlock * data_block) throw(){};

  /// Compute the timestep for the block

  virtual double compute_block( DataBlock * data_block ) throw() = 0; 

};

#endif /* METHOD_METHOD_TIMESTEP_HPP */
