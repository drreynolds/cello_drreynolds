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

const int debug        = 1;
const int debug_hypre  = 1;
const int debug_matrix = 0;

const int trace        = 0;

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
  
  
  if (debug_hypre) 
    printf ("HYPRE_SStructGridCreate (MPI_COMM_WORLD, %d, %d, %p)\n",
	    dim,levels,  &grid_);

  // *******************************************************************
  HYPRE_SStructGridCreate (MPI_COMM_WORLD, 
			   dim, 
			   levels, 
			   &grid_);
  // *******************************************************************

  ItHierarchyLevels itl (hierarchy);

  int part = 0;

  while (Level * level = itl++) {

    ItLevelGridsLocal itg (*level);

    while (Grid * grid = itg++) {

      int lower[3] = {grid->i_lower(0),grid->i_lower(1),grid->i_lower(2)};
      int upper[3] = {grid->i_upper(0),grid->i_upper(1),grid->i_upper(2)};

      // Set extents for boxes that comprise the hypre grid

      if (debug_hypre) printf ("HYPRE_SStructGridSetExtents "
			       "(%p, %d, (%d,%d,%d), (%d,%d,%d)\n",
			       grid_,  part,
			       lower[0],lower[1],lower[2],
			       upper[0],upper[1],upper[2]);

      // *******************************************************************
      HYPRE_SStructGridSetExtents(grid_,
				  part,
				  lower,
				  upper);
      // *******************************************************************
      
    }

    // Create a single cell-centered variable for each grid part (level)

    HYPRE_SStructVariable variable_types[] = { HYPRE_SSTRUCT_VARIABLE_CELL };
    const int numvars = 1;

    if (debug_hypre) printf ("HYPRE_SStructGridSetVariables(%p,%d,%d,(%d))\n",
			     grid_, part, numvars, 
			     variable_types[0]);

    // *******************************************************************
    HYPRE_SStructGridSetVariables(grid_,
				  part,
				  numvars,
				  variable_types);
    // *******************************************************************

    // Set grid part to be periodic, with periodicity determined by the root
    // level size, current level, and refinement factor (assumed to be 2)

    const int r = 2;
    int periodicity[3];

    // Determine periodicity of root Level

    for (int i=0; i<3; i++) {
      // periodicity of root level
      periodicity[i] = hierarchy.level(0).zones(i);
      // adjust for periodicity of given level
      for (int k=0; k < part; k++) {
	periodicity[i] *= r;
      }
    }

    if (debug_hypre) 
      printf ("HYPRE_SStructGridSetPeriodic (%p, %d, (%d,%d,%d))\n",
	      grid_, 0, 
	      periodicity[0], periodicity[1], periodicity[2]);

    // *******************************************************************
    HYPRE_SStructGridSetPeriodic (grid_, 0, periodicity);
    // *******************************************************************

    ++ part;
  }

  // When finished, assemble the hypre grid

  if (debug_hypre) printf ("HYPRE_SStructGridAssemble (%p)\n",
			   grid_);

  // *******************************************************************
  HYPRE_SStructGridAssemble (grid_);
  // *******************************************************************
  
}

//----------------------------------------------------------------------

/// Initialize the discretization stencils.  

/** Creates and initializes a stencil object.  Supports 1, 2, or 3
    dimensional stencils. */

void Hypre::init_stencil (Hierarchy & hierarchy)

