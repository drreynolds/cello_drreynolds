// See LICENSE_CELLO file for license and copyright information

/// @file     test_TreeDensity.cpp
/// @author   James Bordner (jobordner@ucsd.edu)
/// @date     2012-01-23
/// @brief    Test program for the Tree class

#include "main.hpp"
#include "test.hpp"
#include "test_mesh.hpp"

#include "mesh.hpp"
#include "disk.hpp"

PARALLEL_MAIN_BEGIN
{

  PARALLEL_INIT;

  unit_init(0,1);

  //--------------------------------------------------
  // Read input HDF5 into density[] array
  //--------------------------------------------------

  
  if (PARALLEL_ARGC != 3) {
    PARALLEL_PRINTF("Usage: %s <file_name> <field_name>\n\n",PARALLEL_ARGV[0]);
    unit_assert(false);
    PARALLEL_EXIT(1);
  }

  const char * file_name  = PARALLEL_ARGV[1];
  const char * field_name = PARALLEL_ARGV[2];

  FileHdf5 file ("./",file_name);

  file.file_open();

  // H5T_IEEE_F32BE
  int nx,ny,nz;
  scalar_type type = scalar_type_unknown;
  file.data_open (field_name,&type,&nx,&ny,&nz);

  float * density = new float [nx*ny*nz];
  file.data_read(density);

  //--------------------------------------------------
  // Refine on density
  //--------------------------------------------------

  int d=3;
  int r=2;
  int min_level = 0;
  int max_level = 10*2/r;
  Tree tree (d,r);
  TRACE1("max_level=%d",max_level);

  // find the min and max density
  float dmin   =1.0e37;
  float dmax = -1.0e37;

  for (int iz=0; iz<nz; iz++) {
    for (int iy=0; iy<ny; iy++) {
      for (int ix=0; ix<nx; ix++) {
	int i = ix + nx*(iy + ny*iz);
	if (density[i] < dmin) dmin = density[i];
	if (density[i] > dmax) dmax = density[i];
      }
    }
  }
  TRACE2 ("min = %f  max = %f",dmin,dmax);

  //--------------------------------------------------
  // create level array from density
  //--------------------------------------------------

  int * levels = new int [nx*ny*nz];

  // linear interpolate log density between minimum level and maximum level

  float lg_dmin = log(dmin);
  float lg_dmax = log(dmax);
  float mult = 1.0*(max_level - min_level) / (lg_dmax - lg_dmin);

  int c=0;
  int imx = -1000, imn=1000;
  for (int iz=0; iz<nz; iz++) {
    for (int iy=0; iy<ny; iy++) {
      for (int ix=0; ix<nx; ix++) {
	int i = ix + nx*(iy + ny*iz);
	float lg_d = log (density[i]);
	levels[i] = min_level + (lg_d - lg_dmin) * mult;
	if (levels[i] < imn) imn = levels[i];
	if (levels[i] > imx) imx = levels[i];
      }
    }
  }
  TRACE3 ("%d %d %d ",imn,imx,c);

  // --------------------------------------------------
  // Create tree from level array
  // --------------------------------------------------

  Timer timer;
  timer.start();
  create_tree_from_levels (&tree, levels,nx,ny,nz);

  TRACE1 ("Tree initial time = %f",timer.value());
  TRACE1 ("Tree initial nodes = %d",tree.num_nodes());


  // --------------------------------------------------
  // Write tree to file
  // --------------------------------------------------

  int mx=1024,my=1024;
  double th= 0.3*M_PI; // spin
  double ph= 0.1*M_PI;
  double ps= -0.06*M_PI;
  int falloff = 3;
  create_image_from_tree (&tree,"density_3d_1-initial.png",
			  mx,my, 0,max_level, th,ph,ps, 0.5, false, falloff);

  create_image_from_tree (&tree,"density_x_1-nitial.png",
			  mx,my, 0,max_level, 0.0,0.0,0.0, 0.5, true,0);

  // --------------------------------------------------
  // Balance tree
  // --------------------------------------------------

  timer.clear();
  timer.start();

  tree.balance();
  
  TRACE1 ("Tree balanced time = %f",timer.value());
  TRACE1 ("Tree balanced nodes = %d",tree.num_nodes());


  // --------------------------------------------------
  // Write tree to file
  // --------------------------------------------------

  create_image_from_tree (&tree,"density_3d_2-balanced.png",
			  mx,my,  0,max_level, th,ph,ps, 0.5, false, falloff);

  create_image_from_tree (&tree,"density_x_2-balanced.png",
			  mx,my, 0,max_level, 0.0,0.0,0.0, 0.5, true,0);

  // --------------------------------------------------
  // Patch coalescing
  // --------------------------------------------------

  timer.clear();
  timer.start();


  tree.coalesce();

  TRACE1 ("Tree coalesced time = %f",timer.value());
  TRACE1 ("Tree coalesced nodes = %d",tree.num_nodes());

  // --------------------------------------------------
  // Write tree to file
  // --------------------------------------------------

  create_image_from_tree (&tree,"density_3d_3-coalesced.png",
			  mx,my,  0,max_level, th,ph,ps, 0.5, false, falloff);

  create_image_from_tree (&tree,"density_x_3-coalesced.png",
			  mx,my, 0,max_level, 0.0,0.0,0.0, 0.5, true,0);

  // --------------------------------------------------

  unit_finalize();

  delete [] levels;
  delete [] density;

  PARALLEL_EXIT;
}

PARALLEL_MAIN_END

