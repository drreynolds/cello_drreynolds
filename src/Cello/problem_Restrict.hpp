// See LICENSE_CELLO file for license and copyright information

/// @file     problem_Restrict.hpp
/// @author   James Bordner (jobordner@ucsd.edu)
/// @date     2013-05-10
/// @brief    [\ref Problem] Declaration of the Restrict class
///

#ifndef PROBLEM_RESTRICT_HPP
#define PROBLEM_RESTRICT_HPP

class Restrict 
#ifdef CONFIG_USE_CHARM
  : public PUP::able 
#endif

{
  /// @class    Restrict
  /// @ingroup  Problem
  /// @brief    [\ref Problem] 

public: // interface

  /// Constructor
  Restrict() throw();

#ifdef CONFIG_USE_CHARM

  /// CHARM++ PUP::able declaration
  PUPable_abstract(Restrict);

  // /// CHARM++ migration constructor for PUP::able
  // Restrict (CkMigrateMessage *m) : PUP::able(m) {}

  /// CHARM++ Pack / Unpack function
  void pup (PUP::er &p) 
  { TRACEPUP; PUP::able::pup(p); }

#endif

  /// Restrict comm_block_f child values given by (icx,icy,icz) to the parent
  virtual void apply 
  ( precision_type precision,
    void *       values_f, int nd3_f[3], int n3_f[3],
    const void * values_c, int nd3_c[3], int n3_c[3]) = 0;

private: // functions


private: // attributes

  // NOTE: change pup() function whenever attributes change

};

#endif /* PROBLEM_RESTRICT_HPP */