{

  int dim = hierarchy.dimension();

  if (debug_hypre) printf ("HYPRE_SStructStencilCreate (%d,%d,%p)\n",
			   dim,dim*2+1,&stencil_);

  // *******************************************************************
  HYPRE_SStructStencilCreate (dim,dim*2+1,&stencil_);
  // *******************************************************************

  int entries[][3] = { {  0, 0, 0 },
		       {  1, 0, 0 },
		       { -1, 0, 0 },
		       {  0, 1, 0 },
		       {  0,-1, 0 },
		       {  0, 0, 1 },
		       {  0, 0,-1 } };

  if (debug_hypre) 
    printf ("HYPRE_SStructStencilSetEntry (%p,%d,(%d,%d,%d),%d)\n",
	    &stencil_, 0, 
	    entries[0][0], entries[0][1],entries[0][2], 0);

  // *******************************************************************
  HYPRE_SStructStencilSetEntry (stencil_, 0, entries[0], 0);
  // *******************************************************************

  if (dim >= 1) {
    if (debug_hypre) 
      printf ("HYPRE_SStructStencilSetEntry (%p,%d,(%d,%d,%d),%d)\n",
	      &stencil_, 1, 
	      entries[1][0], entries[1][1],entries[1][2], 0);

    // *******************************************************************
    HYPRE_SStructStencilSetEntry (stencil_, 1, entries[1], 0);
    // *******************************************************************
  }
  if (dim >= 1) {
    if (debug_hypre) 
      printf ("HYPRE_SStructStencilSetEntry (%p,%d,(%d,%d,%d),%d)\n",
	      &stencil_, 2, 
	      entries[2][0], entries[2][1],entries[2][2], 0);

    // *******************************************************************
    HYPRE_SStructStencilSetEntry (stencil_, 2, entries[2], 0);
    // *******************************************************************
  }
  if (dim >= 2) {
    if (debug_hypre) 
      printf ("HYPRE_SStructStencilSetEntry (%p,%d,(%d,%d,%d),%d)\n",
	      &stencil_, 3, 
	      entries[3][0], entries[3][1],entries[3][2], 0);

    // *******************************************************************
    HYPRE_SStructStencilSetEntry (stencil_, 3, entries[3], 0);
    // *******************************************************************
  }
  if (dim >= 2) {
    if (debug_hypre) 
      printf ("HYPRE_SStructStencilSetEntry (%p,%d,(%d,%d,%d),%d)\n",
	      &stencil_, 4, 
	      entries[4][0], entries[4][1],entries[4][2], 0);

    // *******************************************************************
    HYPRE_SStructStencilSetEntry (stencil_, 4, entries[4], 0);
    // *******************************************************************
  }
  if (dim >= 3) {
    if (debug_hypre) 
      printf ("HYPRE_SStructStencilSetEntry (%p,%d,(%d,%d,%d),%d)\n",
	      &stencil_, 5, 
	      entries[5][0], entries[5][1],entries[5][2], 0);

    // *******************************************************************
    HYPRE_SStructStencilSetEntry (stencil_, 5, entries[5], 0);
    // *******************************************************************
  }
  if (dim >= 3) {
    if (debug_hypre) 
      printf ("HYPRE_SStructStencilSetEntry (%p,%d,(%d,%d,%d),%d)\n",
	      &stencil_, 6, 
	      entries[6][0], entries[6][1],entries[6][2], 0);

    // *******************************************************************
    HYPRE_SStructStencilSetEntry (stencil_, 6, entries[6], 0);
    // *******************************************************************
  }

}

//----------------------------------------------------------------------

/// Initialize the graph.

/** Creates a graph containing the matrix non-zero structure.  Graph
    edges include both those for non-zeros from the stencil within
    each part (level), and non-zeros for graph entries connecting
    linked parts.

    Setting up the matrix nonzero structure is done with the following
    steps:

     - Define the stencil for all interior grid elements

     - Handle coarse unknowns adjacent to fine unknowns in child
     - Handle fine unknowns adjacent to coarse unknowns in parent

     - Handle coarse unknowns adjacent to fine unknowns in neighbor's child
     - Handle fine unknowns adjacent to coarse unknowns in parent's neighbor
     
    The matrix nonzero structure is generally nonsymmetric.

*/

void Hypre::init_graph (Hierarchy & hierarchy)

