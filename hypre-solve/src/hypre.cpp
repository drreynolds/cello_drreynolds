/// Interface routines to HYPRE

/**
 * 
 * 
 * 
 * @file
 * @author James Bordner <jobordner@ucsd.edu>
 *
 */

#include <mpi.h>
#include <assert.h>
#include <string>
#include <vector>
#include <map>

#include "HYPRE_sstruct_ls.h"

#include "hypre-solve.hpp"

#include "mpi.hpp"
#include "scalar.hpp"
#include "point.hpp"
#include "faces.hpp"
#include "grid.hpp"
#include "level.hpp"
#include "hierarchy.hpp"
#include "sphere.hpp"
#include "domain.hpp"
#include "problem.hpp"
#include "hypre.hpp"

const int debug_hypre           = 0;

const int debug_hypre_hierarchy = 0;
const int debug_hypre_graph     = 0;

const int trace                 = 0;

//======================================================================
// PUBLIC MEMBER FUNCTIONS
//======================================================================

/// Hypre constructor

Hypre::Hypre ()

{
  
}

//----------------------------------------------------------------------

/// Initialize the Grid Hierarchy

/** Creates a hypre grid, with one part per level and one box per Grid
    patch object, for an AMR problem.  Sets grid box extents, grid
    part variables, and periodicity of the root-level grid part. */

void Hypre::init_hierarchy (Hierarchy & hierarchy, 
			    Mpi       & mpi)

{

  Grid::set_mpi (mpi);

  int dim    = hierarchy.dimension();
  int levels = hierarchy.num_levels();

  // Create the hypre grid


  printf ("HYPRE_SStructGridCreate (MPI_COMM_WORLD, "
	  "%d, %d, %p)\n",dim,levels,  &hierarchy.hypre_grid());

  HYPRE_SStructGridCreate (MPI_COMM_WORLD, 
			   dim, 
			   levels, 
			   &hierarchy.hypre_grid());

  ItHierarchyLevels itl (hierarchy);

  int part = 0;

  while (Level * L = itl++) {

    ItLevelLocalGrids itg (*L);

    while (Grid * grid = itg++) {

      int lower[3] = {grid->i_lower(0),grid->i_lower(1),grid->i_lower(2)};
      int upper[3] = {grid->i_upper(0),grid->i_upper(1),grid->i_upper(2)};

      // Set extents for boxes that comprise the hypre grid

      printf ("HYPRE_SStructGridSetExtents(%p, %d, (%d,%d,%d), (%d,%d,%d)\n",
	      hierarchy.hypre_grid(),
	      part,
	      lower[0],lower[1],lower[2],
	      upper[0],upper[1],upper[2]);

      HYPRE_SStructGridSetExtents(hierarchy.hypre_grid(),
				  part,
				  lower,
				  upper);
      
    }

    HYPRE_SStructVariable variable_types[] = { HYPRE_SSTRUCT_VARIABLE_CELL };
    const int numvars = 1;

    // Create a single cell-centered variable for each grid part (level)

    printf ("HYPRE_SStructGridSetVariables(%p,%d,%d,(%d))\n",
	    hierarchy.hypre_grid(), part, numvars, variable_types[0]);

    HYPRE_SStructGridSetVariables(hierarchy.hypre_grid(),
				  part,
				  numvars,
				  variable_types);

    ++ part;
  }

  // Set root-level grid part to be periodic 

  int periodicity[3] = {1,1,1};

  printf ("HYPRE_SStructGridSetPeriodic (%p, %d, (%d,%d,%d))\n",
	  hierarchy.hypre_grid(), 0, 
	  periodicity[0], 
	  periodicity[1], 
	  periodicity[2]);

  HYPRE_SStructGridSetPeriodic (hierarchy.hypre_grid(), 0, periodicity);

  // When finished, assemble the hypre grid

  printf ("HYPRE_SStructGridAssemble (%p)\n",hierarchy.hypre_grid());
  HYPRE_SStructGridAssemble (hierarchy.hypre_grid());
  
}

//----------------------------------------------------------------------

/// Initialize the discretization stencils.  

/** Creates and initializes a stencil object.  Supports 1, 2, or 3
    dimensional stencils. */

void Hypre::init_stencil (Hierarchy & hierarchy)

