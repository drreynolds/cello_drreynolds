// $Id$
// See LICENSE_CELLO file for license and copyright information

#ifndef USER_USER_CONTROL_HPP
#define USER_USER_CONTROL_HPP

/// @file     user_UserControl.hpp
/// @author   James Bordner (jobordner@ucsd.edu)
/// @date     2010-05-11
/// @todo     consolidate initialize() and initialize_block()
/// @brief    Declaration of the UserControl class

class UserControl {

  /// @class    UserControl
  /// @ingroup  User
  /// @brief    Encapsulate top-level control and description of user methods

public: // interface

  /// Constructor
  UserControl(Global * global) throw()
    : global_(global)
  {};

  /// Perform any global initialization independent of specific method

  virtual void initialize (DataDescr * data_descr) throw()
  {};

  /// Perform any global finalization independent of specific method

  virtual void finalize (DataDescr * data_descr) throw()
  {};

  /// Perform any method-independent initialization before a block is updated

  virtual void initialize_block (DataBlock * data_block) throw()
  {};

  /// Perform any method-independent finalization after a block is updated

  virtual void finalize_block (DataBlock * data_block) throw()
  {};

  /// Refresh a block face's boundary / ghost zones given neighboring
  /// block face(s)

  virtual void refresh_ghost(DataBlock * data_block,
			     bool xm=true, bool xp=true, 
			     bool ym=true, bool yp=true, 
			     bool zm=true, bool zp=true) throw()
  {};

protected:

  /// Global objects
  Global * global_;

};

#endif /* USER_USER_CONTROL_HPP */

