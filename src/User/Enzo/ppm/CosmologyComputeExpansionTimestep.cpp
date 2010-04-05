// $Id$
// See LICENSE_ENZO file for license and copyright information

/***********************************************************************
/
/  COMPUTES THE MAXIMUM ALLOWED EXPANSION TIMESTEP AT GIVEN TIME
/
/  written by: Greg Bryan
/  date:       April, 1995
/  modified1:
/
/  PURPOSE:
/
/  NOTE:
/
************************************************************************/
 
#include "cello_hydro.h"
 
/* Function prototypes */
 
int CosmologyComputeExpansionFactor(FLOAT time, FLOAT *a, FLOAT *dadt);
 
 
 
int CosmologyComputeExpansionTimestep(FLOAT time, float *dtExpansion)
{
 
  /* Error check. */
 
  if (InitialTimeInCodeUnits == 0) {
    fprintf(stderr, "The cosmology parameters seem to be improperly set.\n");
    return FAIL;
  }
 
  /* Compute the expansion factors. */
 
  FLOAT a, dadt;
  if (CosmologyComputeExpansionFactor(time, &a, &dadt) == FAIL) {
    fprintf(stderr, "Error in ComputeExpnasionFactors.\n");
    return FAIL;
  }
 
  /* Compute the maximum allwed timestep given the maximum allowed
     expansion factor. */
 
  *dtExpansion = MaxExpansionRate*a/dadt;
 
  return SUCCESS;
}
