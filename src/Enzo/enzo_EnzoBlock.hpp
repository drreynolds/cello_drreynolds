// $Id: enzo_EnzoBlock.hpp 2009 2011-02-22 19:43:07Z bordner $
// See LICENSE_CELLO file for license and copyright information

#ifndef ENZO_ENZO_BLOCK_HPP
#define ENZO_ENZO_BLOCK_HPP

/// @file     enzo_EnzoBlock.hpp
/// @author   James Bordner (jobordner@ucsd.edu)
/// @date     Thu Mar  3 23:01:51 PST 2011
/// @todo     Change public attributes to private
/// @todo     Dynamically allocate arrays
/// @brief    [\ref Enzo] Declaration of the EnzoBlock class

#define OMEGA_TOLERANCE 1.0e-5
 
#ifdef CONFIG_PRECISION_SINGLE
#   define ETA_TOLERANCE 1.0e-5
#endif
#ifdef CONFIG_PRECISION_DOUBLE
#   define ETA_TOLERANCE 1.0e-10
#endif
#ifdef CONFIG_PRECISION_QUADRUPLE
#   define ETA_TOLERANCE 1.0e-20
#endif

//----------------------------------------------------------------------

struct fluxes
{
  long_int LeftFluxStartGlobalIndex [MAX_DIMENSION][MAX_DIMENSION];
  long_int LeftFluxEndGlobalIndex   [MAX_DIMENSION][MAX_DIMENSION];
  long_int RightFluxStartGlobalIndex[MAX_DIMENSION][MAX_DIMENSION];
  long_int RightFluxEndGlobalIndex  [MAX_DIMENSION][MAX_DIMENSION];
  enzo_float *LeftFluxes [MAX_NUMBER_OF_BARYON_FIELDS][MAX_DIMENSION];
  enzo_float *RightFluxes[MAX_NUMBER_OF_BARYON_FIELDS][MAX_DIMENSION];
};

//----------------------------------------------------------------------

class EnzoBlock
#ifdef CONFIG_USE_CHARM
  : public BlockCharm
#else
  : public BlockMpi
#endif
{

  /// @class    EnzoBlock
  /// @ingroup  Enzo
  /// @brief    [\ref Enzo] An EnzoBlock is a Block with Enzo data

public: // interface

#ifdef CONFIG_USE_CHARM

  EnzoBlock
  ( int nx, int ny, int nz,
    double xm, double ym, double zm,
    double hx, double hy, double hz,
    int num_field_blocks) throw();

  EnzoBlock (CkMigrateMessage *m) {};

  EnzoBlock();

#else

  /// Initialize the EnzoBlock object
  EnzoBlock(int ix, int iy, int iz,
	    int nx, int ny, int nz,
	    double xm, double ym, double zm,
	    double xp, double yp, double zp,
	    int num_field_blocks) throw();

#endif

  /// Destructor
  virtual ~EnzoBlock() throw();

  /// Write attributes, e.g. to stdout for debugging
  void write(FILE *fp=stdout) throw ();

  //----------------------------------------------------------------------
  // Original Enzo functions
  //----------------------------------------------------------------------

  enzo_float ComputeTimeStep();
  enzo_float sum_field (int field);
  int ComputeGammaField(enzo_float *GammaField);
  int ComputePressureDualEnergyFormalism(enzo_float time, enzo_float *pressure);
  int ComputePressure(enzo_float time, enzo_float *pressure);
  int ComputeTemperatureField(enzo_float *temperature);
  int CosmologyComputeExpansionFactor(enzo_float time, enzo_float *a, enzo_float *dadt);
  int CosmologyComputeExpansionTimestep(enzo_float time, enzo_float *dtExpansion);
  int CosmologyGetUnits(enzo_float *DensityUnits, enzo_float *LengthUnits, enzo_float *TemperatureUnits, enzo_float *TimeUnits, enzo_float *VelocityUnits, enzo_float Time);
  int FindField(int field, int farray[], int numfields);
  int IdentifyPhysicalQuantities(int &DensNum, int &GENum, int &Vel1Num, int &Vel2Num, int &Vel3Num, int &TENum);
  int IdentifySpeciesFields(int &DeNum, int &HINum, int &HIINum, int &HeINum, int &HeIINum, int &HeIIINum, int &HMNum, int &H2INum, int &H2IINum, int &DINum, int &DIINum, int &HDINum);
  int SetExternalBoundaryValues();
  int SetMinimumSupport(enzo_float &MinimumSupportEnergyCoefficient);
  int SolveHydroEquations ( int CycleNumber, enzo_float dt);
  void print_field (int field);
  int SetExternalBoundary(int FieldRank, int GridDims[], int GridOffset[], int StartIndex[], int EndIndex[], enzo_float *Field, int FieldType);
  void image_dump(const char * file_root, int cycle, double lower, double upper);

  void initialize_hydro ();
  void initialize_image ();
  void initialize_implosion3 (int size_param);
  void initialize_implosion (int size_param);
  void initialize_ppml_implosion3 (int size_param);

  int SolveMHDEquations( int cycle, enzo_float dt);
  void initialize_ppml (int size_param);

public: // functions (TEMPORARILY PUBLIC)

  void initialize () throw();

public: // attributes (YIKES!)

  int CycleNumber;

  enzo_float Time;
  enzo_float OldTime;
  enzo_float dt;

  /// cell cntr acceleration at n+1/2
  enzo_float *AccelerationField[MAX_DIMENSION]; 

  /// Fluxes
  fluxes ** SubgridFluxes;

  /// starting pos (active problem space)
  enzo_float GridLeftEdge[MAX_DIMENSION]; 
  /// total dimensions of all grids
  int GridDimension[MAX_DIMENSION]; 
  /// starting index of the active region
  int GridStartIndex[MAX_DIMENSION]; 
  /// stoping index of the active region
  int GridEndIndex[MAX_DIMENSION]; 
  enzo_float CellWidth[MAX_DIMENSION];
  /// pointers to arrays
  enzo_float *BaryonField[MAX_NUMBER_OF_BARYON_FIELDS]; 
  /// pointers to old arrays
  enzo_float *OldBaryonField[MAX_NUMBER_OF_BARYON_FIELDS]; 

};

#endif /* ENZO_ENZO_BLOCK_HPP */

