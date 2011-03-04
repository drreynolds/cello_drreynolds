// $Id$
// See LICENSE_CELLO file for license and copyright information

/// @file     enzo_EnzoSimulation.cpp
/// @author   James Bordner (jobordner@ucsd.edu)
/// @todo     Create specific class for interfacing Cello code with User code
/// @todo     Move boundary conditions to Boundary object; remove bc_reflecting
/// @todo     Move output to Output object
/// @date     Tue May 11 18:06:50 PDT 2010
/// @brief    Implementation of EnzoSimulation user-dependent class member functions

#include "cello.hpp"

#include "enzo.hpp"

//----------------------------------------------------------------------

EnzoSimulation::EnzoSimulation(Monitor * monitor) throw ()
  : Simulation(monitor),
    enzo_(new EnzoBlock())
{
}

//----------------------------------------------------------------------

EnzoSimulation::~EnzoSimulation() throw()
{
  deallocate_();
}

//----------------------------------------------------------------------

void EnzoSimulation::initialize(FILE * fp) throw()
{
  // Call initialize for Simulation base class
  Simulation::initialize(fp);

  // Initialize enzo namespace variables
  enzo::initialize(parameters_);

  // Initialize EnzoBlock attributes
  enzo_->initialize(parameters_);

}

//----------------------------------------------------------------------

void EnzoSimulation::finalize() throw()
{
  delete enzo_;
  enzo_ = 0;
}

//----------------------------------------------------------------------

void EnzoSimulation::run() throw()
{
  
  INCOMPLETE("EnzoSimulation::run","incomplete");

  // @@@ performance_->start();
  Timer timer;
  Papi papi;
  
  timer.start();
  papi.start();

  // define aliases to required Enzo variables

  int & cycle       = enzo_->CycleNumber;
  enzo_float & time = enzo_->Time;
  enzo_float & dt   = enzo_->dt;

  // ASSUMES MESH IS JUST A SINGLE PATCH
  Patch * patch = mesh_->root_patch();

  // Iterator over all local blocks in the patch

  ItBlocks itBlocks(patch);
  DataBlock * data_block;

  //--------------------------------------------------
  // INITIALIZE FIELDS
  //--------------------------------------------------

  while ((data_block = ++itBlocks)) {

    block_start_(data_block);

    initial_->compute(data_block);

    block_stop_(data_block);

  }

  //--------------------------------------------------
  // BEGIN MAIN LOOP
  //--------------------------------------------------

  while (! stopping_->complete() ) {

    // MONITOR:  timestep and cycle progress

    {
      char buffer[40];
      sprintf (buffer,"cycle %04d time %15.12f", cycle,time);
      monitor_->print(buffer);
    }

    // MESH: CURRENTLY ASSUMES A SINGLE PATCH
    patch = mesh_->root_patch();

    // Local block iterator
    ItBlocks itBlocks(patch);

    // Determine dt

    double dt_block = std::numeric_limits<double>::max();

    while ((data_block = ++itBlocks)) {
      // Copy Cello block data to Enzo object
      block_start_(data_block);
      // Enforce boundary conditions
      // @@@ Move to Boundary
      data_block->field_block()->enforce_boundary(boundary_reflecting);
      // Accumulate block-local dt
      dt_block = MIN(dt_block,timestep_->compute(data_block));
      // Deallocate storage from block_start_();
      block_stop_(data_block);
    }

    // Accumulate block timesteps in patch

    double dt_patch;

#ifdef CONFIG_USE_MPI
    MPI_Allreduce (&dt_block, &dt_patch, 1, MPI_DOUBLE, MPI_MIN,
		   patch->layout()->mpi_comm());
#else
    dt_patch = dt_block;
#endif

    // Set Enzo timestep

    dt = dt_patch;

    // Apply the methods and output

    while ((data_block = ++itBlocks)) {

      block_start_(data_block);

      // Output
      output_images_(data_block, "enzo-p-%06d.%d.png",cycle,10);

      // Loop through methods
      for (size_t i = 0; i < method_list_.size(); i++) {
	Method * method = method_list_[i];
	method -> compute_block (data_block,time,dt);
      }

      block_stop_(data_block);

    } // Block in Patch

    ASSERT("EnzoSimulation::run", "dt == 0", dt != 0.0);

    // Update cycle and time

    cycle += 1;
    time  += dt;

  }

  //--------------------------------------------------
  // END MAIN LOOP
  //--------------------------------------------------

  // Final output

  while ((data_block = ++itBlocks)) {
    block_start_(data_block);
    output_images_(data_block, "enzo-p-%06d.%d.png",cycle,1);
    block_stop_(data_block);
  }


  // @@@ performance_->stop();
  papi.stop();
  timer.stop();

#ifdef CONFIG_USE_PAPI
  PARALLEL_PRINTF ("PAPI Time real   = %f\n",papi.time_real());
  PARALLEL_PRINTF ("PAPI Time proc   = %f\n",papi.time_proc());
  PARALLEL_PRINTF ("PAPI GFlop count = %f\n",papi.flop_count()*1e-9);
  //  PARALLEL_PRINTF ("PAPI GFlop rate  = %f\n",papi.flop_rate()*1e-9);
  PARALLEL_PRINTF ("PAPI GFlop rate  = %f\n",
		   1e-9 * papi.flop_count() / papi.time_real());
  
#endif

  PARALLEL_PRINTF ("Real time = %f\n",timer.value());

}

