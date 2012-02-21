// See LICENSE_CELLO file for license and copyright information

/// @file     parallel_Layout.hpp
/// @author   James Bordner (jobordner@ucsd.edu)
/// @date     2010-04-19
/// @brief    [\ref Parallel] Declaration of the Layout class

#ifndef PARALLEL_LAYOUT_HPP
#define PARALLEL_LAYOUT_HPP


class Layout {

  /// @class    Layout
  /// @ingroup  Parallel
  /// @brief    [\ref Parallel] Specify how a Patch is partitioned into
  /// blocks, and how blocks are assigned to generic processes

public: // interface

  /// Create a Layout with the given blocking, initialized for the root process
  Layout(int nbx=1, int nby=1, int nbz=1) throw();

  /// Set first process id and number of processes
  void set_process_range (int process_first=0, int process_count=1) throw();

  /// Return the first process id and number of processes
  void process_range (int * process_first, int * process_count) const throw();

  /// Return the number of blocks in the layout
  int block_count (int *nbx, int *nby = 0, int *nbz = 0) const throw();

  /// Return the number of local blocks in the layout for given process
  int local_count (int ip) const throw();

  /// Return whether the given 3D global index is assigned to the given process
  bool is_local (int ip, int ibx, int iby = 0, int ibz = 0) const throw();

  /// Return the global 1D index given the local 1D index for the given process
  int global_index (int ip, int ib) const throw();

  /// Return the process id assigned to the given block
  int process (int ib)  const throw();

  /// Return the process id assigned to the given block
  int process3 (int ibx, int iby=0, int ibz=0)  const throw();

  /// Return the 3D indices for the given global block 1D index
  void block_indices (int ib, int * ibx=0, int * iby=0, int * ibz=0) const throw();

  /// Return the 3D indices for the given local block 1D index
  void block_indices (int ip, int ib, int * ibx=0, int * iby=0, int * ibz=0) const throw();

  /// Return the 1D global index for the given block 3D indices
  int block_index (int ibx, int iby=0, int ibz=0) const throw();

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
