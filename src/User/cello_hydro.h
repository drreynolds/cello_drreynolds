#ifndef CELLO_HYDRO_H
#define CELLO_HYDRO_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "defines.h"
#include "monitor.hpp"

//----------------------------------------------------------------------
// DEFINES
//----------------------------------------------------------------------

#define WARNING(MESSAGE) printf ("%s:%d WARNING: %s\n",__FILE__,__LINE__,MESSAGE);

#define ENZO_FAIL                           0 // Error handling
#define ENZO_SUCCESS                        1 // Error handling

#define FALSE                               0 // Needed for fortran
#define TRUE                                1 // Needed for fortran

#define ENZO_FLOAT                          double // Scalar
#define ENZO_FLOAT_UNDEFINED              -99999.0 // use NaN: CosmologyComputeExpansionFactor()
#define ISYM                              "d" // Scalar

#define MAX_DIMENSION                       3 // for array declarations and loops in SolveHydro
#define MAX_NUMBER_OF_BARYON_FIELDS         28 // for array declarations and loops in SolveHydro

#define SIGN(A)   ((A) >  0  ?  1  : -1 )     // upper-case inline function
#define NINT(A)   ((int) ((A) + 0.5*SIGN(A)) ) // rename to round(), upper-case inline function

// defined in cello.h
// #define MIN(A,B)  ((A) < (B) ? (A) : (B))      // upper-case inline function
// #define MAX(A,B)  ((A) > (B) ? (A) : (B))      // upper-case inline function

// #define FORTRAN_NAME(NAME) NAME
#define FORTRAN_NAME(NAME) NAME##_

//----------------------------------------------------------------------

const int Density         = 0;  // Field identifiers: use Field's instead
const int TotalEnergy     = 1;
const int InternalEnergy  = 2;
const int Velocity1       = 4;
const int Velocity2       = 5;
const int Velocity3       = 6;
const int ElectronDensity = 7;
const int HIDensity       = 8;
const int HIIDensity      = 9;
const int HeIDensity      = 10;
const int HeIIDensity     = 11;
const int HeIIIDensity    = 12;
const int HMDensity       = 13;
const int H2IDensity      = 14;
const int H2IIDensity     = 15;
const int DIDensity       = 16;
const int DIIDensity      = 17;
const int HDIDensity      = 18;
const int Metallicity     = 19;

//----------------------------------------------------------------------
// TYPEDEFS
//----------------------------------------------------------------------

typedef int            Eint32;     // c_message only
typedef long long      long_int;   // use long long
typedef long long int  Elong_int;  // use long long
typedef long long unsigned  global_index; // 

//----------------------------------------------------------------------
// EXTERNS
//----------------------------------------------------------------------

// extern int    ComovingCoordinates;
extern int    UseMinimumPressureSupport;
extern float  MinimumPressureSupportParameter;
extern float  ComovingBoxSize;
extern float  HubbleConstantNow;
extern float  OmegaLambdaNow;
extern float  OmegaMatterNow;
extern float  MaxExpansionRate;

  // Chemistry

extern int    MultiSpecies;

  // Gravity

extern int    GravityOn;
extern float *AccelerationField[MAX_DIMENSION];

  // Physics

extern int    PressureFree;
extern float  Gamma;
extern float  GravitationalConstant;

  // Problem-specific

extern int    ProblemType;

  // Method PPM

extern int    PPMFlatteningParameter;
extern int    PPMDiffusionParameter;
extern int    PPMSteepeningParameter;

  // Parallel

extern int    ProcessorNumber;

  // Numerics

extern int    DualEnergyFormalism;
extern float  DualEnergyFormalismEta1;
extern float  DualEnergyFormalismEta2;
extern float  pressure_floor;
extern float  density_floor;
extern float  number_density_floor;
extern float  temperature_floor;

extern float  CourantSafetyNumber;
extern ENZO_FLOAT  InitialRedshift;
extern ENZO_FLOAT  InitialTimeInCodeUnits;
extern ENZO_FLOAT  Time;
extern ENZO_FLOAT  OldTime;

  // Domain

extern ENZO_FLOAT  DomainLeftEdge [MAX_DIMENSION];
extern ENZO_FLOAT  DomainRightEdge[MAX_DIMENSION];

  // Grid

extern int    GridRank;
extern int    GridDimension[MAX_DIMENSION];
extern int    GridStartIndex[MAX_DIMENSION];
extern int    GridEndIndex[MAX_DIMENSION];
extern ENZO_FLOAT  GridLeftEdge[MAX_DIMENSION];
extern ENZO_FLOAT *CellWidth[MAX_DIMENSION];
extern int    ghost_depth[MAX_DIMENSION];

  // Fields

extern int field_density;
extern int field_total_energy;
extern int field_internal_energy;
extern int field_velocity_x;
extern int field_velocity_y;
extern int field_velocity_z;
extern int field_color;

