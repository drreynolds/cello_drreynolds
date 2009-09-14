#include <stdio.h>
#include "cello.h"
#include "node4.h"

const bool debug = false;

Node4::Node4() 
{ 
  child_[0] = NULL;
  child_[1] = NULL;
  child_[2] = NULL;
  child_[3] = NULL;

  neighbor_[0] = NULL;
  neighbor_[1] = NULL;
  neighbor_[2] = NULL;
  neighbor_[3] = NULL;

  parent_ = NULL;
}

Node4 * Node4::child (int num) 
{ 
  return child_[num]; 
}

Node4 * Node4::neighbor (int num) 
{ 
  return neighbor_[num]; 
}

Node4 * Node4::parent () 
{ 
  return parent_; 
}

// Create 4 empty child nodes

int Node4::refine 
(
 const bool * mask_array, 
 int nd0,  int nd1,
 int low0, int up0,  
 int low1, int up1
)
{
  int level = 0;
  if (up0-1 > low0 && up1-1 > low1) {

    if (debug) printf ("refine (%d %d  %d:%d %d:%d)\n",nd0,nd1,low0,up0,low1,up1);

    // determine whether to refine the node

    bool refine_node = false;

    for (int i1=low1; i1<up1; i1++) {
      for (int i0=low0; i0<up0; i0++) {
	int i = i0 + nd0 * i1;
	if (mask_array[i]) {
	  refine_node = true;
	  break;
	}
      }
    }

    // refine the node if needed

    if (refine_node) {

      create_children_();

      int mid0 = (up0 + low0)/2;
      int mid1 = (up1 + low1)/2;
      int l0 = child_[corner_UL]->refine 
	(mask_array,nd0,nd1,low0,mid0,low1,mid1);
      int l1 = child_[corner_DL]->refine 
	(mask_array,nd0,nd1,mid0,up0,low1,mid1);
      int l2 = child_[corner_UR]->refine 
	(mask_array,nd0,nd1,low0,mid0,mid1,up1);
      int l3 = child_[corner_DR]->refine 
	(mask_array,nd0,nd1,mid0,up0,mid1,up1);

      level = (l0 > level) ? l0 : level;
      level = (l1 > level) ? l1 : level;
      level = (l2 > level) ? l2 : level;
      level = (l3 > level) ? l3 : level;

      ++ level;

    }

  } // if not at bottom of recursion

  return level;
}

void Node4::create_children_()
{
  child_[0] = new Node4();
  child_[1] = new Node4();
  child_[2] = new Node4();
  child_[3] = new Node4();

  // Initialize neighbors

  // right internal children

  child_[corner_DR]->neighbor_[face_U] = child_[corner_UR];
  child_[corner_UR]->neighbor_[face_D] = child_[corner_DR];

  // up internal children

  child_[corner_UL]->neighbor_[face_R] = child_[corner_UR];
  child_[corner_UR]->neighbor_[face_L] = child_[corner_UL];

  // left internal children

  child_[corner_DL]->neighbor_[face_U] = child_[corner_UL];
  child_[corner_UL]->neighbor_[face_D] = child_[corner_DL];

  // down internal children

  child_[corner_DL]->neighbor_[face_R] = child_[corner_DR];
  child_[corner_DR]->neighbor_[face_L] = child_[corner_DL];


  // right external children

  child_[corner_UR]->neighbor_[face_U] = (neighbor_[face_U]) ? neighbor_[face_U]->child_[corner_DR] : NULL;
  child_[corner_DR]->neighbor_[face_D] = (neighbor_[face_D]) ? neighbor_[face_D]->child_[corner_UR] : NULL;

  // up external children

  child_[corner_UL]->neighbor_[face_L] = (neighbor_[face_L]) ? neighbor_[face_L]->child_[corner_UR] : NULL;
  child_[corner_UR]->neighbor_[face_R] = (neighbor_[face_R]) ? neighbor_[face_R]->child_[corner_UL] : NULL;

  // left external children

  child_[corner_UL]->neighbor_[face_U] = (neighbor_[face_U]) ? neighbor_[face_U]->child_[corner_DL] : NULL;
  child_[corner_DL]->neighbor_[face_D] = (neighbor_[face_D]) ? neighbor_[face_D]->child_[corner_UL] : NULL;

  // down external children

  child_[corner_DL]->neighbor_[face_L] = (neighbor_[face_L]) ? neighbor_[face_L]->child_[corner_DR] : NULL;
  child_[corner_DR]->neighbor_[face_R] = (neighbor_[face_R]) ? neighbor_[face_R]->child_[corner_DL] : NULL;

  // Initialize parent

  child_[0]->parent_ = this;
  child_[1]->parent_ = this;
  child_[2]->parent_ = this;
  child_[3]->parent_ = this;

}

