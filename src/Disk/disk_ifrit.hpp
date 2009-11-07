#ifndef DISK_IFRIT_HPP
#define DISK_IFRIT_HPP

/** 
 *********************************************************************
 *
 * @file      disk_ifrit.hpp
 * @brief     Definition of the Ifrit class
 * @author    James Bordner
 * @date      Thu Feb 21 16:05:34 PST 2008
 *
 * $Id: disk_ifrit.hpp 965 2009-10-15 20:09:26Z bordner $
 *
 *********************************************************************
 */
 
class Ifrit {

/** 
 *********************************************************************
 *
 * @class     Ifrit
 * @brief     Class for writing and reading IFRIT files
 * @ingroup   Storage
 *
 * An Ifrit object currently corresponds to a single IFRIT file / group
 * / dataset.
 *
 *********************************************************************
 */

  //-------------------------------------------------------------------
  // PUBLIC OPERATIONS
  //-------------------------------------------------------------------

public:

  /// Initialize the Ifrit object
  Ifrit() {};
  ~Ifrit() {};
  void read_bin  (std::string name, Scalar * buffer, int * nx, int * ny, int * nz);
  void write_bin (std::string name, Scalar * buffer, int   nx, int   ny, int   nz);

  //-------------------------------------------------------------------
  // PRIVATE ATTRIBUTES
  //-------------------------------------------------------------------

private:

  //-------------------------------------------------------------------
  // PRIVATE OPERATIONS
  //-------------------------------------------------------------------

private:

};

#endif /* DISK_IFRIT_HPP */

