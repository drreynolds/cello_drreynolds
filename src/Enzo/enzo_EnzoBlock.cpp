// See LICENSE_CELLO file for license and copyright information

/// @file     enzo_EnzoBlock.cpp
/// @author   James Bordner (jobordner@ucsd.edu)
/// @date     Thu Mar  3 23:02:02 PST 2011
/// @brief    Implementation of the EnzoBlock class

#include "cello.hpp"

#include "enzo.hpp"

//======================================================================


EnzoBlock::EnzoBlock
(
 int ix, int iy, int iz,
 int nbx, int nby, int nbz,
 int nx, int ny, int nz,
 double xm, double ym, double zm,
 double xp, double yp, double zp,
 int num_field_blocks) throw()
  : Block (ix,iy,iz,
	   nbx,nby,nbz,nx,ny,nz,xm,ym,zm,xp,yp,zp,num_field_blocks),
    Time_(0),
    CycleNumber(0),
    OldTime(0),
    dt(0),
    SubgridFluxes(0)
{
  TRACE1 ("sizeof EnzoBlock = %d",sizeof(EnzoBlock));
  int i,j;

  for (i=0; i<MAX_DIMENSION; i++) {
    AccelerationField[i] = 0;

    for (i=0; i<MAX_DIMENSION; i++) {
      AccelerationField[i] = 0;
      GridLeftEdge[i] = 0;
      GridDimension[i] = 0;
      GridStartIndex[i] = 0;
      GridEndIndex[i] = 0;
      CellWidth[i] = 0;
    }

    for (j=0; j<MAX_NUMBER_OF_BARYON_FIELDS; j++) {
      BaryonField[j] = 0;
      OldBaryonField[j] = 0;
    }

  }
}

#ifdef CONFIG_USE_CHARM

EnzoBlock::EnzoBlock
(
 int nbx, int nby, int nbz,
 int nx, int ny, int nz,
 double xm, double ym, double zm,
 double xp, double yp, double zp,
 int num_field_blocks) throw()
  : Block (nbx,nby,nbz,nx,ny,nz,xm,ym,zm,xp,yp,zp,num_field_blocks),
    Time_(0),
    CycleNumber(0),
    OldTime(0),
    dt(0),
    SubgridFluxes(0)
{
  TRACE1 ("sizeof EnzoBlock = %d",sizeof(EnzoBlock));
  int i,j;
  for (i=0; i<MAX_DIMENSION; i++) {
    AccelerationField[i] = 0;

    for (i=0; i<MAX_DIMENSION; i++) {
      AccelerationField[i] = 0;
      GridLeftEdge[i] = 0;
      GridDimension[i] = 0;
      GridStartIndex[i] = 0;
      GridEndIndex[i] = 0;
      CellWidth[i] = 0;
    }

    for (j=0; j<MAX_NUMBER_OF_BARYON_FIELDS; j++) {
      BaryonField[j] = 0;
      OldBaryonField[j] = 0;
    }

  }
}

#endif

//----------------------------------------------------------------------

EnzoBlock::~EnzoBlock() throw ()
{
}

//======================================================================

