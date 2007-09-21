
/// Main driver for hypre AMR test solver

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
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include <mpi.h>

#include <map>
#include <string>
#include <vector>


#include "HYPRE_sstruct_ls.h"

#include "hypre-solve.hpp"

#include "scalar.hpp"
#include "performance.hpp"
#include "point.hpp"
#include "sphere.hpp"
#include "faces.hpp"
#include "mpi.hpp"
#include "grid.hpp"
#include "level.hpp"
#include "domain.hpp"
#include "hierarchy.hpp"
#include "parameters.hpp"
#include "problem.hpp"
#include "hypre.hpp"

const int debug    = 0;
const int trace    = 0;
const int geomview = 0;

//======================================================================
// BEGIN MAIN
//======================================================================

int main(int argc, char **argv)
{

  // Determine executable name

  std::string exec_name (argv[0]);
  int size = exec_name.rfind("/");
  exec_name.replace(0,size+1,"");

  // --------------------------------------------------
  // MPI initialization
  // --------------------------------------------------

  if (debug) printf ("DEBUG %s:%d\n",__FILE__,__LINE__);

  Mpi mpi (&argc,&argv);

  if (debug) printf ("DEBUG %s:%d mpi.np() = %d\n",__FILE__,__LINE__,mpi.np());

  // --------------------------------------------------
  // JBPERF initialization
  // --------------------------------------------------


  // ***************
  JBPERF_BEGIN("HS");
  JBPERF_START("0-total");
  // ***************

  if (argc==2) {

    std::string filename (argv[1]);

    // --------------------------------------------------
    // Problem initialization
    // --------------------------------------------------

    // create a new problem and read it in

    Problem problem;

    // ***************
    JBPERF_START("1-problem");
    // ***************

    _BARRIER_;
    _TRACE_;

    problem.read  (filename);

    // ***************
    JBPERF_STOP("1-problem");
    // ***************

    // --------------------------------------------------
    // Initialize the hierarchy
    // --------------------------------------------------

    _TRACE_;
    _BARRIER_;

    Hierarchy & hierarchy = problem.hierarchy();

    // determine interconnections between grids

    // ***************
    JBPERF_START("2-grids");
    // ***************

    _TRACE_;
    _BARRIER_;
    hierarchy.init_grids(problem.domain(), mpi);
    _TRACE_;
    _BARRIER_;

    if (geomview) {
      FILE * fp = fopen ("hierarchy.vect","w");
      hierarchy.geomview_grid (fp);
      fclose (fp);
    }

    // ***************
    JBPERF_STOP("2-grids");
    // ***************

    if (debug) problem.print ();

    // Determine problem size the hard way

    _TRACE_;
    _BARRIER_;

    ItLevelGridsAll itg (problem.hierarchy().level(0));

    int lower[3],upper[3];
    int i;
    for (i=0; i<3; i++) {
      lower[i] = INT_MAX;
      upper[i] = INT_MIN;
    }
    while (Grid *grid = itg++) {
      grid->print();
      for (i=0; i<3; i++) {
	lower[i] = MIN(grid->i_lower(i),lower[i]);
	upper[i] = MAX(grid->i_upper(i),upper[i]);
      }
    }

    // Assume problem is a square box, otherwise exit

    assert (upper[0] - lower[0] + 1 == upper[1] - lower[1] + 1);
    assert (upper[0] - lower[0] + 1 == upper[2] - lower[2] + 1);

    _TRACE_;
    _BARRIER_;
    
    JBPERF_GLOBAL("problem-size",upper[0] - lower[0] + 1);

    // --------------------------------------------------
    // Initialize hypre
    // --------------------------------------------------

    _TRACE_;
    _BARRIER_;
    Hypre hypre;
    _TRACE_;
    _BARRIER_;

  // ***************
    JBPERF_START("4-hypre-init");
  // ***************

    _TRACE_;
    _BARRIER_;
    hypre.init_hierarchy (problem.parameters(),hierarchy,mpi);
    _TRACE_;
    _BARRIER_;

    // Initialize the stencils
    
    _TRACE_;
    _BARRIER_;
    hypre.init_stencil (hierarchy);
    _TRACE_;
    _BARRIER_;

    // Initialize the graph

    _TRACE_;
    _BARRIER_;
    hypre.init_graph (hierarchy);
    _TRACE_;
    _BARRIER_;

    // Initialize the linear system

    _TRACE_;
    _BARRIER_;
    hypre.init_linear (problem.parameters(),
		       hierarchy,
		       problem.points(),
		       problem.spheres());
    _TRACE_;
    _BARRIER_;

  // ***************
    JBPERF_STOP("4-hypre-init");
  // ***************

    // --------------------------------------------------
    // Solve the linear system Ax = b
    // --------------------------------------------------

  // ***************
    JBPERF_START("5-hypre-solve");
  // ***************

    _TRACE_;
    _BARRIER_;
    hypre.solve (problem.parameters(),hierarchy);
    _TRACE_;
    _BARRIER_;

  // ***************
    JBPERF_STOP("5-hypre-solve");
  // ***************

    // --------------------------------------------------
    // Evaluate the solution
    // --------------------------------------------------

    _TRACE_;
    _BARRIER_;
    hypre.evaluate (hierarchy);
    _TRACE_;
    _BARRIER_;

    // --------------------------------------------------
    // jbPerf Finalize
    // --------------------------------------------------

    // ****************
    JBPERF_STOP("0-total");
    JBPERF_END("HS");
    // ***************

    // --------------------------------------------------
    // MPI Finalize
    // --------------------------------------------------

    mpi.barrier();

    _TRACE_;
    _BARRIER_;

    if (mpi.ip() == 0) { printf ("End %s\n",exec_name.c_str()); fflush(stdout); }

    MPI_Finalize ();

    // MPI_Finalize() called by Mpi destructor

  } else {

    printf ("Usage: %s <input-file>\n",argv[0]);

  }

}


