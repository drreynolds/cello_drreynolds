// $Id$
// See LICENSE_ENZO file for license and copyright information

/***********************************************************************
/
/  COMPUTES THE EXPANSION FACTORS (A & DADT) AT THE REQUESTED TIME
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
 
#include "cello.hpp"

#include "enzo.hpp"

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
 
// function prototypes
 
int EnzoBlock::CosmologyComputeExpansionFactor
(enzo_float time, enzo_float *a, enzo_float *dadt)
{
 
  /* Error check. */
  using namespace enzo;
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
