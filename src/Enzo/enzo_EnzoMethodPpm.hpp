// $Id$
// See LICENSE_CELLO file for license and copyright information

/// @file     enzo_EnzoMethodPpm.hpp
/// @author   James Bordner (jobordner@ucsd.edu) 
/// @date     Thu Apr  1 16:14:38 PDT 2010
/// @brief    [\ref Enzo] Implementation of Enzo PPM hydro method

#ifndef ENZO_ENZO_METHOD_PPM_HPP
#define ENZO_ENZO_METHOD_PPM_HPP

class EnzoMethodPpm : public MethodHyperbolic {

  /// @class    EnzoMethodPpm
  /// @ingroup  Enzo
  /// @brief    [\ref Enzo] Encapsulate Enzo's PPM hydro method

public: // interface

  EnzoMethodPpm(Error      * error,
		Monitor    * monitor,
		Parameters * parameters,
		EnzoDescr  * enzo):
    MethodHyperbolic(error,monitor,parameters),
    enzo_(enzo)
  {};

  /// Perform any method-specific initialization

  void initialize (DataDescr * data_descr) throw();

  /// Perform any method-specific finalizations steps, e.g. to
  /// deallocate any dynamically-allocated memory

  void finalize (DataDescr * data_descr) throw();

  /// Initialize PPM variable that may change.  Called once per
  /// block per timestep.

  void initialize_block (DataBlock * data_block) throw();

  /// Finalize PPM after advancing a block a timestep, e.g. to deallocate
  /// any dynamically-allocated variables

  void finalize_block (DataBlock * data_block) throw();
  /// Apply the method to advance a block one timestep 

  void advance_block(DataBlock * data_block,
		     double t, double dt) throw();

  /// Return the name of the method

  std::string method_name() const throw() 
  { return "ppm"; };

private:

  EnzoDescr * enzo_;

};

#endif /* ENZO_ENZO_METHOD_PPM_HPP */