extern int field_magnetic_x;
extern int field_magnetic_y;
extern int field_magnetic_z;

extern int field_density_xp;
extern int field_velocity_x_xp;
extern int field_velocity_y_xp;
extern int field_velocity_z_xp;
extern int field_magnetic_x_xp;
extern int field_magnetic_y_xp;
extern int field_magnetic_z_xp;

extern int field_density_yp;
extern int field_velocity_x_yp;
extern int field_velocity_y_yp;
extern int field_velocity_z_yp;
extern int field_magnetic_x_yp;
extern int field_magnetic_y_yp;
extern int field_magnetic_z_yp;

extern int field_density_zp;
extern int field_velocity_x_zp;
extern int field_velocity_y_zp;
extern int field_velocity_z_zp;
extern int field_magnetic_x_zp;
extern int field_magnetic_y_zp;
extern int field_magnetic_z_zp;


extern int    NumberOfBaryonFields;
extern float *BaryonField[MAX_NUMBER_OF_BARYON_FIELDS];
extern float *OldBaryonField[MAX_NUMBER_OF_BARYON_FIELDS];
extern int    FieldType[MAX_NUMBER_OF_BARYON_FIELDS];

  // Boundary

  enum bc_type 
    {
      bc_unknown    = 0, 
      bc_reflecting = 1, 
      bc_outflow    = 2, 
      bc_inflow     = 3, 
      bc_periodic   = 4
    };

extern int      BoundaryRank;
extern int      BoundaryDimension[MAX_DIMENSION];
extern int      BoundaryFieldType[MAX_NUMBER_OF_BARYON_FIELDS];
extern bc_type *BoundaryType[MAX_NUMBER_OF_BARYON_FIELDS][MAX_DIMENSION][2];
extern float   *BoundaryValue[MAX_NUMBER_OF_BARYON_FIELDS][MAX_DIMENSION][2];  

  // problem

extern int   CycleNumber;
extern float dt;



//----------------------------------------------------------------------
// FUNCTION PROTOTYPES
//----------------------------------------------------------------------

int ComputeGammaField(float *GammaField);

int ComputePressure(ENZO_FLOAT time, float *pressure);

int ComputePressureDualEnergyFormalism(ENZO_FLOAT time, float *pressure);

int ComputeTemperatureField(float *temperature);

int IdentifyPhysicalQuantities(int &DensNum, int &GENum,   int &Vel1Num, 
			       int &Vel2Num, int &Vel3Num, int &TENum);

int IdentifySpeciesFields(int &DeNum, int &HINum, int &HIINum, 
			  int &HeINum, int &HeIINum, int &HeIIINum,
			  int &HMNum, int &H2INum, int &H2IINum,
			  int &DINum, int &DIINum, int &HDINum);

int SetMinimumSupport(float &MinimumSupportEnergyCoefficient);

int SolveHydroEquations(int cycle, float dt);
int SolveMHDEquations(int cycle, float dt);

float ComputeTimeStep();
int SetExternalBoundaryValues();
int SetExternalBoundary(int FieldRank, 
			int GridDims[],
			int GridOffset[],
			int StartIndex[], 
			int EndIndex[],
			float *Field, 
			int FieldType);



class Enzo;
void initialize_hydro (Enzo * enzo);

// PPM

void initialize_implosion (int size);
void initialize_implosion3 (int size);
void initialize_image ();

// PPML

void initialize_ppml (int size);
void initialize_ppml_implosion3 (int size);

float sum_field (int field);
float print_field (int field);
void data_dump (const char * file_root, int cycle);

void image_dump (const char * file_root, 
		 int          cycle, 
		 double       lower, 
		 double       upper,
		 Monitor *    monitor);

// timestep
int CosmologyComputeExpansionTimestep(ENZO_FLOAT time, float *dtExpansion);
int CosmologyComputeExpansionFactor(ENZO_FLOAT time, ENZO_FLOAT *a, ENZO_FLOAT *dadt);

extern "C" void FORTRAN_NAME(calc_dt)(
                  int *rank, int *idim, int *jdim, int *kdim,
                  int *i1, int *i2, int *j1, int *j2, int *k1, int *k2,
                  ENZO_FLOAT *dx, ENZO_FLOAT *dy, ENZO_FLOAT *dz, 
		  float *gamma, int *ipfree, float *aye,
                  float *d, float *p, float *u, float *v, float *w,
			     float *dt, float *dtviscous);
 
 
extern "C" void FORTRAN_NAME(calc_dt_ppml)(
                  int *idim, int *jdim, int *kdim,
                  int *i1, int *i2, int *j1, int *j2, int *k1, int *k2,
                  ENZO_FLOAT *dx, ENZO_FLOAT *dy, ENZO_FLOAT *dz,
                  float *dn, float *vx, float *vy, float *vz, 
                             float *bx, float *by, float *bz, 
			     float *dt);
 
#endif /* CELLO_HYDRO_H */

