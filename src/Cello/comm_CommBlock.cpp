// See LICENSE_CELLO file for license and copyright information

/// @file     comm_CommBlock.cpp
/// @author   James Bordner (jobordner@ucsd.edu)
/// @date     Mon Feb 28 13:22:26 PST 2011
/// @brief    Implementation of the CommBlock object
/// @todo     Remove hierarchy dependency via Initial--only need domain bounds

#include "cello.hpp"

#include "mesh.hpp"
#include "main.hpp"
#include "charm_simulation.hpp"

#include "enzo.hpp" /* temp */

//----------------------------------------------------------------------

CommBlock::CommBlock
(
 Index index,
 int nbx, int nby, int nbz,          // Forest size
 int nx, int ny, int nz,             // Block cells
 int level,                          // Block level
 double xpm, double ypm, double zpm, // Forest begin
 double xb, double yb, double zb,    // Block size
 int num_field_blocks,
 bool testing
) throw ()
  : index_(index),
    cycle_(0),
    time_(0),
    dt_(0),
    index_initial_(0),
    level_(level),
    level_active_(-1)
{ 

  TRACE("ENTER CommBlock::CommBlock()");

#ifdef CONFIG_USE_CHARM
  index.print("create");
#endif

  TRACE3("CommBlock::CommBlock nb (%d %d %d)",nbx,nby,nbz);
  TRACE3("CommBlock::CommBlock  n (%d %d %d)",nx,ny,nz);
  TRACE1("CommBlock::CommBlock  l %d",level);
  TRACE3("CommBlock::CommBlock xm(%f %f %f)",xpm,ypm,zpm);
  TRACE3("CommBlock::CommBlock  b(%f %f %f)",xb,yb,zb);

  int ibx,iby,ibz;
  index.array(&ibx,&iby,&ibz);

  block_ = new Block  (nx, ny, nz, num_field_blocks,
		       xpm+ibx*xb, xpm+(ibx+1)*xb,
		       ypm+iby*yb, ypm+(iby+1)*yb,
		       zpm+ibz*zb, zpm+(ibz+1)*zb);

#ifdef CONFIG_USE_CHARM
  SimulationCharm * simulation  = proxy_simulation.ckLocalBranch();
  FieldDescr * field_descr = simulation->field_descr();

  // Set the CommBlock cycle and time to match Simulation's

  TRACE("CommBlock::p_initial Setting time");
  set_cycle(simulation->cycle());
  set_time (simulation->time());
  set_dt   (simulation->dt());
  // Allocate block data

  block()->allocate(field_descr);

#endif
  // Perform any additional initialization for derived class 

  initialize ();

  initialize_(ibx,iby,ibz, nbx,nby,nbz, nx,ny,nz, testing);

#ifdef CONFIG_USE_CHARM
  sync_refresh_.stop() = count_refresh_();
  apply_initial_();
#endif /* CONFIG_USE_CHARM */

  //

}

//----------------------------------------------------------------------

void CommBlock::initialize_
(
 int ibx, int iby, int ibz,
 int nbx, int nby, int nbz,
 int nx, int ny, int nz,
 bool testing
 )
 {
   size_[0] = nbx;
   size_[1] = nby;
   size_[2] = nbz;

#ifdef CONFIG_USE_CHARM

   if (! testing) {
     // Count CommBlocks on each processor
   
     SimulationCharm * simulation_charm  = 
       dynamic_cast<SimulationCharm *> (proxy_simulation.ckLocalBranch());

     TRACE1 ("simulation_charm = %p",simulation_charm);
     TRACE1 ("simulation = %p",proxy_simulation.ckLocalBranch());
     TRACE1 ("proxy_simulation = %p",&proxy_simulation);
     if (simulation_charm) simulation_charm->insert_block();
   }
#endif

   for (int ic=0; ic<8; ic++) depth_[ic] = 0;
 }

//----------------------------------------------------------------------

#ifdef CONFIG_USE_CHARM
void CommBlock::apply_initial_() throw ()
{

  SimulationCharm * simulation  = proxy_simulation.ckLocalBranch();
  FieldDescr * field_descr = simulation->field_descr();

  // Apply initial conditions

  index_initial_ = 0;
  while (Initial * initial = simulation->problem()->initial(index_initial_++)) {
    initial->enforce_block(this,field_descr, simulation->hierarchy());
  }
}
#endif

//----------------------------------------------------------------------

CommBlock::~CommBlock() throw ()
{ 
#ifdef CONFIG_USE_CHARM

  if (block_) delete block_;
  block_ = 0;

  SimulationCharm * simulation_charm  = proxy_simulation.ckLocalBranch();

  if (simulation_charm) simulation_charm->delete_block();
#endif

}

//----------------------------------------------------------------------

CommBlock::CommBlock(const CommBlock & block) throw ()
/// @param     block  Object being copied
{
  copy_(block);
}

//----------------------------------------------------------------------

CommBlock & CommBlock::operator = (const CommBlock & block) throw ()
/// @param     block  Source object of the assignment
/// @return    The target assigned object
{
  copy_(block);
  return *this;
}

