// $Id$
// See LICENSE_CELLO file for license and copyright information

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string>

#include "pngwriter.h"

#include "test.hpp"
#include "parallel.hpp"
#include "monitor.hpp"

int main(int argc, char ** argv)
{
  unit_class ("Monitor");

  Parallel * parallel = Parallel::instance();
  parallel->initialize(&argc,&argv);

  Monitor  * monitor  = Monitor::instance();

  int n = 128;

  // Allocate array

  printf ("pngwriter version = %g\n",pngwriter::version());

  float * array = new float [n*n*n];
  for (int i=0; i<n*n*n; i++) array[i]=0;

  // Set values to 1 for radius between n/8 and n/4 (note n/2 is boundary)
  for (int iz=0; iz<n; iz++) {
    double z = (iz - 0.5*n) / n;
    for (int iy=0; iy<n; iy++) {
      double y = (iy - 0.5*n) / n;
      for (int ix=0; ix<n; ix++) {
	int i = ix + n*(iy + n*iz);
	double x = (ix - 0.5*n) / n;
	//	array[i] = y;
	double r = sqrt(x*x + y*y + z*z);
	if (0.25 < r && r < 0.5) array[i] = 1;
      }
    }
  }

  unit_func("Monitor");
  unit_assert(true);

  unit_func("image");
  double map1[] = {0,0,0, 1,1,1};
  
  monitor->image("test1.png",array,n,n,n,0,0,0,n,n,n,0,reduce_sum,0,1,map1,2);
  unit_assert(true);

  double map2[] = {0,0,0, 1,0,0, 1,1,1};
  monitor->image("test2.png",array,n,n,n,0,0,0,n,n,n,0,reduce_sum,0,1,map2,3);
  unit_assert(true);

  double map3[] = {0,0,0, 1,0,0, 0,1,0, 0,0,1, 1,1,1};
  monitor->image("test3.png",array,n,n,n,0,0,0,n,n,n,0,reduce_sum,0,1,map3,5);
  unit_assert(true);

  double map4[] = {1,0,0, 1,1,0, 0,1,0, 0,1,1, 0,0,1, 1,0,1};
  monitor->image("test4.png",array,n,n,n,0,0,0,n,n,n,0,reduce_sum,0,1,map4,6);
  unit_assert(true);

  parallel->finalize();

}
