
/// Hierarchy class source file

/**
 * 
 * 
 * 
 * @file
 * @author James Bordner <jobordner@ucsd.edu>
 *
 */

#include <assert.h>
#include <stdio.h>
#include <vector>
#include <map>

#include "HYPRE_sstruct_ls.h"

#include "scalar.hpp"
#include "discret.hpp"
#include "mpi.hpp"
#include "grid.hpp"
#include "level.hpp"
#include "hierarchy.hpp"

//----------------------------------------------------------------------

const int debug = 1;

//----------------------------------------------------------------------

/// Hierarchy class constructor.

/** Currently does nothing */

Hierarchy::Hierarchy () throw ()
  : dimension_(0)
{
}
	  
//----------------------------------------------------------------------

Hierarchy::~Hierarchy () throw ()

/// Hierarchy class descructor.

/** Currently does nothing */

{
}

//======================================================================

/// Insert a grid into the hierarchy.

/** Appends the grid's pointer to the grids_ vector */

void Hierarchy::insert_grid (Grid * pgrid) throw ()
{
  if (debug) printf ("Hierarchy::insert_grid()\n");
  if (pgrid->id() >= grids_.size()) {
    if (debug) printf ("DEBUG: resizing Hierarchy::grids_ from %d to %d\n",
		       grids_.size(),pgrid->id() + 1);
    grids_.resize (pgrid->id() + 1);
  }
  grids_[pgrid->id()] = pgrid;

}

//======================================================================

/// Initialize grid inter-connections.

/** After all grids are inserted into the hierarchy, this function
    determines each grid's parent, containing level, children, and
    neighbors. */

void Hierarchy::init_grids () throw ()
{
  if (debug) printf ("Hierarchy::init_levels()\n");

  init_grid_parents_();
  init_grid_levels_();
  init_grid_children_();
  init_grid_neighbors_();

}

//------------------------------------------------------------------------

/// Determines the parent grid of all grids in the hierarchy.

void Hierarchy::init_grid_parents_ () throw ()
{
  int i,j,j1,j2,k;

  for (i=0; i<num_grids (); i++) {
    Grid * g = &grid(i);
    Grid * p = (g->id_parent() >= 0) ? & grid(g->id_parent()) : 0;
    this->set_parent(g,p);
  }
}

//------------------------------------------------------------------------

/// Determines the level of each grid in the hierarchy.

void Hierarchy::init_grid_levels_ () throw ()
{
  bool done = false;
  while (! done) {
    done = true;
    int i;
    for (i=0; i<num_grids (); i++) {
      Grid * g = &grid(i);
      // If grid's level < 0, then we haven't determined its level yet
      if (g->level() < 0) { 

	// Grids without parents are in level 0 ...

	if (parent(g) == 0) {
	  g->set_level(0);
	  insert_in_level_ (0,*g);
	}
	
	// ... grids with parents of known level have level = 1 + parent level ...

	else if (parent(g)->level() >= 0) {
	  int level = parent(g)->level() + 1;
	  g->set_level(level);
	  insert_in_level_ (level,*g);
	} 
	
	// ... otherwise a grid's parents is in an unknown level, so we're not done yet

	else {
	  done = false;
	}
      }
    }
  }
}

//------------------------------------------------------------------------

///  Initialize grid children.

void Hierarchy::init_grid_children_ () throw ()
{
  int i;
  for (i=0; i<num_grids(); i++) {
    Grid * g = & grid(i);
    // If a grid has a parent, then the grid is the parent's child
    if (parent(g) != 0) {
      parent(g)->set_child(*g);
    }
  }
}


//------------------------------------------------------------------------

///  Find each grid's neighbors.

/**  If a grid is in level 0, then find its neighbors by comparing
     with all other grids in the level.  Otherwise, if a grid is in a
     level > 0, then we can save time by only testing grids that are
     children of the parent, and children of all the parents
     neighbors. */

