
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
#include <math.h>

#include <vector>
#include <map>

#include "HYPRE_sstruct_ls.h"

#include "hypre-solve.hpp"

#include "scalar.hpp"
#include "point.hpp"
#include "domain.hpp"
#include "faces.hpp"
#include "mpi.hpp"
#include "grid.hpp"
#include "level.hpp"
#include "hierarchy.hpp"


//----------------------------------------------------------------------

const int trace    = 1;
const int debug    = 1;
const int geomview = 1;

//----------------------------------------------------------------------

/// Hierarchy class constructor.

/** Currently does nothing */

Hierarchy::Hierarchy () throw ()
  : dimension_(0)
{
  grids0_.push_back(0);
  levels0_.push_back(0);
}
	  
//----------------------------------------------------------------------

Hierarchy::~Hierarchy () throw ()

/// Hierarchy class descructor.

/** Currently does nothing */

{
}

//======================================================================

/// Insert a grid into the hierarchy.

/** Inserts the grid's pointer into the grids0_ vector, at the
  * position given by the grid's id(), and always maintaining a 0
  * sentinel at the end of grids0_ */

void Hierarchy::insert_grid (Grid * pgrid) throw ()
{
  // Insert grid into the list of all grids in the Hierarchy

  if (unsigned(pgrid->id()) + 1 >= grids0_.size()) {
    grids0_.resize (pgrid->id() + 2);
    grids0_[grids0_.size() - 1] = 0;
  }
  grids0_[pgrid->id()] = pgrid;
}

//======================================================================

/// Initialize the hierarchy after all grids have been inserted

/** After all grids are inserted into the hierarchy, this function
    determines each grid's parent, containing level, children, 
    neighbors, face-zone categories, and global indices. */

void Hierarchy::initialize (Domain & domain,
			    Mpi    & mpi) throw ()
{
  if (debug) printf ("Hierarchy::init_levels()\n");

  init_grid_parents_();
  init_grid_levels_();
  init_grid_children_();
  init_grid_neighbors_();
  init_indices_();                // DEPENDENCY: Requires init_grid_levels_()
  init_grid_faces_(domain, mpi);  // DEPENDENCY: Requires init_indices_()
}

//------------------------------------------------------------------------

/// Determines the parent grid of all grids in the hierarchy.

