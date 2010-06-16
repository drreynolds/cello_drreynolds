// $Id: method_method.cpp 1262 2010-03-03 15:44:05Z bordner $
// See LICENSE_CELLO file for license and copyright information

/// @file     method_Method.cpp
/// @author   James Bordner (jobordner@ucsd.edu)
/// @date     Mon Jul 13 11:12:25 PDT 2009
/// @brief    Implements the Method base class

#include <string>

#include "method.hpp"
#include "field.hpp"
#include "monitor.hpp"

//----------------------------------------------------------------------

Method::Method() throw ()
{
}

//----------------------------------------------------------------------

void Method::add_argument_
(
 argument_type argument,
 std::string   argument_name,
 access_type   access_type,
 DataDescr   * data_descr
 ) throw()
/// @param         argument  Type of argument, field or particle
/// @param         argument_name  Name of the argument, e.g. "Density"
/// @param         access_type    Access type of the argument, e.g. read, write
{

  // Monitor output
  Monitor * monitor = Monitor::instance();
  char buffer[100];
  sprintf (buffer,"Method %s: adding %s", method_name_.c_str(), argument_name.c_str());
  monitor->print (buffer);

  // Add method argument information
  argument_types_.push_back(argument);
  argument_names_.push_back(argument_name);
  access_types_.push_back  (access_type);

  // If data_descr is passed in (default = 0), then verify that the argument
  // is defined
  if (data_descr) {
    char buffer [ ERROR_MESSAGE_LENGTH ];
    switch (argument) {
    case argument_field:
      sprintf (buffer, 
	       "Required Field %s is not defined in the field descriptor",
	       argument_name.c_str());
      ASSERT("MethodEnzoPpm::initialize_method",
	     buffer, data_descr->field_descr()->is_field(argument_name));
      break;
    case argument_particle:
    case argument_unknown:
      break;
    }
  }

}
