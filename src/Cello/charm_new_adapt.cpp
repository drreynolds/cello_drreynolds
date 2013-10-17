// See LICENSE_CELLO file for license and copyright information

/// @file     charm_new_adapt.cpp
/// @author   James Bordner (jobordner@ucsd.edu)
/// @date     2013-04-25
/// @brief    Charm-related mesh adaptation control functions
///
/// create_mesh()
///
/// 1. Apply refinement criteria
/// 2. Notify neighbors of intent
/// 3. Update intent based on neighbor's (goto 2)
/// 4. After quiescence, insert() / delete() array elements
/// 5. After contribute(), doneInserting()
///
/// adapt_mesh()
///
/// 1. Apply refinement criteria
/// 2. Notify neighbors of intent
/// 3. Update intent based on neighbor's (goto 2)
/// 4. After quiescence, insert() / delete() array elements
///
/// 1. APPLY REFINEMENT CRITERIA
///
///    adapt_mesh() or create_mesh()
///
///    Call determine_adapt() to decide whether each existing leaf
///    block should coarsen, refine or stay the same.  If creating
///    initial mesh, the new CommBlocks will apply initial conditions,
///    and recursively call create_mesh.  Set quiescence call to
///    q_adapt_next().
///
/// 2. NOTIFY ALL NEIGHBORS OF INTENT
///
///    notify_neighbors()
///
///    Loop over all neighbors and call p_get_neighbor_level() to
///    notify them of intended level.
///
/// 3. UPDATE OWN NEW LEVEL BASED ON NEIGHBORS' NEW LEVELS
///
///    p_get_neighbor_level()
///
///    Updates intended level based on neighbor's intended level.  If
///    intended level changes, call notify_neighbors() to notify all
///    neighbors of updated intent.  If not a leaf, then call
///    p_get_neighbor_level() recursively on select children.
///
/// 4. AFTER QUIESCENCE, INSERT() / DELETE() ARRAY ELEMENTS
///
///    q_adapt_next()
///
///    After quiescence, all refinement decisions are known.  Call
///    refine() and coarsen() on appropriate elements to perform actual
///    block refinement and coarsening, inserting and deleting
///    elements from the block array.  Set quiescence call to 
///    q_adapt_end().
///
/// 5. AFTER QUIESCENCE, CALL doneInserting() AND CONTINUE TO NEXT STEP
///
///    Since all refine elements have been created, the root block
///    calls doneInserting() on the chare array, and continues to the
///    next phase after mesh adaption.
///
///--------------------------------------------------
///
///    refine()
///
///    create new child blocks, including upsampled data and child
///    neighbor lists.  New child blocks call create_mesh() if at
///    start.
///
///--------------------------------------------------
///
///    can_coarsen()
///    
///    Alert parent via p_child_can_coarsen() that can coarsen.
///
///
///    p_child_can_coarsen()
///
///    Count children that can coarsen.  If all, then call coarsen()
///
///
///    coarsen_parent()
///
///    Request data and neighbor levels from children using p_delete().
///  
///
///    p_delete()
///
///    Send data and neighbor levels to parent using
///    p_get_child_data(), remove self from block array, and delete.
///
///
///    p_get_child_data()
///
///    Retrieve child block's data.
///
///----------------------------------------------------------------------

#ifdef TEMP_NEW_ADAPT

const char * adapt_str[] = {"unknown","coarsen","same","refine"};

// #define DEBUG_ADAPT

//--------------------------------------------------
char buffer [100];

#ifdef DEBUG_ADAPT

#define TRACE_LEVEL_NEW(msg)						\
  {									\
    sprintf (buffer,"ADAPT TRACE line %d %s level_new %d level %d",	\
	     __LINE__, msg,level_new_,this->level());			\
    index_.print(buffer,-1,2);						\
  }
 

