/*
 * ENZO: THE NEXT GENERATION
 *
 * A parallel astrophysics and cosmology application
 *
 * Copyright (C) 2008 James Bordner
 * Copyright (C) 2008 Laboratory for Computational Astrophysics
 * Copyright (C) 2008 Regents of the University of California
 *
 * See CELLO_LICENSE in the main directory for full license agreement
 *
 */

#ifndef ARRAY_HPP
#define ARRAY_HPP

/** 
 *********************************************************************
 *
 * @file      array.hpp
 * @brief     Declaration of the Array class
 * @author    James Bordner
 * @date      Thu Feb 21 13:54:19 PST 2008
 * @bug       none
 *
 * $Id$
 * 
 *********************************************************************
 */

class Array {

/** 
 *********************************************************************
 *
 * @class     Array
 * @brief     Encapsulate a fortran-style 1D,2D, or 3D array
 * @ingroup   Data
 *
 * Parallelism will be controlled by an object in the Parallel module.
 * IO is controlled by a function in the IO module.
 *
 *********************************************************************
 */

private:

  //-------------------------------------------------------------------
  // PRIVATE ATTRIBUTES
  //-------------------------------------------------------------------

  /// Length of array
  int     N_;

  /// Shape of array, right-padded with 1's
  int     n_[3];

  /// Array values stored in column-major ordering
  Scalar *a_;

  //-------------------------------------------------------------------
  // PUBLIC OPERATIONS
  //-------------------------------------------------------------------

public:

  /// Create a new uninitialized Array object
  Array();
  /// Create a new initialized Array object
  Array(int  n0, int  n1=1, int n2=1);
  /// Deallocate the array
  ~Array();
  /// Copy an array into this one, deallocating any existing data
  void copy (const Array &);
  /// Resize the array, deallocating any existing data
  void resize (int n0, int n1=1, int n2=1);
  /// Return the size of the array
  void size (int * n0, int * n1=0, int * n2=0) const;
  /// Return the total length of the array
  int  length() const;
  /// Return a pointer to the array values
  Scalar * values () const;
  /// Return the given array element
  Scalar & operator() (int  i0, int  i1=0, int  i2=0);
  /// Set all values to 0, or to the given value if supplied
  void clear(Scalar value = 0.0);

  //-------------------------------------------------------------------
  // PRIVATE OPERATIONS
  //-------------------------------------------------------------------

private:

  /// Allocate array values
  void allocate_ (int n0, int n1=1, int n2=1);

  /// Deallocate array values
  void deallocate_();

  /// Copy array values a[0:N-1] to Array
  void copy_ (Scalar * a);

};

#endif