void EnzoBlock::write(FILE * fp) throw ()
{
  fprintf (fp,"EnzoBlock: ComovingCoordinates %d\n",
	   ComovingCoordinates);
  fprintf (fp,"EnzoBlock: UseMinimumPressureSupport %d\n",
	   UseMinimumPressureSupport);
  fprintf (fp,"EnzoBlock: MinimumPressureSupportParameter %g\n",
	   MinimumPressureSupportParameter);
  fprintf (fp,"EnzoBlock: ComovingBoxSize %g\n",
	   ComovingBoxSize);
  fprintf (fp,"EnzoBlock: HubbleConstantNow %g\n",
	   HubbleConstantNow);
  fprintf (fp,"EnzoBlock: OmegaLambdaNow %g\n",
	   OmegaLambdaNow);
  fprintf (fp,"EnzoBlock: OmegaMatterNow %g\n",
	   OmegaMatterNow);
  fprintf (fp,"EnzoBlock: MaxExpansionRate %g\n",
	   MaxExpansionRate);

  // Chemistry

  fprintf (fp,"EnzoBlock: MultiSpecies %d\n",
	   MultiSpecies);

  // Gravity

  fprintf (fp,"EnzoBlock: GravityOn %d\n",
	   GravityOn);
  //  fprintf (fp,"EnzoBlock: *AccelerationField %g\n",
  //           *AccelerationField[MAX_DIMENSION)];

  // Physics

  fprintf (fp,"EnzoBlock: PressureFree %d\n",
	   PressureFree);
  fprintf (fp,"EnzoBlock: Gamma %g\n",
	   Gamma);
  fprintf (fp,"EnzoBlock: GravitationalConstant %g\n",
	   GravitationalConstant);

  // Problem-specific

  fprintf (fp,"EnzoBlock: ProblemType %d\n",
	   ProblemType);

  // Method PPM

  fprintf (fp,"EnzoBlock: PPMFlatteningParameter %d\n",
	   PPMFlatteningParameter);
  fprintf (fp,"EnzoBlock: PPMDiffusionParameter %d\n",
	   PPMDiffusionParameter);
  fprintf (fp,"EnzoBlock: PPMSteepeningParameter %d\n",
	   PPMSteepeningParameter);

  // Numerics

  fprintf (fp,"EnzoBlock: DualEnergyFormalism %d\n",
	   DualEnergyFormalism);
  fprintf (fp,"EnzoBlock: DualEnergyFormalismEta1 %g\n",
	   DualEnergyFormalismEta1);
  fprintf (fp,"EnzoBlock: DualEnergyFormalismEta2 %g\n",
	   DualEnergyFormalismEta2);
  fprintf (fp,"EnzoBlock: pressure_floor %g\n",
	   pressure_floor);
  fprintf (fp,"EnzoBlock: density_density_floor %g\n",
	   density_floor);
  fprintf (fp,"EnzoBlock: number_density_floor %g\n",
	   number_density_floor);
  fprintf (fp,"EnzoBlock: temperature_floor %g\n",
	   temperature_floor);

  fprintf (fp,"EnzoBlock: CourantSafetyNumber %g\n",
	   CourantSafetyNumber);
  fprintf (fp,"EnzoBlock: InitialRedshift %g\n",
	   InitialRedshift);
  fprintf (fp,"EnzoBlock: InitialTimeInCodeUnits %g\n",
	   InitialTimeInCodeUnits);
  fprintf (fp,"EnzoBlock: Time %g\n",
	   Time());
  fprintf (fp,"EnzoBlock: OldTime %g\n",
	   OldTime);

  // Domain

  fprintf (fp,"EnzoBlock: DomainLeftEdge %g %g %g\n",
	   DomainLeftEdge [0],DomainLeftEdge [0],DomainLeftEdge [0]);
  fprintf (fp,"EnzoBlock: DomainRightEdge %g %g %g\n",
	   DomainRightEdge[0],DomainRightEdge[1],DomainRightEdge[2]);

  // Fields

  if (field_density != -1) 
    fprintf (fp,"EnzoBlock: field_density %d\n", field_density);
  if (field_total_energy != -1) 
    fprintf (fp,"EnzoBlock: field_total_energy %d\n", field_total_energy);
  if (field_internal_energy != -1) 
    fprintf (fp,"EnzoBlock: field_internal_energy %d\n", field_internal_energy);
  if (field_velocity_x != -1) 
    fprintf (fp,"EnzoBlock: field_velocity_x %d\n", field_velocity_x);
  if (field_velocity_y != -1) 
    fprintf (fp,"EnzoBlock: field_velocity_y %d\n", field_velocity_y);
  if (field_velocity_z != -1) 
    fprintf (fp,"EnzoBlock: field_velocity_z %d\n", field_velocity_z);
  if (field_color != -1) 
    fprintf (fp,"EnzoBlock: field_color %d\n", field_color);

  if (field_velox != -1)
    fprintf (fp,"EnzoBlock: field_velox %d\n", field_velox);
  if (field_veloy != -1)
    fprintf (fp,"EnzoBlock: field_veloy %d\n", field_veloy);
  if (field_veloz != -1)
    fprintf (fp,"EnzoBlock: field_veloz %d\n", field_veloz);
  if (field_bfieldx != -1)
    fprintf (fp,"EnzoBlock: field_bfieldx %d\n", field_bfieldx);
  if (field_bfieldy != -1)
    fprintf (fp,"EnzoBlock: field_bfieldy %d\n", field_bfieldy);
  if (field_bfieldz != -1)
    fprintf (fp,"EnzoBlock: field_bfieldz %d\n", field_bfieldz);

  if (field_dens_rx != -1)
    fprintf (fp,"EnzoBlock: field_dens_rx %d\n", field_dens_rx);
  if (field_velox_rx != -1)
    fprintf (fp,"EnzoBlock: field_velox_rx %d\n", field_velox_rx);
  if (field_veloy_rx != -1)
    fprintf (fp,"EnzoBlock: field_veloy_rx %d\n", field_veloy_rx);
  if (field_veloz_rx != -1)
    fprintf (fp,"EnzoBlock: field_veloz_rx %d\n", field_veloz_rx);
  if (field_bfieldx_rx != -1)
    fprintf (fp,"EnzoBlock: field_bfieldx_rx %d\n", field_bfieldx_rx);
  if (field_bfieldy_rx != -1)
    fprintf (fp,"EnzoBlock: field_bfieldy_rx %d\n", field_bfieldy_rx);
  if (field_bfieldz_rx != -1)
    fprintf (fp,"EnzoBlock: field_bfieldz_rx %d\n", field_bfieldz_rx);

  if (field_dens_ry != -1)
    fprintf (fp,"EnzoBlock: field_dens_ry %d\n", field_dens_ry);
  if (field_velox_ry != -1)
    fprintf (fp,"EnzoBlock: field_velox_ry %d\n", field_velox_ry);
  if (field_veloy_ry != -1)
    fprintf (fp,"EnzoBlock: field_veloy_ry %d\n", field_veloy_ry);
  if (field_veloz_ry != -1)
    fprintf (fp,"EnzoBlock: field_veloz_ry %d\n", field_veloz_ry);
  if (field_bfieldx_ry != -1)
    fprintf (fp,"EnzoBlock: field_bfieldx_ry %d\n", field_bfieldx_ry);
  if (field_bfieldy_ry != -1)
    fprintf (fp,"EnzoBlock: field_bfieldy_ry %d\n", field_bfieldy_ry);
  if (field_bfieldz_ry != -1)
    fprintf (fp,"EnzoBlock: field_bfieldz_ry %d\n", field_bfieldz_ry);

  if (field_dens_rz != -1)
    fprintf (fp,"EnzoBlock: field_dens_rz %d\n", field_dens_rz);
  if (field_velox_rz != -1)
    fprintf (fp,"EnzoBlock: field_velox_rz %d\n", field_velox_rz);
  if (field_veloy_rz != -1)
    fprintf (fp,"EnzoBlock: field_veloy_rz %d\n", field_veloy_rz);
  if (field_veloz_rz != -1)
    fprintf (fp,"EnzoBlock: field_veloz_rz %d\n", field_veloz_rz);
  if (field_bfieldx_rz != -1)
    fprintf (fp,"EnzoBlock: field_bfieldx_rz %d\n", field_bfieldx_rz);
  if (field_bfieldy_rz != -1)
    fprintf (fp,"EnzoBlock: field_bfieldy_rz %d\n", field_bfieldy_rz);
  if (field_bfieldz_rz != -1)
    fprintf (fp,"EnzoBlock: field_bfieldz_rz %d\n", field_bfieldz_rz);

  // Grid

  fprintf (fp,"EnzoBlock: GridRank %d\n",    GridRank);
  fprintf (fp,"EnzoBlock: GridDimension %d %d %d\n",
	   GridDimension[0],GridDimension[1],GridDimension[2]);
  fprintf (fp,"EnzoBlock: GridStartIndex %d %d %d\n",
	   GridStartIndex[0],GridStartIndex[1],GridStartIndex[2]);
  fprintf (fp,"EnzoBlock: GridEndIndex %d %d %d\n",
	   GridEndIndex[0],GridEndIndex[1],GridEndIndex[2]);
  fprintf (fp,"EnzoBlock: GridLeftEdge %g %g %g\n",
	   GridLeftEdge[0],GridLeftEdge[1],GridLeftEdge[2]);

  fprintf (fp,"EnzoBlock: CellWidth %g %g %g\n", 
	   CellWidth[0], CellWidth[1], CellWidth[2] );

  fprintf (fp,"EnzoBlock: ghost %d %d %d\n",
	   ghost_depth[0],ghost_depth[1],ghost_depth[2]);


  fprintf (fp,"EnzoBlock: NumberOfBaryonFields %d\n",
	   NumberOfBaryonFields);
  int i;
  for (i=0; i<NumberOfBaryonFields; i++) {
    fprintf (fp,"EnzoBlock: BaryonField[%d] %p\n",
	     i, BaryonField[i]);
    fprintf (fp,"EnzoBlock: OldBaryonField[%d] %p\n",
	     i, OldBaryonField[i]);
    fprintf (fp,"EnzoBlock: FieldType[%d] %d\n",
	     i, FieldType[i]);
  }

  fprintf (fp,"EnzoBlock: BoundaryRank %d\n", BoundaryRank);
  fprintf (fp,"EnzoBlock: BoundaryDimension %d %d %d\n",
	   BoundaryDimension[0],BoundaryDimension[1],BoundaryDimension[2]);

  // unknown, reflecting, outflow, inflow, periodic
  //  const char * bc_string[] = 
  //    {"unknown", "reflecting", "outflow", "inflow", "periodic"};

  for (i=0; i<NumberOfBaryonFields; i++) {

    fprintf (fp,"EnzoBlock: BoundaryFieldType[%d] %d\n", 
	     i, BoundaryFieldType[i]);

    fprintf (fp,"EnzoBlock: BoundaryType[%d] %p %p %p %p %p %p\n", i, 
	     BoundaryType[i][0][0],
	     BoundaryType[i][0][1],
	     BoundaryType[i][1][0],
	     BoundaryType[i][1][1],
	     BoundaryType[i][2][0],
	     BoundaryType[i][2][1]);

    fprintf (fp,"EnzoBlock: BoundaryValue[%d] %p %p %p %p %p %p\n",i,
	     BoundaryValue[i][0][0],
	     BoundaryValue[i][0][1],
	     BoundaryValue[i][1][0],
	     BoundaryValue[i][1][1],
	     BoundaryValue[i][2][0],
	     BoundaryValue[i][2][1]);  
  }

  // problem

  fprintf (fp,"EnzoBlock: CycleNumber %d\n",   CycleNumber);
  fprintf (fp,"EnzoBlock: dt %g\n", dt);

  // fluxes

  fprintf (fp,"EnzoBlock: SubgridFluxes %p\n", SubgridFluxes);
  

}

