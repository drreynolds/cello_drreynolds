// $Id: enzo_MethodEnzoControl.cpp 1388 2010-04-20 23:57:46Z bordner $
// See LICENSE_CELLO file for license and copyright information

/// @file     enzo_MethodEnzoControl.cpp
/// @author   James Bordner (jobordner@ucsd.edu)
/// @todo     Create specific class for interfacing Cello code with User code
/// @date     Tue May 11 18:06:50 PDT 2010
/// @brief    Implementation of MethodEnzoControl user-dependent class member functions

#include "user.hpp"
#include "enzo.hpp"
#include "error.hpp"
#include "parameters.hpp"
#include "parallel.hpp"

#include "cello_hydro.h"

//----------------------------------------------------------------------

void MethodEnzoControl::initialize (DataDescr * data_descr) throw()
{
  // Extract field descriptor from data descriptor

  FieldDescr * field_descr = data_descr->field_descr();

  // Cosmology parameters

  Parameters * parameters = global_ -> parameters();

  parameters->set_current_group ("Physics");

  enzo_->ComovingCoordinates  = parameters->value_logical ("cosmology",false);
  enzo_->Gamma                = parameters->value_scalar  ("gamma",5.0/3.0);

  //  if (ComovingCoordinates) {

  parameters->set_current_subgroup ("cosmology");

  enzo_->InitialRedshift   = parameters->value_scalar ("initial_redshift",  20.0);
  enzo_->HubbleConstantNow = parameters->value_scalar ("hubble_constant_now",0.701);
  enzo_->OmegaLambdaNow    = parameters->value_scalar ("omega_lambda_now",   0.721);
  enzo_->OmegaMatterNow    = parameters->value_scalar ("omega_matter_now",   0.279);
  enzo_->MaxExpansionRate  = parameters->value_scalar ("max_expansion_rate", 0.01);
  enzo_->ComovingBoxSize   = parameters->value_scalar ("comoving_box_size", 64.0);
    //  }

  int k = 0;

  enzo_->field_density         = field_descr->insert_field("density");
  k = MAX(k,enzo_->field_density);
  enzo_->FieldType[enzo_->field_density]      = Density;

  enzo_->field_total_energy    = field_descr->insert_field("total_energy");
  k = MAX(k,enzo_->field_total_energy);
  enzo_->FieldType[enzo_->field_total_energy] = TotalEnergy;

  if (enzo_->DualEnergyFormalism) {
    enzo_->field_internal_energy = field_descr->insert_field("internal_energy");
    k = MAX(k,enzo_->field_internal_energy);
    enzo_->FieldType[enzo_->field_internal_energy] = InternalEnergy;
  }    

  parameters->set_current_group("Physics");
  enzo_->GridRank = parameters->value_integer ("dimensions",0);

  if (enzo_->GridRank >= 1) {
    enzo_->field_velocity_x      = field_descr->insert_field("velocity_x");
    k = MAX(k,enzo_->field_velocity_x);
    enzo_->FieldType[enzo_->field_velocity_x] = Velocity1;
  }

  if (enzo_->GridRank >= 2) {
    enzo_->field_velocity_y      = field_descr->insert_field("velocity_y");
    k = MAX(k,enzo_->field_velocity_y);
    enzo_->FieldType[enzo_->field_velocity_y] = Velocity2;
  }

  if (enzo_->GridRank >= 3) {
    enzo_->field_velocity_z      = field_descr->insert_field("velocity_z");
    k = MAX(k,enzo_->field_velocity_z);
    enzo_->FieldType[enzo_->field_velocity_z] = Velocity3;
  }

  enzo_->field_color      = field_descr->insert_field("electron_density");
  k = MAX(k,enzo_->field_color);
  enzo_->FieldType[enzo_->field_color] = ElectronDensity;

  // PPM parameters

  parameters->set_current_group ("Method","ppm");

  enzo_->PressureFree = parameters->value_scalar("pressure_free",false);
  enzo_->UseMinimumPressureSupport 
    =              parameters->value_logical("use_minimum_pressure_support",false);
  enzo_->MinimumPressureSupportParameter 
    =              parameters->value_integer("minimum_pressure_support_parameter",100);
  enzo_->PPMFlatteningParameter = parameters->value_logical ("flattening", false);
  enzo_->PPMDiffusionParameter  = parameters->value_logical ("diffusion",  false);
  enzo_->PPMSteepeningParameter = parameters->value_logical ("steepening", false);

  double floor_default = 1e-6;
  enzo_->pressure_floor       = parameters->value_scalar("pressure_floor",      floor_default);
  enzo_->density_floor        = parameters->value_scalar("density_floor",       floor_default);
  enzo_->temperature_floor    = parameters->value_scalar("temperature_floor",   floor_default);
  enzo_->number_density_floor = parameters->value_scalar("number_density_floor",floor_default);

  enzo_->DualEnergyFormalism     = parameters->value_logical ("dual_energy",false);
  enzo_->DualEnergyFormalismEta1 = parameters->value_scalar  ("dual_energy_eta_1",0.001);
  enzo_->DualEnergyFormalismEta2 = parameters->value_scalar  ("dual_energy_eta_1",0.1);

  // Chemistry parameters

  enzo_->MultiSpecies = 0;    // 0:0 1:6 2:9 3:12

  // Gravity parameters

  enzo_->GravityOn                       = 0;    // Whether gravity is included
  enzo_->GravitationalConstant           = 1.0;  // used only in SetMinimumSupport()
  enzo_->AccelerationField[0]            = NULL;
  enzo_->AccelerationField[1]            = NULL;
  enzo_->AccelerationField[2]            = NULL;

  //Problem specific parameter

  enzo_->ProblemType = 0;

  // Field parameters

  parameters->set_current_group ("Field");

  enzo_->ghost_depth[0] = (enzo_->GridRank >= 1) ? 
    parameters->list_value_integer(0,"ghosts",3) : 0;
  enzo_->ghost_depth[1] = (enzo_->GridRank >= 2) ? 
    parameters->list_value_integer(1,"ghosts",3) : 0;
  enzo_->ghost_depth[2] = (enzo_->GridRank >= 3) ? 
    parameters->list_value_integer(2,"ghosts",3) : 0;

  enzo_->NumberOfBaryonFields = field_descr->field_count();

  ASSERT ("initialize",
	  "MAX_NUMBER_OF_BARYON_FIELDS is too small",
	  enzo_->NumberOfBaryonFields <= MAX_NUMBER_OF_BARYON_FIELDS);

  enzo_->Time                   = 0;

  // Domain parameters

  parameters->set_current_group ("Domain");
  
  enzo_->DomainLeftEdge [0] = parameters->list_value_scalar(0,"extent",0.0);
  enzo_->DomainRightEdge[0] = parameters->list_value_scalar(1,"extent",1.0);
  enzo_->DomainLeftEdge [1] = parameters->list_value_scalar(2,"extent",0.0);
  enzo_->DomainRightEdge[1] = parameters->list_value_scalar(3,"extent",1.0);
  enzo_->DomainLeftEdge [2] = parameters->list_value_scalar(4,"extent",0.0);
  enzo_->DomainRightEdge[2] = parameters->list_value_scalar(5,"extent",1.0);

  // Initial conditions

  parameters->set_current_group ("Initial");

  enzo_->InitialTimeInCodeUnits = parameters->value_scalar ("time",0.0);
  enzo_->Time = enzo_->InitialTimeInCodeUnits;
  enzo_->OldTime = enzo_->Time;

  // Parallel parameters

  GroupProcessMpi parallel;

  enzo_->ProcessorNumber = parallel.rank();

  parameters->set_current_group ("Field");
  
  enzo_->CourantSafetyNumber = parameters->value_scalar ("courant",0.6);


}