//----------------------------------------------------------------------

void EnzoSimulation::read() throw()
{
  INCOMPLETE("EnzoSimulation::read","");
}

//----------------------------------------------------------------------

void EnzoSimulation::write() throw()
{
  INCOMPLETE("EnzoSimulation::write","");
}

//======================================================================

Timestep * 
EnzoSimulation::create_timestep_ ( std::string name ) throw ()
/// @param name   Name of the timestep method to create (ignored)
{
  return new EnzoTimestep(enzo_);
}

//----------------------------------------------------------------------

Initial * 
EnzoSimulation::create_initial_ ( std::string name ) throw ()
/// @param name   Name of the initialization method to create
{
  
  Initial * initial = 0;

  if (name == "implosion_2d") {
    initial = new EnzoInitialImplosion2 (enzo_);
  }

  return initial;
}

//----------------------------------------------------------------------

Stopping * 
EnzoSimulation::create_stopping_ (std::string name) throw ()
/// @param name   Name of the stopping method to create (ignored)
{
  //--------------------------------------------------
  parameters_->set_current_group ("Stopping");
  //--------------------------------------------------

  // parameter: Stopping::cycle
  // parameter: Stopping::time

  int    stop_cycle = parameters_->value_integer("cycle",-1);
  double stop_time  = parameters_->value_scalar("time",-1.0);

  return new EnzoStopping(enzo_,stop_cycle,stop_time);
}

//----------------------------------------------------------------------

Boundary * 
EnzoSimulation::create_boundary_ ( std::string name ) throw ()
/// @param name   Name of the initialization method to create
{
  
  Boundary * boundary = 0;

  return boundary;
}

//----------------------------------------------------------------------

Method * 
EnzoSimulation::create_method_ ( std::string name ) throw ()
/// @param name   Name of the method to create
{

  Method * method = 0;

  if (name == "ppm")
    method = new EnzoMethodPpm  (parameters_,enzo_);
  if (name == "ppml")
    method = new EnzoMethodPpml (parameters_,enzo_);

  if (method == 0) {
    char buffer[80];
    sprintf (buffer,"Cannot create Method '%s'",name.c_str());
    ERROR("EnzoSimulation::create_method", buffer);
  }

  return method;
}

//======================================================================

