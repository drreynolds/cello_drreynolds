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
 * SYNOPSIS:
 *
 *    
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
#include <stdio.h>
#include <math.h>
 
#ifdef CRAYX1
 
double arcsinh(double x)
{
  if( x == 0.0 ) {
    return (0.0);
  }
  return (log(x+sqrt(x*x+1.0)));
}
 
#else
 
double arcsinh(double x)
{
  return (asinh(x));
}
 
#endif
