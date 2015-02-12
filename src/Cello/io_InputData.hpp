// See LICENSE_CELLO file for license and copyright information

/// @file     io_InputData.hpp 
/// @author   James Bordner (jobordner@ucsd.edu) 
/// @date     2012-03-23
/// @brief    [\ref Io] Declaration of the InputData class

#ifndef IO_INPUT_DATA_HPP
#define IO_INPUT_DATA_HPP

class Factory;
class Hierarchy;
class FieldDescr;

class InputData : public Input {

  /// @class    InputData
  /// @ingroup  Io
  /// @brief    [\ref Io] define interface for data I/O

public: // functions

  /// Empty constructor for Charm++ pup()
  InputData() throw() {}

  /// Create an uninitialized InputData object
  InputData(const Factory * factory) throw();

  /// Close the file if it is open
  virtual ~InputData() throw();

  /// Charm++ PUP::able declarations
  PUPable_decl(InputData);

  /// Charm++ PUP::able migration constructor
  InputData (CkMigrateMessage *m) : Input(m) {}

  /// CHARM++ Pack / Unpack function
  void pup (PUP::er &p);

public: // virtual functions

  /// Open (or create) a file for IO
  virtual void open () throw();

  /// Close file for IO
  virtual void close () throw();

  /// Whether the file is open or not
  virtual bool is_open () throw();

  /// Finalize input
  virtual void finalize () throw ();

  /// Read hierarchy data from disk
  virtual void read_hierarchy
  ( Hierarchy * hierarchy,
    const FieldDescr * field_descr ) throw();

  /// Read block data from disk
  virtual Block * read_block
  ( Block *            block,
    std::string        block_name,
    const FieldDescr * field_descr) throw();

  /// Read local field from disk
  virtual void read_field
  ( FieldBlock * field_block,
    const FieldDescr * field_descr,
    int field_index) throw();

protected:


};

#endif /* IO_INPUT_DATA_HPP */
