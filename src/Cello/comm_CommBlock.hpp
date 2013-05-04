// See LICENSE_CELLO file for license and copyright information

/// @file     comm_CommBlock.hpp
/// @author   James Bordner (jobordner@ucsd.edu)
/// @date     Fri Apr  2 14:09:42 PDT 2010
/// @brief    [\ref Comm] Declaration of the CommBlock class

#ifndef COMM_COMMBLOCK_HPP
#define COMM_COMMBLOCK_HPP

class Block;
class Factory;
class GroupProcess;
class FieldDescr;
class Hierarchy;
class Simulation;

#ifdef CONFIG_USE_CHARM
#include "mesh.decl.h"
class CommBlock : public CBase_CommBlock
#else
class CommBlock
#endif
{
  /// @class    CommBlock
  /// @ingroup  Comm
  /// @brief    [\ref Comm] Handles parallel communication and synchronization of mesh Blocks

  friend class IoBlock;

public: // interface

  /// create a CommBlock with the given block count, lower extent, block
  /// size, and number of field blocks
  CommBlock
  (
#ifndef CONFIG_USE_CHARM
   Simulation * simulation,
#endif
   Index index,
   int nx, int ny, int nz,
   int level,
   int num_field_blocks,
   int count_adapt,
   bool testing=false
) throw();

  /// Initialize an empty CommBlock
  CommBlock()  { };

#ifdef CONFIG_USE_CHARM

  /// CB
  void pup(PUP::er &p)
{
  TRACEPUP;

  CBase_CommBlock::pup(p);

  p | sync_refresh_;

  p | index_;
  p | cycle_;
  p | time_;
  p | dt_;
  p | index_initial_;
  p | level_;
  PUParray(p,depth_,8);
  p | count_adapt_;

}

#endif

//----------------------------------------------------------------------

#ifdef CONFIG_USE_CHARM

  /// Initialize a migrated CommBlock
  /// CB
  CommBlock (CkMigrateMessage *m) 
    : CBase_CommBlock(m) { };

#endif


  /// Index of the CommBlock
  Index index() const { return index_; }

#ifdef CONFIG_USE_CHARM

  /// Initialize CommBlock for the simulation.
  /// CB
  void p_initial();

  // /// Call current Initial::enforce() on the block
  // void p_initial_enforce();

  /// Refresh ghost zones and apply boundary conditions
  /// CB
  void p_refresh();

  /// Apply the numerical methods on the block
  /// CB
  void p_compute(int cycle, double time, double dt);

  /// Refresh a FieldFace
  /// CB
  void x_refresh(int n, char buffer[],int fx, int fy, int fz);

  /// Contribute block data to ith output object in the simulation
  /// CB
  void p_write (int index_output);

  /// Contribute block data to the Initial input object
  /// CB
  void p_read (int index_input = 0)
  {  INCOMPLETE("CommBlock::p_read");  }


  /// Entry function after prepare() to call Simulation::p_output()
  /// CB
  void p_output(CkReductionMsg * msg);

  /// Initiate mesh adaptation on given level 
  /// CB

  /// Begin the adaptation phase
  void p_adapt_enter();
  void p_adapt (int count_adapt);
  void q_adapt ();
  void q_adapt_exit ();
  void adapt();
  void refine();
  void coarsen();
  void p_balance();
  int determine_refine();
  /// Update the depth of the given child
  void p_update_depth (int ic, int depth);
  void p_print(std::string message) {  
    index().print(message.c_str());
    TRACE2("%s level %d",message.c_str(),level_);
    TRACE9("%s depth %d%d%d%d%d%d%d%d",message.c_str(),
	   depth_[0],depth_[1],depth_[2],depth_[3],
	   depth_[4],depth_[5],depth_[6],depth_[7]);
  }



  //--------------------------------------------------

  /// Output, Monitor, Stopping [reduction], and Timestep [reduction]
  /// B
  void prepare();

  /// Implementation of refresh
  /// B / CB
  void refresh();

  /// Boundary and Method
  /// B
  void compute();

  //==================================================

#else /* ! CONFIG_USE_CHARM */

  /// Refresh ghost data
  void refresh_ghosts(const FieldDescr * field_descr,
		      const Hierarchy * hierarchy,
		      int fx, int fy, int fz,
		      int index_field_set = 0) throw();
#endif

  //----------------------------------------------------------------------
  // Big Three
  //----------------------------------------------------------------------

  /// Destructor
  virtual ~CommBlock() throw();

  /// Copy constructor
  CommBlock(const CommBlock & block) throw();

  /// Assignment operator
  CommBlock & operator= (const CommBlock & block) throw();

  /// Return the Block associated with this CommBlock
  /// CB
  Block * block() throw() { return block_; };
  const Block * block() const throw() { return block_; };

//----------------------------------------------------------------------

  /// B | CB
  void index_forest (int * ibx = 0, int * iby = 0, int * ibz = 0) const throw();

  /// Return the index of this CommBlock in the array
  /// CB / remove

  /// Return the name of the block
  std::string name () const throw()
  {
    std::stringstream convert;
    convert << "block_" << id_();
    return convert.str();
  }

  /// Return the size the CommBlock array
  void size_forest (int * nx, int * ny, int * nz) const throw();

  /// Compute the lower extent of the CommBlock in the domain
  void lower(double * xm, double * ym = 0, double * zm = 0) const throw ();

  /// Compute the upper extent of the CommBlock in the domain
  void upper(double * xp, double * yp = 0, double * zp = 0) const throw ();

  void index_global
  ( int *ix, int *iy, int *iz,  
    int *nx, int *ny, int *nz ) const;

  /// Return the current cycle number
  int cycle() const throw() { return cycle_; };

  /// Return the current time
  double time() const throw() { return time_; };

  /// Return the level in the Hierarchy
  int level() const throw() { return level_; };


  /// Return the current timestep
  double dt() const throw() { return dt_; };
 
  /// Return which block faces lie along a domain boundary
  void is_on_boundary (bool boundary[3][2]) throw();

  /// Return whether this CommBlock is a leaf in the octree forest
  bool is_leaf() const;

public: // virtual functions

  /// Set CommBlock's cycle
  virtual void set_cycle (int cycle) throw()
  { cycle_ = cycle;}

  /// Set CommBlock's time
  virtual void set_time (double time) throw()
  { time_  = time; }

  /// Set CommBlock's timestep
  virtual void set_dt (double dt) throw()
  { dt_  = dt; }

  /// Initialize CommBlock
  virtual void initialize () throw()
  {
    TRACE("CommBlock::initialize()\n");
  }

  /// Return the local simulation object
  Simulation * simulation() const;

protected: // functions

  std::string id_ () const throw ()
  {
    char buffer[27];
    sprintf (buffer,"%08X-%08X-%08X",
	     index_.value(0),
	     index_.value(1),
	     index_.value(2));
    return buffer;
  }

  void initialize_  ( int nx, int ny, int nz,    bool testing);

  /// Allocate and copy in attributes from give CommBlock
  void copy_(const CommBlock & block) throw();

  /// Return adapt_coarsen, adapt_refine, or adapt_same given two adapt results
  int update_adapt_ (int a1, int a2) const throw();

  /// Return the (lower) indices of the CommBlock in the level, 
  /// and the number of indices

#ifdef CONFIG_USE_CHARM

  /// Apply all initial conditions to this Block
  void apply_initial_() throw();

  /// Determine which faces require boundary updates or communication
  void determine_boundary_
  (
   bool is_boundary[3][2],
   bool * axm,
   bool * axp,
   bool * aym,
   bool * ayp,
   bool * azm,
   bool * azp
   );

  /// Update boundary conditions
  void update_boundary_ ();

  /// Count number of expected faces to be communicated
  int count_refresh_ ();

#endif

protected: // attributes

#ifndef CONFIG_USE_CHARM
  /// Pointer to parent simulation object (MPI only)
  Simulation * simulation_;
#endif

  ///
  /// Mesh Block that this CommBlock controls
  Block * block_;

#ifdef CONFIG_USE_CHARM

  /// Counter when refreshing faces
  /// CB: use Sync

  Sync sync_refresh_;

#endif

  //--------------------------------------------------

  Index index_;

  //--------------------------------------------------

  /// Current cycle number
  /// B / CB
  int cycle_;

  /// Current time
  /// B
  double time_;

  /// Current timestep
  /// B
  double dt_;

  //--------------------------------------------------
  
  /// Index of current initialization routine
  int index_initial_;

  /// MESH REFINEMENT

  /// Mesh refinement level
  int level_;

  /// Depth of each child, starting with 0 if no child
  int depth_[8];

  /// Counter for adaption phase.  (Usually 1 except possibly
  /// initialization)
  int count_adapt_;
};

#endif /* COMM_COMMBLOCK_HPP */

