// $Id: data_DataDescr.hpp 1258 2010-03-02 01:07:36Z bordner $
// See LICENSE_CELLO file for license and copyright information

/// @file     data_DataDescr.hpp
/// @author   James Bordner (jobordner@ucsd.edu)
/// @date     Fri Apr  2 14:09:42 PDT 2010
/// @brief    Declaration of the DataDescr class

#ifndef DATA_DATA_DESCR_HPP
#define DATA_DATA_DESCR_HPP

#include <vector>

#include "particle.hpp"
#include "field.hpp"

class DataDescr {

  /// @class    DataDescr
  /// @ingroup  Data
  /// @brief    Container class for all data descriptors (currently just fields)

public: // interface

  /// Initialize the DataDescr object
  DataDescr() throw()
  : field_descr_()
  {}

  //----------------------------------------------------------------------
  // Field functions
  //----------------------------------------------------------------------

  /// Set the Field descriptor

  void set_field_descr (FieldDescr * field_descr) throw()
  { field_descr_ = field_descr; };

  /// Return the Field descriptor

  FieldDescr * field_descr () throw()
  { return field_descr_; };

private: // attributes

  FieldDescr    * field_descr_;

};

#endif /* DATA_DATA_DESCR_HPP */

