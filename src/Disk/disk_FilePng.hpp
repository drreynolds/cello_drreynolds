// See LICENSE_CELLO file for license and copyright information

#ifndef DISK_FILE_PNG_HPP
#define DISK_FILE_PNG_HPP

/// @file     disk_FilePng.hpp
/// @author   James Bordner (jobordner@ucsd.edu)
/// @date     2011-09-02
/// @brief    [\ref Disk] Interface for the FilePng class

class FilePng : public File {

  /// @class    FilePng
  /// @ingroup  Disk
  /// @brief    [\ref Disk] Class for writing and reading PNG files

public: // interface

  /// Initialize the FilePng object
  FilePng(std::string path, std::string name) throw();

  /// open the file
  virtual void file_open () throw();

  /// Close the file
  virtual void file_close () throw();

  /// Read data from the file
  virtual void data_read 
  ( void * buffer, std::string name, enum scalar_type * type, 
    int * n0, int * n1=0, int * n2=0, int * n3=0, int * n4=0) throw();

  /// Write data to the file
  virtual void data_write 
  ( const void * buffer, std::string name, enum scalar_type type, 
    int n0, int n1=0, int n2=0, int n3=0, int n4=0) throw();

  // /// Open the given group
  // void open_group (std::string group) throw();

  // /// Close the current group
  // void close_group () throw();

private: // functions

private: // attributes

};

#endif /* DISK_FILE_HDF5_HPP */

