// $Id$
// See LICENSE_CELLO file for license and copyright information

#ifndef METHOD_METHOD_CONTROL_HPP
#define METHOD_METHOD_CONTROL_HPP

/// @file     method_MethodControl.hpp
/// @author   James Bordner (jobordner@ucsd.edu)
/// @date     2010-05-11
/// @todo     consolidate initialize() and initialize_block()
/// @todo     move control functionality from Method to application
/// @brief    Declaration of the MethodControl class

class MethodControl {

  /// @class    MethodControl
  /// @ingroup  Method
  /// @brief    Encapsulate top-level control and description of method methods

public: // interface

  /// Constructor
  MethodControl(Global * global) throw()
    : global_(global),
      time_stop_(-1),
      cycle_stop_(-1)
  {};

  /// Perform any global initialization independent of specific method

  virtual void initialize (DataDescr * data_descr) throw()
  {};

  /// Perform any global finalization independent of specific method

  virtual void finalize (DataDescr * data_descr) throw()
  {};

  /// Perform any method-independent initialization before a block is updated

  virtual void initialize_block (DataBlock * data_block) throw()
  {};

  /// Perform any method-independent finalization after a block is updated

  virtual void finalize_block (DataBlock * data_block) throw()
  {};

  /// Refresh a block face's boundary / ghost zones given neighboring
  /// block face(s)

  virtual void refresh_ghost(DataBlock * data_block,
			     bool xm=true, bool xp=true, 
			     bool ym=true, bool yp=true, 
			     bool zm=true, bool zp=true) throw()
  {};

  /// Return whether the simulation is complete

  bool done (int cycle, double time) throw()
  {
    bool cycle_converged = (cycle != -1) ? (cycle >= cycle_stop_) : false;
    bool time_converged  = (time != -1)  ? (time  >= time_stop_)  : false;

    return (cycle_converged || time_converged);
  }

  /// Set stop cycle; default is to not check time

  void set_cycle_stop(int cycle) throw()
  { cycle_stop_ = cycle; }

  /// Set stop time; default is to not check time

  void set_time_stop(int time) throw()
  { time_stop_ = time; }

protected:

  /// Global objects
  Global * global_;

  /// Stopping time
  double time_stop_;

  /// Stopping cycle (root-level)
  int cycle_stop_;

};

#endif /* METHOD_METHOD_CONTROL_HPP */

