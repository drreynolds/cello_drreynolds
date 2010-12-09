// $Id$
// See LICENSE_CELLO file for license and copyright information

/// @file     parallel_Layout.hpp
/// @author   James Bordner (jobordner@ucsd.edu)
/// @date     Thu Feb 25 16:20:17 PST 2010
/// @brief    Declaration of the Layout class

#ifndef PARALLEL_LAYOUT_HPP
#define PARALLEL_LAYOUT_HPP


class Layout {

  /// @class    Layout
  /// @ingroup  Parallel
  /// @brief Specify how a Patch is partitioned into blocks, and how
  /// blocks are assigned to generic processes

public: // interface

  /// Create a Layout with the default one process and one block
  Layout() throw();

  /// Set first process id and number of processes
  void set_range (int process_first=0, int process_count=1) throw();

  /// Set how many blocks are in the layout
  void set_blocks (int nb0, int nb1=1, int nb2=1) throw();

  /// Return the first process id and number of processes
  void range (int * process_first, int * process_count) throw();

  /// Return the number of blocks in the layout
  int blocks (int *b0, int *b1, int *b2) throw();

  /// Return the process id assigned to the given block
  int process (int ibx, int iby, int ibz)  throw();

private: // attributes

  /// Starting process id
  int process_first_;

  /// Number of processes
  int process_count_;

  /// number of compute blocks per thread
  int block_count_[3];

private: // functions

};
#endif /* PARALLEL_LAYOUT_HPP */
