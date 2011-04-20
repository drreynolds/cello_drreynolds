// $Id: mesh_Factory.hpp 2009 2011-02-22 19:43:07Z bordner $
// See LICENSE_CELLO file for license and copyright information

#ifndef MESH_FACTORY_HPP
#define MESH_FACTORY_HPP

/// @file     mesh_Factory.hpp
/// @author   James Bordner (jobordner@ucsd.edu)
/// @date     Tue Mar 15 15:29:56 PDT 2011
/// @brief    [\ref Mesh] Declaration of the Factory class

class Factory {

  /// @class    Factory
  /// @ingroup  Mesh
  /// @brief    [\ref Mesh] Abstract class for creating concrete Mesh, Mesh, Patch, and Block objects

public: // interface

#ifdef CONFIG_USE_CHARM
  virtual void pup(PUP::er &) {}
#endif

  /// Create a new Mesh [abstract factory design pattern]
  virtual Mesh * create_mesh
  (int nx,  int ny,  int nz,
   int nbx, int nby, int nbz) const throw ();

  /// Create a new Patch [abstract factory design pattern]
  virtual Patch * create_patch
  (GroupProcess * group_process,
   int nx,   int ny,  int nz,
   int nbx,  int nby, int nbz,
   double xm, double ym, double zm,
   double xp, double yp, double zp) const throw();

  /// Create a new Block [abstract factory design pattern]
  virtual Block * create_block
  (int ibx, int iby, int ibz,
   int nbx, int nby, int nbz,
   int nx, int ny, int nz,
   double xm, double ym, double zm,
   double hx, double hy, double hz,
   int num_field_blocks = 1) const throw();

};

#endif /* MESH_FACTORY_HPP */

