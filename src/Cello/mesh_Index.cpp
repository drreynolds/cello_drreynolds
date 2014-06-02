// See LICENSE_CELLO file for license and copyright information

/// @file     mesh_Index.cpp
/// @author   James Bordner (jobordner@ucsd.edu)
/// @date     2013-05-06
/// @brief    Implementation of Index class for indexing nodes in a forest of octrees

#include "mesh.hpp"

//----------------------------------------------------------------------

Index::Index() { clear(); }

//----------------------------------------------------------------------

Index::Index(const Index & index) 
{
  copy_(index);
}

//----------------------------------------------------------------------

Index::Index(int ix, int iy, int iz) 
{
  clear();
  set_array(ix,iy,iz);
}

//----------------------------------------------------------------------

Index & Index::operator = (const Index & index)
{
  copy_(index);
  return *this;
}

//----------------------------------------------------------------------

bool Index::operator == (const Index & index) const
{
  return (v_[0] == index.v_[0] && 
	  v_[1] == index.v_[1] &&
	  v_[2] == index.v_[2]);
}

//----------------------------------------------------------------------

bool Index::operator != (const Index & index) const
{
  return ! (*this == index);
}

//----------------------------------------------------------------------

void Index::clear () 
{
  for (int axis=0; axis<3; axis++) {
    a_[axis].array = 0;
    a_[axis].tree = 0;
    a_[axis].level = 0;
  }
}

//----------------------------------------------------------------------

Index Index::index_parent () const
{
  Index index = *this;
  int level = index.level();
  if (level > 0) {
    index.set_child (level,0,0,0);
    index.set_level(level - 1);
  } else {
    WARNING("Index::index_parent()",
	    "Attempting to access parent of root");
  }
  return index;
}

//----------------------------------------------------------------------

Index Index::index_child (int icx, int icy, int icz) const
{
  Index index = *this;
  int level = index.level();
  index.set_level(level+1);
  index.set_child (level+1,icx,icy,icz);
  return index;
}

//----------------------------------------------------------------------

bool Index::is_on_boundary (int axis, int face, int narray) const
{

  if (face == 0) face = -1;

  int level = this->level();
  int array = a_[axis].array;
  int tree  = a_[axis].tree;

  // update tree bits

  int shift_level = (1 << (INDEX_MAX_TREE_BITS - level));

  tree += face*shift_level; 

  int shift_overflow = (1 << INDEX_MAX_TREE_BITS);

  if (tree & shift_overflow) {

    if (narray == 1) return true;

    tree &= ~(shift_overflow);

    return ! ((0 <= array+face) && (array+face < narray) );

  } else {
    
    return false;
  }

}

//----------------------------------------------------------------------

bool Index::is_on_boundary 
(
 const int if3[3],
 const int n3[3]
 ) const
{

  const int level = this->level();

  for (int axis = 0; axis < 3; axis++) {
    
    if (n3[axis] > 1) {
      int array = a_[axis].array;
      int tree  = a_[axis].tree;

      // update tree bits

      int shift_level = (1 << (INDEX_MAX_TREE_BITS - level));

      tree += if3[axis]*shift_level; 

      // update array if necessary

      int shift_overflow = (1 << INDEX_MAX_TREE_BITS);

      if (tree & shift_overflow) {

	tree &= ~(shift_overflow);

	if ( ! ((0 <= array+if3[axis]) && (array+if3[axis] < n3[axis])) )
	  return true;
    
      }
    }
  }

  return false;
}

//----------------------------------------------------------------------

Index Index::index_neighbor (const int if3[3], const int n3[3]) const
{
  Index index = *this;

  const int level = index.level();

  for (int axis = 0; axis < 3; axis++) {
    
    int array = index.a_[axis].array;
    int tree  = index.a_[axis].tree;

    // update tree bits

    int shift_level = (1 << (INDEX_MAX_TREE_BITS - level));

    tree += if3[axis]*shift_level; 

    // update array if necessary

    int shift_overflow = (1 << INDEX_MAX_TREE_BITS);

    if (tree & shift_overflow) {

      tree &= ~(shift_overflow);

      array = (n3[axis] + array + if3[axis]) % n3[axis];

    }
    index.a_[axis].array = array;
    index.a_[axis].tree  = tree;
  }
  return index;
}

//----------------------------------------------------------------------

void Index::child (int level, int * icx, int * icy, int * icz) const
{
  ASSERT ("Index::child","level must be at least 1",level>0);
  int shift = INDEX_MAX_TREE_BITS - level;
  if (icx) (*icx) = (a_[0].tree >> shift) & 1;
  if (icy) (*icy) = (a_[1].tree >> shift) & 1;
  if (icz) (*icz) = (a_[2].tree >> shift) & 1;
}

//----------------------------------------------------------------------

bool Index::is_root() const
{ return (v_[0]==0 && v_[1]==0 && v_[2]==0); }

//----------------------------------------------------------------------

void Index::array (int * ix, int *iy, int *iz) const
{ 
  if (ix) (*ix) = a_[0].array;
  if (iy) (*iy) = a_[1].array;
  if (iz) (*iz) = a_[2].array;
}

//----------------------------------------------------------------------

int Index::level () const
{
  return a_[0].level + INDEX_MAX_LEVEL_AXIS_RANGE*
    (    a_[1].level + INDEX_MAX_LEVEL_AXIS_RANGE*
	 a_[2].level );  
}

//----------------------------------------------------------------------

void Index::set_level(int level)
{ 
  int shift = INDEX_MAX_LEVEL_AXIS_BITS;
  int mask  = INDEX_MAX_LEVEL_AXIS_RANGE - 1;
  a_[0].level = (level >> (0*shift)) & mask;
  a_[1].level = (level >> (1*shift)) & mask;
  a_[2].level = (level >> (2*shift)) & mask;
  clean();
}

