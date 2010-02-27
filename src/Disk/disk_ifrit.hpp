// $Id$
// See LICENSE_CELLO file for license and copyright information

#ifndef DISK_IFRIT_HPP
#define DISK_IFRIT_HPP

/// @file      disk_ifrit.hpp
/// @brief     Interface for the Ifrit class
/// @author    James Bordner
/// @date      Thu Feb 21 16:05:34 PST 2008
 
class Ifrit {

  /// @class    Ifrit
  /// @brief    Class for writing and reading IFRIT files
  /// @ingroup  Disk
  ///
  /// An Ifrit object currently corresponds to a single IFRIT file /
  /// group dataset.  "IFrIT is a powerful tool that can be used to
  /// visualize 3-dimensional data sets."
  /// http://sites.google.com/site/ifrithome/

public: /// interface

  /// Initialize the Ifrit object
  Ifrit() {};

  /// Delete the Ifrit object
  ~Ifrit() {};

  /// Read a 3d array from an ifrit file
  void read_bin  (std::string name, 
		  Scalar *    buffer, 
		  int *       nx, 
		  int *       ny, 
		  int *       nz) throw ();

  /// Write a 3d array to an ifrit file
  void write_bin (std::string name, 
		  Scalar *    buffer, 
		  int         nx, 
		  int         ny, 
		  int         nz) throw();

private: /// functions


private: /// attributes

};

#endif /* DISK_IFRIT_HPP */