void Hierarchy::init_grid_neighbors_ () throw ()
{

  int i,j;

  // For level == 0, test all pairs

  for (i=0; i<level(0).num_grids(); i++) {

    Grid * g1 = & level(0).grid(i);

    for (j=i+1; j<level(0).num_grids(); j++) {

      Grid * g2 = & level(0).grid(j);

      if (g1->is_adjacent(*g2)) {
	if (debug) printf ("DEBUG grids %d and %d are adjacent\n",
			   g1->id(),g2->id());
	g1->set_neighbor (*g2);
	g2->set_neighbor (*g1);
      }
    }
  }

  // For levels > 0, only test parents' children, and parents'
  // neighbors' children.  If two grids' parents are not the same or
  // not neighbors, then they necessarily cannot be neighbors.

  int k,j1,j2;

  for (k=1; k<num_levels(); k++) {

    for (i=0; i<level(k).num_grids(); i++) {

      Grid * g1 = & level(k).grid(i);

      // Check parents' children

      for (j=0; j<parent(g1)->num_children(); j++) {
	Grid * g2 = & parent(g1)->child(j);
	if (g1->is_adjacent(*g2) && g1->id() > g2->id()) {
	  assert_neighbors (*g1,*g2);
	}
      }

      // Check parents' neighbors' children

      for (j1=0; j1<parent(g1)->num_neighbors(); j1++) {
	Grid * gn = & parent(g1)->neighbor(j1);
	for (j2=0; j2<gn->num_children(); j2++) {
	  Grid * g2 = & gn->child(j2);
	  if (g1->is_adjacent(*g2) && g1->id() > g2->id()) {
	    if (debug) printf ("DEBUG grids %d and %d are adjacent cousins\n",
			       g1->id(),g2->id());
	    assert_neighbors (*g1,*g2);
	  }
	}
      }
    }
  }
}

//------------------------------------------------------------------------

/// Initialize discretization

/** After all grid inter-connections are determined, this function
    determines the neighbor structure for each individual zone along
    the boundary. Only performed for local grids. */

void Hierarchy::init_discret () throw ()

{
  int k,i;

  // For level == 0, check neighbors and boundary

  // For level >= 1, check parent, parents neighbors neighbors and boundary

  for (k=0; k<num_levels(); k++) {

    for (i=0; i<level(k).num_grids(); i++) {

      Grid & grid = level(k).grid(i);

      if (grid.is_local()) {

	// Update each grid's Discret's boundary mask

	for (int j=0; j < grid.num_neighbors(); j++) {

	  Grid & neighbor = grid.neighbor(j);
	  int gl[3],gu[3];

	  grid.find_neighbor_indices (neighbor,gl,gu);

	  if (debug) 
	    printf ("DEBUG %s:%d  Neighbor indices of %d = (%d,%d,%d) : (%d,%d,%d)\n",
		    __FILE__,__LINE__,grid.id(),gl[0],gl[1],gl[2],gu[0],gu[1],gu[2]);

	  // find axis and face

	  int axis = -1, face = -1;

	  if (gl[0] == -1)                   {axis = 0; face = 0;}
	  if (gl[0] == grid.num_unknowns(0)) {axis = 0; face = 1;}
	  if (gl[1] == -1)                   {axis = 1; face = 0;}
	  if (gl[1] == grid.num_unknowns(1)) {axis = 1; face = 1;}
	  if (gl[2] == -1)                   {axis = 2; face = 0;}
	  if (gl[2] == grid.num_unknowns(2)) {axis = 2; face = 1;}

	  assert (axis >= 0);
	  assert (face >= 0);

	  // determine index bounds 

	  int in0, in1, jn0, jn1;
	  in0 = gl[(axis+1)%3];
	  in1 = gu[(axis+1)%3];
	  jn0 = gl[(axis+2)%3];
	  jn1 = gu[(axis+2)%3];

	  if (debug) printf ("DEBUG_DISCRET axis=%d face=%d [(%d,%d) - (%d,%d)]\n",
			     axis,face,in0,jn0,in1,jn1);
      
	  // set Discret boundary mask

	  for (int in=in0; in<=in1; in++) {
	    for (int jn=jn0; jn<=jn1; jn++) {
	      grid.discret().neighbor_cell(axis,face,in,jn) = 0;
	    }
	  }

	  if (debug) grid.discret().print();
	}
      }
    }
  }
}

//======================================================================

void Hierarchy::print () throw ()
{
  printf ("Hierarchy\n");
  for (int i=0; i<num_levels(); i++) {
    level(i).print();
  }
}

//----------------------------------------------------------------------

void Hierarchy::write (FILE *fp) throw ()
{
  if (fp == 0) fp = stdout;

  fprintf (fp,"Hierarchy\n");
  for (int i=0; i<num_levels(); i++) {
    level(i).write();
  }
}

//======================================================================
// PRIVATE MEMBER FUNCTIONS
//======================================================================

void Hierarchy::insert_in_level_ (int level, Grid & grid) throw ()
{
  // Resize levels_[] if needed
  if (level >= levels_.size()) {
    if (debug) printf ("DEBUG: resizing Hierarchy::levels_ from %d to %d\n",
		       levels_.size(),level + 1);
    levels_.resize (level + 1);
  }
  if (levels_.at(level) == 0) {
    if (debug) printf ("DEBUG: creating new Level at %d\n",level);
    levels_[level] = new Level(level);
  }
  levels_[level]->insert_grid (grid);
}
