// $Id: user_MethodEnzoControl.hpp 1258 2010-03-02 01:07:36Z bordner $
// See LICENSE_ENZO file for license and copyright information

/// @file     user_MethodEnzoControl.hpp
/// @author   James Bordner (jobordner@ucsd.edu) 
/// @date     Thu Apr  1 16:14:38 PDT 2010
/// @brief    Implementation of Enzo's UserControl

#ifndef USER_METHOD_ENZO_CONTROL_HPP
#define USER_METHOD_ENZO_CONTROL_HPP


class MethodEnzoControl : public UserControl {

  /// @class    MethodEnzoControl
  /// @ingroup  Enzo
  /// @brief    Method control class MethodEnzoControl for Enzo-P

public: // interface

  /// Create a new MethodEnzoControl

  MethodEnzoControl(Global * global,
		    Enzo * enzo)
    : UserControl(global),
      enzo_(enzo)
  {};

  /// Perform any global initialization independent of specific method

  void initialize (DataDescr * data_descr) throw();

  /// Perform any global finalization independent of specific method

  void finalize (DataDescr * data_descr) throw();

  /// Perform any method-independent initialization before a block is updated

  void initialize_block (DataBlock * data_block) throw();

  /// Perform any method-independent finalization after a block is updated

  void finalize_block (DataBlock * data_block) throw();

  /// Refresh a subset of ghost zones given neighboring block face(s)

  void refresh_ghost(DataBlock * data_block, 
		     bool xm=true, bool xp=true, 
		     bool ym=true, bool yp=true, 
		     bool zm=true, bool zp=true) throw();

private:
  
  Enzo * enzo_;
};

#endif /* USER_METHOD_ENZO_CONTROL_HPP */
