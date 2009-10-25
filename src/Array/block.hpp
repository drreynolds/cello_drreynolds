#ifndef BLOCK_HPP
#define BLOCK_HPP

//345678901234567890123456789012345678901234567890123456789012345678901234567890

/** 
*********************************************************************
*
* @file      block.hpp
* @brief     Declaration of the Block class
* @author    James Bordner
* @date      Mon Oct 12 14:38:21 PDT 2009
* @bug       none
* @note      
*
* Defines up to a 4-D fortran-like array for storing 1 or more 3D
* arrays.  Axes can be permuted, including the index selecting the
* array for storing interleaved arrays.
*
* $Id$
* 
*********************************************************************
*/

class Block {

  /** 
*********************************************************************
*
* @class     Block
* @brief     Interface class between Array and low-level (C/fortran) routines
* @ingroup   Array
*
* DEPENDENCIES
*
*
*********************************************************************
*/

  //-------------------------------------------------------------------
  // PUBLIC OPERATIONS
  //-------------------------------------------------------------------

public:

  /// Create a new uninitialized Block object
  Block() throw() {};

  /// Create a new uninitialized Block object
  Block(Scalar * array, 
	int * p,
	int ndx,  int ndy,  int ndz,  int nda=1,
	int nx=0, int ny=0, int nz=0, int na=0)
    throw() :
    array_(array)
  { 
    if (p) {
      p_[p[0]] = 0; // default 1 = x
      p_[p[1]] = 1; // default 2 = y
      p_[p[2]] = 2; // default 3 = z
      p_[p[3]] = 3; // default 4 = a
    } else {
      p_[0] = 0;
      p_[1] = 1;
      p_[2] = 2;
      p_[3] = 3;
    }
    q_[p_[0]] = 0;
    q_[p_[1]] = 1;
    q_[p_[2]] = 2;
    q_[p_[3]] = 3;

    nd_[p_[0]] = ndx;
    nd_[p_[1]] = ndy;
    nd_[p_[2]] = ndz;
    nd_[p_[3]] = nda;

    n_[p_[0]] = nx ? nx : ndx;
    n_[p_[1]] = ny ? ny : ndy;
    n_[p_[2]] = nz ? nz : ndz;
    n_[p_[3]] = na ? na : nda;

    m_[p_[0]] = n_[p_[3]];
    m_[p_[1]] = n_[p_[0]]*m_[p_[0]];
    m_[p_[2]] = n_[p_[1]]*m_[p_[1]];
    m_[p_[3]] = n_[p_[2]]*m_[p_[2]];

  }

  /// Return allocated block dimensions
  void get_dim (int * ndx, int * ndy = NULL, int * ndz = NULL) const throw()
  { 
    if (ndx) *ndx = nd_[p_[0]];
    if (ndy) *ndy = nd_[p_[1]];
    if (ndz) *ndz = nd_[p_[2]];
  }

  /// Return the array size
  void get_size (int * nx, int * ny = NULL, int * nz = NULL, int *na = 0) const throw()
  {
    if (nx) *nx = n_[p_[0]];
    if (ny) *ny = n_[p_[1]];
    if (nz) *nz = n_[p_[2]];
    if (na) *na = n_[p_[3]];
  }

  /// Return increments for loop index calculations
  void get_inc (int * mx, int * my = NULL, int * mz = NULL, int * ma = NULL) const throw () 	
  {
    if (mx) *mx = m_[p_[0]];
    if (my) *my = m_[p_[1]];
    if (mz) *mz = m_[p_[2]];
    if (ma) *ma = m_[p_[3]];
  }

  /// Return a pointer to the ith array
  Scalar * array(int i=0) const throw()
  {
    return & array_[i*m_[3]];
  }


  /// Return stride for the array
  void get_stride() const throw ()
  {
    INCOMPLETE_MESSAGE("Block::get_stride()","");
  }
  
private:

  /// Pointer to the 0 element of the array
  Scalar * array_;

  /// Dimensions of the array
  int nd_[4];

  /// Size of the array
  int n_[4];

  /// Increments
  int m_[4];

  /// Permutation p_[0:3] = [ix, iy, iz, ia]
  int p_[4];

  /// Inverse permutation q_[ix,iy,iz,ia] = [0:3]
  int q_[4];

};   

#endif /* BLOCK_HPP */