#define PUT_NEIGHBOR_LEVEL(INDEX,IC3,IF3,LEVEL_NOW,LEVEL_NEW,MSG)	\
  {									\
    std::string bit_str = INDEX.bit_string(-1,2);			\
    sprintf (buffer,"%s %s p_get_neighbor_level() "			\
	     "face %2d %2d %2d  child %d %d %d  "			\
	     "%d -> %d :%d",						\
	     MSG,bit_str.c_str(),					\
	     IF3[0],IF3[1],IF3[2],					\
	     IC3[0],IC3[1],IC3[2],					\
	     LEVEL_NOW,LEVEL_NEW,__LINE__);				\
    index_.print(buffer,-1,2);						\
    check_child_(IC3,"PUT_NEIGHBOR_LEVEL",__FILE__,__LINE__);		\
    check_face_(IF3,"PUT_NEIGHBOR_LEVEL",__FILE__,__LINE__);		\
    thisProxy[INDEX].p_get_neighbor_level				\
      (index_,IC3,IF3,LEVEL_NOW,LEVEL_NEW);				\
  }
#else /* DEBUG_ADAPT */

#define TRACE_LEVEL_NEW(msg)	/* NULL */

#define PUT_NEIGHBOR_LEVEL(INDEX,IC3,IF3,LEVEL_NOW,LEVEL_NEW,MSG)	\
  thisProxy[INDEX].p_get_neighbor_level (index_,IC3,IF3,LEVEL_NOW,LEVEL_NEW);
#endif /* DEBUG_ADAPT */
//--------------------------------------------------

#include "simulation.hpp"
#include "mesh.hpp"
#include "comm.hpp"

#include "charm_simulation.hpp"
#include "charm_mesh.hpp"

//======================================================================

void CommBlock::create_mesh()
{

  TRACE_ADAPT("BEGIN ADAPT create_mesh()");

  const int rank = simulation()->dimension();
  sync_coarsen_.set_stop(NC(rank));
  sync_coarsen_.clear();

  if (is_leaf()) {

    int level_maximum = simulation()->config()->initial_max_level;

    level_new_ = desired_level_(level_maximum);
    TRACE_LEVEL_NEW("create_mesh");

    notify_neighbors(level_new_);

  }

  CkStartQD (CkCallback(CkIndex_CommBlock::q_adapt_next(), 
			thisProxy[thisIndex]));

}

//----------------------------------------------------------------------

void CommBlock::adapt_mesh()
{

  TRACE_ADAPT("BEGIN ADAPT adapt_mesh()");

  sync_coarsen_.clear();

  if (is_leaf()) {

    int level_maximum = simulation()->config()->mesh_max_level;

    level_new_ = desired_level_(level_maximum);
    TRACE_LEVEL_NEW("adapt_mesh");

    notify_neighbors(level_new_);

  }

  CkStartQD (CkCallback(CkIndex_CommBlock::q_adapt_next(), 
			thisProxy[thisIndex]));
}

//----------------------------------------------------------------------

int CommBlock::desired_level_(int level_maximum)
{
  int level = this->level();
  int level_desired = level;

  adapt_ = determine_adapt();

  if (adapt_ == adapt_coarsen && level > 0) 
    level_desired = level - 1;
  else if (adapt_ == adapt_refine  && level < level_maximum) 
    level_desired = level + 1;
  else {
    adapt_ = adapt_same;
    level_desired = level;
  }

  return level_desired;
}

//----------------------------------------------------------------------

int CommBlock::determine_adapt()
{
  if (! is_leaf()) return adapt_same;

  FieldDescr * field_descr = simulation()->field_descr();

  int index_refine = 0;
  int adapt = adapt_unknown;

  Problem * problem = simulation()->problem();
  Refine * refine;

  while ((refine = problem->refine(index_refine++))) {

    adapt = std::max(adapt,refine->apply(this, field_descr));

  }

  return adapt;
}

//----------------------------------------------------------------------

