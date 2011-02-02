// $Id$
// See LICENSE_CELLO file for license and copyright information

#ifndef ENZO_HPP
#define ENZO_HPP

/// @file     enzo.hpp
/// @author   James Bordner (jobordner@ucsd.edu)
/// @date     2010-04-02
/// @brief    Include file for the \ref Enzo component

//----------------------------------------------------------------------
// System includes
//----------------------------------------------------------------------

#include <vector>
#include <string>

//----------------------------------------------------------------------
// Cello include file
//----------------------------------------------------------------------

#include "cello.hpp"

//----------------------------------------------------------------------
// Component dependencies
//----------------------------------------------------------------------

#include "method.hpp"
#include "simulation.hpp"

//----------------------------------------------------------------------
// Component class includes
//----------------------------------------------------------------------

#include "enzo_defines.hpp"
#include "enzo_typedefs.hpp"
#include "enzo_fortran.hpp"

#include "enzo_EnzoDescr.hpp"

#include "enzo_EnzoItBlocks.hpp"
#include "enzo_EnzoSimulation.hpp"
#include "enzo_EnzoControl.hpp"
#include "enzo_EnzoTimestep.hpp"
#include "enzo_EnzoInitialImplosion2.hpp"
#include "enzo_EnzoMethodPpm.hpp"
#include "enzo_EnzoMethodPpml.hpp"

#endif /* ENZO_HPP */

