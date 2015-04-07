// See LICENSE_CELLO file for license and copyright information

/// @file     control_charm.cpp
/// @author   James Bordner (jobordner@ucsd.edu)
/// @date     2014-02-13
/// @brief    Functions controling control flow of charm entry functions
/// @ingroup  Control

#include "simulation.hpp"
#include "mesh.hpp"
#include "control.hpp"

#include "charm_simulation.hpp"
#include "charm_mesh.hpp"

#ifdef CELLO_VERBOSE
#   define VERBOSE(A)						\
  fprintf (simulation()->fp_debug(),"[%s] %d TRACE %s\n",	\
	   name_.c_str(),__LINE__,A);				\
  if (index_.is_root()) {					\
    Monitor * monitor = simulation()->monitor();		\
    monitor->print("Control", A);				\
  } 
#else
#   define VERBOSE(A) ;
#endif

//--------------------------------------------------

const int cycle_phase[] = {
  phase_adapt_enter,
  phase_adapt_exit,
  phase_output_enter,
  phase_output_exit,
  phase_stopping_enter,
  phase_stopping_exit,
  phase_compute_enter,
  phase_compute_exit
};

const char * cycle_sync[] = {
  "none",      // phase_adapt_enter
  "quiescence", // phase_adapt_exit
  "none",      // phase_output_enter
  "none",       // phase_output_exit
  "contribute", // phase_stopping_enter
  "none",       // phase_stopping_exit
  "none",       // phase_compute_enter
  "none"        // phase_compute_exit
};

#define CYCLE_PHASE_COUNT (sizeof(cycle_phase) / sizeof(cycle_phase[0]))

//----------------------------------------------------------------------

void Block::initial_exit_()
{  control_sync(phase_adapt_enter,"contribute"); }

//----------------------------------------------------------------------

void Block::adapt_enter_()
{
  VERBOSE("adapt_enter");

  performance_switch_ (perf_adapt,__FILE__,__LINE__);

  if ( do_adapt_()) {

    VERBOSE("adapt_enter");

    adapt_begin_();
    
  } else {

    control_sync(phase_output_enter,"none");

  }
}

//----------------------------------------------------------------------

void Block::adapt_exit_()
{
  VERBOSE("adapt_exit");

  control_sync(phase_output_enter,"quiescence");
}

//----------------------------------------------------------------------

void Block::refresh_enter_() 
{
  VERBOSE("refresh_enter");

  performance_switch_(perf_refresh,__FILE__,__LINE__);

  refresh_begin_();
}

//----------------------------------------------------------------------

void Block::refresh_exit_()
{
  VERBOSE("refresh_exit");

  update_boundary_();

  control_sync(refresh_phase_, refresh_sync_);
}

//----------------------------------------------------------------------

void Block::compute_enter_ ()
{
  VERBOSE("compute_enter");

  performance_switch_(perf_compute,__FILE__,__LINE__);

  compute_begin_();
}

//----------------------------------------------------------------------

void Block::compute_exit_ ()
{
  VERBOSE("compute_exit");

  control_sync(phase_adapt_enter,"none");
}

//----------------------------------------------------------------------

void Block::output_enter_ ()
{
  VERBOSE("output_enter");

  performance_switch_ (perf_output,__FILE__,__LINE__);

  output_begin_();
}

//----------------------------------------------------------------------

void Block::output_exit_()
{

  VERBOSE("output_exit");

  
  if (index_.is_root()) {

    proxy_simulation[0].p_monitor();
  }

  control_sync(phase_stopping_enter,"contribute");
}

//----------------------------------------------------------------------

void Block::stopping_enter_()
{

  VERBOSE("stopping_enter");

  performance_switch_(perf_stopping,__FILE__,__LINE__);

  stopping_begin_();

}

//----------------------------------------------------------------------

void Block::stopping_exit_()
{

  VERBOSE("stopping_exit");

  if (stop_) {

    control_sync(phase_exit,"contribute");

  } else {

    control_sync(phase_compute_enter,"none");

  }

}

//======================================================================

#ifdef NEW_CONTROL

