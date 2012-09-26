// See LICENSE_CELLO file for license and copyright information

/// @file     enzo_EnzoInitialImplosion2.hpp
/// @author   James Bordner (jobordner@ucsd.edu)
/// @date     Tue Jan  4 19:26:38 PST 2011
/// @brief    [\ref Enzo] Initialization routine for 2D implosion problem

#ifndef ENZO_ENZO_INITIAL_IMPLOSION2_HPP
#define ENZO_ENZO_INITIAL_IMPLOSION2_HPP

class EnzoInitialImplosion2 : public Initial {

  /// @class    EnzoInitialImplosion2
  /// @ingroup  Enzo
  /// @brief    [\ref Enzo] Initialization routine for 2D implosion problem

public: // interface

  /// Constructor
  EnzoInitialImplosion2(int cycle, double time) throw();

#ifdef CONFIG_USE_CHARM
  /// CHARM++ Pack / Unpack function
  inline void pup (PUP::er &p)
  {
    TRACEPUP;
    // NOTE: change this function whenever attributes change
    Initial::pup(p);
  }
#endif

  /// Initialize the block

  virtual void enforce 
  (
   Block * block,
   const FieldDescr * field_descr,
   const Hierarchy * hierarchy
   ) throw();

};

#endif /* ENZO_ENZO_INITIAL_IMPLOSION2_HPP */

