// See LICENSE_CELLO file for license and copyright information

/// @file     particle_ParticleDescr.hpp
/// @author   James Bordner (jobordner@ucsd.edu)
/// @date     2009-11-17
/// @brief    [\ref Particle] Declaration for the ParticleDescr class

#ifndef PARTICLE_PARTICLE_DESCR_HPP
#define PARTICLE_PARTICLE_DESCR_HPP

class ParticleDescr {

  /// @class    ParticleDescr
  /// @ingroup  Particle
  /// @brief    [\ref Particle] Interface for the ParticleDescr class

public: // public

  /// Initialize a ParticleDescr object
  ParticleDescr()
    : name_()
  { };

  /// Set ParticleDescr name
  void set_name (std::string name)
  { name_ = name; };

  /// Get ParticleDescr name
  std::string name ()
  { return name_; };

private: // attributes

  /// String defining the Particle's name
  std::string name_;

};

#endif /* PARTICLE_PARTICLE_DESCR_HPP */

