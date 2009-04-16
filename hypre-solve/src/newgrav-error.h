//345678901234567890123456789012345678901234567890123456789012345678901234567890

/// Error and warning definitions

/**
 * 
 * @file      error.hpp
 * @brief     Macros related to error and warning messages
 * @author    James Bordner
 * @bug       none
 *
 * $Id$
 *
 */

#define WARNING(MESSAGE) { \
   int _ip_; \
   MPI_Comm_rank (MPI_COMM_WORLD, &_ip_); \
   printf ("WARNING %d %s:%d %s\n",_ip_,__FILE__,__LINE__,MESSAGE);	\
   fflush(stdout); \
}

#define ERROR(MESSAGE) { \
   int _ip_; \
   MPI_Comm_rank (MPI_COMM_WORLD, &_ip_); \
   printf ("ERROR %d %s:%d %s\n",_ip_,__FILE__,__LINE__,MESSAGE); \
  fflush(stdout); \
  exit(1); \
}

#define WARNING_ROOT(X) { \
   int _ip_; \
   MPI_Comm_rank (MPI_COMM_WORLD, &_ip_); \
   if (_ip_==0) { \
      printf ("WARNING %d %s:%d" X "\n",__FILE__,__LINE__); \
      fflush(stdout);\
   } \
  }

#define NOT_IMPLEMENTED(X) { \
   printf ("%s:%d WARNING: " X " is not implemented yet\n",__FILE__,__LINE__); \
   fflush(stdout); \
}

#define TEMPORARY(X) { \
   printf ("%s:%d TEMPORARY: " X " is temporary code\n",__FILE__,__LINE__); \
   fflush(stdout); \
}
