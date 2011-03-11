// $Id$
// See LICENSE_CELLO file for license and copyright information

/// @file      monitor_Monitor.cpp
/// @author    James Bordner (jobordner@ucsd.edu)
/// @date      Thu Feb 21 12:45:56 PST 2009
/// @todo      simplify image call
/// @brief     Routines for simple output of text, plots, and graphs

#include "cello.hpp"

#include "monitor.hpp" 

Monitor * Monitor::instance_ = 0; // (singleton design pattern)

//----------------------------------------------------------------------

void Monitor::header () const
{
  print ("==============================================");
  print ("");
  print ("  .oooooo.             oooo  oooo            ");
  print (" d8P'  `Y8b            `888  `888            ");
  print ("888           .ooooo.   888   888   .ooooo.  ");
  print ("888          d88' `88b  888   888  d88' `88b ");
  print ("888          888ooo888  888   888  888   888 ");
  print ("`88b    ooo  888    .o  888   888  888   888 ");
  print (" `Y8bood8P'  `Y8bod8P' o888o o888o `Y8bod8P' ");
  print ("");
  print ("A Parallel Adaptive Mesh Refinement Framework");
  print ("");  
  print ("                James Bordner");
  print ("  Laboratory for Computational Astrophysics");
  print ("        San Diego Supercomputer Center");
  print ("     University of California, San Diego");
  print ("");  

  char c_single = ' ';
  char c_double = ' ';
  char c_quad   = ' ';
  char c_charm  = ' ';
  char c_mpi    = ' ';
  char c_papi   = ' ';

#ifdef CONFIG_PRECISION_SINGLE
  c_single = '*';
#endif
#ifdef CONFIG_PRECISION_DOUBLE
  c_double = '*';
#endif
#ifdef CONFIG_PRECISION_QUADRUPLE
  c_quad = '*';
#endif
#ifdef CONFIG_USE_CHARM
  c_charm = '*';
#endif
#ifdef CONFIG_USE_MPI
  c_mpi = '*';
#endif
#ifdef CONFIG_USE_PAPI
  c_papi = '*';
#endif

  char s_single[80];
  char s_double[80];
  char s_quad  [80];
  char s_charm [80];
  char s_mpi   [80];
  char s_papi  [80];

  sprintf (s_single,"(%c) CONFIG_PRECISION_SINGLE",c_single);
  sprintf (s_double,"(%c) CONFIG_PRECISION_DOUBLE",c_double);
  sprintf (s_quad,  "(%c) CONFIG_PRECISION_QUAD",  c_quad);
  sprintf (s_charm, "(%c) CONFIG_USE_CHARM",       c_charm);
  sprintf (s_mpi,   "(%c) CONFIG_USE_MPI",         c_mpi);
  sprintf (s_papi,  "[%c] CONFIG_USE_PAPI",        c_papi);

  print ("==============================================");
  print (s_single);
  print (s_double);
  print (s_quad);
  print ("");
  print (s_charm);
  print (s_mpi);
  print ("");
  print (s_papi);
  print ("==============================================");

}

//----------------------------------------------------------------------

void Monitor::print (std::string message, ...) const
{
  if (active_) {

    va_list fargs;

    // Process any input arguments

    char buffer[MONITOR_LENGTH+1];
    va_start(fargs,message);
    vsnprintf (buffer,MONITOR_LENGTH, message.c_str(),fargs);
    va_end(fargs);
    
    // Write output with line header
    time_t rawtime;
    struct tm * t;
    time(&rawtime);
    t = localtime (&rawtime);
    const char * month[] = 
      {"Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec"};
    PARALLEL_PRINTF ("%s %02d %02d:%02d:%02d %s\n",
		     month[t->tm_mon],
		     t->tm_mday,
		     t->tm_hour,
		     t->tm_min,
		     t->tm_sec,
		     buffer);
  }
};
