// $Id: test_Block.cpp 1696 2010-08-04 05:56:36Z bordner $
// See LICENSE_CELLO file for license and copyright information

/// @file     test_Block.cpp
/// @author   James Bordner (jobordner@ucsd.edu)
/// @date     Tue Mar  1 20:42:57 UTC 2011
/// @todo     Create CHARM version of test_Block
/// @brief    Test program for the Block class

#include "test.hpp"

#include "mesh.hpp"

#include PARALLEL_CHARM_INCLUDE(test_Block.decl.h)

PARALLEL_MAIN_BEGIN
{

  PARALLEL_INIT;

  unit_init();

  unit_class("Block");

  Block * block = new Block (0,0,0, 
			     3,4,5,
			     -1.0,-2.0,-3.0,
			     1.0,2.0,3.0,1);

  unit_func("Block");
  unit_assert (block != NULL);

  //----------------------------------------------------------------------

  double lower[3];
  double upper[3];

  block->lower(&lower[0],&lower[1],&lower[2]);
  block->upper(&upper[0],&upper[1],&upper[2]);

  unit_func("lower");
  unit_assert(lower[0] == -1.0);
  unit_assert(lower[1] == -2.0);
  unit_assert(lower[2] == -3.0);

  unit_func("upper");
  unit_assert(upper[0] ==  1.0);
  unit_assert(upper[1] ==  2.0);
  unit_assert(upper[2] ==  3.0);

  unit_finalize();

  PARALLEL_EXIT;
}

PARALLEL_MAIN_END

#include PARALLEL_CHARM_INCLUDE(test_Block.def.h)