{

  int dim = hierarchy.dimension();

  printf ("HYPRE_SStructStencilCreate (%d,%d,%p)\n",dim,dim*2+1,&stencil_);
  HYPRE_SStructStencilCreate (dim,dim*2+1,&stencil_);

  int entries[][3] = { {  0, 0, 0 },
		       {  1, 0, 0 },
		       { -1, 0, 0 },
		       {  0, 1, 0 },
		       {  0,-1, 0 },
		       {  0, 0, 1 },
		       {  0, 0,-1 } };

  printf ("HYPRE_SStructStencilSetEntry (%p,%d,(%d,%d,%d),%d)\n",
	  &stencil_, 0, entries[0][0], entries[0][1],entries[0][2], 0);
  HYPRE_SStructStencilSetEntry (stencil_, 0, entries[0], 0);

  if (dim >= 1) {
    printf ("HYPRE_SStructStencilSetEntry (%p,%d,(%d,%d,%d),%d)\n",
	    &stencil_, 1, entries[1][0], entries[1][1],entries[1][2], 0);
    HYPRE_SStructStencilSetEntry (stencil_, 1, entries[1], 0);
  }
  if (dim >= 1) {
    printf ("HYPRE_SStructStencilSetEntry (%p,%d,(%d,%d,%d),%d)\n",
	    &stencil_, 2, entries[2][0], entries[2][1],entries[2][2], 0);
    HYPRE_SStructStencilSetEntry (stencil_, 2, entries[2], 0);
  }
  if (dim >= 2) {
    printf ("HYPRE_SStructStencilSetEntry (%p,%d,(%d,%d,%d),%d)\n",
	    &stencil_, 3, entries[3][0], entries[3][1],entries[3][2], 0);
    HYPRE_SStructStencilSetEntry (stencil_, 3, entries[3], 0);
  }
  if (dim >= 2) {
    printf ("HYPRE_SStructStencilSetEntry (%p,%d,(%d,%d,%d),%d)\n",
	    &stencil_, 4, entries[4][0], entries[4][1],entries[4][2], 0);
    HYPRE_SStructStencilSetEntry (stencil_, 4, entries[4], 0);
  }
  if (dim >= 3) {
    printf ("HYPRE_SStructStencilSetEntry (%p,%d,(%d,%d,%d),%d)\n",
	    &stencil_, 5, entries[5][0], entries[5][1],entries[5][2], 0);
    HYPRE_SStructStencilSetEntry (stencil_, 5, entries[5], 0);
  }
  if (dim >= 3) {
    printf ("HYPRE_SStructStencilSetEntry (%p,%d,(%d,%d,%d),%d)\n",
	    &stencil_, 6, entries[6][0], entries[6][1],entries[6][2], 0);
    HYPRE_SStructStencilSetEntry (stencil_, 6, entries[6], 0);
  }

}

//----------------------------------------------------------------------

/// Initialize the graph.

/** Creates a graph containing the matrix non-zero structure.  Graph
    edges include both those for non-zeros from the stencil within
    each part (level), and non-zeros for graph entries connecting
    linked parts.

    Setting up the matrix elements is done with the following
    steps:

     - Define the stencil for all interior grid elements

     - Handle matrix elements defining the boundary conditions

     - Zero-out matrix elements covered by a refined grid

     - Handle matrix elements connecting neighboring grids

     - Handle coarse unknowns adjacent to fine unknowns in child
     - Handle fine unknowns adjacent to coarse unknowns in parent

     - Handle coarse unknowns adjacent to fine unknowns in neighbor's child
     - Handle fine unknowns adjacent to coarse unknowns in parent's neighbor
     
     - Handle any remaining connections

    The matrix generated is generally nonsymmetric.

*/

void Hypre::init_graph (Hierarchy & hierarchy)

{
  // Create the hypre graph object

  HYPRE_SStructGraphCreate (MPI_COMM_WORLD, 
			    hierarchy.hypre_grid(), 
			    &hierarchy.hypre_graph());

  ItHierarchyLevels itl (hierarchy);

  int part = 0;

  while (Level * L = itl++) {

    // Set the stencil for each part

    _TRACE_;
    printf ("HYPRE_SStructGraphSetStencil (%p,%d,%d,%p)\n",
	    hierarchy.hypre_graph(),
	    part,
	    0,
	    &stencil_);
    HYPRE_SStructGraphSetStencil (hierarchy.hypre_graph(),
				  part,
				  0,
				  stencil_);
    _TRACE_;

    ItLevelLocalGrids itg (*L);

    while (Grid * grid = itg++) {

      // Handle matrix elements defining the boundary conditions

      init_graph_boundary_(*grid);

      // Zero-out matrix elements covered by a refined grid patch

      init_graph_zero_covered_(*grid);

      // Handle matrix elements connecting neighboring grid patches

      init_graph_neighbors_(*grid);

      // Handle coarse unknowns adjacent to fine unknowns in child

      init_graph_children_(*grid);

      // Handle fine unknowns adjacent to coarse unknowns in parent

      init_graph_parent_(*grid);

      // Handle coarse unknowns adjacent to fine unknowns in neighbor's child

      init_graph_neighbors_children_(*grid);

      // Handle fine unknowns adjacent to coarse unknowns in parent's neighbor

      init_graph_parents_neighbor_(*grid);

      // Handle any remaining connections 

      init_graph_check_(*grid);

    }
    ++ part;
  }

  // Assemble the graph

  HYPRE_SStructGraphAssemble (hierarchy.hypre_graph());

}

