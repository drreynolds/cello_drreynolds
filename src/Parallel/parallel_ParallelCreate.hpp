// $Id: parallel_ParallelCreate.hpp 1275 2010-03-09 01:05:14Z bordner $
// See LICENSE_CELLO file for license and copyright information

/// @file     parallel_ParallelCreate.hpp
/// @author   James Bordner (jobordner@ucsd.edu)
/// @todo     Rename Parallel  ParallelSerial
/// @todo     Add ParallelCharm (for CkExit(), etc.)
/// @date     2009-10-16
/// @brief    Interface for the Parallel class

#ifndef PARALLEL_PARALLEL_CREATE_HPP
#define PARALLEL_PARALLEL_CREATE_HPP

#include <stdio.h>
#include <stdlib.h>
#include "parallel.hpp"

class ParallelCreate {

  /// @class    ParallelCreate
  /// @ingroup  Parallel
  /// @brief    Factory class for creating Parallel* objects

public: // interface

  /// Initialize a Parallel object
  ParallelCreate() 
  {};

  Parallel * create (enum parallel_type type)
  { 
    Parallel * parallel;
    switch (type) {
    case parallel_mpi:
#ifdef CONFIG_USE_MPI
      parallel = new ParallelMpi;
#else
      parallel = new ParallelSerial;
#endif
      break;
    case parallel_serial:
      parallel = new ParallelSerial;
      break;
    }
    return parallel;
  }
};

#endif /* PARALLEL_PARALLEL_CREATE_HPP */

