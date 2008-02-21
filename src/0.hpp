#ifndef FOO_HPP
#define FOO_HPP
/** 
 *********************************************************************
 *
 * @file      foo.hpp
 * @brief     Brief description of the file
 * @author    Bart Simpson
 * @date      Thu Feb 21 12:45:56 PST 2008
 * @bug       Example bug description 
 * @note      Modified by Alfred E. Newman, Thu Feb 21 12:49:16 PST 2008
 *            fixed bug
 * @note      Modified by Alfred E. Newman, Thu Feb 21 12:49:16 PST 2008
 *            fixed bug
 *
 * Detailed description of the file
 *
 *********************************************************************
 */

class Foo {

/** 
 *********************************************************************
 *
 * @class     Foo
 * @brief     Brief description of the class
 * @ingroup   Group 
 *
 * Detailed description of the class
 *
 *********************************************************************
 */

private:

  //-------------------------------------------------------------------
  // PRIVATE ATTRIBUTES
  //-------------------------------------------------------------------

  /// A private attribute
  int private_attribute_;

  //-------------------------------------------------------------------
  // PUBLIC OPERATIONS
  //-------------------------------------------------------------------

public:

  /// A public operation
  void Foo(Foo &);

  //-------------------------------------------------------------------
  // PRIVATE OPERATIONS
  //-------------------------------------------------------------------

private:

  /// A private operation
  void foo_();

};

#endif
