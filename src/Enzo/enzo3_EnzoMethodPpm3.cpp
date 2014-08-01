// See LICENSE_CELLO file for license and copyright information

/// @file     enzo_EnzoMethodPpm3.cpp
/// @author   James Bordner (jobordner@ucsd.edu)
/// @date     Fri Apr  2 17:05:23 PDT 2010
/// @brief    Implements the EnzoMethodPpm3 class

#include "cello.hpp"

#include "enzo.hpp"

//----------------------------------------------------------------------

EnzoMethodPpm3::EnzoMethodPpm3 () : Method()
{
  // PPM parameters initialized in EnzoBlock::initialize()
}

//----------------------------------------------------------------------

void EnzoMethodPpm3::pup (PUP::er &p)
{

  // NOTE: change this function whenever attributes change

  TRACEPUP;

  Method::pup(p);

}

//----------------------------------------------------------------------

void EnzoMethodPpm3::compute ( CommBlock * comm_block) throw()
{
  if (!comm_block->is_leaf()) return;

  EnzoBlock * enzo_block = static_cast<EnzoBlock*> (comm_block);

  enzo_block->SolveHydroEquations3
    ( comm_block->cycle(), comm_block->time(), comm_block->dt() );

}

//----------------------------------------------------------------------

double EnzoMethodPpm3::timestep ( CommBlock * comm_block ) throw()
{
  double dt = std::numeric_limits<double>::max();

  INCOMPLETE("EnzoMethodPpm3::timestep()");

//       FORTRAN_NAME(calc_dt)(&GridRank, GridDimension, GridDimension+1,
// 			     GridDimension+2,
// 			     GridStartIndex, GridEndIndex,
// 			     GridStartIndex+1, GridEndIndex+1,
// 			     GridStartIndex+2, GridEndIndex+2,
// 			     &HydroMethod, &ZEUSQuadraticArtificialViscosity,
// 			     CellWidth[0], CellWidth[1], CellWidth[2],
// 			     GridVelocity, GridVelocity+1, GridVelocity+2,
// 			     &Gamma, &PressureFree, &afloat,
// 			     BaryonField[DensNum], pressure_field,
// 			     BaryonField[Vel1Num], BaryonField[Vel2Num],
// 			     BaryonField[Vel3Num], &dtBaryons, &dtViscous);


  return dt;
}

//   if (ProcessorNumber != MyProcessorNumber)
//     return huge_number;
 
//   this->DebugCheck("ComputeTimeStep");
 
//   /* initialize */
 
//   float dt, dtTemp;
//   float dtBaryons      = huge_number;
//   float dtViscous      = huge_number;
//   float dtParticles    = huge_number;
//   float dtExpansion    = huge_number;
//   float dtAcceleration = huge_number;
//   float dtMHD          = huge_number;
//   float dtConduction   = huge_number;
//   float dtGasDrag      = huge_number;
//   int dim, i, j, k, index, result;
 
//   /* Compute the field size. */
 
//   int size = 1;
//   for (dim = 0; dim < GridRank; dim++)
//     size *= GridDimension[dim];
 
//   /* If using comoving coordinates, compute the expansion factor a.  Otherwise,
//      set it to one. */
 
//   FLOAT a = 1, dadt;
//   if (ComovingCoordinates)
//     CosmologyComputeExpansionFactor(Time, &a, &dadt);
//   float afloat = float(a);
 
//   /* 1) Compute Courant condition for baryons. */
 
//   if (NumberOfBaryonFields > 0 && (HydroMethod != HD_RK) && (HydroMethod != MHD_RK)) {
 
//     /* Find fields: density, total energy, velocity1-3. */
 
//     int DensNum, GENum, Vel1Num, Vel2Num, Vel3Num, TENum;
//     if (this->IdentifyPhysicalQuantities(DensNum, GENum, Vel1Num, Vel2Num,
// 					 Vel3Num, TENum) == FAIL) {
//       fprintf(stderr, "ComputeTimeStep: IdentifyPhysicalQuantities error.\n");
//       exit(FAIL);
//     }

//     /* For one-zone free-fall test, just compute free-fall time. */
//     if (ProblemType == 63) {
//       float *force_factor = new float[size];
//       if (this->ComputeOneZoneCollapseFactor(force_factor) == FAIL) {
// 	ENZO_FAIL("Error in ComputeOneZoneCollapseFactor.\n");
//       }

//       dt = huge_number;
//       for (k = GridStartIndex[2]; k <= GridEndIndex[2]; k++) { // nothing
// 	for (j = GridStartIndex[1]; j <= GridEndIndex[1]; j++) { // metallicity
// 	  for (i = GridStartIndex[0]; i <= GridEndIndex[0]; i++) { // energy