void EnzoSimulation::block_start_ (DataBlock * data_block) throw ()
{

  FieldBlock * field_block = data_block->field_block();

  double xm,xp,ym,yp,zm,zp;

  data_block->extent(&xm,&xp,&ym,&yp,&zm,&zp);

  enzo_->GridLeftEdge[0]    = xm;
  enzo_->GridLeftEdge[1]    = ym;
  enzo_->GridLeftEdge[2]    = zm;

  // Grid dimensions

  int nx,ny,nz;
  field_block -> size (&nx,&ny,&nz);

  enzo_->GridDimension[0]  = nx + 2*enzo::ghost_depth[0];
  enzo_->GridDimension[1]  = ny + 2*enzo::ghost_depth[1];
  enzo_->GridDimension[2]  = nz + 2*enzo::ghost_depth[2];
  enzo_->GridStartIndex[0] = enzo::ghost_depth[0];
  enzo_->GridStartIndex[1] = enzo::ghost_depth[1];
  enzo_->GridStartIndex[2] = enzo::ghost_depth[2];
  enzo_->GridEndIndex[0]   = enzo::ghost_depth[0] + nx - 1;
  enzo_->GridEndIndex[1]   = enzo::ghost_depth[1] + ny - 1;
  enzo_->GridEndIndex[2]   = enzo::ghost_depth[2] + nz - 1;

  // Initialize CellWidth

  double h3[3];
  field_block->cell_width(data_block,&h3[0],&h3[1],&h3[2]);

  for (int dim=0; dim<enzo::GridRank; dim++) {
    enzo_->CellWidth[dim] = h3[dim];
  }

  // Initialize BaryonField[] pointers

  for (int field = 0; field < enzo::NumberOfBaryonFields; field++) {
    enzo_->BaryonField[field] = (enzo_float *)field_block->field_values(field);
  }

 
  //   // Boundary
  //   /* If using comoving coordinates, compute the expansion factor a.  Otherwise,
  //      set it to one. */
 
  //   /* 1) Compute Courant condition for baryons. */
 
  //    // boundary
 
  enzo::BoundaryDimension[0] = enzo_->GridDimension[0];
  enzo::BoundaryDimension[1] = enzo_->GridDimension[1];
  enzo::BoundaryDimension[2] = enzo_->GridDimension[2];

  for (int field=0; field<enzo::NumberOfBaryonFields; field++) {
    enzo::BoundaryFieldType[field] = enzo::FieldType[field];
    for (int axis = axis_x; axis <= axis_z; axis++) {
      for (int face = face_lower; face <= face_upper; face++) {
	enzo::BoundaryType [field][axis][face] = NULL;
	if (data_block->boundary_face((face_enum)(face),
 				       (axis_enum)(axis))) {
 	  int n1 = enzo_->GridDimension[(axis+1)%3];
	  int n2 = enzo_->GridDimension[(axis+2)%3];
	  int size = n1*n2;
	  enzo::BoundaryType [field][axis][face] = new bc_enum [size];
	  enzo::BoundaryValue[field][axis][face] = NULL;
	  for (int i2 = 0; i2<n2; i2++) {
	    for (int i1 = 0; i1<n1; i1++) {
	      int i = i1 + n1*i2;
	      enzo::BoundaryType[field][axis][face][i] = bc_reflecting;
	    }
	  }
	}
      }
    }
  }

  // @@@ WRITE OUT ENZO DESCRIPTION FOR DEBUGGING
  //  enzo_->write(stdout);
}

//----------------------------------------------------------------------

void EnzoSimulation::block_stop_ ( DataBlock * data_block ) throw ()
{
  WARNING("EnzoSimulation::block_stop_",
	  "global BoundaryType in enzo namespace is deleted at block level");

  for (int field=0; field<enzo::NumberOfBaryonFields; field++) {
    for (int axis = 0; axis < 3; axis++) {
      for (int face = 0; face < 2; face++) {
	delete [] enzo::BoundaryType [field][axis][face];
      }
    }
  }
}

//----------------------------------------------------------------------

void EnzoSimulation::output_images_
(
 DataBlock * data_block,
 const char * file_format,
 int cycle,
 int cycle_skip
 ) throw ()
{

  if (! (cycle_skip && cycle % cycle_skip == 0)) return;

  FieldBlock *       field_block = data_block->field_block();
  const FieldDescr * field_descr = field_block->field_descr();
  int nx,ny,nz;
  int gx,gy,gz;
  int mx,my,mz;
  field_block->enforce_boundary(boundary_reflecting);
  field_block->size(&nx,&ny,&nz);
  int count = field_descr->field_count();
  for (int index = 0; index < count; index++) {
    field_descr->ghosts(index,&gx,&gy,&gz);
    mx=nx+2*gx;
    my=ny+2*gy;
    mz=nz+2*gz;
    char filename[255];
    std::string field_name = field_descr->field_name(index);
    Scalar * field_values = (Scalar *)field_block->field_values(index);
    sprintf (filename,file_format,
	     enzo_->CycleNumber,index);
    monitor_->image (filename, field_values, mx,my,mz, 2, reduce_sum, 
    		     0.0, 1.0);
  }
}

//----------------------------------------------------------------------

void EnzoSimulation::deallocate_() throw()
{
  delete enzo_;
  enzo_ = 0;
  delete stopping_;
  stopping_ = 0;
  delete timestep_;
  timestep_ = 0;
  delete initial_;
  initial_ = 0;
  for (size_t i=0; i<method_list_.size(); i++) {
    delete method_list_[i];
    method_list_[i] = 0;
  }
  
}
