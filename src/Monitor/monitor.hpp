//
// $Id$
//
// See LICENSE_CELLO file for license and copyright information
//

#ifndef MONITOR_HPP
#define MONITOR_HPP

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



/** 
 *********************************************************************
 *
 * @file      monitor.hpp
 * @brief     Declaration for Monitor clas
 * @author    James Bordner
 * @date      2009-10-05
 *
 * Declaration for Monitor clas
 *
 * $Id$
 *
 *********************************************************************
 */

#include <string>

#include "cello.h"

#include "performance_timer.hpp"

enum enum_reduce {
  reduce_unknown,
  reduce_min,
  reduce_max,
  reduce_avg,
  reduce_sum
};

class Monitor {

/** 
 *********************************************************************
 *
 * @class     Monitor
 * @brief     Functions for user monitoring of the execution status
 * @ingroup   Monitor
 *
 * The Monitor component is used to communicate information about the
 * running simulation to the user. Information can be output in
 * several forms, including text files, HTML files, plots, or other
 * (generally small) image files. Information is assumed to be from a
 * correctly-running simulation: anomalous errors or warnings are
 * output by the Error component. It is assumed that stdout is not
 * used for monitor output, except for possibly displaying header text
 * with basic information about Cello and the simulation being run.
 *
 *********************************************************************
 */

private:

  //-------------------------------------------------------------------
  // PRIVATE ATTRIBUTES
  //-------------------------------------------------------------------

public:

  //-------------------------------------------------------------------
  // PUBLIC OPERATIONS
  //-------------------------------------------------------------------

  /// Initialize the Monitor object

  Monitor(FILE * fp = stdout,
	  bool active = true) 
    : active_(active),
      fp_(fp)
  {  timer_.start();};

  /// Print a message

  void print (std::string message)
  {
    if (active_) fprintf (fp_,"%6.1f %s\n",timer_.value(),message.c_str());
  };

  /// Generate a PNG image of an array

  void image (std::string name, 
	      Scalar * array, 
	      int nx,  int ny,  int nz,   // Array dimensions
	      int nx0, int ny0, int nz0,  // lower inclusive subarray indices
	      int nx1, int ny1, int nz1,  // upper exclusive subarray indices
	      int         axis,           // Axis along which to project
	      enum_reduce op_reduce,      // Reduction operation along axis
	      Scalar min, Scalar max,     // Limits for color map
	      const double * color_map,   // color map [r0 g0 b0 r1 g1 b1 ...]
	      int            color_length // length of color map / 3
	      );
  
private:

  bool   active_;  // Whether monitoring is activated.  Used for e.g. np != 0.
  FILE * fp_;      // File pointer for message logging
  Timer  timer_;   // Timer from Performance

};

#endif /* MONITOR_HPP */