//----------------------------------------------------------------------

void CommBlock::index_forest (int * ix, int * iy, int * iz) const throw ()
{
  index_.array(ix,iy,iz);
}

//----------------------------------------------------------------------

void CommBlock::size_forest (int * nx=0, int * ny=0, int * nz=0) const throw ()
{
  if (nx) (*nx)=size_[0]; 
  if (ny) (*ny)=size_[1]; 
  if (nz) (*nz)=size_[2]; 
}

//======================================================================
// MPI FUNCTIONS
//======================================================================

#ifdef CONFIG_USE_CHARM
#else /* CONFIG_USE_CHARM */

void CommBlock::refresh_ghosts(const FieldDescr * field_descr,
			       const Hierarchy * hierarchy,
			       int fx, int fy, int fz,
			       int index_field_set) throw()
{
  int ibx,iby,ibz;

  index_forest(&ibx,&iby,&ibz);

  block_->field_block(index_field_set)
    -> refresh_ghosts (field_descr,
		       hierarchy->group_process(),
		       hierarchy->layout(),
		       ibx,iby,ibz, fx,fy,fz);
}

#endif /* CONFIG_USE_CHARM */

#ifdef CONFIG_USE_CHARM

void CommBlock::determine_boundary_
(
 bool is_boundary[3][2],
 bool * fxm, bool * fxp,
 bool * fym, bool * fyp,
 bool * fzm, bool * fzp
 )
{
  
  // return is_boundary[] array of faces on domain boundary

  is_on_boundary (is_boundary);

  int nx,ny,nz;
  block_->field_block()->size (&nx,&ny,&nz);

  // Determine in which directions we need to communicate or update boundary

  if (fxm) *fxm = (nx > 1);
  if (fxp) *fxp = (nx > 1);
  if (fym) *fym = (ny > 1);
  if (fyp) *fyp = (ny > 1);
  if (fzm) *fzm = (nz > 1);
  if (fzp) *fzp = (nz > 1);
}

#endif

//----------------------------------------------------------------------


#ifdef CONFIG_USE_CHARM

void CommBlock::update_boundary_ ()
{

  bool is_boundary[3][2];
  bool fxm,fxp,fym,fyp,fzm,fzp;

  determine_boundary_(is_boundary,&fxm,&fxp,&fym,&fyp,&fzm,&fzp);

  Simulation * simulation = proxy_simulation.ckLocalBranch();

  Boundary * boundary = simulation->problem()->boundary();
  const FieldDescr * field_descr = simulation->field_descr();


  // Update boundaries

  if ( fxm && is_boundary[axis_x][face_lower] ) {
    boundary->enforce(field_descr,this,face_lower,axis_x);
  }
  if ( fxp && is_boundary[axis_x][face_upper] ) {
    boundary->enforce(field_descr,this,face_upper,axis_x);
  }
  if ( fym && is_boundary[axis_y][face_lower] ) {
    boundary->enforce(field_descr,this,face_lower,axis_y);
  }
  if ( fyp && is_boundary[axis_y][face_upper] ) {
    boundary->enforce(field_descr,this,face_upper,axis_y);
  }
  if ( fzm && is_boundary[axis_z][face_lower] ) {
    boundary->enforce(field_descr,this,face_lower,axis_z);
  }
  if ( fzp && is_boundary[axis_z][face_upper] ) {
    boundary->enforce(field_descr,this,face_upper,axis_z);
  }
}

#endif

//----------------------------------------------------------------------

#ifdef CONFIG_USE_CHARM
#endif /* CONFIG_USE_CHARM */

//----------------------------------------------------------------------

#ifdef CONFIG_USE_CHARM

#endif /* CONFIG_USE_CHARM */

//----------------------------------------------------------------------

#ifdef CONFIG_USE_CHARM
#endif /* CONFIG_USE_CHARM */

//======================================================================

void CommBlock::copy_(const CommBlock & comm_block) throw()
{
  block_->copy_(*comm_block.block());
  for (int i=0; i<3; i++) {
    size_[i] = comm_block.size_[i];
  }

  cycle_ = comm_block.cycle_;
  time_  = comm_block.time_;
  dt_    = comm_block.dt_;
  level_ = comm_block.level_;
  level_active_ = comm_block.level_active_;

#ifdef CONFIG_USE_CHARM
  sync_refresh_ = comm_block.sync_refresh_;
#endif
  
}

//----------------------------------------------------------------------

void CommBlock::is_on_boundary (bool is_boundary[3][2]) throw()
{
  int ix,iy,iz;
  index_.array(&ix,&iy,&iz);

  is_boundary[0][0] = (ix == 0);
  is_boundary[1][0] = (iy == 0);
  is_boundary[2][0] = (iz == 0);
  is_boundary[0][1] = (ix == (size_[0] - 1));
  is_boundary[1][1] = (iy == (size_[1] - 1));
  is_boundary[2][1] = (iz == (size_[2] - 1));
}

//----------------------------------------------------------------------