//----------------------------------------------------------------------

void MethodEnzoControl::finalize (DataDescr * data_descr) throw()
{
}

//----------------------------------------------------------------------

void MethodEnzoControl::initialize_block ( DataBlock * data_block ) throw ()
{

  FieldBlock * field_block = data_block->field_block();
  
  double xm,xp,ym,yp,zm,zp;

  field_block->box_extent(&xm,&xp,&ym,&yp,&zm,&zp);

  enzo_->GridLeftEdge[0]    = xm;
  enzo_->GridLeftEdge[1]    = ym;
  enzo_->GridLeftEdge[2]    = zm;

  // Grid dimensions

  int nx,ny,nz;
  field_block -> dimensions (&nx,&ny,&nz);

  enzo_->GridDimension[0]  = nx + 2*enzo_->ghost_depth[0];
  enzo_->GridDimension[1]  = ny + 2*enzo_->ghost_depth[1];
  enzo_->GridDimension[2]  = nz + 2*enzo_->ghost_depth[2];
  enzo_->GridStartIndex[0] = enzo_->ghost_depth[0];
  enzo_->GridStartIndex[1] = enzo_->ghost_depth[1];
  enzo_->GridStartIndex[2] = enzo_->ghost_depth[2];
  enzo_->GridEndIndex[0]   = nx + enzo_->ghost_depth[0] - 1;
  enzo_->GridEndIndex[1]   = ny + enzo_->ghost_depth[1] - 1;
  enzo_->GridEndIndex[2]   = nz + enzo_->ghost_depth[2] - 1;

  // Initialize CellWidth[].  Should be converted to constants hx,hy,hz

  double h3[3];
  field_block->cell_width(&h3[0],&h3[1],&h3[2]);

  for (int dim=0; dim<enzo_->GridRank; dim++) {
    enzo_->CellWidth[dim] = new ENZO_FLOAT[enzo_->GridDimension[dim]];
    for (int i=0; i<enzo_->GridDimension[dim]; i++) {
      enzo_->CellWidth[dim][i] = h3[dim];
    }
  }

  // Initialize BaryonField[] pointers

  for (int field = 0; field < enzo_->NumberOfBaryonFields; field++) {
    enzo_->BaryonField[field] = (float *)field_block->field_values(field);
  }

 
//   // Boundary
//   /* If using comoving coordinates, compute the expansion factor a.  Otherwise,
//      set it to one. */
 
//   /* 1) Compute Courant condition for baryons. */
 
//    // boundary
 
//    BoundaryRank = 2;
//    BoundaryDimension[0] = GridDimension[0];
//    BoundaryDimension[1] = GridDimension[1];

//    for (int field=0; field<NumberOfBaryonFields; field++) {
//      BoundaryFieldType[field] = enzo_->FieldType[field];
//      for (int dim = 0; dim < 3; dim++) {
//        for (int face = 0; face < 2; face++) {
//  	int n1 = GridDimension[(dim+1)%3];
//  	int n2 = GridDimension[(dim+2)%3];
//  	int size = n1*n2;
//  	BoundaryType [field][dim][face] = new bc_type [size];
//  	BoundaryValue[field][dim][face] = NULL;
//  	for (int i2 = 0; i2<n2; i2++) {
//  	  for (int i1 = 0; i1<n1; i1++) {
//  	    int i = i1 + n1*i2;
//  	    BoundaryType[field][dim][face][i] = bc_reflecting;
//  	  }
//  	}
//        }
//      }
//    }

}


//----------------------------------------------------------------------

void MethodEnzoControl::finalize_block ( DataBlock * data_block ) throw ()
{
  ++ enzo_->CycleNumber;
  for (int dim=0; dim < enzo_->GridRank; dim++) {
    delete [] enzo_->CellWidth[dim];
  }
}

//----------------------------------------------------------------------

void MethodEnzoControl::refresh_ghost(DataBlock * data_block, 
				      bool xm, bool xp, 
				      bool ym, bool yp, 
				      bool zm, bool zp) throw()
{

}

//======================================================================

