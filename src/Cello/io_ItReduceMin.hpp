// See LICENSE_CELLO file for license and copyright information

/// @file     io_ItReduceMin.hpp
/// @author   James Bordner (jobordner@ucsd.edu)
/// @date     2011-09-26
/// @brief    [\ref Reduce] Iterator class for averaging

#ifndef IO_IT_REDUCE_MIN_HPP
#define IO_IT_REDUCE_MIN_HPP

class ItReduceMin : public ItReduce {

  /// @class    ItReduceMin
  /// @ingroup  Io
  /// @brief    [\ref Io] Iterator over Reduces in a Reduce

public: //

  /// Prohibit creating new ItReduceMins directly: require ItReduceMin::create()
  ItReduceMin () throw ()
  : ItReduce(std::numeric_limits<double>::max())
  { }


public: // interface

  /// Delete the ItReduceMin object
  virtual ~ItReduceMin () throw ()
  { }
  
  /// Reduce another value
  virtual void next (double value) throw()
  { value_ = MIN(value,value_); }

  /// Reset the Iterator to the beginning
  virtual void first() throw()
  { value_ = std::numeric_limits<double>::max(); }


  /// Return the current value of the reduction operator
  virtual double value() const throw()
  { return value_; }

private: // attributes

};

#endif /* IO_IT_REDUCE_MIN_HPP */
