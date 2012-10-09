// See LICENSE_CELLO file for license and copyright information

/// @file     parameters_Param.cpp
/// @author   James Bordner (jobordner@ucsd.edu)
/// @date     Sun Oct 11 15:02:08 PDT 2009
/// @brief    Implementation of the Param class

#include "cello.hpp"

#include "parameters.hpp"

//----------------------------------------------------------------------

void Param::set (struct param_struct * node)
/// @param   node  The node from which to copy the type and value
{

  value_accessed_ = false;

  switch (node->type) {
  case enum_parameter_integer:
    set_integer_(node->integer_value);
    break;
  case enum_parameter_float:
    set_float_(node->float_value);
    break;
  case enum_parameter_string:
    set_string_(node->string_value);
    break;
  case enum_parameter_logical:
    set_logical_(node->logical_value);
    break;
  case enum_parameter_list:
    set_list_(node->list_value);
    break;
  // case enum_parameter_float_expr:
  //   set_float_expr_(node->op_value);
  //   break;
  // case enum_parameter_logical_expr:
  //   set_logical_expr_(node->op_value);
  //   break;
  case enum_parameter_unknown:
  case enum_parameter_sentinel:
  case enum_parameter_function:
  case enum_parameter_group:
  case enum_parameter_identifier:
    break;
  }
}

//----------------------------------------------------------------------

void Param::dealloc_() 
///
{ 
  switch (type_) {
  case parameter_string: 
    dealloc_string_(); 
    break;
  case parameter_list:   
    dealloc_list_(value_list_); 
    break;
  // case parameter_logical_expr:
  // case parameter_float_expr:
  //   dealloc_node_expr_(value_expr_);
  //   break;
  case parameter_unknown:
  case parameter_integer:
  case parameter_float:
  case parameter_logical:
    break;
  }
} 

//----------------------------------------------------------------------

void Param::write
(
 FILE *      file_pointer,
 std::string full_parameter)
/// @param file_pointer    File pointer to which the parameter is written
/// @param full_parameter  Name of this parameter including groups
{

  // Write the parameter assignment

  size_t i_group = full_parameter.rfind(":");
  std::string parameter = (i_group == std::string::npos) ?
    full_parameter : full_parameter.substr(i_group+1,std::string::npos);

  fprintf (file_pointer,"%s = %s;\n",
	   parameter.c_str(),
	   value_to_string().c_str());

  // Write comment describing parameter access properties
  // if (value_accessed_) {
  //   fprintf (file_pointer," # Accessed\n");
  // } else {
  //   fprintf (file_pointer," # Not accessed\n");
  // }
}

//----------------------------------------------------------------------

std::string Param::value_to_string ()
{
  std::string string_buffer;
  char char_buffer[MAX_BUFFER_LENGTH];

  switch (type_) {
  case parameter_string: 
    string_buffer = std::string("\"") + value_string_ + "\"";
    break;
  case parameter_list:
    string_buffer = "[ ";
    for (size_t i=0; i<value_list_->size(); i++) {
      if ( i > 0 ) string_buffer += ", ";
      string_buffer += (*value_list_)[i]->value_to_string();
    }
    string_buffer += " ]";
    break;
  // case parameter_logical_expr:
  // case parameter_float_expr:
  //   sprintf_expression(value_expr_,char_buffer);
  //   string_buffer = char_buffer;
  //   break;
  case parameter_integer:
    sprintf (char_buffer,"%d",value_integer_);
    string_buffer = char_buffer;
    break;
  case parameter_float:
    // '#' format character forces a decimal point, which is required to
    // differentiate an integer from a float type
    sprintf (char_buffer,FLOAT_FORMAT,value_float_);
    string_buffer =  char_buffer;
    break;
  case parameter_logical:
    string_buffer = value_logical_ ? "true" : "false";
    break;
  case parameter_unknown:
    string_buffer = "UNKNOWN";
    break;
  }  
  return string_buffer;
}
 
//----------------------------------------------------------------------

void Param::dealloc_list_ (list_type * value)
/// @param value List to be deallocated
{
  for (unsigned i=0; i<(*value).size(); i++) {
    if ((*value)[i]->type_ == parameter_list) {
      dealloc_list_ ((*value)[i]->value_list_);
    } else {
      delete ( (*value)[i] );
    }
  }
  delete value;
}

//----------------------------------------------------------------------
