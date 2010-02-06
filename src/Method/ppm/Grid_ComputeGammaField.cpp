/** 
 *********************************************************************
 *
 * @file      
 * @brief     
 * @author    
 * @date      
 * @ingroup
 * @bug       
 * @note      
 *
 *--------------------------------------------------------------------
 *
 * DESCRIPTION:
 *
 *    
 *
 * CLASSES:
 *
 *    
 *
 * FUCTIONS:
 *
 *    
 *
 * USAGE:
 *
 *    
 *
 * REVISION HISTORY:
 *
 *    
 *
 * COPYRIGHT: See the LICENSE_CELLO file in the project directory
 *
 *--------------------------------------------------------------------
 *
 * $Id$
 *
 *********************************************************************
 */
/***********************************************************************
/
/  GRID CLASS (COMPUTE THE GAMMA FIELD)
/
/  written by: Greg Bryan
/  date:       May, 1999
/  modified1:
/
/  PURPOSE:
/
/  RETURNS:
/
************************************************************************/
 
// Compute the ratio of specific heats.

#include "cello_hydro.h"
 
/* function prototypes */
 
 
 
int ComputeGammaField(float *GammaField)
{
 
  int DeNum, HINum, HIINum, HeINum, HeIINum, HeIIINum, HMNum, H2INum, H2IINum,
      DINum, DIINum, HDINum;
 
  /* Compute the size of the fields. */
 
  int i, size = 1;
  for (int dim = 0; dim < GridRank; dim++)
    size *= GridDimension[dim];
 
  if (MultiSpecies < 2)
 
    /* If molecular hydrogen is not being used, just use monotonic.
       (this should not really be called, but provide it just in case). */
 
    for (i = 0; i < size; i++)
      GammaField[i] = Gamma;
 
  else {
 
    /* Find Multi-species fields. */
 
    if (IdentifySpeciesFields(DeNum, HINum, HIINum, HeINum, HeIINum, HeIIINum,
		      HMNum, H2INum, H2IINum, DINum, DIINum, HDINum) == FAIL) {
      fprintf(stderr, "Error in grid->IdentifySpeciesFields.\n");
      return FAIL;
    }
 
    /* Compute the temperature. */
 
    ComputeTemperatureField(GammaField);
 
    /* Compute Gamma with molecular Hydrogen formula from Omukau \& Nishi
       astro-ph/9811308. */
 
    float x, nH2, number_density, GammaH2Inverse, GammaInverse = 1/(Gamma-1.0);
    for (i = 0; i < size; i++) {
 
      /* Compute relative number abundence of molecular hydrogen. */
 
      number_density =
	0.25*(BaryonField[HeINum][i]  + BaryonField[HeIINum][i] +
	      BaryonField[HeIIINum][i]                        ) +
              BaryonField[HINum][i]   + BaryonField[HIINum][i]  +
              BaryonField[DeNum][i];
 
      nH2 = 0.5*(BaryonField[H2INum][i]  + BaryonField[H2IINum][i]);
 
      /* Only do full computation if there is a reasonable amount of H2.
         The second term in GammaH2Inverse accounts for the vibrational
         degrees of freedom. */
 
      GammaH2Inverse = 0.5*5.0;
      if (nH2/number_density > 1e-3) {
	x = GammaField[i]/6100.0;
	if (x < 10.0)
	  GammaH2Inverse = 0.5*(5 + 2.0 * x*x * exp(x)/pow(exp(x)-1.0,2));
      }
 
      /* Add in H2. */
 
      GammaField[i] = 1.0 + (nH2 + number_density) /
                      (nH2*GammaH2Inverse + number_density * GammaInverse);
 
    }
 
  } // end: if (MultiSpecies < 2)
 
  return SUCCESS;
}
