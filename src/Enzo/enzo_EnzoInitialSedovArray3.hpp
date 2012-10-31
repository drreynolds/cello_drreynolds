// See LICENSE_CELLO file for license and copyright information

/// @file     enzo_EnzoInitialSedovArray3.hpp
/// @author   James Bordner (jobordner@ucsd.edu)
/// @date     Tue Jan  4 19:26:38 PST 2011
/// @brief    [\ref Enzo] Initialization routine for 2D implosion problem

#ifndef ENZO_ENZO_INITIAL_SEDOV_ARRAY3_HPP
#define ENZO_ENZO_INITIAL_SEDOV_ARRAY3_HPP

enum hydro_type {
  hydro_unknown,
  hydro_ppm,
  hydro_ppml
};

class EnzoInitialSedovArray3 : public Initial {

  /// @class    EnzoInitialSedovArray3
  /// @ingroup  Enzo
  /// @brief    [\ref Enzo] Initialization routine for 2D implosion problem

public: // interface

  /// CHARM++ constructor
  EnzoInitialSedovArray3() throw() { }
  
  /// Constructor
  EnzoInitialSedovArray3(const EnzoConfig * enzo_config) throw();

#ifdef CONFIG_USE_CHARM

  /// CHARM++ PUP::able declaration
  PUPable_decl(EnzoInitialSedovArray3);

  /// CHARM++ migration constructor
  EnzoInitialSedovArray3(CkMigrateMessage *m) : Initial (m) {}

  /// CHARM++ Pack / Unpack function
  void pup (PUP::er &p);

#endif

  /// Initialize the block

  virtual void enforce 
  ( Block * block, const FieldDescr * field_descr, const Hierarchy * hierarchy ) throw();

private: // attributes

  /// Size of the array of Sedov blasts
  int array_[3];

  /// Whether PPM or PPML is used
  hydro_type hydro_;

};

#endif /* ENZO_ENZO_INITIAL_SEDOV_ARRAY3_HPP */