// 	    index = i + j*GridDimension[0] + k*GridDimension[0]*GridDimension[1];

// 	    dt = min(dt, POW(((3 * pi) / 
// 			      (32 * GravitationalConstant * 
// 			       BaryonField[DensNum][index] *
// 			       (1 - force_factor[index]))), 0.5));
// 	  }
// 	}
//       }

//       delete [] force_factor;
//       dt *= TestProblemData.OneZoneFreefallTimestepFraction;
//       return dt;
//     }
 
//     /* Compute the pressure. */
 
//     float *pressure_field = new float[size];
//     this->ComputePressure(Time, pressure_field);
 
// #ifdef UNUSED
//     int Zero[3] = {0,0,0}, TempInt[3] = {0,0,0};
//     for (dim = 0; dim < GridRank; dim++)
//       TempInt[dim] = GridDimension[dim]-1;
// #endif /* UNUSED */
 
//     /* Call fortran routine to do calculation. */
 
//     if( HydroMethod != MHD_Li)
//       PFORTRAN_NAME(calc_dt)(&GridRank, GridDimension, GridDimension+1,
// 			     GridDimension+2,
// 			     GridStartIndex, GridEndIndex,
// 			     GridStartIndex+1, GridEndIndex+1,
// 			     GridStartIndex+2, GridEndIndex+2,
// 			     &HydroMethod, &ZEUSQuadraticArtificialViscosity,
// 			     CellWidth[0], CellWidth[1], CellWidth[2],
// 			     GridVelocity, GridVelocity+1, GridVelocity+2,
// 			     &Gamma, &PressureFree, &afloat,
// 			     BaryonField[DensNum], pressure_field,
// 			     BaryonField[Vel1Num], BaryonField[Vel2Num],
// 			     BaryonField[Vel3Num], &dtBaryons, &dtViscous);
 

//     if(HydroMethod == MHD_Li){
//       /* 1.5) Calculate minimum dt due to MHD: Maximum Fast MagnetoSonic Shock Speed */
      
//       //Cosmos nees this, for some reason.
//       if(GridRank < 3 ){
// 	if( CellWidth[2] == NULL ) CellWidth[2] = new FLOAT;
// 	CellWidth[2][0] = 1.0;
// 	if( GridRank < 2 ){
// 	  if( CellWidth[1] == NULL ) CellWidth[1] = new FLOAT;
// 	  CellWidth[1][0] = 1.0;
// 	}
//       }
//       int Rank_Hack = 3; //MHD needs a 3d timestep always.
//       FORTRAN_NAME(mhd_dt)(CenteredB[0], CenteredB[1], CenteredB[2],
// 			   BaryonField[Vel1Num], BaryonField[Vel2Num], BaryonField[Vel3Num],
// 			   BaryonField[DensNum], pressure_field, &Gamma, &dtMHD, 
// 			   CellWidth[0], CellWidth[1], CellWidth[2],
// 			   GridDimension, GridDimension + 1, GridDimension +2, &Rank_Hack,
// 			   GridStartIndex, GridEndIndex,
// 			   GridStartIndex+1, GridEndIndex+1,
// 			   GridStartIndex+2, GridEndIndex+2, BaryonField[TENum]);
      
//       dtMHD *= CourantSafetyNumber;
//       dtMHD *= afloat;  
//     }//if HydroMethod== MHD_Li

//     /* Clean up */
 
//     delete [] pressure_field;
 
//     /* Multiply resulting dt by CourantSafetyNumber (for extra safety!). */
 
//     dtBaryons *= CourantSafetyNumber;
 
//   }


//   if (NumberOfBaryonFields > 0 && 
//       (HydroMethod == HD_RK)) {

//     int DensNum, GENum, Vel1Num, Vel2Num, Vel3Num, TENum;
//     if (this->IdentifyPhysicalQuantities(DensNum, GENum, Vel1Num, Vel2Num, 
// 					 Vel3Num, TENum) == FAIL) {
//       fprintf(stderr, "ComputeTimeStep: IdentifyPhysicalQuantities error.\n");
//       exit(FAIL);
//     }

