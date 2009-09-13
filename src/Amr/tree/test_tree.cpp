#include <stdio.h>
#include <stdlib.h>
#include <hdf5.h>

#include <string>

#include "cello.h"

#include "array.hpp"
#include "disk.hpp"
#include "tree4.h"
#include "node4.h"

const bool debug = false;

#include "sdsc.h"
//#include "test.h"  // peak

// read in the gimp-generated image data into a bitmap array
bool * create_bitmap(int * n0, int * n1)
{
  bool * bitmap_array = new bool [width*height];

  *n0 = width;
  *n1 = height;

  int pixel[3];
  char * data = header_data;

  for (int iy=0; iy<height; iy++) {
    for (int ix=0; ix<width; ix++) {
      HEADER_PIXEL(data,pixel);
      if (debug) printf ("%d %d  %d %d %d\n",ix,iy,pixel[0],pixel[1],pixel[2]);
      int i = iy + height*ix;
      bitmap_array[i] = ((pixel[0] + pixel[1] + pixel[2]) != 0);
    }
  }
  return bitmap_array;
}


main()
{
  int n0,n1;

  // read in the gimp image into bitmap
  bool * bitmap = create_bitmap(&n0,&n1);

  if (debug) {
    for (int i0=0; i0<n0; i0++) {
      for (int i1=0; i1<n1; i1++) {
	int i=i0 + n0*i1;
	printf ( bitmap[i] ? "*" : ".");
      }
      printf ("\n");
    }
  }

  Tree4 tree;
  tree.refine(bitmap,n0,n1);
  printf ("levels = %d\n",tree.levels());
  //  tree.print();
  printf ("sizeof (Tree4) = %d\n",int(sizeof(Tree4)));

  // Generate an array containing an "image" of the tree

  int nd0, nd1;
  float * image = tree.create_image(&nd0, &nd1,n0,n1,3);
  printf ("Image size: %d x %d\n",nd0,nd1);

  if (debug) {
    for (int i0=0; i0<nd0; i0++) {
      for (int i1=0; i1<nd1; i1++) {
	int i=i0 + nd0*i1;
	printf ("%1d",int(image[i]));
      }
      printf ("\n");
    }
  }
  Hdf5 hdf5;
  hdf5.file_open("tree.hdf5","w");
  ArraySerial tree_array (image,nd0,nd1,1);
  hdf5.dataset_open ("tree_image",tree_array);
  hdf5.write(tree_array);
  hdf5.dataset_close ();
  hdf5.file_close();
	    
  
}