//----------------------------------------------------------------------

/// Initialize the matrix A

void Hypre::init_matrix (Hierarchy & hierarchy)

{
  printf ("Hypre::init_matrix() is not implemented yet\n"); 
}

//----------------------------------------------------------------------

/// Initialize the right-hand-side vector b

void Hypre::init_rhs (Hierarchy & hierarchy)

{
  printf ("Hypre::init_rhs() is not implemented yet\n"); 
}

//----------------------------------------------------------------------

/// Initialize the linear solver

void Hypre::init_solver (Hierarchy & hierarchy)

{
  printf ("Hypre::init_solver() is not implemented yet\n"); 
}

//----------------------------------------------------------------------

/// Solve the linear system Ax = b

void Hypre::solve (Hierarchy & hierarchy)

{
  printf ("Hypre::solve() is not implemented yet\n"); 
}

//----------------------------------------------------------------------

/// Evaluate the success of the solve

void Hypre::evaluate (Hierarchy & hierarchy)

{
  printf ("Hypre::evaluate() is not implemented yet\n"); 
}

//======================================================================
// PRIVATE MEMBER FUNCTIONS
//======================================================================

/// Handle matrix elements defining the boundary conditions

/** Boundary conditions are periodic, and defined in init_hierarchy()
    using HYPRE_SStructGridSetPeriodic(). */

void Hypre::init_graph_boundary_ (Grid & grid)

{
  /* Do nothing */ ;
}

//------------------------------------------------------------------------

/// Zero-out matrix elements covered by a refined grid

void Hypre::init_graph_zero_covered_ (Grid & grid)

{
  printf ("Hypre::init_graph_zero_covered_ () not implemented\n");
}

//------------------------------------------------------------------------

/// Handle matrix elements connecting neighboring grids

void Hypre::init_graph_neighbors_ (Grid & grid)

{
  printf ("Hypre::init_graph_neighbors_ () not implemented\n");
  //	HYPRE_SStructGraphAddEntries (grid->hypre_graph(),
  //				      grid->level(),
  //				      grid->INDEX
  //				      neighbor.level(),
  //				      neighbor.INDEX,
  //				      variable);
}

//------------------------------------------------------------------------

/// Handle coarse unknowns adjacent to fine unknowns in child

void Hypre::init_graph_children_ (Grid & grid)

{
  printf ("Hypre::init_graph_children_ () not implemented\n");
  //	HYPRE_SStructGraphAddEntries (grid->hypre_graph(),
  //				      grid->level(),
  //				      grid->INDEX
  //				      neighbor.level(),
  //				      neighbor.INDEX,
  //				      variable);
}

//------------------------------------------------------------------------

/// Handle fine unknowns adjacent to coarse unknowns in parent

void Hypre::init_graph_parent_ (Grid & grid)

{
  printf ("Hypre::init_graph_parent_ () not implemented\n");
  //	HYPRE_SStructGraphAddEntries (grid->hypre_graph(),
  //				      grid->level(),
  //				      grid->INDEX
  //				      neighbor.level(),
  //				      neighbor.INDEX,
  //				      variable);
}

//------------------------------------------------------------------------

/// Handle coarse unknowns adjacent to fine unknowns in neighbor's child

void Hypre::init_graph_neighbors_children_ (Grid & grid)

{
  printf ("Hypre::init_graph_neighbors_children_ () not implemented\n");
  //	HYPRE_SStructGraphAddEntries (grid->hypre_graph(),
  //				      grid->level(),
  //				      grid->INDEX
  //				      neighbor.level(),
  //				      neighbor.INDEX,
  //				      variable);
}

//------------------------------------------------------------------------

/// Handle fine unknowns adjacent to coarse unknowns in parent's neighbor

void Hypre::init_graph_parents_neighbor_ (Grid & grid)

{
  printf ("Hypre::init_graph_parents_neighbor_ () not implemented\n");
  //	HYPRE_SStructGraphAddEntries (grid->hypre_graph(),
  //				      grid->level(),
  //				      grid->INDEX
  //				      neighbor.level(),
  //				      neighbor.INDEX,
  //				      variable);
}

//------------------------------------------------------------------------

/// Check for any remaining connections

void Hypre::init_graph_check_ (Grid & grid)

{
}