void Block::control_sync_new(CkCallback cb, int phase, std::string sync, const char * file, int line)
{
  //  printf ("DEBUG %s:%d NEW Block::control_sync_new()\n",__FILE__,__LINE__);

  if (sync == "contribute") {

    contribute(cb);

  } else if (sync == "quiescence") {

    if (index_.is_root()) {
      CkStartQD(cb);
    }

  } else if (sync == "neighbor") {

    control_sync_neighbor_(phase);

  } else if (sync == "array") {

    if (index().is_root()) cb.send(NULL);

  } else if (sync == "none") {

    cb.send(NULL);

  } else {
    ERROR2 ("Block::control_sync()",  
	    "Unknown sync type: phase %s sync type %s", 
	    phase_name[phase],sync.c_str());    
  }
}

#endif /* NEW_CONTROL */

//----------------------------------------------------------------------

void Block::control_sync(int phase, std::string sync)
{
  if (sync == "contribute") {

    CkCallback cb;

    if (                     phase == phase_initial_exit) {
      cb = CkCallback (CkIndex_Block::r_initial_exit(NULL), thisProxy);
    } else if (              phase == phase_adapt_enter) {
      cb = CkCallback (CkIndex_Block::r_adapt_enter(NULL), thisProxy);
    } else if (              phase == phase_adapt_next) {
      cb = CkCallback (CkIndex_Block::r_adapt_next(NULL), thisProxy);
    } else if (              phase == phase_adapt_called) {
      cb = CkCallback (CkIndex_Block::r_adapt_called(NULL), thisProxy);
    } else if (              phase == phase_adapt_end) {
      cb = CkCallback (CkIndex_Block::r_adapt_end(NULL), thisProxy);
    } else if (              phase == phase_adapt_exit) {
      cb = CkCallback (CkIndex_Block::r_adapt_exit(NULL), thisProxy);
    } else if (              phase == phase_refresh_enter) {
      cb = CkCallback (CkIndex_Block::r_refresh_enter(NULL), thisProxy);
    } else if (              phase == phase_refresh_exit) {
      cb = CkCallback (CkIndex_Block::r_refresh_exit(NULL), thisProxy);
    } else if (              phase == phase_output_enter) {
      cb = CkCallback (CkIndex_Block::r_output_enter(NULL), thisProxy);
    } else if (              phase == phase_output_exit) {
      cb = CkCallback (CkIndex_Block::r_output_exit(NULL), thisProxy);
    } else if (              phase == phase_compute_enter) {
      cb = CkCallback (CkIndex_Block::r_compute_enter(NULL), thisProxy);
    } else if (              phase == phase_compute_continue) {
      cb = CkCallback (CkIndex_Block::r_compute_continue(NULL), thisProxy);
    } else if (              phase == phase_compute_exit) {
      cb = CkCallback (CkIndex_Block::r_compute_exit(NULL), thisProxy);
    } else if (              phase == phase_stopping_enter) {
      cb = CkCallback (CkIndex_Block::r_stopping_enter(NULL), thisProxy);
    } else if (              phase == phase_stopping_exit) {
      cb = CkCallback (CkIndex_Block::r_stopping_exit(NULL), thisProxy);
    } else if (              phase == phase_exit) {
      cb = CkCallback (CkIndex_Block::r_exit(NULL), thisProxy);
    } else {
      ERROR2 ("Block::control_sync()",  
	      "Unknown phase: phase %s sync type %s", 
	      phase_name[phase],sync.c_str());    
    }

    contribute(cb);

  } else if (sync == "quiescence") {

    // Quiescence detection through the Main chare

    if (index_.is_root()) {
      if (                    phase == phase_initial_exit) {
	CkStartQD(CkCallback(CkIndex_Main::p_initial_exit(), proxy_main));
      } else if (             phase == phase_adapt_enter) {
	CkStartQD(CkCallback(CkIndex_Main::p_adapt_enter(), proxy_main));
      } else if (             phase == phase_adapt_next) {
	CkStartQD(CkCallback(CkIndex_Main::p_adapt_next(), proxy_main));
      } else if (             phase == phase_adapt_called) {
	CkStartQD(CkCallback(CkIndex_Main::p_adapt_called(), proxy_main));
      } else if (             phase == phase_adapt_end) {
	CkStartQD(CkCallback(CkIndex_Main::p_adapt_end(), proxy_main));
      } else if (             phase == phase_adapt_exit) {
	CkStartQD(CkCallback(CkIndex_Main::p_adapt_exit(), proxy_main));
      } else if (             phase == phase_refresh_enter) {
      	CkStartQD(CkCallback(CkIndex_Main::p_refresh_enter(), proxy_main));
      } else if (             phase == phase_refresh_exit) {
      	CkStartQD(CkCallback(CkIndex_Main::p_refresh_exit(), proxy_main));
      } else if (             phase == phase_output_enter) {
	CkStartQD(CkCallback(CkIndex_Main::p_output_enter(), proxy_main));
      } else if (             phase == phase_output_exit) {
	CkStartQD(CkCallback(CkIndex_Main::p_output_exit(), proxy_main));
      } else if (             phase == phase_compute_enter) {
	CkStartQD(CkCallback(CkIndex_Main::p_compute_enter(), proxy_main));
      } else if (             phase == phase_compute_continue) {
	CkStartQD(CkCallback(CkIndex_Main::p_compute_continue(), proxy_main));
      } else if (             phase == phase_compute_exit) {
	CkStartQD(CkCallback(CkIndex_Main::p_compute_exit(), proxy_main));
      } else if (             phase == phase_stopping_enter) {
	CkStartQD(CkCallback(CkIndex_Main::p_stopping_enter(), proxy_main));
      } else if (             phase == phase_stopping_exit) {
	CkStartQD(CkCallback(CkIndex_Main::p_stopping_exit(), proxy_main));
      } else if (             phase == phase_exit) {
	CkStartQD(CkCallback(CkIndex_Main::p_exit(), proxy_main));
      } else {
	ERROR2 ("Block::control_sync()",  
		"Unknown phase: phase %s sync type %s", 
		phase_name[phase],sync.c_str());    
      }

    }

  } else if (sync == "neighbor") {

    control_sync_neighbor_(phase);

  } else if (sync == "array") {

    if (index().is_root()) {
      if (phase == phase_initial_exit)          thisProxy.p_initial_exit();
      else if (phase == phase_adapt_enter)      thisProxy.p_adapt_enter();
      else if (phase == phase_adapt_next) 	thisProxy.p_adapt_next();
      else if (phase == phase_adapt_called) 	thisProxy.p_adapt_called();
      else if (phase == phase_adapt_end) 	thisProxy.p_adapt_end();
      else if (phase == phase_adapt_exit) 	thisProxy.p_adapt_exit();
      else if (phase == phase_refresh_enter) 	thisProxy.p_refresh_enter();
      else if (phase == phase_refresh_exit) 	thisProxy.p_refresh_exit();
      else if (phase == phase_output_enter) 	thisProxy.p_output_enter();
      else if (phase == phase_output_exit) 	thisProxy.p_output_exit();
      else if (phase == phase_compute_enter) 	thisProxy.p_compute_enter();
      else if (phase == phase_compute_continue) thisProxy.p_compute_continue();
      else if (phase == phase_compute_exit) 	thisProxy.p_compute_exit();
      else if (phase == phase_stopping_enter) 	thisProxy.p_stopping_enter();
      else if (phase == phase_stopping_exit) 	thisProxy.p_stopping_exit();
      else if (phase == phase_exit) 	        thisProxy.p_exit();
      else 
	ERROR2 ("Block::control_sync()",  
		"Unknown phase: phase %s sync type %s", 
		phase_name[phase],sync.c_str());    

    }

  } else if (sync == "none") {

    control_call_phase_(phase);

  } else {
    ERROR2 ("Block::control_sync()",  
	    "Unknown sync type: phase %s sync type %s", 
	    phase_name[phase],sync.c_str());    
  }
}

