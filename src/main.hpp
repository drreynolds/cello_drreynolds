// See LICENSE_CELLO file for license and copyright information

/// @file     main.cpp
/// @author   James Bordner (jobordner@ucsd.edu)
/// @date     Thu Feb 25 16:20:17 PST 2010
/// @brief    Declaration of the Main CHARM++ chare

#ifdef CONFIG_USE_CHARM

#include "cello.hpp"

#include "parallel.hpp"
#include "monitor.hpp"

class Factory;
#if defined(CHARM_ENZO)
#  include "main_enzo.decl.h"
#elif defined(CHARM_SIMULATION)
#  include "main_simulation.decl.h"
#elif defined(CHARM_MESH)
#  include "main_mesh.decl.h"
#else
#  include "main.decl.h"
#endif

extern CProxy_Main proxy_main;

//----------------------------------------------------------------------

class Main : public CBase_Main {

public:

  /// Initialize the Main chare (defined in the calling program)
  Main(CkArgMsg* main);
  
  /// Exit the program
  void p_exit(int count);

private:

   int count_exit_; 
   Monitor * monitor_;

};
#endif