void CommBlock::q_adapt_next()
{
  TRACE_ADAPT("ADAPT q_adapt_next()");

  CkStartQD (CkCallback(CkIndex_CommBlock::q_adapt_end(), 
			thisProxy[thisIndex]));

  update_levels_();

  debug_faces_("q_adapt_next");
  
  TRACE_LEVEL_NEW("q_adapt_next");

  if (is_leaf()) {
    if (level() < level_new_) refine();
    if (level() > level_new_) coarsen();
  }

}

//----------------------------------------------------------------------

void CommBlock::q_adapt_end()
{


  TRACE_ADAPT("ADAPT q_adapt_end");

  next_phase_ = phase_output;

  if (thisIndex.is_root()) {

   thisArray->doneInserting();

   const int initial_cycle = simulation()->config()->initial_cycle;
   const bool is_first_cycle = (initial_cycle == cycle());
   const int level_maximum = simulation()->config()->initial_max_level;

   if (is_first_cycle && level_count_++ < level_maximum) {
     thisProxy.p_create_mesh();
   } else {
     thisProxy.p_refresh_begin();
   }
  }
}
//----------------------------------------------------------------------

void CommBlock::refine()
{

  TRACE_ADAPT("ADAPT refine()");

  adapt_ = adapt_unknown;

  const int rank = simulation()->dimension();
  
  int nx,ny,nz;
  block()->field_block()->size(&nx,&ny,&nz);

  // For each new child

  int ic3[3];
  ItChild it_child (rank);
  while (it_child.next(ic3)) {

    Index index_child = index_.index_child(ic3);

    // If child doesn't exist yet

    if ( ! is_child(index_child) ) {

      // Prolong data

      int narray = 0;  
      char * array = 0;
      int iface[3] = {0,0,0};
      bool lghost[3] = {true,true,true};
      FieldFace * field_face = 
	load_face_ (&narray,&array, iface,ic3,lghost, op_array_prolong);

      int num_field_blocks = 1;
      bool testing = false;

      const Factory * factory = simulation()->factory();

      // Create child block

      factory->create_block 
	(&thisProxy, index_child,
	 nx,ny,nz,
	 num_field_blocks,
	 adapt_step_,
	 cycle_,time_,dt_,
	 narray, array, op_array_prolong,
	 27,&child_face_level_[27*IC3(ic3)],
	 testing);

      delete field_face;

      children_.push_back(index_child);

    }
  }
}

//----------------------------------------------------------------------

void CommBlock::coarsen()
{
  
  TRACE_ADAPT("ADAPT COARSEN coarsen()");

  if (level() > 0) {

    // check that there are no fine-level neighbors
    const int level = this->level();
    const int  rank         = simulation()->dimension();
    const int  rank_refresh = simulation()->config()->field_refresh_rank;
    ItFace it_face(rank,rank_refresh);
    int of3[3];
    bool can_coarsen = true;
    while (it_face.next(of3)) {
      Index in = neighbor_(of3);
      const int level_face = face_level (of3);
      if (level < level_face) can_coarsen = false;
    }

    if (can_coarsen) {
      Index index_parent = index_.index_parent();
      int ic3[3] = {0};
      index_.child(level,ic3+0,ic3+1,ic3+2);
      thisProxy[index_parent].p_child_can_coarsen(ic3);
    }
  }
}
//----------------------------------------------------------------------

