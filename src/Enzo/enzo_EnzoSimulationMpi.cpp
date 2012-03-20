// See LICENSE_CELLO file for license and copyright information

/// @file     enzo_EnzoSimulationMpi.cpp
/// @author   James Bordner (jobordner@ucsd.edu)
/// @date     Tue May 11 18:06:50 PDT 2010
/// @brief    Implementation of EnzoSimulationMpi user-dependent class member functions

#ifndef CONFIG_USE_CHARM

#include "cello.hpp"

#include "enzo.hpp"

//----------------------------------------------------------------------

EnzoSimulationMpi::EnzoSimulationMpi
(
 const char * parameter_file,
 GroupProcess * group_process ) throw ()
  : EnzoSimulation(parameter_file,group_process)
{
}

//----------------------------------------------------------------------

EnzoSimulationMpi::~EnzoSimulationMpi() throw()
{
}

//----------------------------------------------------------------------

void EnzoSimulationMpi::run() throw()
{
  
#ifdef CONFIG_USE_MPI
  ReduceMpi    reduce(group_process_);
#else
  ReduceSerial reduce(group_process_);
#endif

  // get hierarchy extents for later block boundary checks

  double lower[3];
  hierarchy_->lower(&lower[0], &lower[1], &lower[2]);
  double upper[3];
  hierarchy_->upper(&upper[0], &upper[1], &upper[2]);

  //--------------------------------------------------
  // INITIALIZE FIELDS
  //--------------------------------------------------

  ItPatch it_patch(hierarchy_);
  Patch * patch;

  while ((patch = ++it_patch)) {

    ItBlock it_block(patch);
    Block * block;

    while ((block = ++it_block)) {

      problem()->initial()->enforce(hierarchy_,field_descr_,block);

      EnzoBlock * enzo_block = static_cast <EnzoBlock*> (block);

      enzo_block->set_cycle(cycle_);
      enzo_block->set_time(time_);

      enzo_block->initialize();

    }
  }

  while ((patch = ++it_patch)) {

    ItBlock it_block(patch);
    Block * block;

    while ((block = ++it_block)) {

      bool is_boundary [3][2];

      // is_block_on_boundary_ (block,boundary);
      block->is_on_boundary (lower,upper,is_boundary);

      update_boundary_(block,is_boundary);

      refresh_ghost_(block,patch,is_boundary);

    }
  }
  // Perform any scheduled output

  scheduled_output();
  
  //--------------------------------------------------
  // INITIAL STOPPING CRITERIA TEST
  //--------------------------------------------------

  int stop_hierarchy = true;

  while ((patch = ++it_patch)) {

    int    stop_patch  = true;

    ItBlock it_block(patch);
    Block * block;

    while ((block = ++it_block)) {

      EnzoBlock * enzo_block = static_cast <EnzoBlock*> (block);

      int & cycle_block = enzo_block->CycleNumber;
      double time_block =  enzo_block->Time();

      int stop_block = problem()->stopping()->complete(cycle_block,time_block);

      stop_patch = stop_patch && stop_block;

    }

    stop_hierarchy = stop_hierarchy && stop_patch;

  }

  //======================================================================
  // BEGIN MAIN LOOP
  //======================================================================

  while (! stop_hierarchy) {

    //--------------------------------------------------
    // Determine timestep
    //--------------------------------------------------

    double dt_hierarchy = std::numeric_limits<double>::max();

    // Accumulate Patch-local timesteps

    while ((patch = ++it_patch)) {

      double dt_patch = std::numeric_limits<double>::max();

      ItBlock it_block(patch);
      Block * block;

      // Accumulate Block-local timesteps

      while ((block = ++it_block)) {

	// Accumulate Block-local dt

	block->set_cycle(cycle_);
	block->set_time (time_);

	double dt_block = problem()->timestep()->compute(field_descr_,block);

	// Reduce timestep to coincide with scheduled output if needed

	double time_block = static_cast <EnzoBlock*> (block)->Time();

	int index_output=0;
	while (Output * output = problem()->output(index_output++)) {
	  dt_block = output->update_timestep(time_block,dt_block);
	}

	// Reduce timestep to coincide with end of simulation if needed

	double time_stop = problem()->stopping()->stop_time();

	dt_block = MIN (dt_block, (time_stop - time_block));

	// Update patch-level timestep

	dt_patch = MIN(dt_patch,dt_block);

      } // ( block = ++it_block )

      // Update hierarchy-level timestep

      dt_hierarchy = MIN(dt_hierarchy, dt_patch);

    } // ( patch = ++it_patch )

    dt_hierarchy = reduce.reduce_double(dt_hierarchy,reduce_op_min);

    dt_ = dt_hierarchy;

    ASSERT("EnzoSimulation::run", "dt == 0", dt_hierarchy != 0.0);

    monitor_output();

    stop_hierarchy = true;

    //--------------------------------------------------
    // Refresh ghosts and boundary
    //--------------------------------------------------

    while ((patch = ++it_patch)) {

      ItBlock it_block(patch);
      Block * block;

      while ((block = ++it_block)) {

	bool is_boundary [3][2];

	block->is_on_boundary (lower,upper,is_boundary);

	// Refresh ghost zones
	refresh_ghost_(block,patch,is_boundary);

	// Update boundary conditions
	update_boundary_(block,is_boundary);

      }
    }

    //--------------------------------------------------
    // Apply numerical methods
    //--------------------------------------------------

    while ((patch = ++it_patch)) {

      int stop_patch = true;

      ItBlock it_block(patch);
      Block * block;

      while ((block = ++it_block)) {

	double lower_block[3];
	block->lower(&lower_block[axis_x],
		     &lower_block[axis_y],
		     &lower_block[axis_z]);
	double upper_block[3];
	block->upper(&upper_block[axis_x],
		     &upper_block[axis_y],
		     &upper_block[axis_z]);

	EnzoBlock * enzo_block = static_cast <EnzoBlock*> (block);

	int        & cycle_block    = enzo_block->CycleNumber;
	enzo_float   time_block     = enzo_block->Time();
	enzo_float & dt_block       = enzo_block->dt;
	enzo_float & old_time_block = enzo_block->OldTime;

	// UNIFORM TIMESTEP OVER ALL BLOCKS IN HIERARCHY

	dt_block = dt_hierarchy;

	// Loop through methods
	
	int index_method = 0;
	while (Method * method = problem()->method(index_method++)) {

	  double lower[3];
	  double upper[3];
	  block->lower(lower+0,lower+1,lower+2);
	  block->upper(upper+0,upper+1,upper+2);
	  char buffer[10];
	  sprintf (buffer,"%03d-A",cycle_);
	  block->field_block()->print(field_descr_,buffer,lower,upper);

	  method -> compute_block (field_descr_,block,time_block,dt_block);

	  sprintf (buffer,"%03d-B",cycle_);
	  block->field_block()->print(field_descr_,buffer,lower,upper);

	}

	// Update enzo_block values

	old_time_block  = time_block;
	time_block     += dt_block;
	cycle_block    += 1;

	block->set_cycle (cycle_block);
	block->set_time  (time_block);
	block->set_dt    (dt_block);

	// Global cycle and time reduction
	
	int stop_block = problem()->stopping()->complete(cycle_block,time_block);
	
	// Update stopping criteria for patch

	stop_patch = stop_patch && stop_block;

      } // (block = ++it_block)

      // Update stopping criteria for hierarchy

      stop_hierarchy = stop_hierarchy && stop_patch;

    } // (patch = ++it_patch)

    stop_hierarchy = reduce.reduce_int(stop_hierarchy,reduce_op_land);

    cycle_ ++;
    time_ += dt_hierarchy;

    // Perform any scheduled output

    scheduled_output();

  } // while (! stop_hierarchy)

  //======================================================================
  // END MAIN LOOP
  //======================================================================

  monitor_output();

}

