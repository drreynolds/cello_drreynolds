#include "cello_hydro.h"

// Cosmology

int   ComovingCoordinates;
int   UseMinimumPressureSupport;
float MinimumPressureSupportParameter;
float ComovingBoxSize;
float HubbleConstantNow;
float OmegaMatterNow;
float OmegaLambdaNow;
float MaxExpansionRate;

// Chemistry

int MultiSpecies;

// Gravity

int GravityOn;
float *AccelerationField[MAX_DIMENSION]; // cell cntr acceleration at n+1/2

// Physics

int PressureFree;
float Gamma;
float GravitationalConstant;

// Problem-specific

int ProblemType;

// Method PPM

int    PPMFlatteningParameter;
int    PPMDiffusionParameter;
int    PPMSteepeningParameter;

// Parallel

int    ProcessorNumber;

// Numerics

int   DualEnergyFormalism;
float DualEnergyFormalismEta1;
float DualEnergyFormalismEta2;
float pressure_floor;
float density_floor;
float number_density_floor;
float temperature_floor;

// Control

float  time_stop;   // stop after this simulation time
int    cycle_stop;  // stop after this number of cycles

float  CourantSafetyNumber;
FLOAT  InitialRedshift;
FLOAT  InitialTimeInCodeUnits;
FLOAT  Time;
FLOAT  OldTime;

// Domain

FLOAT DomainLeftEdge [MAX_DIMENSION];
FLOAT DomainRightEdge[MAX_DIMENSION];

// Grid

int field_density;
int field_total_energy;
int field_internal_energy;
int field_velocity_x;
int field_velocity_y;
int field_velocity_z;

int    GridRank;
int    GridDimension[MAX_DIMENSION];   // total dimensions of all grids
int    GridStartIndex[MAX_DIMENSION];  // starting index of the active region
int    GridEndIndex[MAX_DIMENSION];    // stoping index of the active region
FLOAT  GridLeftEdge[MAX_DIMENSION];    // starting pos (active problem space)
FLOAT *CellWidth[MAX_DIMENSION];

// Fields

int    NumberOfBaryonFields;                        // active baryon fields
float *BaryonField[MAX_NUMBER_OF_BARYON_FIELDS];    // pointers to arrays
float *OldBaryonField[MAX_NUMBER_OF_BARYON_FIELDS]; // pointers to old arrays
int    FieldType[MAX_NUMBER_OF_BARYON_FIELDS];



