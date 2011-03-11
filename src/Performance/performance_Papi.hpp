// $Id: performance_Papi.hpp 1688 2010-08-03 22:34:22Z bordner $
// See LICENSE_CELLO file for license and copyright information

#ifndef PERFORMANCE_PAPI_HPP
#define PERFORMANCE_PAPI_HPP

/// @file     performance_Papi.hpp
/// @author   James Bordner (jobordner@ucsd.edu)
/// @date     Thu Dec  2 11:22:30 PST 2010
/// @brief    [\ref Performance] Interface to the PAPI library


class Papi {

  /// @class    Papi
  /// @ingroup  Groupname
  /// @brief    [\ref Performance] Class for accessing PAPI counters

public: // interface

  /// Constructor
  Papi() throw()
    : is_started_(false),
      time_real_total_(0),
      time_proc_total_(0),
      flop_count_total_(0),
      flop_rate_(0),
      time_real_(0),
      time_proc_(0),
      flop_count_(0)
      
  {};

  //----------------------------------------------------------------------
  // Big Three
  //----------------------------------------------------------------------

  /// Destructor
  // ~Papi() throw()
  // {};

  // /// Copy constructor
  // Papi(const Papi & papi) throw()
  // {};

  // /// Assignment operator
  // Papi & operator= (const Papi & papi) throw()
  // {return *this};

  //----------------------------------------------------------------------
  // global control
  //----------------------------------------------------------------------

  /// Start counters
  void start() throw()
  {
#ifdef CONFIG_USE_PAPI
    if (is_started_) {
      WARNING("Papi::start",
		      "Counters already started");
    } else {
      is_started_ = true;
      float mflop_rate;
      // @@@@ MEMORY LEAK (4 bytes) r2026 @@@@
      PAPI_flops(&time_real_total_, 
		 &time_proc_total_, 
		 &flop_count_total_,
		 &mflop_rate);
      flop_rate_ = mflop_rate * 1e6;
    }
#endif
  }

  /// Stop counters
  void stop() throw()
  {
#ifdef CONFIG_USE_PAPI
    if (! is_started_) {
      WARNING("Papi::stop",
		      "Counters already stopped");
    } else {
      is_started_ = false;
      float mflop_rate;
      PAPI_flops(&time_real_, 
		 &time_proc_, 
		 &flop_count_,
		 &mflop_rate);
      flop_rate_ = mflop_rate * 1e6;

      time_real_  = time_real_  - time_real_total_;
      time_proc_  = time_proc_  - time_proc_total_;
      flop_count_ = flop_count_ - flop_count_total_;
    }
#endif
  }


  /// Real time between start() and stop()
  float time_real() const throw()
  {
#ifdef CONFIG_USE_PAPI
    if (is_started_) {
      WARNING("Papi::time_real",
		      "Counters must be stopped");
      return 0.0;
    } else {
      return time_real_;
    }
#else
    return 0.0;
#endif
  }

  /// Process time between start() and stop()
  float time_proc() const throw()
  {
#ifdef CONFIG_USE_PAPI
    if (is_started_) {
      WARNING("Papi::time_proc",
		      "Counters must be stopped");
      return 0.0;
    } else {
      return time_proc_;
    }
#else
    return 0.0;
#endif
  }

  /// Return number of flops between start() and stop()
  long long flop_count() const throw()
  {
#ifdef CONFIG_USE_PAPI
    if (is_started_) {
      WARNING("Papi::flop_count",
		      "Counters must be stopped");
      return 0;
    } else {
      return flop_count_;
    }
#else
    return 0;
#endif
  }

  /// Return flop rate between start() and stop()
  float flop_rate() const throw()
  {
#ifdef CONFIG_USE_PAPI
    if (is_started_) {
      WARNING("Papi::flop_rate",
	      "Counters must be stopped");
      return 0.0;
    } else {
      return flop_rate_;
    }
#else
    return 0.0;
#endif
  }

  void print () const throw()
  {
#ifdef CONFIG_USE_PAPI
    Monitor * monitor = Monitor::instance();
    monitor->print ("PAPI Time real   = %f",time_real());
    monitor->print ("PAPI Time proc   = %f",time_proc());
    monitor->print ("PAPI GFlop count = %f",flop_count()*1e-9);
    monitor->print ("PAPI GFlop rate  = %f",flop_count()*1e-9 / time_real());
#endif
  };

private: // attributes

  /// Whether counting has started
  bool is_started_;

  /// Argument 1 to PAPI_flops(): real time
  float time_real_total_;

  /// Argument 2 to PAPI_flops(): process time
  float time_proc_total_;

  /// Argument 3 to PAPI_flops(): floating point operations
  long long flop_count_total_;

  /// Argument 4 to PAPI_flops(): floating point rate in flops/s
  float flop_rate_;

  /// Real time since last start
  float time_real_;

  /// Process time since last stop
  float time_proc_;

  /// Argument 3 to PAPI_flops(): floating point operations
  long long flop_count_;

};

#endif /* PERFORMANCE_PAPI_HPP */