//----------------------------------------------------------------------

void EnzoBlock::set_cycle (int cycle_start) throw ()
{

  Block::set_cycle (cycle_start);

  CycleNumber = cycle_start;
}

//----------------------------------------------------------------------

void EnzoBlock::set_time (double time_start) throw ()
{

  Block::set_time (time_start);

  Time_       = time_start;
  OldTime     = time_start;

}

//----------------------------------------------------------------------

void EnzoBlock::initialize () throw()
{

  Block::initialize();

  double xm,ym,zm;

  lower(&xm,&ym,&zm);

  GridLeftEdge[0]    = xm;
  GridLeftEdge[1]    = ym;
  GridLeftEdge[2]    = zm;

  // Grid dimensions

  int nx,ny,nz;
  field_block_[0] -> size (&nx,&ny,&nz);

  int gx,gy,gz;

  gx = enzo::ghost_depth[0];
  gy = enzo::ghost_depth[1];
  gz = enzo::ghost_depth[2];

  GridDimension[0]  = nx + 2*gx;
  GridDimension[1]  = ny + 2*gy;
  GridDimension[2]  = nz + 2*gz;

  GridStartIndex[0] = gx;
  GridStartIndex[1] = gy;
  GridStartIndex[2] = gz;

  GridEndIndex[0]   = gx + nx - 1;
  GridEndIndex[1]   = gy + ny - 1;
  GridEndIndex[2]   = gz + nz - 1;

  // Initialize CellWidth

  double hx,hy,hz;
  field_block_[0]->cell_width(this,&hx,&hy,&hz);

  CellWidth[0] = hx;
  CellWidth[1] = hy;
  CellWidth[2] = hz;

  // Initialize BaryonField[] pointers

  for (int field = 0; field < enzo::NumberOfBaryonFields; field++) {
    BaryonField[field] = (enzo_float *)field_block_[0]->field_values(field);
  }

}

