// See LICENSE_CELLO file for license and copyright information

/// @file     enzo_EnzoFactory.hpp
/// @author   James Bordner (jobordner@ucsd.edu)
/// @date     Tue Mar 15 15:29:56 PDT 2011
/// @brief    [\ref Enzo] Declaration of the EnzoFactory class

#ifndef ENZO_ENZO_FACTORY_HPP
#define ENZO_ENZO_FACTORY_HPP

class EnzoFactory : public Factory {

  /// @class    EnzoFactory
  /// @ingroup  Enzo
  /// @brief [\ref Enzo] Abstract class for creating concrete Hierarchy,
  /// Patch, and Block objects

public: // interface

  /// Create the Input / Output accessor object for EnzoBlock
  virtual IoBlock * create_io_block () const throw();

  /// Create a new Block  [abstract factory design pattern]
  virtual Block * create_block
  (int ibx, int iby, int ibz,
   int nbx, int nby, int nbz,
   int nx, int ny, int nz,
   double xm, double ym, double zm,
   double hx, double hy, double hz,
   int num_field_blocks = 1) const throw();

#ifdef CONFIG_USE_CHARM
  /// Create a new CHARM++ Block array [abstract factory design pattern]
  virtual CProxy_Block create_block_array
  (int nbx, int nby, int nbz,
   int nx, int ny, int nz,
   double xm, double ym, double zm,
   double hx, double hy, double hz,
   int num_field_blocks = 1) const throw();
#endif

};

#endif /* ENZO_ENZO_FACTORY_HPP */

