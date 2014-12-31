// See LICENSE_CELLO file for license and copyright information

/// @file     enzo_EnzoMethodPpml.cpp
/// @author   James Bordner (jobordner@ucsd.edu)
/// @date     Fri Apr  2 17:05:23 PDT 2010
/// @brief    Implements the EnzoMethodPpml class

//----------------------------------------------------------------------

#include "cello.hpp"

#include "enzo.hpp"

//----------------------------------------------------------------------

EnzoMethodPpml::EnzoMethodPpml(EnzoConfig * enzo_config) 
  : Method(),
    comoving_coordinates_(enzo_config->physics_cosmology)
{
}

//----------------------------------------------------------------------

void EnzoMethodPpml::pup (PUP::er &p)
{
  // NOTE: change this function whenever attributes change

  TRACEPUP;

  Method::pup(p);
  p | comoving_coordinates_;
}

//----------------------------------------------------------------------

void EnzoMethodPpml::compute ( CommBlock * comm_block ) throw()
{

  if (!comm_block->is_leaf()) return;

  const FieldDescr * field_descr = comm_block->field_descr();
  EnzoBlock * enzo_block = static_cast<EnzoBlock*> (comm_block);
  enzo_block->SolveMHDEquations ( field_descr, comm_block->dt() );

  enzo_block->compute_stop();

}

//----------------------------------------------------------------------

double EnzoMethodPpml::timestep (CommBlock * comm_block) throw()
{
 
  EnzoBlock * enzo_comm_block = static_cast<EnzoBlock*> (comm_block);

  /* initialize */
 
  enzo_float dt;
  // enzo_float dtTemp;
  enzo_float dtBaryons      = ENZO_HUGE_VAL;
  // enzo_float dtViscous      = ENZO_HUGE_VAL;
  // enzo_float dtParticles    = ENZO_HUGE_VAL;
  // enzo_float dtExpansion    = ENZO_HUGE_VAL;
  // enzo_float dtAcceleration = ENZO_HUGE_VAL;
  // int dim, i, result;
 
  /* Compute the field size. */
 
  // int size = 1;
  // for (dim = 0; dim < GridRank; dim++)
  //   size *= GridDimension[dim];
 
  /* If using comoving coordinates, compute the expansion factor a.  Otherwise,
     set it to one. */
 
  enzo_float a = 1, dadt;
  
  if (comoving_coordinates_)
    enzo_comm_block->CosmologyComputeExpansionFactor
      (enzo_comm_block->time(), &a, &dadt);
  //  float afloat = float(a);
 
  /* 1) Compute Courant condition for baryons. */
 
  if (EnzoBlock::NumberOfBaryonFields > 0) {
 
    /* Find fields: density, total energy, velocity1-3. */
 
    // int DensNum, GENum, Vel1Num, Vel2Num, Vel3Num, TENum;
    // float *pressure_field;

    // if (HydroMethod != PPML_Isothermal3D) {
    //   if (this->IdentifyPhysicalQuantities(DensNum, GENum, Vel1Num, Vel2Num,
    // 					 Vel3Num, TENum) == FAIL) {
    // 	fprintf(stderr, "ComputeTimeStep: IdentifyPhysicalQuantities error.\n");
    // 	exit(FAIL);
    //   }
 
    // /* Compute the pressure. */
 
    //   pressure_field = new float[size];
    //   if (DualEnergyFormalism)
    // 	result = this->ComputePressureDualEnergyFormalism(Time, pressure_field);
    //   else
    // 	result = this->ComputePressure(Time, pressure_field);
 
    //   if (result == FAIL) {
    // 	fprintf(stderr, "Error in grid->ComputePressure.\n");
    // 	exit(EXIT_FAILURE);
    //   }
    // }

    /* Call fortran routine to do calculation. */
 
    Field field = enzo_comm_block->block()->field();

    enzo_float * d  = (enzo_float *) field.values("density");
    enzo_float * vx = (enzo_float *) field.values("velox");
    enzo_float * vy = (enzo_float *) field.values("veloy");
    enzo_float * vz = (enzo_float *) field.values("veloz");
    enzo_float * bx = (enzo_float *) field.values("bfieldx");
    enzo_float * by = (enzo_float *) field.values("bfieldy");
    enzo_float * bz = (enzo_float *) field.values("bfieldz");

    FORTRAN_NAME(calc_dt_ppml)
      (enzo_comm_block->GridDimension, 
       enzo_comm_block->GridDimension+1, 
       enzo_comm_block->GridDimension+2,
       enzo_comm_block->GridStartIndex, 
       enzo_comm_block->GridEndIndex,
       enzo_comm_block->GridStartIndex+1, 
       enzo_comm_block->GridEndIndex+1,
       enzo_comm_block->GridStartIndex+2, 
       enzo_comm_block->GridEndIndex+2,
       &enzo_comm_block->CellWidth[0], 
       &enzo_comm_block->CellWidth[1], 
       &enzo_comm_block->CellWidth[2],
       d,
       vx, vy, vz,
       bx, by, bz,
       &dtBaryons);
    /* Multiply resulting dt by CourantSafetyNumber (for extra safety!). */
 
    dtBaryons *= EnzoBlock::CourantSafetyNumber;
    
  }
 
  /* 5) calculate minimum timestep */

  dt = std::numeric_limits<enzo_float>::max();

  dt = MIN(dt, dtBaryons);


  return dt;
}

//----------------------------------------------------------------------
