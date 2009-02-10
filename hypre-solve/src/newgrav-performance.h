//345678901234567890123456789012345678901234567890123456789012345678901234567890

/// Performance type definitions

/**
 * 
 * @file      performance.hpp
 * @brief     lcaperf-related defines
 * @author    James Bordner
 * @bug       none
 *
 * $Id$
 *
 */

#ifdef USE_JBPERF

#   include "jbPerf.h"


#   define JBPERF_BEGIN(SEGMENT) jbPerf.begin(SEGMENT)
#   define JBPERF_END(SEGMENT)   jbPerf.end(SEGMENT)
#   define JBPERF_START(REGION)  jbPerf.start(REGION)
#   define JBPERF_STOP(REGION)   jbPerf.stop(REGION)
#   define JBPERF_GLOBAL(NAME,VALUE) jbPerf.global(NAME,VALUE)

#else

#   define JBPERF_BEGIN(SEGMENT)     ;
#   define JBPERF_END(SEGMENT)       ;
#   define JBPERF_START(REGION)      ;
#   define JBPERF_STOP(REGION)       ;
#   define JBPERF_GLOBAL(NAME,VALUE) ;

#endif