//----------------------------------------------------------------------

void Block::control_sync_count_ (int phase, int count)
{
  if (count != 0)  max_sync_[phase] = count;

  ++count_sync_[phase];
#ifdef CELLO_DEBUG
  fprintf (simulation()->fp_debug(),"%d %s counting %d/%d leaf %d %s\n",__LINE__,name_.c_str(),
	   count_sync_[phase],max_sync_[phase],is_leaf(),phase_name[phase]);
#endif

  if (0 < max_sync_[phase] && max_sync_[phase] <= count_sync_[phase]) {
    max_sync_[phase] = 0;
    count_sync_[phase] = 0;
#ifdef CELLO_DEBUG
    fprintf (simulation()->fp_debug(),"%d %s called %s\n",__LINE__,name_.c_str(),phase_name[phase]);
#endif
    control_call_phase_(phase);
  }
}

//----------------------------------------------------------------------

void Block::control_sync_neighbor_(int phase)
{
  if (!is_leaf()) {

#ifdef CELLO_DEBUG
    fprintf (simulation()->fp_debug(),"%d %s called %s\n",__LINE__,name_.c_str(),phase_name[phase]);
#endif
    control_call_phase_ (phase);

  } else {

    const int level        = this->level();
    const int rank = this->rank();

    const int min_face_rank = 0;
    ItFace it_face = this->it_face(min_face_rank,index_);

    int of3[3];

    int num_neighbors = 0;

    while (it_face.next(of3)) {

      int ic3[3] = {0,0,0};

      Index index_neighbor = neighbor_(of3);

      const int level_face = face_level (of3);

      if (level_face == level) {

	// SEND-SAME: Face and level are sent to unique
	// neighboring block in the same level

#ifdef CELLO_DEBUG
	fprintf (simulation()->fp_debug(),
		 "%d %s calling p_control_sync phase %s leaf %d block %s\n",
		 __LINE__,name_.c_str(), phase_name[phase], is_leaf(),
		 index_neighbor.bit_string(-1,2).c_str());
#endif
	thisProxy[index_neighbor].p_control_sync_count(phase,0);

	++num_neighbors;

      } else if (level_face == level - 1) {

	// SEND-COARSE: Face, level, and child indices are sent to
	// unique neighboring block in the next-coarser level

	index_.child (level,&ic3[0],&ic3[1],&ic3[2]);

	int op3[3];
	parent_face_(op3,of3,ic3);

	// avoid redundant calls
	if (op3[0]==of3[0] && 
	    op3[1]==of3[1] && 
	    op3[2]==of3[2]) {

	  Index index_uncle = index_neighbor.index_parent();

#ifdef CELLO_DEBUG
	  fprintf (simulation()->fp_debug(),
		   "%d %s calling p_control_sync phase %s leaf %d block %s\n",
		   __LINE__,name_.c_str(), phase_name[phase], is_leaf(),
		   index_uncle.bit_string(-1,2).c_str());
#endif
	  thisProxy[index_uncle].p_control_sync_count(phase,0);

	  ++num_neighbors;

	}

      } else if (level_face == level + 1) {

	// SEND-FINE: Face and level are sent to all nibling
	// blocks in the next-finer level along the face.

	const int if3[3] = {-of3[0],-of3[1],-of3[2]};
	ItChild it_child(rank,if3);
	while (it_child.next(ic3)) {
	  Index index_nibling = index_neighbor.index_child(ic3);

#ifdef CELLO_DEBUG
	  fprintf (simulation()->fp_debug(),
		   "%d %s calling p_control_sync phase %s leaf %d block %s\n",
		   __LINE__,name_.c_str(), phase_name[phase], is_leaf(),
		   index_nibling.bit_string(-1,2).c_str());
#endif
	  thisProxy[index_nibling].p_control_sync_count(phase,0);

	  ++num_neighbors;
	}

      } else {
	std::string bit_str = index_.bit_string(-1,2);
	WARNING7 ("Block::control_sync_neighbor_()",
		  "phase %d name %s level %d and face (%d %d %d) "
		  "level %d differ by more than 1",
		  phase, name().c_str(), level, of3[0],of3[1],of3[2], level_face);
      }

    }
    control_sync_count_(phase,num_neighbors + 1);
  }
}