//     FLOAT dxinv = 1.0 / CellWidth[0][0]/a;
//     FLOAT dyinv = (GridRank > 1) ? 1.0 / CellWidth[1][0]/a : 0.0;
//     FLOAT dzinv = (GridRank > 2) ? 1.0 / CellWidth[2][0]/a : 0.0;
//     float dt_temp = 1.e-20, dt_ltemp, dt_x, dt_y, dt_z;
//     float rho, p, vx, vy, vz, v2, eint, etot, h, cs, dpdrho, dpde,
//       v_signal_x, v_signal_y, v_signal_z;
//     int n = 0;
//     for (int k = 0; k < GridDimension[2]; k++) {
//       for (int j = 0; j < GridDimension[1]; j++) {
// 	for (int i = 0; i < GridDimension[0]; i++, n++) {
// 	  rho = BaryonField[DensNum][n];
// 	  vx  = BaryonField[Vel1Num][n];
// 	  vy  = BaryonField[Vel2Num][n];
// 	  vz  = BaryonField[Vel3Num][n];

// 	  if (DualEnergyFormalism) {
// 	    eint = BaryonField[GENum][n];
// 	  }
// 	  else {
// 	    etot = BaryonField[TENum][n];
// 	    v2 = vx*vx + vy*vy + vz*vz;
// 	    eint = etot - 0.5*v2;
// 	  }

// 	  EOS(p, rho, eint, h, cs, dpdrho, dpde, EOSType, 2);

// 	  v_signal_y = v_signal_z = 0;

// 	  v_signal_x = (cs + fabs(vx));
// 	  if (GridRank > 1) v_signal_y = (cs + fabs(vy));
// 	  if (GridRank > 2) v_signal_z = (cs + fabs(vz));

// 	  dt_x = v_signal_x * dxinv;
// 	  dt_y = v_signal_y * dyinv;
// 	  dt_z = v_signal_z * dzinv;

// 	  dt_ltemp = my_MAX(dt_x, dt_y, dt_z);

// 	  if (dt_ltemp > dt_temp) {
// 	    dt_temp = dt_ltemp;
// 	  }
//         }
//       }
//     }

//     dtBaryons = CourantSafetyNumber / dt_temp;

//   }


//   // MHD
//   if (NumberOfBaryonFields > 0 && HydroMethod == MHD_RK) {

//     int DensNum, GENum, TENum, Vel1Num, Vel2Num, Vel3Num, 
//       B1Num, B2Num, B3Num, PhiNum;
//     if (this->IdentifyPhysicalQuantities(DensNum, GENum, Vel1Num, Vel2Num, 
// 					 Vel3Num, TENum, B1Num, B2Num, B3Num, PhiNum) == FAIL)
//       ENZO_FAIL("Error in IdentifyPhysicalQuantities.");

//     FLOAT dxinv = 1.0 / CellWidth[0][0]/a;
//     FLOAT dyinv = (GridRank > 1) ? 1.0 / CellWidth[1][0]/a : 0.0;
//     FLOAT dzinv = (GridRank > 2) ? 1.0 / CellWidth[2][0]/a : 0.0;
//     float vxm, vym, vzm, Bm, rhom;
//     float dt_temp = 1.e-20, dt_ltemp, dt_x, dt_y, dt_z;
//     float rho, p, vx, vy, vz, v2, eint, etot, h, cs, cs2, dpdrho, dpde,
//       v_signal_x, v_signal_y, v_signal_z, cf, cf2, temp1, Bx, By, Bz, B2, ca2;
//     int n = 0;
//     float rho_dt, B_dt, v_dt;
//     for (int k = 0; k < GridDimension[2]; k++) {
//       for (int j = 0; j < GridDimension[1]; j++) {
// 	for (int i = 0; i < GridDimension[0]; i++, n++) {
// 	  rho = BaryonField[DensNum][n];
// 	  vx  = BaryonField[Vel1Num][n];
// 	  vy  = BaryonField[Vel2Num][n];
// 	  vz  = BaryonField[Vel3Num][n];
// 	  Bx  = BaryonField[B1Num][n];
// 	  By  = BaryonField[B2Num][n];
// 	  Bz  = BaryonField[B3Num][n];

//           B2 = Bx*Bx + By*By + Bz*Bz;
// 	  if (DualEnergyFormalism) {
// 	    eint = BaryonField[GENum][n];
// 	  }
// 	  else {
// 	    etot = BaryonField[TENum][n];
// 	    v2 = vx*vx + vy*vy + vz*vz;
// 	    eint = etot - 0.5*v2 - 0.5*B2/rho;
// 	  }

// 	  v_signal_y = v_signal_z = 0;

// 	  EOS(p, rho, eint, h, cs, dpdrho, dpde, EOSType, 2);
// 	  cs2 = cs*cs;
// 	  temp1 = cs2 + B2/rho;