//----------------------------------------------------------------------

//
// Given a pointer to a field and its field type, find the equivalent
//   field type in the list of boundary's and apply that boundary value/type.
//   Returns: 0 on failure
//
int EnzoBlock::SetExternalBoundary
(
 int FieldRank, 
 int GridDims[],
 int GridOffset[],
 int StartIndex[], 
 int EndIndex[],
 enzo_float *Field, 
 int FieldType )
{
 
  /* declarations */
 
  int i, j, k, dim, Sign, bindex;
  enzo_float *index;
 
  /* error check: grid ranks */
 
  if (FieldRank != BoundaryRank) {
    fprintf(stderr, "FieldRank(%"ISYM") != BoundaryRank(%"ISYM").\n",
            FieldRank, BoundaryRank);
    return ENZO_FAIL;
  }
 
  /* find requested field type */


  int field;
  for (field = 0; field < NumberOfBaryonFields; field++)
    if (FieldType == BoundaryFieldType[field]) break;
  if (field == NumberOfBaryonFields) {
    fprintf(stderr, "Field type (%"ISYM") not found in Boundary.\n", FieldType);
    return ENZO_FAIL;
  }
 
  /* error check: make sure the boundary type array exists */
 
  for (dim = 0; dim < BoundaryRank; dim++)
    if (BoundaryDimension[dim] != 1) {
      if (BoundaryType[field][dim][0] == NULL) {
	fprintf(stderr, "BoundaryType not yet declared.\n");
	return ENZO_FAIL;
      }
    }
 
  /* set Boundary conditions */
 
  Sign = 1;
  if (FieldType == Velocity1) Sign = -1;
 
  if (BoundaryDimension[0] > 1 && GridOffset[0] == 0) {
 
    /* set x inner (left) face */
 
    for (i = 0; i < StartIndex[0]; i++)
      for (j = 0; j < GridDims[1]; j++)
	for (k = 0; k < GridDims[2]; k++) {
	  index = Field + i + j*GridDims[0] + k*GridDims[1]*GridDims[0];
	  bindex = j+GridOffset[1] + (k+GridOffset[2])*BoundaryDimension[1];
	  switch (BoundaryType[field][0][0][bindex]) {
	  case bc_reflecting:
	    *index = Sign*(*(index + (2*StartIndex[0] - 1 - 2*i)));
	    break;
	  case bc_outflow:
	    *index =       *(index + (  StartIndex[0]     -   i)) ;
	    break;
	  case bc_inflow:
	    *index = BoundaryValue[field][0][0][bindex];
	    break;
	  case bc_periodic:
	    *index = *(index + (EndIndex[0] - StartIndex[0] + 1));
	    break;
	  case bc_unknown:
	  default:
	    fprintf(stderr, "BoundaryType not recognized (x-left).\n");
	    return ENZO_FAIL;
	  }
	}
  }
 
  if (BoundaryDimension[0] > 1 && GridOffset[0]+GridDims[0] == BoundaryDimension[0]) {
 
    /* set x outer (right) face */

    
    for (i = 0; i < GridDims[0]-EndIndex[0]-1; i++)
      for (j = 0; j < GridDims[1]; j++)
	for (k = 0; k < GridDims[2]; k++) {
	  index = Field + i + EndIndex[0]+1 +
	    j*GridDims[0] + k*GridDims[1]*GridDims[0];
	  bindex = j+GridOffset[1] + (k+GridOffset[2])*BoundaryDimension[1];
	  switch (BoundaryType[field][0][1][bindex]) {
	  case bc_reflecting:
	    *index = Sign*(*(index - (2*i + 1)));
	    break;
	  case bc_outflow:
	    *index =       *(index + (-1 - i)) ;
	    break;
	  case bc_inflow:
	    *index = BoundaryValue[field][0][1][bindex];
	    break;
	  case bc_periodic:
	    *index = *(index - (EndIndex[0] - StartIndex[0] + 1));
	    break;
	  case bc_unknown:
	  default:
	    fprintf(stderr, "BoundaryType not recognized (x-right).\n");
	    return ENZO_FAIL;
	  }
	}							
  }
 
  /* set y inner (left) face */
 
  Sign = 1;
  if (FieldType == Velocity2) Sign = -1;
 
  if (BoundaryDimension[1] > 1 && GridOffset[1] == 0) {
 
    for (j = 0; j < StartIndex[1]; j++)
      for (i = 0; i < GridDims[0]; i++)
	for (k = 0; k < GridDims[2]; k++) {
	  index = Field + i + j*GridDims[0] + k*GridDims[1]*GridDims[0];
	  bindex = i+GridOffset[0] + (k+GridOffset[2])*BoundaryDimension[0];
	  switch (BoundaryType[field][1][0][bindex]) {
	  case bc_reflecting:
	    *index = Sign*(*(index + (2*StartIndex[1] - 1 - 2*j)*GridDims[0]));
	    break;
	  case bc_outflow:
	    *index =       *(index + (  StartIndex[1]     - j)*GridDims[0]) ;
	    break;
	  case bc_inflow:
	    *index = BoundaryValue[field][1][0][bindex];
	     break;
	  case bc_periodic:
	    *index = *(index + (EndIndex[1] - StartIndex[1] + 1)*GridDims[0]);
	     break;
	  case bc_unknown:
	  default:
	    fprintf(stderr, "BoundaryType not recognized (y-left).\n");
	    return ENZO_FAIL;
	  }
	}
  }
 
  if (BoundaryDimension[1] > 1 && GridOffset[1]+GridDims[1] == BoundaryDimension[1]) {
 
    /* set y outer (right) face */
 
    for (j = 0; j < GridDims[1]-EndIndex[1]-1; j++)
      for (i = 0; i < GridDims[0]; i++)
	for (k = 0; k < GridDims[2]; k++) {
	  index = Field + i + (j + EndIndex[1]+1)*GridDims[0] +
	    k*GridDims[1]*GridDims[0];
	  bindex = i+GridOffset[0] + (k+GridOffset[2])*BoundaryDimension[0];
	  switch (BoundaryType[field][1][1][bindex]) {
	  case bc_reflecting:
	    *index = Sign*(*(index - (2*j + 1)*GridDims[0]));
	    break;
	  case bc_outflow:
	    *index =       *(index + (-1 - j)*GridDims[0]) ;
	    break;
	  case bc_inflow:
	    *index = BoundaryValue[field][1][1][bindex];
	    break;
	  case bc_periodic:
	    *index = *(index - (EndIndex[1] - StartIndex[1] + 1)*GridDims[0]);
	    break;
	  case bc_unknown:
	  default:
	    fprintf(stderr, "BoundaryType not recognized (y-right).\n");
	    return ENZO_FAIL;
	  }
	}							
  }
 
  /* set z inner (left) face */
 
  Sign = 1;
  if (FieldType == Velocity3) Sign = -1;
 
  if (BoundaryDimension[2] > 1 && GridOffset[2] == 0) {
 
    for (k = 0; k < StartIndex[2]; k++)
      for (i = 0; i < GridDims[0]; i++)
	for (j = 0; j < GridDims[1]; j++) {
	  index = Field + i + j*GridDims[0] + k*GridDims[1]*GridDims[0];
	  bindex = i+GridOffset[0] + (j+GridOffset[1])*BoundaryDimension[0];
	  switch (BoundaryType[field][2][0][bindex]) {
	  case bc_reflecting:
	    *index = Sign*(*(index + (2*StartIndex[2]-1 - 2*k)*GridDims[0]*GridDims[1]));
	    break;
	  case bc_outflow:
	    *index =       *(index + (  StartIndex[2]   - k)*GridDims[0]*GridDims[1]) ;
	    break;
	  case bc_inflow:
	    *index = BoundaryValue[field][2][0][bindex];
	    break;
	  case bc_periodic:
	    *index = *(index + (EndIndex[2]-StartIndex[2]+1)*GridDims[0]*GridDims[1]);
	    break;
	  case bc_unknown:
	  default:
	    fprintf(stderr, "BoundaryType not recognized (z-left).\n");
	    return ENZO_FAIL;
	  }
	}
  }
 
  if (BoundaryDimension[2] > 1 && GridOffset[2]+GridDims[2] == BoundaryDimension[2]) {
 
    /* set z outer (right) face */
 
    for (k = 0; k < GridDims[2]-EndIndex[2]-1; k++)
      for (i = 0; i < GridDims[0]; i++)
	for (j = 0; j < GridDims[1]; j++) {
	  index = Field + i + j*GridDims[0] +
	    (k + EndIndex[2]+1)*GridDims[1]*GridDims[0];
	  bindex = i+GridOffset[0] + (j+GridOffset[1])*BoundaryDimension[0];
	  switch (BoundaryType[field][2][1][bindex]) {
	  case bc_reflecting:
	    *index = Sign*(*(index - (2*k + 1)*GridDims[0]*GridDims[1]));
	    break;
	  case bc_outflow:
	    *index =       *(index + (-1 - k)*GridDims[0]*GridDims[1]) ;
	    break;
	  case bc_inflow:
	    *index = BoundaryValue[field][2][1][bindex];
	    break;
	  case bc_periodic:
	    *index = *(index - (EndIndex[2]-StartIndex[2]+1)*GridDims[0]*GridDims[1]);
	    break;
	  case bc_unknown:
	  default:
	    fprintf(stderr, "BoundaryType not recognized (z-right).\n");
	    return ENZO_FAIL;
	  }
	}							
  }
 
  return ENZO_SUCCESS;
 
}

