// $Id: method_Output.cpp 2093 2011-03-12 01:17:05Z bordner $
// See LICENSE_CELLO file for license and copyright information

/// @file     method_Output.cpp
/// @author   James Bordner (jobordner@ucsd.edu)
/// @date     Wed Mar 16 09:53:31 PDT 2011
/// @brief    Implementation of the Output class

#include "method.hpp"

//----------------------------------------------------------------------

Output::Output (std::string file_name) throw()
  : file_name_(file_name),
    active_(false),
    output_schedule_(output_schedule_unknown),
    cycle_interval_(),
    cycle_list_(),
    time_interval_(),
    time_list_(),
    index_(0)
{
}

//----------------------------------------------------------------------

void Output::set_cycle_interval
(
 int cycle_start,
 int cycle_step,
 int cycle_stop
) throw()
{
  if (output_schedule_ != output_schedule_unknown) {
    WARNING ("Output::set_cycle_interval",
	     "Resetting Output scheduling");
  }

  output_schedule_ = output_schedule_cycle_interval;

  cycle_interval_.clear();

  cycle_interval_.push_back(cycle_start);
  cycle_interval_.push_back(cycle_step);
  cycle_interval_.push_back(cycle_stop);

  active_ = true;
}

//----------------------------------------------------------------------

void Output::set_cycle_list (std::vector<int> cycle_list) throw()
{
  if (output_schedule_ != output_schedule_unknown) {
    WARNING ("Output::set_cycle_list",
	     "Resetting Output scheduling");
  }

  output_schedule_ = output_schedule_cycle_list;

  cycle_list_.clear();

  cycle_list_ = cycle_list;

  active_ = true;

}

//----------------------------------------------------------------------

void Output::set_time_interval
  (double time_start, double time_step, double time_stop) throw()
{
  if (output_schedule_ != output_schedule_unknown) {
    WARNING ("Output::set_time_interval",
	     "Resetting Output scheduling");
  }

  output_schedule_ = output_schedule_time_interval;

  time_interval_.clear();

  time_interval_.push_back(time_start);
  time_interval_.push_back(time_step);
  time_interval_.push_back(time_stop);

  active_ = true;
}

//----------------------------------------------------------------------

void Output::set_time_list (std::vector<double> time_list) throw()
{
  if (output_schedule_ != output_schedule_unknown) {
    WARNING ("Output::set_time_list",
	     "Resetting Output scheduling");
  }

  output_schedule_ = output_schedule_time_list;

  time_list_.clear();

  time_list_ = time_list;

  active_ = true;
}

//----------------------------------------------------------------------

double Output::update_timestep ( double time, double dt) const throw()
{
  if (! active_) return dt;

  double new_dt = dt;

  double time_start;
  double time_step;
  double time_stop;
  double time_dump;
  double time_next;

  time_next = time + dt;

  switch (output_schedule_) {

  case output_schedule_time_list:
    time_dump = time_list_[index_];
    if (time < time_dump && time_dump < time_next) {
      new_dt = time_dump - time;
    }
    break;

  case output_schedule_time_interval:
    // time_interval_
    time_start = time_interval_[0];
    time_step  = time_interval_[1];
    time_stop  = time_interval_[2];
    time_dump = time_start + index_*time_step;
    if (time < time_dump && time_dump < time_next) {
      new_dt = time_dump - time;
    }
    break;
  default:
    // NOP
    break;
  }
  return new_dt;
}

//----------------------------------------------------------------------

bool Output::write_this_cycle ( int cycle, double time ) throw()
{

  bool result = false;

  if (! active_) return false;

  double time_start;
  double time_step;
  double time_stop;
  double time_dump;

  int cycle_start;
  int cycle_step;
  int cycle_stop;
  int cycle_dump;

  switch (output_schedule_) {

  case output_schedule_time_list:
    // time_list_
    time_dump = time_list_[index_];
    if (time > time_dump - 1e-14) {
      result = true;
      active_ = (++index_ < time_list_.size());
    }
    break;

  case output_schedule_time_interval:
    // time_interval_
    // deactive if we've reached the end
    time_start = time_interval_[0];
    time_step  = time_interval_[1];
    time_stop  = time_interval_[2];
    time_dump = time_start + index_*time_step;
    if (time >= time_dump - 1e-14) {
      result = true;
      active_ = (time_start + (++index_)*time_step <= time_stop);
    }
    break;

  case output_schedule_cycle_list:
    // cycle_list_
    cycle_dump = cycle_list_[index_];
    if (cycle >= cycle_dump) {
      result = true;
      active_ = (++index_ < cycle_list_.size());
    }
    break;

  case output_schedule_cycle_interval:
    // cycle_interval_
    // deactive if we've reached the end
    cycle_start = cycle_interval_[0];
    cycle_step  = cycle_interval_[1];
    cycle_stop  = cycle_interval_[2];
    cycle_dump = cycle_start + index_*cycle_step;
    if (cycle >= cycle_dump) {
      result = true;
      active_ = (cycle_start + (++index_)*cycle_step <= cycle_stop);
    }
    break;
  default:
    WARNING("Output::write_next_cycle",
	    "Unknown schedule type for active Output object");
  }

  return result;
}
//----------------------------------------------------------------------

