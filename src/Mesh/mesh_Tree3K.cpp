// $Id$
// See LICENSE_CELLO file for license and copyright information

/// @file     mesh_Tree3K.cpp
/// @author   James Bordner (jobordner@ucsd.edu)
/// @date     2009-10-29
/// @brief    Implementation of Tree3K class for K^3-trees

#include <stdio.h>
#include "cello.hpp"

#include "error.hpp"
#include "mesh.hpp"

//----------------------------------------------------------------------

const bool debug = false;

//----------------------------------------------------------------------

Tree3K::Tree3K(int r)
  : TreeK(r),
    root_(new Node3K(r))
{
}

//----------------------------------------------------------------------

Tree3K::Tree3K(const Tree3K & tree3k) throw()
{ 
  INCOMPLETE_MESSAGE("Tree3K::Tree3K",""); 
}

//----------------------------------------------------------------------

Tree3K & Tree3K::operator= (const Tree3K & tree3k) throw()
{ 
  INCOMPLETE_MESSAGE("Tree3K::operator =","");
  return *this; 
}

//----------------------------------------------------------------------

void Tree3K::refine
(
 const int * level_array, 
 int ndx, int ndy, int ndz,
 int max_level,
 bool is_full
 )
{
  levels_ = root_->refine
    (level_array,ndx,ndy,ndz,0,ndx,0,ndy,0,ndz,0,max_level,is_full);
  if (debug) printf ("%d\n",levels_);
}

//----------------------------------------------------------------------

void Tree3K::balance(bool is_full)
{
  // Repeatedly balance
  int pass = 0;
  bool tree_changed;
  do {
    tree_changed = false;
    root_->balance_pass(tree_changed,is_full);
    if (debug) printf ("Balance pass %d\n",pass);
    pass++;
  } while (tree_changed);
  printf ("passes = %d\n",pass);
}

//----------------------------------------------------------------------

void Tree3K::optimize()
{
  int pass = 0;
  bool tree_changed;
  do {
    tree_changed = false;
    root_->optimize_pass(tree_changed);
    if (debug) printf ("Optimize pass %d\n",pass);
    pass++;
  } while (tree_changed);
  printf ("passes = %d\n",pass);
}

float * Tree3K::create_image (int n,int line_width, int axis)
{
  float * image = new float [n*n];

  root_->fill_image(image,n,n,n,0,n-1,0,n-1,0,n-1,0,levels_,line_width,axis);
  return image;
}

//----------------------------------------------------------------------

void Tree3K::geomview (std::string filename)
{
  FILE * fpr = fopen (filename.c_str(),"w");

  fprintf (fpr,"VECT\n");
  fprintf (fpr,"%d %d 1\n",2+4*num_nodes(), 2+16*num_nodes());

  // Write vertices per polygon
  fprintf (fpr,"1 1 ");
  for (int i=0; i<num_nodes(); i++) {
    fprintf (fpr,"8 3 3 2 ");
  }
  fprintf (fpr,"\n");

  // Write colors changes
  fprintf (fpr,"1 0 ");
  for (int i=0; i<num_nodes(); i++) fprintf (fpr,"0 0 0 0 "); fprintf (fpr,"\n");

  fprintf (fpr,"0 0 0\n");
  fprintf (fpr,"1 1 1\n");

  root_->geomview(fpr,0,1,0,1,0,1,false);

  fprintf (fpr,"1 1 1 0\n");

  fclose(fpr);
}