{
  // Create the hypre graph object

  if (debug_hypre) printf ("HYPRE_SStructGraphCreate (MPI_COMM_WORLD,%p,%p)\n",
			   &grid_, &graph_);

  // *******************************************************************
  HYPRE_SStructGraphCreate (MPI_COMM_WORLD, 
			    grid_, 
			    &graph_);
  // *******************************************************************

  ItHierarchyLevels itl (hierarchy);

  int part = 0;

  while (Level * level = itl++) {

    // Define stencil connections within each part

    if (debug_hypre) printf ("HYPRE_SStructGraphSetStencil (%p,%d,%d,%p)\n",
			     graph_, part, 0, &stencil_);

    // *******************************************************************
    HYPRE_SStructGraphSetStencil (graph_,
				  part,
				  0,
				  stencil_);
    // *******************************************************************

    ItLevelGridsLocal itg (*level);

    while (Grid * grid = itg++) {

      // Define connections for unknowns adjacent to fine unknowns in children

      init_graph_children_(*grid);

      // Define connections for unknowns adjacent to coarse unknowns in parent

      init_graph_parent_(hierarchy,*grid);

      // Define connections for unknowns adjacent to fine unknowns in neighbor's child

      init_graph_neighbors_children_(*grid);

      // Define connections for unknowns adjacent to coarse unknowns
      // in parent's neighbor

      init_graph_parents_neighbor_(*grid);

    }
    ++ part;
  }

  // Assemble the graph

  if (debug_hypre) printf ("HYPRE_SStructGraphAssemble (%p);\n",
			   &graph_);

  // *******************************************************************
  HYPRE_SStructGraphAssemble (graph_);
  // *******************************************************************

}

//----------------------------------------------------------------------

/// Initialize the matrix A

/** Creates a matrix with a given non-zero structure, and sets nonzero
    values.

    Setting up the matrix elements is done with the following
    steps:

     - Set stencil values
     - Clear stencil values in overlapped grids

     - Set coarse unknowns adjacent to fine unknowns in child
     - Set fine unknowns adjacent to coarse unknowns in parent

     - Set coarse unknowns adjacent to fine unknowns in neighbor's child
     - Set fine unknowns adjacent to coarse unknowns in parent's neighbor
     
    The matrix is generally nonsymmetric.

*/

void Hypre::init_matrix (Hierarchy & hierarchy)

