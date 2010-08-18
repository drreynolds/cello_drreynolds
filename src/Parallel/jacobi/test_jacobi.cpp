// $Id: test_Parallel.cpp 1694 2010-08-04 05:51:33Z bordner $
// See LICENSE_CELLO file for license and copyright information

/// @file     test_combine.cpp
/// @author   James Bordner (jobordner@ucsd.edu)
/// @date     Mon Aug  9 10:14:56 PDT 2010
/// @brief    Prototype test program for combining charm++, MPI, and OMP


#include <stdio.h>
#include <stdlib.h>

#include "parallel.def"

#include "patch.hpp"

#include PARALLEL_CHARM_INCLUDE(test_jacobi.decl.h)

#include "jacobi.hpp"

// ------------------------------ MAIN ------------------------------

PARALLEL_MAIN_BEGIN

{

  // Initialize default comm rank and size

  int ip = 0;
  int np = 1;

  // Determine actual comm rank and size

  PARALLEL_INIT;
#ifdef CONFIG_USE_MPI
  MPI_Comm_rank (MPI_COMM_WORLD,&ip);
  MPI_Comm_size (MPI_COMM_WORLD,&np);
#endif

  // Initialize problem parameters

  if (PARALLEL_ARGC != 3) {
    if (ip==0) {
      PARALLEL_PRINTF ("\nUsage: %s N M\n\n",PARALLEL_ARGV[0]);
      PARALLEL_PRINTF ("   N: problem size = N*N*N\n");
      PARALLEL_PRINTF ("   M: block size   = M*M*M\n\n");
    }
    
    PARALLEL_EXIT;
  }    

  PARALLEL_PRINTF ("(ip,np) = (%d,%d)\n",ip,np);

  int n = atoi(PARALLEL_ARGV[1]);  // Problem size = n*n*n
  int m = atoi(PARALLEL_ARGV[2]);  // Block size = m*m*m
  int nb = n/m;           // Number of blocks per dimension
  int n3 = n*n*n;
  int m3 = m*m*m;
  int nb3 = nb*nb*nb;

  PARALLEL_PRINTF ("n=%d\n",n);

  // Create data blocks

  CProxy_Patch patch = CProxy_Patch::ckNew(nb,nb,nb);

  Block * block[nb3];
  for (int iz=0; iz<nb; iz++) {
    double zm = 1.0* iz   /nb;
    double zp = 1.0*(iz+1)/nb;
    for (int iy=0; iy<nb; iy++) {
      double ym = 1.0* iy   /nb;
      double yp = 1.0*(iy+1)/nb;
      for (int ix=0; ix<nb; ix++) {
	double xm = 1.0* ix   /nb;
	double xp = 1.0*(ix+1)/nb;
	int i = ix + nb*(iy + nb*iz);
	block[i] = new Block(m, xm,xp,ym,yp,zm,zp);
      }
    }
  }

  for (int i=0; i<nb; i++) {
    delete block[i];
  }

  PARALLEL_EXIT;

};

PARALLEL_MAIN_END

#include PARALLEL_CHARM_INCLUDE(test_jacobi.def.h)