//----------------------------------------------------------------------

void EnzoSimulationMpi::update_boundary_ (Block * block, bool is_boundary[3][2]) throw()
{
  // Update boundary conditions

  if (dimension_ >= 1) {
    if (is_boundary[axis_x][face_lower]) 
      problem()->boundary()->enforce(field_descr_,block,face_lower,axis_x);
    if (is_boundary[axis_x][face_upper]) 
      problem()->boundary()->enforce(field_descr_,block,face_upper,axis_x);
  }
  if (dimension_ >= 2) {
    if (is_boundary[axis_y][face_lower]) 
      problem()->boundary()->enforce(field_descr_,block,face_lower,axis_y);
    if (is_boundary[axis_y][face_upper]) 
      problem()->boundary()->enforce(field_descr_,block,face_upper,axis_y);
  }
  if (dimension_ >= 3) {
    if (is_boundary[axis_z][face_lower]) 
      problem()->boundary()->enforce(field_descr_,block,face_lower,axis_z);
    if (is_boundary[axis_z][face_upper]) 
      problem()->boundary()->enforce(field_descr_,block,face_upper,axis_z);
  }
}

//----------------------------------------------------------------------

void EnzoSimulationMpi::refresh_ghost_ 
(
 Block * block, 
 Patch * patch, 
 bool    is_boundary[3][2]
 ) throw()
{
  // Refresh ghost zones

  int ibx,iby,ibz;
  block->index_patch(&ibx,&iby,&ibz);

  bool periodic = problem()->boundary()->is_periodic();

  // FOLLOWING IS SIMILAR TO Block::p_refresh_face()

  int nx,ny,nz;
  block->field_block()->size (&nx,&ny,&nz);

  // Determine axes that may be neighbors

  bool axm = (nx > 1) && (periodic || ! is_boundary[axis_x][face_lower]);
  bool axp = (nx > 1) && (periodic || ! is_boundary[axis_x][face_upper]); 
  bool aym = (ny > 1) && (periodic || ! is_boundary[axis_y][face_lower]);
  bool ayp = (ny > 1) && (periodic || ! is_boundary[axis_y][face_upper]);
  bool azm = (nz > 1) && (periodic || ! is_boundary[axis_z][face_lower]);
  bool azp = (nz > 1) && (periodic || ! is_boundary[axis_z][face_upper]);

  int px = (ibx % 2 == 0) ? -1 : 1;
  int py = (iby % 2 == 0) ? -1 : 1;
  int pz = (ibz % 2 == 0) ? -1 : 1;

  if (px == 1) SWAP(axm,axp);
  if (py == 1) SWAP(aym,ayp);
  if (pz == 1) SWAP(azm,azp);

  if (field_descr_->refresh_face(2)) {
    // TRACE3("p %d %d %d",px,py,pz);
    // TRACE6("a %d %d  %d %d  %d %d",axm,axp,aym,ayp,azm,azp);
    if (axm) block->refresh_ghosts(field_descr_,patch,+px,0,0);
    if (axp) block->refresh_ghosts(field_descr_,patch,-px,0,0);
    if (aym) block->refresh_ghosts(field_descr_,patch,0,+py,0);
    if (ayp) block->refresh_ghosts(field_descr_,patch,0,-py,0);
    if (azm) block->refresh_ghosts(field_descr_,patch,0,0,+pz);
    if (azp) block->refresh_ghosts(field_descr_,patch,0,0,-pz);
  }

}

#endif /* ! CONFIG_USE_CHARM */