void CommBlock::notify_neighbors(int level_new)
{
  // Loop over all neighobrs and (if not coarsening) tell them desired
  // refinement level
  //  if (adapt_ != adapt_coarsen) {
  if (1) {

    const int level        = this->level();
    const int rank         = simulation()->dimension();
    const int rank_refresh = simulation()->config()->field_refresh_rank;

    ItFace it_face(rank,rank_refresh);
    int of3[3];

    while (it_face.next(of3)) {

      int ic3[3] = {0,0,0};

      Index index_neighbor = neighbor_(of3);

      const int level_face = face_level (of3);

      if (level_face == level) {

	// (X) SEND-SAME: Face and level are sent to unique
	// neighboring block in the same level

	PUT_NEIGHBOR_LEVEL(index_neighbor,ic3,of3,level,level_new,"send-same");

      } else if (level_face == level - 1) {

	// (X) SEND-COARSE: Face, level, and child indices are sent to
	// unique neighboring block in the next-coarser level


	index_.child (level,&ic3[0],&ic3[1],&ic3[2]);

	int op3[3];
	parent_face_(op3,of3,ic3);

	// avoid redundant calls
	if (op3[0]==of3[0] && 
	    op3[1]==of3[1] && 
	    op3[2]==of3[2]) {

	  Index index_uncle = index_neighbor.index_parent();
	  PUT_NEIGHBOR_LEVEL(index_uncle,ic3,of3,level,level_new,"send-coarse");

	}

      } else if (level_face == level + 1) {

	// (X) SEND_FINE: Face and level are sent to all nibling
	// blocks in the next-finer level along the face.

	const int if3[3] = {-of3[0],-of3[1],-of3[2]};
	ItChild it_child(rank,if3);
	while (it_child.next(ic3)) {
	  Index index_nibling = index_neighbor.index_child(ic3);
	  PUT_NEIGHBOR_LEVEL(index_nibling,ic3,of3,level,level_new,"send-fine");
    	}

      } else {
	std::string bit_str = index_.bit_string(-1,2);
	WARNING3 ("CommBlock::notify_neighbor()",
		"%s level %d and face level %d differ by more than 1",
		bit_str.c_str(),level,level_face);
      }

    }
  }
}

//----------------------------------------------------------------------

