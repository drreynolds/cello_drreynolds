// See LICENSE_CELLO file for license and copyright information

/// @file     main.cpp
/// @author   James Bordner (jobordner@ucsd.edu)
/// @date     2011-08-10
/// @brief    Implementation of main-level CHARM entry functions in main.ci

#include "cello.hpp"
#include "test.hpp"
#include "parallel.hpp"
#include "monitor.hpp"

#include "main.hpp"

//----------------------------------------------------------------------

CProxy_Main proxy_main;

#ifdef CHARM_ENZO
#include "simulation.hpp"
extern CProxy_SimulationCharm proxy_simulation;
#include "enzo_finalize.hpp"
#endif

//----------------------------------------------------------------------

CkReduction::reducerType r_method_turbulence_type;

extern CkReductionMsg * r_method_turbulence(int n, CkReductionMsg ** msgs);

void register_method_turbulence(void)
{
  r_method_turbulence_type = CkReduction::addReducer(r_method_turbulence); 
}

//----------------------------------------------------------------------

void Main::p_exit(int count)
{
  DEBUG("Main::p_exit");
  count_exit_++;
  unit_finalize();
  if (count_exit_ >= count) {
    count_exit_ = 0;
    exit_();
  }
}

void Main::exit_()
{

#ifdef CHARM_ENZO

  Simulation * simulation = proxy_simulation.ckLocalBranch();

  enzo_finalize(simulation);

#endif

  Monitor::instance()->print ("","END CELLO");
  //    unit_finalize();
  // Fake unit_init() for index.php (test.hpp is not included since
  // enzo.ci and test.ci conflict)
  PARALLEL_EXIT;
}


//----------------------------------------------------------------------

void Main::p_checkpoint(int count, std::string dir_name)
{
  
  count_checkpoint_++;
  if (count_checkpoint_ >= count) {
    count_checkpoint_ = 0;
    // Write parameter file

    char dir_char[255];
    strcpy(dir_char,dir_name.c_str());

    // --------------------------------------------------
    // ENTRY: #1 OutputCheckpoint::write_simulation()-> SimulationCharm::s_write()
    // ENTRY: checkpoint if Simulation is root
    // --------------------------------------------------
#ifdef CHARM_ENZO
    CkCallback callback(CkIndex_SimulationCharm::r_write_checkpoint(),proxy_simulation);
    CkStartCheckpoint (dir_char,callback);
#endif
  }
  // --------------------------------------------------
}


//----------------------------------------------------------------------

void Main::p_output_enter()
{
#ifdef CHARM_ENZO
  proxy_simulation.ckLocalBranch()->hierarchy()->block_array()->p_output_enter();
#endif
}

//----------------------------------------------------------------------

void Main::p_output_exit()
{
#ifdef CHARM_ENZO
  proxy_simulation.ckLocalBranch()->hierarchy()->block_array()->p_output_exit();
#endif
}

//----------------------------------------------------------------------

void Main::p_compute_enter()
{
#ifdef CHARM_ENZO
  proxy_simulation.ckLocalBranch()->hierarchy()->block_array()->p_compute_enter();
#endif
}

//----------------------------------------------------------------------

void Main::p_compute_exit()
{
#ifdef CHARM_ENZO
  proxy_simulation.ckLocalBranch()->hierarchy()->block_array()->p_compute_exit();
#endif
}

//----------------------------------------------------------------------

void Main::p_stopping_enter()
{
#ifdef CHARM_ENZO
  proxy_simulation.ckLocalBranch()->hierarchy()->block_array()->p_stopping_enter();
#endif
}

//----------------------------------------------------------------------

void Main::p_stopping_exit()
{
#ifdef CHARM_ENZO
  proxy_simulation.ckLocalBranch()->hierarchy()->block_array()->p_stopping_exit();
#endif
}

//----------------------------------------------------------------------

void Main::p_exit()
{
#ifdef CHARM_ENZO
  proxy_simulation.ckLocalBranch()->hierarchy()->block_array()->p_exit();
#endif
}

//----------------------------------------------------------------------

void Main::p_adapt_enter()
{
#ifdef CHARM_ENZO
  proxy_simulation.ckLocalBranch()->hierarchy()->block_array()->p_adapt_enter();
#endif
}

//----------------------------------------------------------------------

void Main::p_initial_exit()
{
#ifdef CHARM_ENZO
  proxy_simulation.ckLocalBranch()->hierarchy()->block_array()->p_initial_exit();
#endif
}

//----------------------------------------------------------------------

void Main::p_adapt_end()
{
#ifdef CHARM_ENZO
  proxy_simulation.ckLocalBranch()->hierarchy()->block_array()->p_adapt_end();
#endif
}

//----------------------------------------------------------------------

void Main::p_adapt_next()
{
#ifdef CHARM_ENZO
  proxy_simulation.ckLocalBranch()->hierarchy()->block_array()->p_adapt_next();
#endif
}

//----------------------------------------------------------------------

void Main::p_adapt_called()
{
#ifdef CHARM_ENZO
  proxy_simulation.ckLocalBranch()->hierarchy()->block_array()->p_adapt_called();
#endif
}

//----------------------------------------------------------------------

void Main::p_adapt_exit()
{
#ifdef CHARM_ENZO
  proxy_simulation.ckLocalBranch()->hierarchy()->block_array()->p_adapt_exit();
#endif
}

//----------------------------------------------------------------------

void Main::p_refresh_enter()
{
#ifdef CHARM_ENZO
  proxy_simulation.ckLocalBranch()->hierarchy()->block_array()->p_refresh_enter();
#endif
}

//----------------------------------------------------------------------

void Main::p_refresh_exit()
{
#ifdef CHARM_ENZO
  proxy_simulation.ckLocalBranch()->hierarchy()->block_array()->p_refresh_exit();
#endif
}

//----------------------------------------------------------------------

// SEE enzo_EnzoMethodTurbulence.cpp for context
CkReductionMsg * r_method_turbulence(int n, CkReductionMsg ** msgs)
{
  double accum[9] = { 0.0 };
  accum[7] = std::numeric_limits<double>::max();
  accum[8] = std::numeric_limits<double>::min();

  for (int i=0; i<n; i++) {
    double * values = (double *) msgs[i]->getData();
    accum [0] += values[0];
    accum [1] += values[1];
    accum [2] += values[2];
    accum [3] += values[3];
    accum [4] += values[4];
    accum [5] += values[5];
    accum [6] += values[6];
    accum [7] = std::min(accum[7],values[7]);
    accum [8] = std::max(accum[8],values[8]);
  }
  return CkReductionMsg::buildNew(9*sizeof(double),accum);
}

//----------------------------------------------------------------------

#if defined(CHARM_ENZO)
#  include "main_enzo.def.h"
#elif defined(CHARM_SIMULATION)
#  include "main_simulation.def.h"
#elif defined(CHARM_MESH)
#  include "main_mesh.def.h"
#else
#  include "main.def.h"
#endif

