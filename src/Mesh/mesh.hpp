// See LICENSE_CELLO file for license and copyright information

/// @file     mesh.hpp
/// @author   James Bordner (jobordner@ucsd.edu)
/// @date     Thu Mar 11 17:20:03 PST 2010
/// @brief    Include file for the \ref Mesh component 

#ifndef MESH_HPP
#define MESH_HPP

//----------------------------------------------------------------------
// System includes
//----------------------------------------------------------------------

#include <stdlib.h>
#include <string>
#include <vector>
#include <memory>

#ifdef CONFIG_USE_CHARM
#  include "pup_stl.h"
#endif

//----------------------------------------------------------------------
// Component dependencies
//----------------------------------------------------------------------

#include "cello.hpp"

#include "parallel.hpp"
#include "memory.hpp"
#include "io.hpp"

#ifdef CONFIG_USE_CHARM
#  include "simulation.hpp"
#endif

// NOTE: field.hpp currently includes method.hpp: can get rid of
// dependency, but only if field.hpp include is moved after Component
// class includes
#include "field.hpp" 

//----------------------------------------------------------------------
// Component class includes
//----------------------------------------------------------------------

// Hierarchy and components
#include "mesh_Block.hpp"
#include "mesh_Patch.hpp"
#include "mesh_Hierarchy.hpp"
#include "mesh_Factory.hpp"

// Octree and components
#include "mesh_Node.hpp"
#include "mesh_Octree.hpp"

// Iterators
#include "mesh_It.hpp"
#include "mesh_ItPatch.hpp"
#include "mesh_ItBlock.hpp"

#endif /* MESH_HPP */