// Perform a pass of trying to remove level-jumps 
bool Node4::normalize_pass()
{

  if (is_leaf()) {

    // refine self if any neighbors have grandchildren

    bool refine_self;

    int any = 0;

    refine_self = 
      (neighbor_[face_R] && 
       neighbor_[face_R]->child_[corner_DL] && 
       neighbor_[face_R]->child_[corner_DL]->child_[any]);
    refine_self = refine_self ||
      (neighbor_[face_R] && 
       neighbor_[face_R]->child_[corner_UL] && 
       neighbor_[face_R]->child_[corner_UL]->child_[any]);
    refine_self = refine_self ||
      (neighbor_[face_U] && 
       neighbor_[face_U]->child_[corner_DL] && 
       neighbor_[face_U]->child_[corner_DL]->child_[any]);
    refine_self = refine_self ||
      (neighbor_[face_U] && 
       neighbor_[face_U]->child_[corner_DR] && 
       neighbor_[face_U]->child_[corner_DR]->child_[any]);
    refine_self = refine_self || 
      (neighbor_[face_L] && 
       neighbor_[face_L]->child_[corner_DR] && 
       neighbor_[face_L]->child_[corner_DR]->child_[any]);
    refine_self = refine_self ||
      (neighbor_[face_L] && 
       neighbor_[face_L]->child_[corner_UR] && 
       neighbor_[face_L]->child_[corner_UR]->child_[any]);
    refine_self = refine_self ||
      (neighbor_[face_D] && 
       neighbor_[face_D]->child_[corner_UL] && 
       neighbor_[face_D]->child_[corner_UL]->child_[any]);
    refine_self = refine_self || 
      (neighbor_[face_D] && 
       neighbor_[face_D]->child_[corner_UR] && 
       neighbor_[face_D]->child_[corner_UR]->child_[any]);

    if (refine_self) {

      create_children_();

      return 
	child_[0]->normalize_pass() ||
	child_[1]->normalize_pass() ||
	child_[2]->normalize_pass() ||
	child_[3]->normalize_pass();

    } else {

      return false;

    }

  } else {

    // Non-leaf node: normalize children

    return 
      child_[0]->normalize_pass() ||
      child_[1]->normalize_pass() ||
      child_[2]->normalize_pass() ||
      child_[3]->normalize_pass();
  }

}

// Fill the image region with values
void Node4::fill_image
(
 float * image,
 int nd0,  int nd1,
 int low0, int high0,  
 int low1, int high1,
 int level,
 int num_levels
 )
{
  int i0,i1,i;
  // Fill interior
  for (i1=low1; i1<=high1; i1++) {
    for (i0=low0; i0<=high0; i0++) {
      i = i0 + nd0 * i1;
      image[i] = num_levels - level + 1;
    }
  }

  // Draw border
  for (i0=low0; i0<=high0; i0++) {
    i1 = low1;
    image[i0 + nd0*i1] = 0;
    i1 = high1;
    image[i0 + nd0*i1] = 0;
  }

  for (i1=low1; i1<=high1; i1++) {
    i0 = low0;
    image[i0 + nd0*i1] = 0;
    i0 = high0;
    image[i0 + nd0*i1] = 0;
  }
    

  // Recurse

  int mid0 = (high0 + low0)/2;
  int mid1 = (high1 + low1)/2;

  if (child_[corner_UL] != NULL) {
    child_[corner_UL]->fill_image (image,nd0,nd1,low0,mid0, low1,mid1, level + 1, num_levels);
  }
  if (child_[corner_DL] != NULL) {
    child_[corner_DL]->fill_image (image,nd0,nd1,mid0,high0,low1,mid1, level + 1, num_levels);
  }
  if (child_[corner_UR] != NULL) {
    child_[corner_UR]->fill_image (image,nd0,nd1,low0,mid0, mid1,high1,level + 1, num_levels);
  }
  if (child_[corner_DR] != NULL) {
    child_[corner_DR]->fill_image (image,nd0,nd1,mid0,high0,mid1,high1,level + 1, num_levels);
  }
}