// 	  ca2 = Bx*Bx/rho;
// 	  cf2 = 0.5 * (temp1 + sqrt(temp1*temp1 - 4.0*cs2*ca2));
// 	  cf = sqrt(cf2);
// 	  v_signal_x = (cf + fabs(vx));

// 	  if (GridRank > 1) {
// 	    ca2 = By*By/rho;
// 	    cf2 = 0.5 * (temp1 + sqrt(temp1*temp1 - 4.0*cs2*ca2));
// 	    cf = sqrt(cf2);
// 	    v_signal_y = (cf + fabs(vy));
// 	  }

// 	  if (GridRank > 2) {
// 	    ca2 = Bz*Bz/rho;
// 	    cf2 = 0.5 * (temp1 + sqrt(temp1*temp1 - 4.0*cs2*ca2));
// 	    cf = sqrt(cf2);
// 	    v_signal_z = (cf + fabs(vz));
// 	  }

// 	  dt_x = v_signal_x * dxinv;
// 	  dt_y = v_signal_y * dyinv;
// 	  dt_z = v_signal_z * dzinv;

// 	  dt_ltemp = my_MAX(dt_x, dt_y, dt_z);

// 	  if (dt_ltemp > dt_temp) {
// 	    dt_temp = dt_ltemp;
// 	    rho_dt = rho;
// 	    B_dt = sqrt(Bx*Bx+By*By+Bz*Bz);
// 	    v_dt = max(fabs(vx), fabs(vy));
// 	    v_dt = max(v_dt, fabs(vz));
// 	  }

//         }
//       }
//     }
//     dtMHD = CourantSafetyNumber / dt_temp;
//     //    fprintf(stderr, "ok %g %g %g\n", dt_x,dt_y,dt_z);
//     //    if (dtMHD*TimeUnits/yr < 5) {
//     //float ca = B_dt/sqrt(rho_dt)*VelocityUnits;
//     //printf("dt=%g, rho=%g, B=%g\n, v=%g, ca=%g, dt=%g", dtMHD*TimeUnits/yr, rho_dt*DensityUnits, B_dt*MagneticUnits, 
//     //    v_dt*VelocityUnits/1e5, ca/1e5, LengthUnits/(dxinv*ca)/yr*CourantSafetyNumber);
//     // }

//   } // HydroMethod = MHD_RK

 
//   /* 2) Calculate dt from particles. */
 
//   if (NumberOfParticles > 0 || NumberOfActiveParticles > 0) {
 
//     /* Compute dt constraint from particle velocities. */
 
//     for (dim = 0; dim < GridRank; dim++) {
//       float dCell = CellWidth[dim][0]*a;
//       for (i = 0; i < NumberOfParticles; i++) {
//         dtTemp = dCell/max(fabs(ParticleVelocity[dim][i]), tiny_number);
// 	dtParticles = min(dtParticles, dtTemp);
//       }
//       for (i = 0; i < NumberOfActiveParticles; i++) {
// 	dtTemp = dCell/max(fabs(ActiveParticles[i]->ReturnVelocity()[dim]), tiny_number);
// 	dtParticles = min(dtParticles, dtTemp);	
//       }
//     }
 
//     /* Multiply resulting dt by ParticleCourantSafetyNumber. */
 
//     dtParticles *= ParticleCourantSafetyNumber;
 
//   }
 

//   /* 3) Find dt from expansion. */
 
//   if (ComovingCoordinates)
//     if (CosmologyComputeExpansionTimestep(Time, &dtExpansion) == FAIL) {
//       fprintf(stderr, "nudt: Error in ComputeExpansionTimestep.\n");
//       exit(FAIL);
//     }
 
//   /* 4) Calculate minimum dt due to acceleration field (if present). */
 
//   if (SelfGravity) {
//     for (dim = 0; dim < GridRank; dim++)
//       if (AccelerationField[dim])
// 	for (i = 0; i < size; i++) {
// 	  dtTemp = sqrt(CellWidth[dim][0]/
// 			fabs(AccelerationField[dim][i])+tiny_number);
// 	  dtAcceleration = min(dtAcceleration, dtTemp);
// 	}
//     if (dtAcceleration != huge_number)
//       dtAcceleration *= 0.5;
//   }

//   /* 5) Calculate minimum dt due to thermal conduction. */

//   if(IsotropicConduction || AnisotropicConduction){
//     if (this->ComputeConductionTimeStep(dtConduction) == FAIL) 
//       ENZO_FAIL("Error in ComputeConductionTimeStep.\n");

//     dtConduction *= float(NumberOfGhostZones);     // for subcycling 
//   }

