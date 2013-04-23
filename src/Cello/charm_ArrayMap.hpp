// See LICENSE_CELLO file for license and copyright information

/// @file     charm_ArrayMap.hpp
/// @author   James Bordner (jobordner@ucsd.edu)
/// @date     yyyy-mm-dd
/// @brief    [\ref Charm] Declaration of the ArrayMap class
///

#ifndef CHARM_ARRAY_MAP_HPP
#define CHARM_ARRAY_MAP_HPP

#include "simulation.decl.h"

class ArrayMap: public CBase_ArrayMap {

  /// @class    ArrayMap
  /// @ingroup  Charm
  /// @brief    [\ref Charm] 

public:
  int *mapping;

  ArrayMap(int nx, int ny, int nz);
  ~ArrayMap();
  int procNum(int, const CkArrayIndex &idx);

#ifdef CONFIG_USE_CHARM
  /// CHARM++ migration constructor for PUP::able
  ArrayMap (CkMigrateMessage *m) : CBase_ArrayMap(m) {}

  /// CHARM++ Pack / Unpack function
  inline void pup (PUP::er &p)
  {
    CBase_ArrayMap::pup(p);
    // NOTE: change this function whenever attributes change
    p | nx_;
    p | ny_;
    p | nz_;
  }
#endif

private:

  int nx_, ny_, nz_;

};

#endif /* CHARM_ARRAY_MAP_HPP */

