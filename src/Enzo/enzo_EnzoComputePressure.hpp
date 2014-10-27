// See LICENSE_CELLO file for license and copyright information

/// @file     enzo_EnzoComputePressure.hpp
/// @author   James Bordner (jobordner@ucsd.edu) 
/// @date     2014-10-27 22:37:41
/// @brief    [\ref Enzo] Implementation of Enzo's ComputePressure functions

#ifndef ENZO_ENZO_COMPUTE_PRESSURE_HPP
#define ENZO_ENZO_COMPUTE_PRESSURE_HPP

class EnzoComputePressure : public Compute {

  /// @class    EnzoComputePressure
  /// @ingroup  Enzo
  /// @brief    [\ref Enzo] Encapsulate Enzo's ComputePressure functions

public: // interface

  /// Create a new EnzoComputePressure object
  EnzoComputePressure(double gamma);

  /// Charm++ PUP::able declarations
  PUPable_decl(EnzoComputePressure);
  
  /// Charm++ PUP::able migration constructor
  EnzoComputePressure (CkMigrateMessage *m) {}

  /// CHARM++ Pack / Unpack function
  void pup (PUP::er &p);
  
  /// Perform the computation on the block
  virtual void compute( CommBlock * comm_block) throw();

private: // functions

  template <typename T>
  void compute_(CommBlock * comm_block);

private: // attributes

  double gamma_;

};

#endif /* ENZO_ENZO_COMPUTE_PRESSURE_HPP */