//   /* 6) GasDrag time step */
//   if (UseGasDrag && GasDragCoefficient != 0.) {
//     dtGasDrag = 0.5/GasDragCoefficient;
//   }


//   /* 7) calculate minimum timestep */
 
//   dt = min(dtBaryons, dtParticles);
//   dt = min(dt, dtMHD);
//   dt = min(dt, dtViscous);
//   dt = min(dt, dtAcceleration);
//   dt = min(dt, dtExpansion);
//   dt = min(dt, dtConduction);
//   dt = min(dt, dtGasDrag);

// #ifdef TRANSFER

//   /* 8) If star formation, set a minimum timestep */

//   float TemperatureUnits, DensityUnits, LengthUnits, 
//     VelocityUnits, TimeUnits, aUnits = 1;

//   if (GetUnits(&DensityUnits, &LengthUnits, &TemperatureUnits,
// 	       &TimeUnits, &VelocityUnits, Time) == FAIL) {
//     ENZO_FAIL("Error in GetUnits.");
//   }

//   float mindtNOstars;  // Myr
//   const int NumberOfStepsInLifetime = 5;
//   float dtStar = huge_number;

//   if (STARFEED_METHOD(POP3_STAR))
//     mindtNOstars = 3;  // Myr
//   if (STARFEED_METHOD(STAR_CLUSTER))
//     mindtNOstars = 10;  // Myr

//   if (STARFEED_METHOD(POP3_STAR) || STARFEED_METHOD(STAR_CLUSTER))
//     if (G_TotalNumberOfStars > 0 && minStarLifetime < 1e6)
//       dtStar = minStarLifetime/NumberOfStepsInLifetime;
//     else
//       dtStar = 3.1557e13*mindtNOstars/TimeUnits;

//   dt = min(dt, dtStar);



//   /* 9) If using radiation pressure, calculate minimum dt */

//   float dtRadPressure = huge_number;
//   float absVel, absAccel;

//   if (RadiationPressure && RadiativeTransfer) {

//     int RPresNum1, RPresNum2, RPresNum3;
//     if (IdentifyRadiationPressureFields(RPresNum1, RPresNum2, RPresNum3) 
// 	== FAIL) {
//       ENZO_FAIL("Error in IdentifyRadiationPressureFields.");
//     }

//     for (i = 0; i < size; i++)
//       for (dim = 0; dim < GridRank; dim++) {
// 	dtTemp = sqrt(CellWidth[dim][0] / (fabs(BaryonField[RPresNum1+dim][i])+
// 					   tiny_number));
// 	dtRadPressure = min(dtRadPressure, dtTemp);
//       }
    
//     if (dtRadPressure < huge_number)
//       dtRadPressure *= 0.5;

//     dt = min(dt, dtRadPressure);

//   } /* ENDIF RadiationPressure */

//   /* 10) Safety Velocity to limit timesteps */

//   float dtSafetyVelocity = huge_number;
//   if (TimestepSafetyVelocity > 0)
//     dtSafetyVelocity = a*CellWidth[0][0] / 
//       (TimestepSafetyVelocity*1e5 / VelocityUnits);    // parameter in km/s

//   dt = min(dt, dtSafetyVelocity);


//   /* 9) FLD Radiative Transfer timestep limitation */
//   if (RadiativeTransferFLD)
//     dt = min(dt, MaxRadiationDt);
  
// #endif /* TRANSFER */
 
//   /* Debugging info. */
  
//   if (debug1) {
//     printf("ComputeTimeStep = %"ESYM" (", dt);
//     if (HydroMethod != MHD_RK && HydroMethod != MHD_Li && NumberOfBaryonFields > 0)
//       printf("Bar = %"ESYM" ", dtBaryons);
//     if (HydroMethod == MHD_RK || HydroMethod == MHD_Li)
//       printf("dtMHD = %"ESYM" ", dtMHD);
//     if (HydroMethod == Zeus_Hydro)
//       printf("Vis = %"ESYM" ", dtViscous);
//     if (ComovingCoordinates)
//       printf("Exp = %"ESYM" ", dtExpansion);
//     if (dtAcceleration != huge_number)
//       printf("Acc = %"ESYM" ", dtAcceleration);
//     if (NumberOfParticles)
//       printf("Part = %"ESYM" ", dtParticles);
//     if (IsotropicConduction || AnisotropicConduction)
//       printf("Cond = %"ESYM" ",(dtConduction));
//     if (UseGasDrag)
//       printf("Drag = %"ESYM" ",(dtGasDrag));
//     printf(")\n");
//   }
 
//   return dt;
