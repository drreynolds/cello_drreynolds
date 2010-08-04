// $Id$
// See LICENSE_CELLO file for license and copyright information

/// @file     test_Data.cpp
/// @author   James Bordner (jobordner@ucsd.edu)
/// @date     2010-04-02
/// @brief    Test program for the DataBlock class

#include "cello.hpp"

#include "error.hpp"
#include "test.hpp"
#include "data.hpp"
#include "field.hpp"

int main()
{

  unit_init();

  unit_class ("DataDescr");

  DataDescr data_descr;

  unit_assert (data_descr.field_descr() != NULL)

  unit_finalize();

}