//----------------------------------------------------------------------

void Block::control_call_phase_ (int phase)
{
#ifdef CELLO_DEBUG
  fprintf (simulation()->fp_debug(),"%d %s called %s\n",
	   __LINE__,name_.c_str(),phase_name[phase]);
#endif
  if      (phase == phase_initial_exit)     initial_exit_();
  else if (phase == phase_adapt_called)     adapt_called_();
  else if (phase == phase_adapt_enter)      adapt_enter_();      
  else if (phase == phase_adapt_next)       adapt_next_();
  else if (phase == phase_adapt_end)        adapt_end_();
  else if (phase == phase_adapt_exit)       adapt_exit_();
  else if (phase == phase_refresh_enter)    refresh_enter_();
  else if (phase == phase_refresh_exit)     refresh_exit_();
  else if (phase == phase_output_enter)     output_enter_();
  else if (phase == phase_output_exit)      output_exit_();
  else if (phase == phase_compute_enter)    compute_enter_();
  else if (phase == phase_compute_continue) compute_continue_();
  else if (phase == phase_compute_exit)     compute_exit_();
  else if (phase == phase_stopping_enter)   stopping_enter_();
  else if (phase == phase_stopping_exit)    stopping_exit_();
  else if (phase == phase_exit)             exit_();
  else 
    ERROR1 ("Block::control_call_phase_()","Unknown phase: phase %s", 
	    phase_name[phase]);    
}
//======================================================================