//----------------------------------------------------------------------

int EnzoBlock::CosmologyComputeExpansionFactor
(enzo_float time, enzo_float *a, enzo_float *dadt)
{
 
  /* Error check. */

  if (InitialTimeInCodeUnits == 0) {
    
    char error_message[ERROR_LENGTH];
    sprintf(error_message, "The cosmology parameters seem to be improperly set");
    ERROR("CosmologyComputeExpansionFactor",error_message);
  }
 
  *a = ENZO_FLOAT_UNDEFINED;
 
  /* Find Omega due to curvature. */
 
  enzo_float OmegaCurvatureNow = 1 - OmegaMatterNow - OmegaLambdaNow;
 
  /* Convert the time from code units to Time * H0 (c.f. CosmologyGetUnits). */
 
  enzo_float TimeUnits = 2.52e17/sqrt(OmegaMatterNow)/HubbleConstantNow/
                    pow(1 + InitialRedshift,enzo_float(1.5));
 
  enzo_float TimeHubble0 = time * TimeUnits * (HubbleConstantNow*3.24e-18);
 
  /* 1) For a flat universe with OmegaMatterNow = 1, it's easy. */
 
  if (fabs(OmegaMatterNow-1) < OMEGA_TOLERANCE &&
      OmegaLambdaNow < OMEGA_TOLERANCE)
    *a      = pow(time/InitialTimeInCodeUnits, enzo_float(2.0/3.0));
 
#define INVERSE_HYPERBOLIC_EXISTS
 
#ifdef INVERSE_HYPERBOLIC_EXISTS
 
  enzo_float eta, eta_old, x;
  int i;
 
  /* 2) For OmegaMatterNow < 1 and OmegaLambdaNow == 0 see
        Peebles 1993, eq. 13-3, 13-10.
	Actually, this is a little tricky since we must solve an equation
	of the form eta - sinh(eta) + x = 0..*/
 
  if (OmegaMatterNow < 1 && OmegaLambdaNow < OMEGA_TOLERANCE) {
 
    x = 2*TimeHubble0*pow(1.0 - OmegaMatterNow, 1.5) / OmegaMatterNow;
 
    /* Compute eta in a three step process, first from a third-order
       Taylor expansion of the formula above, then use that in a fifth-order
       approximation.  Then finally, iterate on the formula itself, solving for
       eta.  This works well because parts 1 & 2 are an excellent approximation
       when x is small and part 3 converges quickly when x is large. */
 
    eta = pow(6*x, enzo_float(1.0/3.0));                     // part 1
    eta = pow(120*x/(20+eta*eta), enzo_float(1.0/3.0));      // part 2
    for (i = 0; i < 40; i++) {                          // part 3
      eta_old = eta;
      eta = asinh(eta + x);
      if (fabs(eta-eta_old) < ETA_TOLERANCE) break;
    }
    if (i == 40) {
      fprintf(stderr, "Case 2 -- no convergence after %"ISYM" iterations.\n", i);
      return ENZO_FAIL;
    }
 
    /* Now use eta to compute the expansion factor (eq. 13-10, part 2). */
 
    *a = OmegaMatterNow/(2*(1 - OmegaMatterNow))*(cosh(eta) - 1);
    *a *= (1 + InitialRedshift);    // to convert to code units, divide by [a]
  }
 
  /* 3) For OmegaMatterNow > 1 && OmegaLambdaNow == 0, use sin/cos.
        Easy, but skip it for now. */
 
  if (OmegaMatterNow > 1 && OmegaLambdaNow < OMEGA_TOLERANCE) {
  }
 
  /* 4) For flat universe, with non-zero OmegaLambdaNow, see eq. 13-20. */
 
  if (fabs(OmegaCurvatureNow) < OMEGA_TOLERANCE &&
      OmegaLambdaNow > OMEGA_TOLERANCE) {
    *a = pow(enzo_float(OmegaMatterNow/(1 - OmegaMatterNow)), enzo_float(1.0/3.0)) *
         pow(enzo_float(sinh(1.5 * sqrt(1.0 - OmegaMatterNow)*TimeHubble0)),
	     enzo_float(2.0/3.0));
    *a *= (1 + InitialRedshift);    // to convert to code units, divide by [a]
  }
 
#endif /* INVERSE_HYPERBOLIC_EXISTS */
 
  /* Compute the derivative of the expansion factor (Peebles93, eq. 13.3). */
 
  enzo_float TempVal = (*a)/(1 + InitialRedshift);
  *dadt = sqrt( 2.0/(3.0*OmegaMatterNow*(*a)) *
	       (OmegaMatterNow + OmegaCurvatureNow*TempVal +
		OmegaLambdaNow*TempVal*TempVal*TempVal));
 
  /* Someday, we'll implement the general case... */
 
  if ((*a) == ENZO_FLOAT_UNDEFINED) {
    fprintf(stderr, "Cosmology selected is not implemented.\n");
    return ENZO_FAIL;
  }
 
  return ENZO_SUCCESS;
}

//---------------------------------------------------------------------- 
 
int EnzoBlock::CosmologyComputeExpansionTimestep
(enzo_float time, enzo_float *dtExpansion)
{
 
  /* Error check. */
 
  if (InitialTimeInCodeUnits == 0) {
    fprintf(stderr, "The cosmology parameters seem to be improperly set.\n");
    return ENZO_FAIL;
  }
 
  /* Compute the expansion factors. */
 
  enzo_float a, dadt;
  if (CosmologyComputeExpansionFactor(time, &a, &dadt) == ENZO_FAIL) {
    fprintf(stderr, "Error in ComputeExpnasionFactors.\n");
    return ENZO_FAIL;
  }
 
  /* Compute the maximum allwed timestep given the maximum allowed
     expansion factor. */
 
  *dtExpansion = MaxExpansionRate*a/dadt;
 
  return ENZO_SUCCESS;
}