{
  // Create the hypre matrix object

  if (debug_hypre) printf ("HYPRE_SStructMatrixCreate (MPI_COMM_WORLD,%p,%p)\n",
			   &graph_, &matrix_);

  // *******************************************************************
  HYPRE_SStructMatrixCreate (MPI_COMM_WORLD, 
			     graph_, 
			     &matrix_);
  // *******************************************************************

  // Initialize the hypre matrix object

  if (debug_hypre) printf ("HYPRE_SStructMatrixInitialize (%p)\n",
			   &matrix_);

  // *******************************************************************
  HYPRE_SStructMatrixInitialize (matrix_);
  // *******************************************************************

  ItHierarchyLevels itl (hierarchy);

  int part = 0;

  while (Level * level = itl++) {

    // Define stencil connections within each part

    if (debug_hypre) printf ("HYPRE_SStructMatrixSetStencil (%p,%d,%d,%p)\n",
			     matrix_, part, 0, &stencil_);

    ItLevelGridsLocal itg (*level);

    while (Grid * grid = itg++) {

      // Set matrix stencil values for each grid

      init_matrix_stencil_(*grid);

      // Clear matrix stencil values under child grids

      init_matrix_clear_(*grid);

      // Set matrix values for unknowns adjacent to fine unknowns in children

      init_matrix_children_(*grid);

      // Set matrix values for unknowns adjacent to coarse unknowns in parent

      init_matrix_parent_(hierarchy,*grid);

      // Set matrix values for unknowns adjacent to fine unknowns in neighbor's child

      init_matrix_neighbors_children_(*grid);

      // Set matrix values for unknowns adjacent to coarse unknowns in
      // parent's neighbor

      init_matrix_parents_neighbor_(*grid);

    }
    ++ part;
  }

  // Assemble the matrix

  if (debug_hypre) printf ("HYPRE_SStructMatrixAssemble (%p);\n",
			   &matrix_);

  // *******************************************************************
  HYPRE_SStructMatrixAssemble (matrix_);
  // *******************************************************************

  // Write the matrix to a file for debugging

  if (debug_matrix) {

    if (debug_hypre) printf ("HYPRE_SStructMatrixAssemble (%p);\n",
			     &matrix_);

    // *******************************************************************
    HYPRE_SStructMatrixPrint ("matrix",matrix_,1);
    // *******************************************************************

  }

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

/// Handle coarse unknowns adjacent to fine unknowns in child

void Hypre::init_graph_children_ (Grid & grid)

{
  ItGridChildren itchild (grid);
  while (Grid * child = itchild++) {
    _TRACE_;
    // *******************************************************************
    //	HYPRE_SStructGraphAddEntries (grid->hypre_graph(),
    //				      grid->level(),
    //				      grid->INDEX
    //				      neighbor.level(),
    //				      neighbor.INDEX,
    //				      variable);
    // *******************************************************************
  }
  printf ("Hypre::init_graph_children_ () not implemented\n");
}

//------------------------------------------------------------------------

/// Handle fine unknowns adjacent to coarse unknowns in parent

void Hypre::init_graph_parent_ (Hierarchy & hierarchy, Grid & grid)

{
  printf ("Hypre::init_graph_parent_ () not implemented\n");
  Grid * parent = hierarchy.parent(grid);
  _TRACE_;
  // *******************************************************************
  //	HYPRE_SStructGraphAddEntries (grid->hypre_graph(),
  //				      grid->level(),
  //				      grid->INDEX
  //				      neighbor.level(),
  //				      neighbor.INDEX,
  //				      variable);
  // *******************************************************************
}

//------------------------------------------------------------------------

/// Handle coarse unknowns adjacent to fine unknowns in neighbor's child

void Hypre::init_graph_neighbors_children_ (Grid & grid)

{
  printf ("Hypre::init_graph_neighbors_children_ () not implemented\n");
  // *******************************************************************
  //	HYPRE_SStructGraphAddEntries (grid->hypre_graph(),
  //				      grid->level(),
  //				      grid->INDEX
  //				      neighbor.level(),
  //				      neighbor.INDEX,
  //				      variable);
  // *******************************************************************
}

//------------------------------------------------------------------------

/// Handle fine unknowns adjacent to coarse unknowns in parent's neighbor

void Hypre::init_graph_parents_neighbor_ (Grid & grid)

{
  printf ("Hypre::init_graph_parents_neighbor_ () not implemented\n");
  // *******************************************************************
  //	HYPRE_SStructGraphAddEntries (grid->hypre_graph(),
  //				      grid->level(),
  //				      grid->INDEX
  //				      neighbor.level(),
  //				      neighbor.INDEX,
  //				      variable);
  // *******************************************************************
}

//------------------------------------------------------------------------

/// Set matrix stencil values for the grid

void Hypre::init_matrix_stencil_ (Grid & grid)

{
  _TRACE_;

  int part         = grid.level();
  int lower[3]     = { grid.i_lower(0), grid.i_lower(1), grid.i_lower(2) };
  int upper[3]     = { grid.i_upper(0), grid.i_upper(1), grid.i_upper(2) };
  int var          = 0;
  int count        = grid.num_unknowns();
  int entries[7]   = { 0,1,2,3,4,5,6 };

  double * values  = new double [7*count];

  for (int i=0; i<7*count; i++) {
    values[i]  = (i%7==0) ? 6 : -1;
  }

  if (debug) printf ("count = %d\n",count);

  if (debug_hypre) 
    printf("HYPRE_SStructMatrixSetBoxValues (%p,%d,(%d,%d,%d),(%d,%d,%d),"
	   "%d,%d,(%d,...%d),(%g,%g,%g,...,%g,%g,%g))\n",
	   &matrix_,
	   part,
	   lower[0],lower[1],lower[2],
	   upper[0],upper[1],upper[2],
	   var,
	   7,
	   entries[0],entries[6],
	   values[0],values[1],values[2],
	   values[7*count-3],values[7*count-2],values[7*count-1]);
  // *******************************************************************
  HYPRE_SStructMatrixSetBoxValues (matrix_,
				   part,
				   lower,
				   upper,
				   var,
				   7,
				   entries,
				   values);
  // *******************************************************************

 delete [] values;

}

//------------------------------------------------------------------------

/// Clear matrix stencil values under child grids

void Hypre::init_matrix_clear_ (Grid & grid)

{
  ItGridChildren itchildren (grid);
  
  while (Grid * child = itchildren++) {
    _TRACE_;
  }

  // *******************************************************************
  //	HYPRE_SStructMatrixAddEntries (grid->hypre_matrix(),
  //				      grid->level(),
  //				      grid->INDEX
  //				      neighbor.level(),
  //				      neighbor.INDEX,
  //				      variable);
  // *******************************************************************

  printf ("Hypre::init_matrix_clear_ () not implemented\n");
}

//------------------------------------------------------------------------

/// Define connections for unknowns adjacent to fine unknowns in children

void Hypre::init_matrix_children_ (Grid & grid)

{
  ItGridChildren itchild (grid);
  while (Grid * child = itchild++) {

    _TRACE_;

    // *******************************************************************
    //	HYPRE_SStructMatrixAddEntries (grid->hypre_matrix(),
    //				      grid->level(),
    //				      grid->INDEX
    //				      neighbor.level(),
    //				      neighbor.INDEX,
    //				      variable);
    // *******************************************************************

  }
  printf ("Hypre::init_matrix_children_ () not implemented\n");
}

//------------------------------------------------------------------------

/// Set matrix values for unknowns adjacent to coarse unknowns in parent

void Hypre::init_matrix_parent_ (Hierarchy & hierarchy, Grid & grid)

{
  printf ("Hypre::init_matrix_parent_ () not implemented\n");
  Grid * parent = hierarchy.parent(grid);
  _TRACE_;
  // *******************************************************************
  //	HYPRE_SStructMatrixAddEntries (grid->hypre_matrix(),
  //				      grid->level(),
  //				      grid->INDEX
  //				      neighbor.level(),
  //				      neighbor.INDEX,
  //				      variable);
  // *******************************************************************
}

//------------------------------------------------------------------------

/// Set matrix values for unknowns adjacent to fine unknowns in neighbor's child

void Hypre::init_matrix_neighbors_children_ (Grid & grid)

{
  printf ("Hypre::init_matrix_neighbors_children_ () not implemented\n");
  // *******************************************************************
  //	HYPRE_SStructMatrixAddEntries (grid->hypre_matrix(),
  //				      grid->level(),
  //				      grid->INDEX
  //				      neighbor.level(),
  //				      neighbor.INDEX,
  //				      variable);
  // *******************************************************************
}

//------------------------------------------------------------------------

/// Set matrix values for unknowns adjacent to coarse unknowns in
/// parent's neighbor

void Hypre::init_matrix_parents_neighbor_ (Grid & grid)

{
  printf ("Hypre::init_matrix_parents_neighbor_ () not implemented\n");
  // *******************************************************************
  //	HYPRE_SStructMatrixAddEntries (grid->hypre_matrix(),
  //				      grid->level(),
  //				      grid->INDEX
  //				      neighbor.level(),
  //				      neighbor.INDEX,
  //				      variable);
  // *******************************************************************
}

