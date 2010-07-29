// $Id: parallel_ParallelMpi.hpp 1633 2010-07-21 18:47:53Z bordner $
// See LICENSE_CELLO file for license and copyright information

/// @file     parallel_Mpi.hpp
/// @author   James Bordner (jobordner@ucsd.edu)
/// @date     Thu Oct 15 10:40:37 PDT 2009 
/// @brief    Interface for the ParallelMpi class

#ifndef PARALLEL_PARALLEL_MPI_HPP
#define PARALLEL_PARALLEL_MPI_HPP

enum send_type {
  send_standard,
  send_buffered,
  send_synchronous,
  send_ready };

enum data_type {
  data_single,
  data_double };

class GroupProcessMpi : public GroupProcess {

  /// @class    GroupProcessMpi
  /// @ingroup  Parallel
  /// @brief    MPI helper functions

public: // interface

  /// Initialize an GroupProcessMpi object
  GroupProcessMpi(int process_first     = 0,
		  int process_last_plus = -1,
		  int process_stride    = 1) throw();

public: // interface (Group)

  /// Synchronize between all compute elements in the Group
  void barrier()  throw();

  /// Synchronize between two compute elements in the Group
  void sync (int rank, int tag=0) throw();

  //--------------------------------------------------

  /// Initiate sending an array
  void * send_begin
  (int rank_dest, void * buffer, int size, int tag=0) throw();

  /// Test completeness of sending an array
  bool send_test(void * handle) throw();

  /// Complete sending an array
  void send_wait(void * handle) throw();

  /// Clean up after sending an array
  void send_end(void * handle) throw();

  /// Initiate receiving an array
  void * recv_begin
  (int rank_source, void * buffer, int size, int tag=0) throw();

  /// Complete receiving an array
  void recv_wait(void * handle) throw();

  /// Test completeness of receiving an array
  bool recv_test (void * handle) throw();

  /// Clean up after receiving an array
  void recv_end(void * handle) throw();

  //--------------------------------------------------

  /// Add an array to a list of arrays to send in bulk
  void bulk_send_add(int rank_dest, void * buffer, int size, int tag=0) throw();

  /// Initiate a bulk send of multiple arrays
  void * bulk_send() throw();

  /// Complete a bulk send of multiple arrays
  void bulk_send_wait(void * handle) throw();

  /// Add an array to a list of arrays to receive in bulk
  void bulk_recv_add(int rank_source, void * buffer, int size, int tag=0) throw();

  /// Initiate a bulk receive of multiple arrays
  void * bulk_recv() throw();

  /// Complete a bulk receive of multiple arrays
  void bulk_recv_wait(void * handle) throw();

  /// Set whether send is blocking or non-blocking
  void set_type_send (send_type type)  throw()
  { send_type_ = type; };

  /// Set whether send is type or non-type
  send_type type_send ()  throw()
  { return send_type_; };

private: // functions

  void check_mpi_err_(const char * function, int ierr)
  {
    if (ierr != 0) {
      char message[ERROR_MESSAGE_LENGTH];
      char function[ERROR_MESSAGE_LENGTH];
      sprintf (message,"%d: MPI error %d",rank_,ierr);
      sprintf (function,"GroupProcessMpi::%s",function);
      ERROR_MESSAGE(function,message);
    }
  }

private: // attributes

  /// Communicator for the group
  MPI_Comm comm_;

  /// First process in the group
  int process_first_;

  /// Last process (plus one) in the group 
  int process_last_plus_;

  /// Stride of processes in the group
  int process_stride_;


  /// Whether to use standard, buffered, synchronous, or ready sends
  enum send_type send_type_;

};

#endif /* PARALLEL_PARALLEL_MPI_HPP */

