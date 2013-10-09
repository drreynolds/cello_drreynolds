// See LICENSE_CELLO file for license and copyright information

/// @file     io_Schedule.hpp 
/// @author   James Bordner (jobordner@ucsd.edu) 
/// @date     Mon Mar 14 17:35:56 PDT 2011
/// @brief    [\ref Io] Declaration for the Schedule component

#ifndef IO_SCHEDULE_HPP
#define IO_SCHEDULE_HPP

//----------------------------------------------------------------------
/// @enum     schedule_type
/// @brief    Scheduling types

enum schedule_enum {
  schedule_type_unknown,
  schedule_type_cycle_interval,
  schedule_type_cycle_list,
  schedule_type_time_interval,
  schedule_type_time_list
};

typedef int schedule_type;

class Schedule {

  /// @class    Schedule
  /// @ingroup  Io
  /// @brief    [\ref Io] define interface for various types of output

public: // functions

  /// Create an uninitialized Schedule object with the given file_name format
  Schedule() throw();

  /// CHARM++ Pack / Unpack function
  inline void pup (PUP::er &p)
  {
    TRACEPUP;
    // NOTE: change this function whenever attributes change

    p | active_;
    p | schedule_type_;
    p | cycle_interval_;
    p | cycle_list_;
    p | cycle_skip_;
    p | time_interval_;
    p | time_list_;
    p | time_skip_;
    p | index_;
  }

  /// Set cycle interval (start, step, stop)
  void set_cycle_interval
  (int cycle_start, int cycle_step, int cycle_stop) throw();

  /// Set cycle list
  void set_cycle_list (std::vector<int> cycle_list) throw();

  /// Set time interval (start, step, stop)
  void set_time_interval
  (double time_start, double time_step, double time_stop) throw();

  /// Set time list
  void set_time_list (std::vector<double> time_list) throw();

  /// Set whether the Schedule object is active or not
  void set_active(bool active) throw()
  { active_ = active; };

  /// Return whether the output object is active
  bool is_active() const throw()
  { return active_; };

  /// Whether to perform IO this cycle
  bool write_this_cycle ( int cycle, double time ) throw();

  /// Reduce timestep if next write time is greater than time + dt
  double update_timestep(double time, double dt) const throw();

  /// Set cycle to skip
  void set_skip_cycle (int cycle) throw()
  { cycle_skip_.push_back(cycle); }

  /// Set times to skip
  void set_skip_time (double time) throw()
  { time_skip_.push_back(time); }

protected: // attributes

  /// Whether Schedule is currently active
  bool active_;

  /// Schedule type of the Schedule object
  schedule_type schedule_type_;

  /// cycle start, step, and stop of schedule
  std::vector<int> cycle_interval_;

  /// List of cycles to perform schedule
  std::vector<int> cycle_list_;

  /// List of cycles to skip even if scheduled
  std::vector<int> cycle_skip_;

  /// time start, step, and stop of schedule
  std::vector<double> time_interval_;

  /// List of times to perform schedule
  std::vector<double> time_list_;

  /// List of times to skip even if scheduled
  std::vector<double> time_skip_;

  /// Index of time or cycle interval or list for next output
  size_t index_;

};

#endif /* IO_SCHEDULE_HPP */
