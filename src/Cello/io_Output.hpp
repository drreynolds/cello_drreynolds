// See LICENSE_CELLO file for license and copyright information

/// @file     io_Output.hpp 
/// @author   James Bordner (jobordner@ucsd.edu) 
/// @date     Mon Mar 14 17:35:56 PDT 2011
/// @brief    [\ref Io] Declaration of the Output class

#ifndef IO_OUTPUT_HPP
#define IO_OUTPUT_HPP

class Factory;
class FieldDescr;
class Hierarchy;
class ItField;
class Patch;
class Schedule;
class Simulation;

class Output {

  /// @class    Output
  /// @ingroup  Io
  /// @brief [\ref Io] define interface for various types of IO for
  /// Simulations

public: // functions

  /// Create an uninitialized Output object
  Output(const Factory * factory) throw();

  /// Delete an Output object
  virtual ~Output() throw();

  /// Set file name
  void set_filename (std::string filename,
		     std::vector<std::string> fileargs) throw();

  /// Set field iterator
  void set_it_field (ItField * it_field) throw()
  { it_field_ = it_field; }
  
  /// Return the IoBlock object
  IoBlock * io_block () const throw() { return io_block_; }

  /// Return the IoFieldBlock object
  IoFieldBlock * io_field_block () const throw() { return io_field_block_; }

  /// Return the File object pointer
  File * file() throw() 
  { return file_; };

  /// Return the Schedule object pointer
  Schedule * schedule() throw() 
  { return schedule_; };

  /// Return the filename for the file format and given arguments
  std::string expand_file_name () const throw();

  int process_stride () const throw () 
  { return process_stride_; };

  /// Return whether output is scheduled for this cycle
  bool is_scheduled (int cycle, double time);

  /// Return whether this process is a writer
  bool is_writer () const throw () 
  { return (process_ == process_writer()); };

  /// Return the process id of the writer for this process id
  int process_writer() const throw()
  {
    return process_ - (process_ % process_stride_);
  }

  /// Return the updated timestep if time + dt goes past a scheduled output
  double update_timestep (double time, double dt) const;

#ifdef CONFIG_USE_CHARM

  void counter_increment() { count_reduce_++;  }

  int counter_value() const { return (count_reduce_); }

  void counter_reset() { count_reduce_ = 0; }

  void set_index_charm(int index_charm) { index_charm_ = index_charm; }

#endif

public: // virtual functions

  /// Initialize next output
  virtual void init () throw() = 0;

  /// Open (or create) a file for IO
  virtual void open () throw() = 0;

  /// Close file for IO
  virtual void close () throw() = 0;

  /// Finalize output
  virtual void finalize () throw ()
  { count_output_ ++; }

  /// Write an entire simulation to disk
  virtual void write_simulation
  (
   Factory    * factory,
   FieldDescr * field_descr,
   Hierarchy  * hierarchy,
   Simulation * simulation
   ) throw();

  /// Write local hierarchy data to disk
  virtual void write_hierarchy
  ( const FieldDescr * field_descr,
    Hierarchy * hierarchy) throw();

  /// Write local patch data to disk
  virtual void write_patch
  ( const FieldDescr * field_descr,
    Patch * patch,
    int ixp0=0, int iyp0=0, int izp0=0) throw();

  /// Write local block data to disk
  virtual void write_block
  ( const FieldDescr * field_descr,
    Block * block,
    int ixp0=0, int iyp0=0, int izp0=0) throw();

  /// Write local field to disk
  virtual void write_field
  ( const FieldDescr * field_descr,
    FieldBlock * field_block, int field_index) throw() = 0;

  /// Prepare local array with data to be sent to remote chare for processing
  virtual void prepare_remote (int * n, char ** buffer) throw() = 0;

  /// Accumulate and write data sent from a remote processes
  virtual void update_remote  ( int n, char * buffer) throw() = 0;

  /// Free local array if allocated; NOP if not
  virtual void cleanup_remote (int * n, char ** buffer) throw() = 0;

protected: // attributes

  /// File object for output
  File * file_;

  /// Scheduler for this output
  Schedule * schedule_;

  /// Only processes with id's divisible by process_stride_ writes
  /// (1: all processes write; 2: 0,2,4,... write; np: root process writes)
  int process_stride_;

  /// ID of this process
  int process_;

#ifdef CONFIG_USE_CHARM

  /// counter for reduction of data from non-writers to writers
  int count_reduce_;

#endif

  /// Simulation cycle for next IO
  int cycle_;

  /// Output counter
  int count_output_;

  /// Simulation time for next IO
  double time_;

  /// Name of the file to write, including format arguments
  std::string file_name_;

  /// Format strings for file name, if any ("cycle", "time", etc.)
  std::vector<std::string> file_args_;

  /// Iterator over field id's
  ItField * it_field_;

  /// I/O Block data accessor
  IoBlock * io_block_;

  /// I/O FieldBlock data accessor
  IoFieldBlock * io_field_block_;

#ifdef CONFIG_USE_CHARM
  /// Index of this Output object in Simulation
  size_t index_charm_;
#endif

};

#endif /* IO_OUTPUT_HPP */
