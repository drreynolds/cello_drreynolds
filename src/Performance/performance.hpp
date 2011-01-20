// $Id$
// See LICENSE_CELLO file for license and copyright information

#ifndef PERFORMANCE_HPP
#define PERFORMANCE_HPP

/// @file     performance.hpp
/// @author   James Bordner (jobordner@ucsd.edu)
/// @date     Wed Oct 14 23:40:13 PDT 2009
/// @brief    Include file for the \ref Performance component

#include <vector>
#include <sys/time.h>
#ifdef __linux__
#include <unistd.h>
#endif

#include "cello.hpp"

#ifdef CONFIG_USE_PAPI
#  include "papi.h"
#endif

#include "performance_Counters.hpp"
#include "performance_Performance.hpp"
#include "performance_Timer.hpp"
#include "performance_Papi.hpp"

#endif /* PERFORMANCE_HPP */
