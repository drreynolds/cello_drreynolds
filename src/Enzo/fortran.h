#include "enzo_defines.hpp"
/* #include "fortran_types.def" */

#define DEBUG_MESSAGE            CALL f_warning(__FILE__,__LINE__)
#define ERROR_MESSAGE            CALL f_error(__FILE__,__LINE__)
#define WARNING_MESSAGE          CALL f_warning(__FILE__,__LINE__)

#ifdef CONFIG_PRECISION_SINGLE
#  define ENZO_REAL real*4
#  define ENZO_TINY 1e-10
#endif

#ifdef CONFIG_PRECISION_DOUBLE
#  define ENZO_REAL real*8
#  define ENZO_TINY 1e-20
#endif

#ifdef CONFIG_PRECISION_QUAD
#  define ENZO_REAL real*16
#  define ENZO_TINY 1e-40
#endif

/* --------------------------------------------------------------------- */

/* Enzo3.0 defines */

#ifdef CONFIG_PRECISION_SINGLE
#  define CONFIG_PFLOAT_4
#  define CONFIG_BFLOAT_4
#endif

#ifdef CONFIG_PRECISION_DOUBLE
#  define CONFIG_PFLOAT_8
#  define CONFIG_BFLOAT_8
#endif

#ifdef CONFIG_PRECISION_QUAD
#  define CONFIG_PFLOAT_16
#  define CONFIG_BFLOAT_16
#endif