void CommBlock::p_get_neighbor_level
(
 Index index_debug,
 int ic3[3],
 int if3[3], 
 int level_face,
 int level_face_new
 )
{
  debug_faces_("p_get_neighbor start");

  const int level        = this->level();

  const int of3[3] = {-if3[0],-if3[1],-if3[2] };
  const int rank = simulation()->dimension();
  const int rank_refresh = simulation()->config()->field_refresh_rank;

  const std::string bit_str = index_debug.bit_string(-1,2);			

  if (is_leaf()) {

    if (level == level_face) {

#ifdef DEBUG_ADAPT
      sprintf (buffer,"%s %s p_get_neighbor_level() face %2d %2d %2d "
	       "child %d %d %d  "
	       "%d -> %d",
	       "recv-same",bit_str.c_str(),					
	       if3[0],if3[1],if3[2],					
	       ic3[0],ic3[1],ic3[2],					
	       level_face,level_face_new);					
      index_.print(buffer,-1,2);
#endif
  
      // (X) RECV-SAME: Face and level are received from unique
      // neighbor.  Unique face level is updated, and levels on
      // possibly multiple faces of multiple children are updated.

      set_face_level_new (of3, level_face_new);

      ItChild it_child (rank,of3);
      int jc3[3];
      while (it_child.next(jc3)) {

	int jf3[3];

	ItFace it_face (rank,rank_refresh,jc3,of3);

	while (it_face.next(jf3)) {
	  set_child_face_level_new(jc3,jf3,level_face_new);
	}
      }

    } else if (level == level_face - 1) {

#ifdef DEBUG_ADAPT

      sprintf (buffer,"%s %s p_get_neighbor_level() face %2d %2d %2d "
	       "child %d %d %d  "
	       "%d -> %d",
	       "recv-fine",bit_str.c_str(),					
	       if3[0],if3[1],if3[2],					
	       ic3[0],ic3[1],ic3[2],					
	       level_face,level_face_new);					
      index_.print(buffer,-1,2);						
#endif
  
      // (*) RECV-FINE: Face, level, and sender child indices are
      // received from possibly non-unique neighbor for the
      // corresponding face.  If neighbor level indicates the neighbor
      // needs to refine, then refinement is triggered in this block
      // as well.  Face levels are ignored, since the face either
      // stays the same, or refinement is triggered, in which case the
      // child face levels are used.  The neighbor level for the
      // unique face and unique child facing the sending child is
      // updated.

      set_face_level_new (of3, level_face_new);

      ItChild it_child (rank,of3);
      int jc3[3];
      Index index_neighbor = neighbor_(of3).index_child(ic3);
      while (it_child.next(jc3)) {

	Index index_child = index_.index_child(jc3);
	// int jc3[3];
	// facing_child_(jc3,ic3,if3);
	TRACE9("recv-fine facing child %d %d %d  child %d %d %d  face %d %d %d",
	       jc3[0],jc3[1],jc3[2],ic3[0],ic3[1],ic3[2],if3[0],if3[1],if3[2]);

	int jf3[3];
	ItFace it_face (rank,rank_refresh,jc3,of3);

	std::string n1_str = index_neighbor.bit_string(-1,2);			

	TRACE1("recv-fine index_neighbor %s",n1_str.c_str());

	while (it_face.next(jf3)) {
	  TRACE3("recv-fine it_face %d %d %d",
		 jf3[0],jf3[1],jf3[2]);
	  Index in = neighbor_(jf3,&index_child);

	  std::string n2_str = in.bit_string(-1,2);			
	  TRACE1("recv-fine in %s",n2_str.c_str());
	  
	  if (in == index_neighbor) {
	    set_child_face_level_new(jc3,jf3,level_face_new);
	  }
	}
       
      }

    } else if (level == level_face + 1) {

#ifdef DEBUG_ADAPT
      sprintf (buffer,"%s %s p_get_neighbor_level() face %2d %2d %2d "
	       "child %d %d %d  "
	       "%d -> %d",
	       "recv-coarse",bit_str.c_str(),					
	       if3[0],if3[1],if3[2],					
	       ic3[0],ic3[1],ic3[2],					
	       level_face,level_face_new);					
      index_.print(buffer,-1,2);						
#endif
  
      // ( ) RECV-COARSE: Face and level are received from unique
      // neighbor.  Possibly multiple faces of block are updated
      // corresponding to the coarse neighbor's face.  Levels of
      // possibly multiple faces of possibly multiple child faces are
      // updated.

      ItFace it_face (rank,rank_refresh,ic3,of3);

      int jf3[3];
      while (it_face.next(jf3)) {
	set_face_level_new (jf3, level_face_new);

	ItChild it_child (rank,jf3);

	while (it_child.next(ic3)) {

	  int kf3[3];

	  ItFace it_face (rank,rank_refresh,ic3,jf3);

	  while (it_face.next(kf3)) {
	    TRACE6("recv-fine: child %d %d %d  face %d %d %d",
		   ic3[0],ic3[1],ic3[2],kf3[0],kf3[1],kf3[2]);
	    set_child_face_level_new(ic3,kf3,level_face_new);
	  }
	}
      }

    } else  {

      WARNING2 ("CommBlock::notify_neighbor()",
		"level %d and face level %d differ by more than 1",
		level,level_face);
    }

    if (level < level_face_new - 1) {

	// cannot coarsen
	sync_coarsen_.set_index(-1);
      level_new_ = level_face_new - 1;
      TRACE_LEVEL_NEW("p_get_neighbor_level");

      notify_neighbors(level_new_);

    }

  } else { // not a leaf

    Index index_neighbor = neighbor_(of3);

    // Forward to children if internal node
    
    ItChild it_child (rank,if3);
    int jc3[3];
    while (it_child.next(jc3)) {
      Index index_nibling = index_neighbor.index_child(jc3);
      PUT_NEIGHBOR_LEVEL(index_nibling,ic3,if3,level_face,level_face_new,"RECURSE");
    }
  }

  debug_faces_("p_get_neighbor stop");
}

//----------------------------------------------------------------------

void CommBlock::p_child_can_coarsen(int ic3[3])
{
  TRACE_ADAPT("p_child_can_coarsen()");

  if (sync_coarsen_.next()) {
    // all children can coarsen, so initiate coarsening
    const int rank = simulation()->dimension();
    ItChild it_child (rank);
    int ic3[3];
    while (it_child.next(ic3)) {
      Index index_child = index_.index_child(ic3);
      thisProxy[index_child].p_request_data();
    }
  }
}

