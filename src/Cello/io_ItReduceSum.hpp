// See LICENSE_CELLO file for license and copyright information

/// @file     io_ItReduceSum.hpp
/// @author   James Bordner (jobordner@ucsd.edu)
/// @date     2011-09-26
/// @brief    [\ref Reduce] Iterator class for averaging

#ifndef IO_IT_REDUCE_SUM_HPP
#define IO_IT_REDUCE_SUM_HPP

class ItReduceSum : public ItReduce {

  /// @class    ItReduceSum
  /// @ingroup  Io
  /// @brief    [\ref Io] Iterator over Reduces in a Reduce


public: // interface

  /// Create an ItReduceSum object
  ItReduceSum () throw ()
  : ItReduce(0.0)
  {}

  /// Delete the ItReduceSum object
  virtual ~ItReduceSum () throw ()
  {}
  
#ifdef CONFIG_USE_CHARM
  /// CHARM++ Pack / Unpack function
  inline void pup (PUP::er &p)
  {
    // NOTE: change this function whenever attributes change
    ItReduce::pup(p);
  }
#endif

  /// Reduce another value
  virtual void next (double value) throw()
  { value_ += value; }

  /// Reset the Iterator to the beginning
  virtual void first() throw()
  { value_ = 0.0; }

  /// Return the current value of the reduction operator
  virtual double value() const throw()
  { return value_; }

private: // attributes

};

#endif /* IO_IT_REDUCE_SUM_HPP */
