// See LICENSE_CELLO file for license and copyright information

/// @file     parallel_Reduce.hpp
/// @author   James Bordner (jobordner@ucsd.edu)
/// @date     Fri Mar 11 12:07:45 PST 2011
/// @brief    [\ref Parallel] Declaration and implementation of ReduceCharm

#ifndef PARALLEL_REDUCE_CHARM_HPP
#define PARALLEL_REDUCE_CHARM_HPP

#include "error.hpp"

class ReduceCharm : public Reduce {

  /// @class    ReduceCharm
  /// @ingroup  Parallel
  /// @brief    [\ref Parallel] Implementation of ReduceCharm

public: // interface

  /// Constructor
  ReduceCharm(const GroupProcess * group_process) throw()
    : Reduce (group_process)
  { /* EMPTY */ };

  /// Destructor
  virtual ~ReduceCharm() throw()
  { /* EMPTY */ };


  /// CHARM++ Pack / Unpack function
  inline void pup (PUP::er &p)
  {
    TRACEPUP;
    Reduce::pup(p);
    // NOTE: change this function whenever attributes change
    // this function deliberately empty
  }

  /// Local reduction of the given value
  virtual int reduce_int
  (  int              local,
     enum_reduce_op   reduce_op )  throw()
  {
    return local;
  }

  /// Local reduction of the given value
  virtual double reduce_double
  (  double              local,
     enum_reduce_op   reduce_op )  throw()
  {
    return local;
  }

};

#endif /* PARALLEL_REDUCE_CHARM_HPP */

