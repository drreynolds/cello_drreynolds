// $Id: lcaperf_counters_basic.hpp 2009 2011-02-22 19:43:07Z bordner $
// See LICENSE file for license and copyright information

#ifndef LCAPERF_COUNTERS_BASIC_HPP
#define LCAPERF_COUNTERS_BASIC_HPP

/// @file     lcaperf_counters_basic.hpp
/// @author   James Bordner (jobordner@ucsd.edu)
/// @date     Thu May 19 18:24:37 PDT 2011
/// @brief    [\ref lcaperf] Declaration of the CountersBasic class

/// @enum
/// @brief Indices of basic counters

class CountersBasic : public Counters {

  /// @class    CountersBasic
  /// @ingroup  lcaperf
  /// @brief    [\ref lcaperf] Basic counters

public: // interface

  /// Constructor
  CountersBasic() throw();

  //----------------------------------------------------------------------
  // Big Three
  //----------------------------------------------------------------------

  /// Destructor
  ~CountersBasic() throw();

  /// Copy constructor
  CountersBasic(const CountersBasic & CountersBasic) throw();

  /// Assignment operator
  CountersBasic & operator= (const CountersBasic & CountersBasic) throw();

  //----------------------------------------------------------------------

protected: // functions

  /// Create and start a new set of counters for a new key
  virtual long long * start_();

  /// stop a set of counters
  virtual void stop_(long long * );

  /// Update global counters for the given key
  virtual void update_ (std::string key, long long * counters);
  
  //----------------------------------------------------------------------

private: // functions

  mutable long long time_begin_;

  inline long long wall_time_() const
  {
    struct timeval tv;
    struct timezone tz;
    gettimeofday (&tv,&tz);
    long long value = 1000000L * tv.tv_sec + tv.tv_usec;
    if (time_begin_==-1) time_begin_=value;
    return value - time_begin_;
  };

};

#endif /* LCAPERF_COUNTERS_BASIC_HPP */

