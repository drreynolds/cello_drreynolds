// See LICENSE_CELLO file for license and copyright information

/// @file     mesh_test.cpp
/// @author   James Bordner (jobordner@ucsd.edu)
/// @date     2012-01-12
/// @brief    Functions for Mesh test programs

#include "mesh.hpp"

//----------------------------------------------------------------------

Tree * test_tree_22()
{

  // +-------+---+-+-+
  // |       |   + + +
  // |       +-+-+-+-+
  // |       + + +   |
  // +-+-+---+-+-+---+
  // + + +   |   |   |
  // +-+-+-+-+---+---|
  // |   + + +   |   |
  // +---+-+-+-------+
  //
  // Refined nodes: root, 0 1 3 01 02 30 33

  int d = 2;
  int r = 2;

  Tree * tree = new Tree (d,r);
  int max_levels = 4;

  NodeTrace * node_trace = new NodeTrace (tree->root_node(),max_levels);
  
  // root
  tree->refine_node (node_trace);
  // 0
  node_trace->push(0);
  tree->refine_node (node_trace);
  // 01
  node_trace->push(1);
  tree->refine_node (node_trace);
  // 02
  node_trace->pop();
  node_trace->push(2);
  tree->refine_node (node_trace);
  // 1
  node_trace->pop();
  node_trace->pop();
  node_trace->push(1);
  tree->refine_node (node_trace);
  // 3
  node_trace->pop();
  node_trace->push(3);
  tree->refine_node (node_trace);
  // 30
  node_trace->push(0);
  tree->refine_node (node_trace);
  // 33
  node_trace->pop();
  node_trace->push(3);
  tree->refine_node (node_trace);

  delete node_trace;

  return tree;
}

Tree * test_tree_32()
{

  // 3D version of the following;
  // +-------+---+-+-+
  // |   |   |   |   |
  // +---+-+-+-+-+---+
  // |   +-+-+-+-+   |
  // +---+-+-+-+-+---+
  // |   +-+-+-+-+   |
  // +---+-+-+-+-+---+
  // |   |   |   |   |
  // +---+-+-+-------+
  //
  // Refined nodes: root, 0 07 1 16 2 25 3 34 4 43 5 52 6 61 7 70

  int d = 3;
  int r = 2;

  Tree * tree = new Tree (d,r);
  int max_levels = 4;

  NodeTrace * node_trace = new NodeTrace (tree->root_node(),max_levels);

  // root
  tree->refine_node (node_trace);

  // 0
  node_trace->push(0);
  tree->refine_node (node_trace);
  // 07
  node_trace->push(7);
  tree->refine_node (node_trace);
  // 1
  node_trace->pop();
  node_trace->pop();
  node_trace->push(1);
  tree->refine_node (node_trace);
  // 16
  node_trace->push(6);
  tree->refine_node (node_trace);
  // 2
  node_trace->pop();
  node_trace->pop();
  node_trace->push(2);
  tree->refine_node (node_trace);
  // 25
  node_trace->push(5);
  tree->refine_node (node_trace);
  // 3
  node_trace->pop();
  node_trace->pop();
  node_trace->push(3);
  tree->refine_node (node_trace);
  // 34
  node_trace->push(4);
  tree->refine_node (node_trace);
  // 4
  node_trace->pop();
  node_trace->pop();
  node_trace->push(4);
  tree->refine_node (node_trace);
  // 43
  node_trace->push(3);
  tree->refine_node (node_trace);
  // 5
  node_trace->pop();
  node_trace->pop();
  node_trace->push(5);
  tree->refine_node (node_trace);
  // 52
  node_trace->push(2);
  tree->refine_node (node_trace);
  // 6
  node_trace->pop();
  node_trace->pop();
  node_trace->push(6);
  tree->refine_node (node_trace);
  // 61
  node_trace->push(1);
  tree->refine_node (node_trace);
  // 7
  node_trace->pop();
  node_trace->pop();
  node_trace->push(7);
  tree->refine_node (node_trace);
  // 70
  node_trace->push(0);
  tree->refine_node (node_trace);

  delete node_trace;

  return tree;
}

