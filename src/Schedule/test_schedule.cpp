//345678901234567890123456789012345678901234567890123456789012345678901234567890

/** 
 *********************************************************************
 *
 * @file      test_schedule.cpp
 * @brief     Program implementing unit tests for the Schedule class
 * @author    James Bordner
 * @date      Thu Feb 21 16:47:35 PST 2008
 *
 * $Id$
 *
 *********************************************************************
 */
 
#include "cello.h"

#include "error.hpp"
#include "test.hpp"

int main(int argc, char ** argv)
{
  bool passed = false;

  unit_class ("Schedule");
  unit_open();

  //  Schedule schedule;

  unit_assert(passed); //FAILS

  unit_close();
}