//----------------------------------------------------------------------

void CommBlock::p_child_cannot_coarsen()
{
  sync_coarsen_.add_index(-1);
}

//----------------------------------------------------------------------

void CommBlock::p_request_data()
{

  TRACE_ADAPT("ADAPT COARSEN p_request_data");

  const int level = this->level();

  Index index_parent = index_.index_parent();
  int ic3[3];
  index_parent.child(level,ic3,ic3+1,ic3+2);

  int narray; 
  char * array;
  int iface[3] = {0,0,0};
  bool lghost[3] = {true,true,true};
  FieldFace * field_face = 
    load_face_(&narray,&array, iface, ic3, lghost, op_array_restrict);

  int nf = face_level_.size();
  int face_level[nf];
  for (int i=0; i<nf; i++) face_level[i] = face_level_[i];

  thisProxy[index_parent].p_get_child_data
    (ic3, narray,array, nf,face_level);

  delete field_face;
  
}

//----------------------------------------------------------------------

void CommBlock::p_get_child_data
(
 int ic3[3],
 int na, char * array,
 int nf, int * child_face_level
 )
{
  TRACE_ADAPT("ADAPT COARSEN p_get_child_data");

  if (sync_coarsen_.next()) {
    const int rank = simulation()->dimension();
    ItChild it_child (rank);
    int ic3[3];
    while (it_child.next(ic3)) {
      Index index_child = index_.index_child(ic3);
      thisProxy[index_child].p_delete();
    }
    children_.clear();
  }
}

//----------------------------------------------------------------------

void CommBlock::p_delete()
{
  TRACE_ADAPT("ADAPT COARSEN p_delete");

  thisProxy[thisIndex].ckDestroy();
}

//----------------------------------------------------------------------

void CommBlock::x_refresh_child 
(
 int    n, 
 char * buffer, 
 int    ic3[3]
 )
{
  TRACE_ADAPT("ADAPT B4 x_refresh_child()");

  int  iface[3]  = {0,0,0};
  bool lghost[3] = {true,true,true};
  store_face_(n,buffer, iface, ic3, lghost, op_array_restrict);
}

//======================================================================

void CommBlock::initialize_child_face_levels_()
{
  TRACE_ADAPT("ADAPT A4 initialize_child_face_levels_()");
  const int  rank         = simulation()->dimension();
  const int  rank_refresh = simulation()->config()->field_refresh_rank;
  const int level = this->level();

  int ic3[3];
  ItChild it_child(simulation()->dimension());

  // For each child

  while (it_child.next(ic3)) {

    Index index_child = index_.index_child(ic3);
    int if3[3];
    ItFace it_face(rank,rank_refresh);

    // For each child face

    while (it_face.next(if3)) {

      int ip3[3];
      parent_face_(ip3,if3,ic3);
      Index in = neighbor_ (if3,&index_child);
      Index inp = in.index_parent();
      // Determine level for the child's face
      int level_child_face = (inp == thisIndex) ? 
	level + 1 : face_level(ip3);
      // Set the child's face to the level
      set_child_face_level(ic3,if3, level_child_face);
    }

    if3[0]=if3[1]=if3[2]=0;
    set_child_face_level(ic3,if3,level+1);

  }

  child_face_level_new_ = child_face_level_;

  debug_faces_("init_child ");
}

//----------------------------------------------------------------------

Index CommBlock::neighbor_ 
(
 const int of3[3],
 Index *   ind
 ) const
{
  Index index = (ind != 0) ? (*ind) : index_;

  int na3[3];
  size_forest (&na3[0],&na3[1],&na3[2]);
  const bool periodic  = simulation()->problem()->boundary()->is_periodic();
  Index in = index.index_neighbor
    (of3[0],of3[1],of3[2],na3,periodic);
  return in;
}

//----------------------------------------------------------------------

