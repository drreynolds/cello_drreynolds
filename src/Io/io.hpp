// See LICENSE_CELLO file for license and copyright information

#ifndef IO_HPP
#define IO_HPP

/// @file     io.hpp
/// @author   James Bordner (jobordner@ucsd.edu)
/// @date     2011-11-01
/// @brief    Include file for the \ref Io component 

//----------------------------------------------------------------------
// System includes
//----------------------------------------------------------------------


//----------------------------------------------------------------------
// Component dependencies
//----------------------------------------------------------------------

#include "cello.hpp"

#include "error.hpp"
#include "parallel.hpp"
#include "memory.hpp"

#include "simulation.hpp"
#include "mesh.hpp"
#include "disk.hpp"
#include "field.hpp" 

//----------------------------------------------------------------------
// Component class includes
//----------------------------------------------------------------------

#include "io_Io.hpp"

#include "io_IoSimulation.hpp"
#include "io_IoField.hpp"

#include "io_Type.hpp"
#include "io_TypeImage.hpp"
#include "io_TypeRestart.hpp"
#include "io_TypeData.hpp"

#include "io_Output.hpp"
#include "io_OutputImage.hpp"

#endif /* IO_HPP */

