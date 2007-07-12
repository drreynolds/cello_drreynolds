//======================================================================
//
//        File: level.cpp
//
//     Summary: Level class source file
//
// Description:
//
//      Author: James Bordner <jobordner@ucsd.edu>
//
//        Date: 2007-03-26
//
//======================================================================

#include <assert.h>
#include <stdio.h>
#include <string>
#include <vector>

#include "HYPRE_sstruct_ls.h"

#include "scalar.hpp"
#include "discret.hpp"
#include "grid.hpp"
#include "level.hpp"

//----------------------------------------------------------------------

const int debug = 1;

//----------------------------------------------------------------------

Level::Level (int n) throw ()
  : n_ (n)
{
}
	  
//----------------------------------------------------------------------

Level::~Level () throw ()
{
}

//======================================================================

void Level::insert_grid (Grid & grid) throw ()
{
  printf ("DEBUG inserting grid into level %d\n",n_);
  grids_.push_back (& grid);
}

//======================================================================

void Level::print () throw ()
{
  for (int i=0; i<num_grids(); i++) {
    printf ("Level %d\n",n_);
    grid(i).print();
  }
}

//----------------------------------------------------------------------

void Level::write (FILE *fp) throw ()
{
  if (fp == 0) fp = stdout;

  for (int i=0; i<num_grids(); i++) {
    fprintf (fp,"Level %d\n",n_);
    grid(i).write(fp);
  }
}

