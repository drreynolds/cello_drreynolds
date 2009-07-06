//345678901234567890123456789012345678901234567890123456789012345678901234567890

#ifndef ARRAY_SCALAR_HPP
#define ARRAY_SCALAR_HPP

/// Defines Scalar

/**
 * @file    array_scalar_.hpp
 * @brief   Header file for the Array class
 * @author  James Bordner 
 * @version 1.0
 *
 * Definitions
 *
 * (*) Scalar
 *
 * $Id$
 * 
 */
// $Log$

/// Define Scalar to be double.  Should be more flexible and easy to change.

#define Scalar double
#define SCALAR_SCANF "%lf"
#define SCALAR_PRINTF "%le "
#define MPI_SCALAR MPI_DOUBLE

#endif
