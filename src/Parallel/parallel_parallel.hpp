// $Id: parallel.hpp 1275 2010-03-09 01:05:14Z bordner $
// See LICENSE_CELLO file for license and copyright information

#ifndef PARALLEL_PARALLEL_HPP
#define PARALLEL_PARALLEL_HPP

/// @file     parallel.hpp
/// @author   James Bordner (jobordner@ucsd.edu)
/// @date     2009-10-16
/// @brief    Interface for the Parallel class

#include <string>

class Parallel {

  /// @class    Parallel
  /// @ingroup  Parallel
  /// @todo     Split into ParallelProcesses and ParallelThreads or similar
  /// @brief    Class for encapsulating different, possibly multiple,
  /// parallel technologies

public: // interface

  /// Initialize
  virtual void initialize(int * argc = 0, char ***argv = 0) 
  { initialized_ = true; }

  /// Finalize
  virtual void finalize()
  { initialized_ = false; }

  /// Get total number of processors
  virtual int process_count()
  { return 1; }

  /// Get rank of this process
  virtual int process_rank()
  { return 0; }

  /// Get total number of threads in this node
  virtual int thread_count()
  { return 1; }

  /// Get rank of this thread
  virtual int thread_rank()
  { return 0; }

  /// Get rank
  virtual std::string name()
  { return "0"; }

  /// Return whether this is the root process
  virtual bool is_root()
  { return true; }

  bool is_initialized()
  { return initialized_; }

public: // static functions

  /// Get single instance of the Parallel object
  static Parallel * instance() throw ();

protected: // functions

  /// Initialize a Parallel object (singleton design pattern)
  Parallel() 
  {};

  /// Delete a Parallel object (singleton design pattern)
  ~Parallel() 
  {};

  void set_initialized_ (bool initialized)
  { initialized_ = initialized; }

private: // static attributes

  /// Single instance of the Parallel object (singleton design pattern)
  static Parallel * instance_;

private: // attributes

  bool initialized_;

};

#endif /* PARALLEL_PARALLEL_HPP */

