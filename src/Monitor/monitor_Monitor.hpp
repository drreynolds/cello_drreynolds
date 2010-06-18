// $Id: monitor.hpp 1261 2010-03-03 00:14:11Z bordner $
// See LICENSE_CELLO file for license and copyright information

/// @file     monitor_Monitor.hpp
/// @author   James Bordner (jobordner@ucsd.edu)
/// @date     2009-10-05
/// @brief    Declaration of the Monitor class

#ifndef MONITOR_MONITOR_HPP
#define MONITOR_MONITOR_HPP

#include "parallel.hpp"
#include "error.hpp"

/// @enum     enum_reduce
/// @brief    Reduction operator, used for image projections
enum reduce_type {
  reduce_unknown, /// Unknown reduction
  reduce_min,     /// Minimal value along the axis
  reduce_max,     /// Maximal value along the axis
  reduce_avg,     /// Average value along the axis
  reduce_sum      /// Sum of values along the axis
};

class Monitor {

  /// @class    Monitor
  /// @ingroup  Monitor
  /// @todo     Make calling image() easier
  /// @brief    Functions for user monitoring of the execution status
  ///
  /// The Monitor component is used to communicate information about
  /// the running simulation to the user. Information can be output in
  /// several forms, including text files, HTML files, plots, or other
  /// (generally small) image files. Information is assumed to be from
  /// a correctly-running simulation: anomalous errors or warnings are
  /// output by the Error component. It is assumed that stdout is not
  /// used for monitor output, except for possibly displaying header
  /// text with basic information about Cello and the simulation being
  /// run.

public: // interface

  /// Get single instance of the Monitor object
  static Monitor * instance() throw ()
  { 
    // Delayed creation since Parallel must be initialized
    if (Monitor::instance_ == 0) {
      if (Parallel::instance()->is_initialized()) {
	instance_ = new Monitor(Parallel::instance());
      } else {
	ERROR_MESSAGE("Monitor::instance","Monitor::instance() called before Parallel::initialize()");
      }
    }
    return instance_;
  };

  /// Print the Cello header 
  void header ();

  /// Print a message to stdout
  void print (std::string message, FILE * fp = stdout)
  {
    if (active_) fprintf (fp,"%s %6.1f %s\n",
			  Parallel::instance()->name().c_str(),
			  timer_.value(),message.c_str());
  };

  /// Generate a PNG image of an array
  void image (std::string name, 
	      void * array,
	      precision_type precision,
	      int nx,  int ny,  int nz,   // Array dimensions
	      int nx0, int ny0, int nz0,  // lower inclusive subarray indices
	      int nx1, int ny1, int nz1,  // upper exclusive subarray indices
	      int         axis,           // Axis along which to project
	      reduce_type op_reduce,      // Reduction operation along axis
	      double min, double max,     // Limits for color map
	      const double * color_map,   // color map [r0 g0 b0 r1 g1 b1 ...]
	      int            color_length // length of color map / 3
	      );
  
private: // functions

  /// Initialize the Monitor object (singleton design pattern)
  Monitor(Parallel * parallel) 
    : parallel_(parallel),
      active_(parallel->is_root())
  {  
    timer_.start(); 
  }

private: // attributes

  Parallel * parallel_; // Parallel object, used for is_root()
  bool   active_;  // Whether monitoring is activated.  Used for e.g. ip != 0.
  Timer  timer_;   // Timer from Performance
  
  /// Single instance of the Monitor object (singleton design pattern)
  static Monitor * instance_;

};

#endif /* MONITOR_MONITOR_HPP */

