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
   int ibx, int iby, int ibz,
   int nbx, int nby, int nbz,
   int nx, int ny, int nz,
   double xmp, double ymp, double zmp,
   double xb, double yb, double zb,
   int num_field_blocks,
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

  PUParray(p,index_,3);
  PUParray(p,size_,3);
  p | cycle_;
  p | time_;
  p | dt_;

}

#endif

//----------------------------------------------------------------------

#ifdef CONFIG_USE_CHARM

  /// Initialize a migrated CommBlock
  /// CB
  CommBlock (CkMigrateMessage *m) 
    : CBase_CommBlock(m) { };

#endif

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
  void p_read (int index_input = 0);

  /// Entry function after prepare() to call Simulation::p_output()
  /// CB
  void p_output(CkReductionMsg * msg);

  /// Initiate mesh adaptation on given level 
  /// CB
  void p_adapt (int level);

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

  /// Return the current cycle number
  int cycle() const throw() { return cycle_; };

  /// Return the current time
  double time() const throw() { return time_; };

  /// Return the current timestep
  double dt() const throw() { return dt_; };
 
  /// Return which block faces lie along a domain boundary
  void is_on_boundary (bool boundary[3][2]) throw();

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
    DEBUG ("DEBUG CommBlock::initialize()\n");
  }

protected: // functions

  int id_ () const throw ()
  { return index_[0] + size_[0] * (index_[1] + size_[1] * index_[2]); }


  void initialize_
  ( int ibx, int iby, int ibz,
    int nbx, int nby, int nbz,
    int nx, int ny, int nz,
    double xpm, double ypm, double zpm, // Domain begin
    double xb, double yb, double zb,    // CommBlock width
    bool testing);

  /// Allocate and copy in attributes from give CommBlock
  void copy_(const CommBlock & block) throw();

#ifdef CONFIG_USE_CHARM

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

  /// Mesh Block that this CommBlock controls
  /// CB
  Block * block_;

#ifdef CONFIG_USE_CHARM

  /// Counter when refreshing faces
  /// CB: use Sync

  Sync sync_refresh_;

#endif

  //--------------------------------------------------

  /// Index into array [redundant with CHARM thisIndex.x .y .z]
  /// CB
  int index_[3];

  /// Size of array [redundant with CHARM thisIndex.x .y .z]
  /// CB
  int size_[3];

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

  /// Whether face is on the domain boundary
};

#endif /* COMM_COMMBLOCK_HPP */

