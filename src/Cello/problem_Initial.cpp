// See LICENSE_CELLO file for license and copyright information

/// @file     io_Problem.cpp
/// @author   James Bordner (jobordner@ucsd.edu)
/// @date     Wed Mar 16 09:53:31 PDT 2011
/// @brief    Implementation of the Problem class

#include "cello.hpp"
#include "main.hpp"
#include "simulation.hpp"
#include "field.hpp"
#include "problem.hpp"

//----------------------------------------------------------------------

#ifdef CONFIG_USE_CHARM
extern CProxy_SimulationCharm  proxy_simulation;
#endif

//----------------------------------------------------------------------

void Initial::enforce_
( Simulation * simulation ) throw()
{
  enforce (simulation->hierarchy(), simulation->field_descr());
}

//----------------------------------------------------------------------

void Initial::enforce_ 
(
 Hierarchy * hierarchy,
 const FieldDescr * field_descr
 ) throw()
{
  ItPatch it_patch (hierarchy);

  while (Patch * patch = ++it_patch) {
#ifdef CONFIG_USE_CHARM
    ((CProxy_Patch *)patch)->p_initial();
#else
    enforce (patch, field_descr, hierarchy);
#endif
  }
}

//----------------------------------------------------------------------

void Initial::enforce_
(
 Patch * patch,
 const FieldDescr * field_descr,
 const Hierarchy * hierarchy
 ) throw()

{

#ifdef CONFIG_USE_CHARM

  patch->block_array()->p_initial();

#else

  ItBlock it_block (patch);
  while (Block * block = ++it_block) {
    // NO OFFSET: ASSUMES ROOT PATCH
    enforce (block, field_descr, hierarchy);
  }

#endif

}

//----------------------------------------------------------------------

void Initial::enforce_
(
 Block * block,
 const FieldDescr * field_descr,
 const Hierarchy * hierarchy
 ) throw()
{
  // Enforce fields

  // ItFieldRange it_field (field_descr->field_count());

  // for (it_field.first(); ! it_field.done(); it_field.next()  ) {
  //   const FieldBlock * field_block = block->field_block();
  //   enforce (field_block,  field_descr, it_field.value());
  // }
  ERROR("Initial::enforce_",
	"This function should not be called");
}
