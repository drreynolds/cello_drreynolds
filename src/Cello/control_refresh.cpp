// See LICENSE_CELLO file for license and copyright information

/// @file     control_refresh.cpp
/// @author   James Bordner (jobordner@ucsd.edu)
/// @date     2013-04-26
/// @brief    Charm-related functions associated with refreshing ghost zones
/// @ingroup  Control

#include "simulation.hpp"
#include "mesh.hpp"
#include "control.hpp"

#include "charm_simulation.hpp"
#include "charm_mesh.hpp"

//----------------------------------------------------------------------

#ifdef TEMP_NEW_REFRESH
void Block::refresh_begin_(Refresh * refresh) 
#else
void Block::refresh_begin_() 
#endif
{
#ifdef TEMP_NEW_REFRESH
  /* */
#else
  Refresh * refresh = simulation()->problem()->refresh(index_refresh_);
#endif

  if ((  is_leaf() && children_.size() != 0) ||
      (! is_leaf() && children_.size() == 0)) {

    WARNING4("Block::refresh_begin_()",
             "%s: is_leaf() == %s && children_.size() == %d"
             "setting is_leaf_ <== %s",
             name_.c_str(), is_leaf()?"true":"false",
             children_.size(),is_leaf()?"false":"true");
    is_leaf_ = ! is_leaf();
  }

  // Check if Block should have been deleted
  if (delete_) {
    WARNING1("refresh_begin_()",
	     "%s: refresh called on deleted Block",
	     name_.c_str());
    return;
  }

#ifdef TEMP_NEW_REFRESH
  refresh->sync().reset();
#endif

  simulation()->set_phase(phase_refresh);

  // Refresh if Refresh object exists and have data

  int if3[3];
  int ic3[3];
  if (refresh && is_leaf()) {

#ifdef TEMP_NEW_REFRESH
    const int min_face_rank = refresh->min_face_rank();
#else
    const int min_face_rank = 
      simulation()->config()->refresh_min_face_rank[index_refresh_];
#endif

    const int level = this->level();

    int count = 0;
    ItNeighbor it_neighbor = this->it_neighbor(min_face_rank,index_);
    int if3[3];
    int ic3[3];
    while (it_neighbor.next()) {
      Index index_neighbor = it_neighbor.index();
      it_neighbor.face(if3);
      it_neighbor.child(ic3);
      int level_face = it_neighbor.face_level();
      ++count;

      if (level_face == level) {
	refresh_load_face_ (refresh_same,index_neighbor,if3,ic3);
      } else if (level_face == level + 1) {
	refresh_load_face_ (refresh_fine,index_neighbor,if3,ic3);
      } else if (level_face == level - 1) {
	refresh_load_face_ (refresh_coarse,index_neighbor,if3,ic3);
      }

    }

#ifdef TEMP_NEW_REFRESH
    // call with self to set counter
    refresh_load_face_(refresh_same,index(),if3,ic3,count + 1);
#endif

  } else {
    refresh_load_face_(refresh_same,index(),if3,ic3,1);
  }

#ifndef TEMP_NEW_REFRESH
  control_sync (CkIndex_Block::p_refresh_exit(),"neighbor",2);
#endif

}

//----------------------------------------------------------------------

void Block::refresh_load_face_
( int type_refresh,
  Index index_neighbor,
  int iface[3],
  int ichild[3],
  int count)
{
#ifdef TEMP_NEW_REFRESH
  if (count != 0) {

     refresh_.sync().set_stop(count);

  } else {
#endif

    FieldFace * field_face;

    int type_op_array;

    int level = index_.level();
    switch (type_refresh) {
    case refresh_coarse:

      index_.child(level,ichild,ichild+1,ichild+2);

      type_op_array = op_array_restrict;

      break;

    case refresh_same:

      type_op_array = op_array_copy;

      break;

    case refresh_fine:

      type_op_array = op_array_prolong;

      break;

    default:
      type_op_array = op_array_unknown;
      ERROR1("Block::refresh_face()",
	     "Unknown type_refresh %d",
	     type_refresh);

    }

    int n; 
    char * array;
    bool lghost[3] = {false,false,false};

    std::vector<int> field_list = refresh()->field_list();

    field_face = load_face (&n, &array,
			    iface, ichild, lghost,
			    type_op_array,
			    field_list);

    int jface[3] = {-iface[0], -iface[1], -iface[2]};

    thisProxy[index_neighbor].x_refresh_send_face
      (n,array, type_refresh, jface, ichild);

    delete field_face;
#ifdef TEMP_NEW_REFRESH
  }
  if (refresh_.sync().next()) {
    control_sync (CkIndex_Block::p_refresh_exit(),"neighbor",2);
  }
#endif
}

//----------------------------------------------------------------------

void Block::refresh_store_face_
(int n, char * buffer, int type_refresh,
 int iface[3], int ichild[3],int count
)
{
  if (count==0) {
  bool lghost[3] = {false,false,false};

  std::vector<int> field_list = refresh()->field_list();

  int op_array;
  switch (type_refresh) {
 case refresh_coarse:  op_array = op_array_restrict;  break;
 case refresh_same:    op_array = op_array_copy;      break;
 case refresh_fine:    op_array = op_array_prolong;   break;
 default:
   op_array = op_array_unknown;
   ERROR1("Block::refresh_store_face_()",
    "Unknown type_refresh %d",  type_refresh);
   break;
}

  store_face_(n,buffer,
    iface, ichild, lghost,
    op_array,
    field_list);
}
}

//----------------------------------------------------------------------