void Hierarchy::init_grid_parents_ () throw ()
{
  ItHierarchyGridsAll itg (*this);
  while (Grid * g = itg++) {
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

    // Loop through grids, and try to determine their level if unknown

    ItHierarchyGridsAll itg (*this);
    while (Grid * g = itg++) {

      // If grid's level < 0, then we haven't determined its level yet

      if (g->level() < 0) { 

	// Grids without parents must be in level 0 ...

	if (parent(*g) == 0) {
	  g->set_level(0);
	  insert_in_level_ (0,*g);
	}
	
	// ... grids with parents of known level have level = 1 + parent level ...

	else if (parent(*g)->level() >= 0) {
	  int level = parent(*g)->level() + 1;
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
  ItHierarchyGridsAll itg (*this);
  while (Grid * g = itg++) {
    // If a grid has a parent, then the grid is the parent's child
    if (parent(*g) != 0) {
      parent(*g)->set_child(*g);
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

      for (j=0; j<parent(*g1)->num_children(); j++) {
	Grid * g2 = & parent(*g1)->child(j);
	if (g1->is_adjacent(*g2) && g1->id() > g2->id()) {
	  assert_neighbors (*g1,*g2);
	}
      }

      // Check parents' neighbors' children

      for (j1=0; j1<parent(*g1)->num_neighbors(); j1++) {
	Grid * gn = & parent(*g1)->neighbor(j1);
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

///  Find each grid's face categories.

/** After all grid inter-connections are determined, this function
    determines the neighbor structure for each face-zone. Only
    performed for local grids, and grids neighboring local grids. 
    Allocates Faces for grids that haven't had them allocated yet. */

void Hierarchy::init_grid_faces_ (Domain & domain,
				  Mpi    & mpi) throw ()

{
  int axis, face;

  // ------------------------------------------------------------
  // First determine grid neighbors for each face-zone
  // ------------------------------------------------------------

  ItHierarchyLevels itl (*this);
  while (Level * level = itl++) {

    ItLevelGridsAll itg (*level);

    while (Grid * grid = itg++) {

      int ig[3][2];
      grid->i_lower(ig[0][0],ig[1][0],ig[2][0]);
      grid->i_upper(ig[0][1],ig[1][1],ig[2][1]);

      ItGridNeighbors itn (*grid);

      while (Grid * neighbor = itn++) {

	int in[3][2];
	neighbor->i_lower(in[0][0],in[1][0],in[2][0]);
	neighbor->i_upper(in[0][1],in[1][1],in[2][1]);

	if (grid->is_local() || neighbor->is_local()) {
	  
	  int axis,face;
	  int i0,il0,iu0;
	  int i1,il1,iu1;
	  // Determine index bounds of face intersection
	  if (grid->shared_face(*neighbor,axis,face,il0,il1,iu0,iu1)) {
	    for (i0=il0; i0<iu0; i0++) {
	      for (i1=il1; i1<iu1; i1++) {
		grid->faces().neighbor(axis,face,i0,i1) = neighbor;
	      }
	    }
	  }
	}
      }
    }
  }

  // ------------------------------------------------------------
  // Label boundary face-zones
  // ------------------------------------------------------------

  // Get hierarchy root-grid index extents

  int ih0[3][2];
  i_lower0(ih0[0][0],ih0[1][0],ih0[2][0]);
  i_upper0(ih0[0][1],ih0[1][1],ih0[2][1]);

  // Loop over grids in the root level

  ItLevelGridsLocal itgl (this->level(0));
  while (Grid * grid = itgl++) {

    // Get grid index extents

    int ig[3][2];
    grid->i_lower(ig[0][0],ig[1][0],ig[2][0]);
    grid->i_upper(ig[0][1],ig[1][1],ig[2][1]);

    // Label zones in grid faces adjacent to domain faces

    for (axis = 0; axis < 3; axis++) {
      for (face = 0; face < 2; face++) {
	if (ih0[axis][face] == ig[axis][face]) {
	  grid->faces().label(axis,face,Faces::_boundary_);
	}
      }
    }
  }

  // ------------------------------------------------------------
  // Label covered face-zones
  // ------------------------------------------------------------

  // Loop over non-root levels in the hierarchy, finest to coarsest

  //  ItHierarchyLevelsReverse itl(*this);

  for (int ilevel = num_levels()-1; ilevel>0; ilevel--) {

    Level *level = &this->level(ilevel);

    // Loop over grids in the level

    ItLevelGridsAll itg (*level);

    while (Grid * grid = itg++) {

      // Get grid's index extents

      int ig[3][2];
      grid->i_lower(ig[0][0],ig[1][0],ig[2][0]);
      grid->i_upper(ig[0][1],ig[1][1],ig[2][1]);

      ig[0][1]++;
      ig[1][1]++;
      ig[2][1]++;

      // Get the grid's parent

      Grid * parent = this->parent(*grid);

      // Get parent's index extents

      int ip[3][2];
      parent->i_lower(ip[0][0],ip[1][0],ip[2][0]);
      parent->i_upper(ip[0][1],ip[1][1],ip[2][1]);

      ip[0][1]++;
      ip[1][1]++;
      ip[2][1]++;

      // Find grid faces that intersect parent's

      for (axis = 0; axis < 3; axis++) {
	for (face = 0; face < 2; face++) {

	  bool is_aligned = ig[axis][face] == 2*ip[axis][face];

	  int i1=(axis+1) % 3;
	  int i2=(axis+2) % 3;
	  bool is_enclosed_1 = 2*ip[i1][0] <= ig[i1][0] && ig[i1][1] <= 2*ip[i1][1];
	  bool is_enclosed_2 = 2*ip[i2][0] <= ig[i2][0] && ig[i2][1] <= 2*ip[i2][1];

	  // grid face intersects parent face iff  faces are in the same
	  // plane and the grid face is enclosed by the parent face

	  if (is_aligned && is_enclosed_1 && is_enclosed_2) {
	    if (debug) printf ("%s:%d Grid %d and parent %d are aligned\n",
			       __FILE__,__LINE__,grid->id(),parent->id());
	    // Loop over grid face, marking covered parent face zones as 
	    //	    int i
	  }
	}
      }
    }
  }

  // ------------------------------------------------------------
  // Label fine and neighbor face-zones
  // ------------------------------------------------------------

  // ------------------------------------------------------------
  // Label coarse face-zones
  // ------------------------------------------------------------

  
  if (geomview) {
    char filename[20];
    int ip = mpi.ip();
    for (int ilevel=0; ilevel<this->num_levels(); ilevel++) {
      sprintf (filename,"level%d-p%d.quad",ilevel,ip);
      FILE * fp = fopen (filename,"w");
      this->level(ilevel).geomview_face(fp);
      fclose(fp);
    }
  }
  }

  void Hierarchy::init_indices_ () throw()
{
  _TRACE_;
  // Determine problem size the hard way

  ItLevelGridsAll itg (level(0));

  int i,lower[3],upper[3];

  for (i=0; i<3; i++) {
    lower[i] = INT_MAX;
    upper[i] = INT_MIN;
  }

  while (Grid *grid = itg++) {
    for (i=0; i<3; i++) {
      lower[i] = MIN(grid->i_lower(i),lower[i]);
      upper[i] = MAX(grid->i_upper(i),upper[i]);
    }
  }

  for (i=0; i<3; i++) {
    il0_[i] = lower[i];
    n0_[i] = upper[i] - lower[i] + 1;
  }
  _TRACE_;
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

void Hierarchy::geomview_grid (FILE *fpr, bool full) throw ()
{

  // Color mapping for levels

  int bcolor[] = {1, 1, 0, 0, 0, 1, 1};
  int rcolor[] = {1, 0, 1, 0, 1, 0, 0};
  int gcolor[] = {1, 0, 0, 1, 1, 1, 1};

  int ng = num_grids();
  int nl = num_levels();

  // Write header

  if (full) {

    // Print first two lines of geomview *.vect file
    fprintf (fpr,"VECT\n");
    fprintf (fpr,"%d %d %d\n",4*ng, 16*ng, nl);

    //
    for (int i=0; i<ng; i++) fprintf (fpr,"8 3 3 2 "); fprintf (fpr,"\n");

    ItHierarchyLevels itl(*this);
    while (Level *level = itl++) {
      fprintf (fpr,"1 0 0 0 ");
      for (int i=1; i<level->num_grids(); i++) {
	fprintf (fpr,"0 0 0 0 "); 
      }
      fprintf (fpr,"\n");
    }
  }

  // For each level, print out all grids in the level

  ItHierarchyLevels itl(*this);

  while (Level * level = itl++) {

    ItLevelGridsAll itg (*level);

    while (Grid * grid = itg++) {

      grid->geomview_grid(fpr,0);

    }

  }

  if (full) {
    for (int i=0; i<nl; i++) {
      int j=i%7;  // 7 is length of [rgb]color[] arrays
      fprintf (fpr,"%d %d %d 0\n",rcolor[j],gcolor[j],bcolor[j]);
    }
  }
}

//----------------------------------------------------------------------

void Hierarchy::write (FILE *fp) throw ()
{
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
  // Resize levels0_[] if needed
  if (unsigned(level + 1) >= levels0_.size()) {
    levels0_.resize (level + 2);
    levels0_[levels0_.size() - 1] = 0;
  }
  if (levels0_.at(level) == 0) {
    if (debug) printf ("DEBUG: creating new Level at %d\n",level);
    levels0_[level] = new Level(level);
  }
  levels0_[level]->insert_grid (grid);
}
