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

#ifdef CONFIG_USE_CHARM

void Initial::pup (PUP::er &p)
{
  // NOTE: change this function whenever attributes change

  TRACEPUP;

  PUP::able::pup(p);

  p | cycle_;
  p | time_;

}

#endif

//----------------------------------------------------------------------

void Initial::enforce_simulation_
( Simulation * simulation ) throw()
{
  enforce_hierarchy (simulation->hierarchy(), simulation->field_descr());
}

//----------------------------------------------------------------------

void Initial::enforce_hierarchy_
(
 Hierarchy * hierarchy,
 const FieldDescr * field_descr
 ) throw()
{
#ifdef CONFIG_USE_CHARM

  // if (hierarchy->group_process()->is_root()) {
  //   hierarchy->block_array()->p_initial();
  // }

#else /* CONFIG_USE_CHARM */

  ItBlock it_block (hierarchy);
  while (CommBlock * block = ++it_block) {
    // NO OFFSET: ASSUMES ROOT PATCH
    enforce_block (block, field_descr, hierarchy);
  }

#endif /* CONFIG_USE_CHARM */

}

//----------------------------------------------------------------------

void Initial::enforce_block_
(
 CommBlock * block,
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
  ERROR("Initial::enforce_block_",
	"This function should not be called");
}