//----------------------------------------------------------------------

void Index::clean ()
{
  for (int level = this->level()+1; level<INDEX_MAX_TREE_BITS; level++) {
    set_child(level,0,0,0);
  }
}

//----------------------------------------------------------------------

void Index::set_array(int ix, int iy, int iz)
{ 
  // right-bits = array
  ASSERT4 ("Index::set_array",
	   "Array size (%d %d %d) out of range (maximum %d)",
	   ix,iy,iz,INDEX_MAX_ARRAY_INDEX,
	   ((0 <= ix && ix < INDEX_MAX_ARRAY_INDEX) &&
	    (0 <= iy && iy < INDEX_MAX_ARRAY_INDEX) &&
	    (0 <= iz && iz < INDEX_MAX_ARRAY_INDEX)));
  a_[0].array = ix;
  a_[1].array = iy;
  a_[2].array = iz;
}

//----------------------------------------------------------------------

int Index::tree (int axis) const
{ return a_[axis].tree; }
  
//----------------------------------------------------------------------

void Index::set_child(int level, int ix, int iy, int iz)
{
  ASSERT ("Index::set_child","level must be at least 1",level>0);
  int shift = (INDEX_MAX_TREE_BITS - level);
  int mask  = ~(1 << shift);
  a_[0].tree = (a_[0].tree & mask) | (ix<<shift);
  a_[1].tree = (a_[1].tree & mask) | (iy<<shift);
  a_[2].tree = (a_[2].tree & mask) | (iz<<shift);
}

//----------------------------------------------------------------------

void Index::print (const char * msg,
		   int max_level,
		   int rank,
		   bool no_nl,
		   void * simulation
) const
{

  print_(stdout,msg,max_level,rank,no_nl);

#ifdef CELLO_DEBUG
  FILE * fp_debug = ((Simulation *)simulation)->fp_debug();
  print_(fp_debug,msg,max_level,rank,no_nl);
#endif
}

//----------------------------------------------------------------------

void Index::print_ (FILE * fp,
		    const char * msg,
		    int max_level,
		    int rank,
		    bool no_nl) const
{

  std::string buffer;

  const int level = this->level();

  if (max_level == -1) max_level = level;

  int nb = 0;

  for (int axis=0; axis<rank; axis++) {
    nb = std::max(nb,num_bits_(a_[axis].array));
  }

  buffer = buffer + "[ ";
  for (int axis=0; axis<rank; axis++) {

    for (int i=nb; i>=0; i--) {
      int bit = (a_[axis].array & ( 1 << i));
      if (fp != NULL) buffer = buffer + (bit?"1":"0");
    }

    for (int i=0; i<max_level; i++) {
      if (i==0) {
	buffer = buffer + ":";
      }

      if (i < level) {
	int ic3[3];
	child (i+1, &ic3[0], &ic3[1], &ic3[2]);
	buffer = buffer + (ic3[axis] ? "1":"0");
      } else {
	buffer = buffer + " ";
      }
	
    }
    buffer = buffer + " ";
      
  }
  buffer = buffer + "] ";

  buffer = buffer + msg;

  if (! no_nl) buffer = buffer + "\n";

  fprintf (fp,buffer.c_str());
  fflush(fp);
}

//----------------------------------------------------------------------

void Index::write (int ip,
		   const char * msg,
		   int max_level,
		   int rank) const
{
  char filename[80];
  sprintf (filename,"index.%s.%d",msg,ip);
  FILE * fp = fopen(filename,"a");

  if (max_level == -1) max_level = this->level();
    
  // fprintf (fp,"INDEX %p %s: ", this,msg);
  fprintf (fp,"INDEX %s: ", msg);

  int nb = 0;

  for (int axis=0; axis<rank; axis++) {
    nb = std::max(nb,num_bits_(a_[axis].array));
  }

  for (int axis=0; axis<rank; axis++) {

    for (int i=nb; i>=0; i--) {
      int bit = (a_[axis].array & ( 1 << i));
      fprintf (fp,"%d",bit?1:0);
    }

    for (int level=0; level<max_level; level++) {

      if (level == 0) fprintf (fp,":");
      int ic3[3];
      child (level+1, &ic3[0], &ic3[1], &ic3[2]);
      fprintf (fp,"%d",ic3[axis]);
	
    }
    fprintf (fp," ");
      
  }
  fprintf (fp,"\n");

  fflush(fp);
}

//----------------------------------------------------------------------

int Index::num_bits_(int value) const
{
  int nb = 32;
  while ( --nb >= 0 && ! (value & (1 << nb))) 
    ;
  return (std::max(nb,0));

}

//----------------------------------------------------------------------

std::string Index::bit_string(int max_level,int rank, int num_bits) const
{
  const int level = this->level();

  if (max_level == -1) max_level = this->level();

  std::string bits = "";
  const std::string separator = "_";

  if (num_bits == 0) {
    for (int axis=0; axis<rank; axis++) {
      num_bits = std::max (num_bits,num_bits_(a_[axis].array));
    }
  } else --num_bits;

  for (int axis=0; axis<rank; axis++) {

    for (int i=num_bits; i>=0; i--) {
      int bit = (a_[axis].array & ( 1 << i));
      bits = bits + (bit?"1":"0");
    }

    for (int i=0; i<max_level; i++) {

      if (i == 0) bits = bits + ":";
      if (i < level) {
	int ic3[3];
	child (i+1, &ic3[0], &ic3[1], &ic3[2]);
	bits = bits + (ic3[axis]?"1":"0");
      } else 
	bits = bits + separator;
    }
    if (axis<rank-1) bits = bits + separator;
      
  }
  return bits;
}

//======================================================================