//----------------------------------------------------------------------

int * create_levels_from_image (const char * pngfile, 
			  int * nx, int * ny, int max_levels)
// return an array of integer values between [0 and max_levels)
// corresponding to the grayscale values of the input png file,
// indexed by ix + nx*iy
{

  pngwriter png;

  png.readfromfile(pngfile);

  (*nx) = png.getwidth();
  (*ny) = png.getheight();

  int size = (*nx)*(*ny);

  int * level_array = new int [size];

  for (int i=0; i<size; i++) level_array[i] = 0;


  int max = 0;
  for (int iy=0; iy<*ny; iy++) {
    for (int ix=0; ix<*nx; ix++) {
      int pixel = png.read(ix+1,iy+1);
      int i = ix + (*nx)*iy;
      level_array[i] = max_levels * 1.0*pixel / 256;
      if (level_array[i] > max) max = level_array[i];
    }
  }

  png.close();

  return level_array;
}

//----------------------------------------------------------------------

int * create_image_from_tree (Tree * tree, const char * filename, 
			      int nx, int ny, int max_levels)
{

  pngwriter png (nx+1,ny+1,0,filename);

  int i;

  ItNode it_node (tree, max_levels);
  int xmn=1000,xmx=-1000;
  int ymn=1000,ymx=-1000;
  while (Node * node = ++it_node) {
    const NodeTrace * node_trace  = it_node.node_trace();
    double xmin = 0.0; double xmax = 1.0;
    double ymin = 0.0; double ymax = 1.0;
    double h = 0.5;
    int level = node_trace->level();
    // determine node boundaries scaled by [0:1,0:1]
    for (i=1; i<=level; i++) {
      int index_curr = node_trace->index_level(i);
      switch (index_curr) {
      case 0:  xmax -= h; ymax -= h; break;
      case 1:  xmin += h; ymax -= h; break;
      case 2:  xmax -= h; ymin += h; break;
      case 3:  xmin += h; ymin += h; break;
      default: printf ("ERROR index_curr = %d\n",index_curr); exit(1);
      }
      h*=0.5;
    }
    int ixmin=xmin*nx; int ixmax = xmax*nx;
    int iymin=ymin*ny; int iymax = ymax*ny;

    int ix,iy;
    for (ix=ixmin; ix<=ixmax; ix++) {
      iy = iymin;      
      if (ix+1<xmn) xmn=ix+1;
      if (iy+1<ymn) ymn=iy+1;
      if (ix+1>xmx) xmx=ix+1;
      if (iy+1>ymx) ymx=iy+1;
      png.plot(ix+1, iy+1, 1.0, 1.0, 1.0);
      iy = iymax;      
      if (ix+1<xmn) xmn=ix+1;
      if (iy+1<ymn) ymn=iy+1;
      if (ix+1>xmx) xmx=ix+1;
      if (iy+1>ymx) ymx=iy+1;
      png.plot(ix+1, iy+1, 1.0, 1.0, 1.0);
    }
    for (iy=iymin; iy<=iymax; iy++) {
      ix = ixmin;      
      if (ix+1<xmn) xmn=ix+1;
      if (iy+1<ymn) ymn=iy+1;
      if (ix+1>xmx) xmx=ix+1;
      if (iy+1<ymx) ymx=iy+1;
      png.plot(ix+1, iy+1, 1.0, 1.0, 1.0);
      ix = ixmax;      
      if (ix+1<xmn) xmn=ix+1;
      if (iy+1<ymn) ymn=iy+1;
      if (ix+1>xmx) xmx=ix+1;
      if (iy+1>ymx) ymx=iy+1;
      png.plot(ix+1, iy+1, 1.0, 1.0, 1.0);
    }

  }
  png.close();
}