void CommBlock::parent_face_
(int       ip3[3],
 const int if3[3],
 const int ic3[3]) const
{
  ip3[0] = if3[0];
  ip3[1] = if3[1];
  ip3[2] = if3[2];
  if (if3[0] == +1 && ic3[0] == 0) ip3[0] = 0;
  if (if3[0] == -1 && ic3[0] == 1) ip3[0] = 0;
  if (if3[1] == +1 && ic3[1] == 0) ip3[1] = 0;
  if (if3[1] == -1 && ic3[1] == 1) ip3[1] = 0;
  if (if3[2] == +1 && ic3[2] == 0) ip3[2] = 0;
  if (if3[2] == -1 && ic3[2] == 1) ip3[2] = 0;
}

//----------------------------------------------------------------------

FieldFace * CommBlock::load_face_
(
 int *   n,
 char ** array, 
 int if3[3],
 int ic3[3],
 bool lg3[3],
 int op_array_type
 )
{
  FieldFace * field_face = create_face_ (if3,ic3,lg3, op_array_type);

  field_face->load(n, array);
  return field_face;
}

//----------------------------------------------------------------------

FieldFace * CommBlock::store_face_
(
 int narray, char * array, 
 int if3[3], int ic3[3], bool lg3[3],
 int op_array_type
 )
{
  FieldFace * field_face = create_face_ (if3,ic3,lg3, op_array_type);

  field_face->store(narray, array);
  delete field_face;
  return NULL;
}

//----------------------------------------------------------------------

FieldFace * CommBlock::create_face_
(int if3[3], int ic3[3], bool lg3[3],
 int op_array_type
 )
{
  Problem * problem        = simulation()->problem();
  FieldDescr * field_descr = simulation()->field_descr();
  FieldBlock * field_block = block_->field_block();

  FieldFace * field_face = new FieldFace (field_block,field_descr);

  if (op_array_type == op_array_restrict) {
    field_face->set_restrict(problem->restrict(),ic3[0],ic3[1],ic3[2]);
  } else if (op_array_type == op_array_prolong) {
    field_face->set_prolong(problem->prolong(),  ic3[0],ic3[1],ic3[2]);
  }

  field_face->set_face (if3[0],if3[1],if3[2]);
  field_face->set_ghost(lg3[0],lg3[1],lg3[2]);
  return field_face;
}

//----------------------------------------------------------------------

void CommBlock::debug_faces_(const char * mesg)
{
#ifndef DEBUG_ADAPT
  return;
#endif

  TRACE_ADAPT(mesg);
  int if3[3] = {0};
  int ic3[3] = {0};

  for (ic3[1]=1; ic3[1]>=0; ic3[1]--) {
    for (if3[1]=1; if3[1]>=-1; if3[1]--) {

      index_.print(mesg,-1,2,true);

      for (if3[0]=-1; if3[0]<=1; if3[0]++) {
	PARALLEL_PRINTF ((ic3[1]==1) ? "%d " : "  ",face_level(if3));
      }
      PARALLEL_PRINTF ("| ") ;
      for (if3[0]=-1; if3[0]<=1; if3[0]++) {
	PARALLEL_PRINTF ((ic3[1]==1) ? "%d " : "  ",face_level_new(if3));
      }
      PARALLEL_PRINTF ("| ");
      for (ic3[0]=0; ic3[0]<2; ic3[0]++) {
	for (if3[0]=-1; if3[0]<=1; if3[0]++) {
	  for (if3[0]=-1; if3[0]<=1; if3[0]++) {
	    PARALLEL_PRINTF ("%d ",child_face_level(ic3,if3));
	  }
	}
      }
      PARALLEL_PRINTF ("| ");
      for (ic3[0]=0; ic3[0]<2; ic3[0]++) {
	for (if3[0]=-1; if3[0]<=1; if3[0]++) {
	  for (if3[0]=-1; if3[0]<=1; if3[0]++) {
	    PARALLEL_PRINTF ("%d ",child_face_level_new(ic3,if3));
	  }
	}
      }
      PARALLEL_PRINTF ("\n");
    }
  }
}
#endif /* #ifdef TEMP_NEW_ADAPT */

