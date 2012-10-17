// See LICENSE_CELLO file for license and copyright information

/// @file     parameters_Config.cpp
/// @author   James Bordner (jobordner@ucsd.edu)
/// @date     2012-10-03
/// @brief    Implementation of the Config class 

#include "cello.hpp"
#include "parameters.hpp"

//----------------------------------------------------------------------

#ifdef CONFIG_USE_CHARM

void Config::pup (PUP::er &p)
{
  TRACEPUP;
  // NOTE: change this function whenever attributes change
  p | boundary_type;

  PUParray(p,domain_lower,3);
  PUParray(p,domain_upper,3);

  p | num_fields;
  p | field_alignment;
  PUParray(p,field_centering,3);
  p | field_courant;
  p | field_fields;
  PUParray(p,field_ghosts,3);
  if (p.isUnpacking()) {
    TRACE3 ("gx,gy,gz = %d %d %d",field_ghosts[0],field_ghosts[1],field_ghosts[2]);
  }

  p | field_padding;
  p | field_precision;
  p | field_refresh_corners;
  p | field_refresh_edges;
  p | field_refresh_faces;

  p | initial_cycle;
  p | initial_type;
  p | initial_time;
  p | initial_name;
  PUParray(p,initial_value,MAX_FIELDS);

  PUParray(p,mesh_root_blocks,3);
  p | mesh_root_rank;
  PUParray(p,mesh_root_size,3);

  p | method_sequence;

  p | monitor_debug;

  p | num_file_groups;
  p | output_file_groups;
  PUParray (p,output_type,MAX_FILE_GROUPS);
  PUParray (p,output_image_axis,MAX_FILE_GROUPS);
  PUParray (p,output_image_colormap,MAX_FILE_GROUPS);
  PUParray (p,output_image_colormap_alpha,MAX_FILE_GROUPS);
  PUParray (p,output_field_list,MAX_FILE_GROUPS);
  PUParray (p,output_stride,MAX_FILE_GROUPS);
  PUParray (p,output_name,MAX_FILE_GROUPS);
  PUParray (p,output_dir,MAX_FILE_GROUPS);
  PUParray (p,output_schedule_type,MAX_FILE_GROUPS);
  PUParray (p,output_schedule_var,MAX_FILE_GROUPS);
  PUParray (p,output_schedule_start,MAX_FILE_GROUPS);
  PUParray (p,output_schedule_stop,MAX_FILE_GROUPS);
  PUParray (p,output_schedule_step,MAX_FILE_GROUPS);
  PUParray (p,output_schedule_list,MAX_FILE_GROUPS);

  p | stopping_cycle;
  p | stopping_time;

  p | testing_cycle_final;
  p | testing_time_final;

  p | timestep_type;

}

#endif

//----------------------------------------------------------------------

void Config::read(Parameters * parameters) throw()
{
  TRACE("BEGIN Config::read()");
  boundary_type = parameters->value_string("Boundary:type","");

  for (int i=0; i<3; i++)  {
    domain_lower[i] = parameters->list_value_float(i, "Domain:lower", 0.0);
    domain_upper[i] = parameters->list_value_float(i, "Domain:upper", 0.0);
  }


  num_fields = parameters->list_length("Field:fields"); 

  ASSERT2 ("Config::read","Number of fields %d exceeds MAX_FIELDS %d",
	   num_fields, MAX_FIELDS, num_fields <= MAX_FIELDS);

  field_fields.resize(num_fields);
  for (int i=0; i<num_fields; i++) {
    field_fields[i] = parameters->list_value_string(i, "Field:fields");
  }

  if (parameters->type("Field:ghosts") == parameter_integer) {
    field_ghosts[0] = parameters->value_integer("Field:ghosts",0);
    field_ghosts[1] = parameters->value_integer("Field:ghosts",0);
    field_ghosts[2] = parameters->value_integer("Field:ghosts",0);
  } else if (parameters->type("Field:ghosts") == parameter_list) {
    field_ghosts[0] = parameters->list_value_integer(0,"Field:ghosts",0);
    field_ghosts[1] = parameters->list_value_integer(1,"Field:ghosts",0);
    field_ghosts[2] = parameters->list_value_integer(2,"Field:ghosts",0);
  }

  field_alignment = parameters->value_integer("Field:alignment",8);

  field_centering[0].resize(num_fields);
  field_centering[1].resize(num_fields);
  field_centering[2].resize(num_fields);
  for (int i=0; i<num_fields; i++) {

    std::string param_name = 
      std::string("Field:") + field_fields[i] + ":centering";

    field_centering[0][i] = parameters->list_value_logical(0,param_name,true);
    field_centering[1][i] = parameters->list_value_logical(1,param_name,true);
    field_centering[2][i] = parameters->list_value_logical(2,param_name,true);
    
  }

  field_courant = parameters->value_float  ("Field:courant",0.6);

  field_padding = parameters->value_integer("Field:padding",0);

  // Field precision

  std::string precision_str = parameters->value_string("Field:precision","default");

  if      (precision_str == "default")   field_precision = precision_default;
  else if (precision_str == "single")    field_precision = precision_single;
  else if (precision_str == "double")    field_precision = precision_double;
  else if (precision_str == "quadruple") field_precision = precision_quadruple;
  else {
    ERROR1 ("Config::read()", "Unknown precision %s",
	    precision_str.c_str());
  }

  field_refresh_corners = parameters->value_logical ("Field:refresh:corners",true);
  field_refresh_edges   = parameters->value_logical ("Field:refresh:edges",  true);
  field_refresh_faces   = parameters->value_logical ("Field:refresh:faces",  true);

  mesh_root_rank = parameters->value_integer("Mesh:root_rank",0);

  if (mesh_root_rank < 2) field_ghosts[1] = 0;
  if (mesh_root_rank < 3) field_ghosts[2] = 0;
  

  initial_cycle = parameters->value_integer("Initial:cycle",0);
  initial_time  = parameters->value_float  ("Initial:time",0.0);
  initial_type  = parameters->value_string("Initial:type","default");

  //  initial_name;
  //  initial_value

  mesh_root_blocks[0] = parameters->list_value_integer(0,"Mesh:root_blocks",1);
  mesh_root_blocks[1] = parameters->list_value_integer(1,"Mesh:root_blocks",1);
  mesh_root_blocks[2] = parameters->list_value_integer(2,"Mesh:root_blocks",1);

#ifndef CONFIG_USE_CHARM
  int root_blocks = mesh_root_blocks[0]*mesh_root_blocks[1]*mesh_root_blocks[2];
  GroupProcess * group_process = GroupProcess::create();
  ASSERT4 ("Config::read()",
	   "Product of Mesh:root_blocks = [%d %d %d] must equal MPI_Comm_size",
	   mesh_root_blocks[0],
	   mesh_root_blocks[1],
	   mesh_root_blocks[2],
	   group_process->size(),
	   root_blocks==group_process->size());
  delete group_process;
#endif


  mesh_root_size[0] = parameters->list_value_integer(0,"Mesh:root_size",1);
  mesh_root_size[1] = parameters->list_value_integer(1,"Mesh:root_size",1);
  mesh_root_size[2] = parameters->list_value_integer(2,"Mesh:root_size",1);

  int size = parameters->list_length("Method:sequence");
  method_sequence.resize(size);
  for (int i=0; i<size; i++) {
    method_sequence[i] = parameters->list_value_string(i,"Method:sequence");
  }

  monitor_debug = parameters->value_logical("Monitor:debug",false);

  // Output

  parameters->group_set(0,"Output");

  num_file_groups = parameters->list_length("file_groups");

  ASSERT2 ("Config::read","Number of file groups %d exceeds MAX_FILE_GROUPS %d",
	   num_file_groups, MAX_FILE_GROUPS, num_file_groups <= MAX_FILE_GROUPS);

  parameters->group_set(0,"Output");


  output_file_groups.resize(num_file_groups);

  for (int index=0; index<num_file_groups; index++) {

    TRACE1 ("index = %d",index);

    output_file_groups[index] = 
      parameters->list_value_string (index,"Output:file_groups","unknown");

    parameters->group_set(1,output_file_groups[index]);

    output_type[index] = parameters->value_string("type","unknown");

    if (output_type[index] == "unknown") {
      ERROR1("Config::read",
	     "Output:%s:type parameter is undefined",
	     output_file_groups[index].c_str());
    }

    output_stride[index] = parameters->value_integer("stride",0);

    if (parameters->type("dir") == parameter_string) {
      output_dir[index].resize(1);
      output_dir[index][0] = parameters->value_string("dir","");
    } else if (parameters->type("dir") == parameter_list) {
      int size = parameters->list_length("dir");
      if (size > 0) output_dir[index].resize(size);
      for (int i=0; i<size; i++) {
	output_dir[index][i] = parameters->list_value_string(i,"dir","");
	TRACE3("output_dir[%d][%d] = %s",index,i,output_dir[index][i].c_str());
      }
    }

    TRACE1("index = %d",index);
    if (parameters->type("name") == parameter_string) {
      TRACE0;
      output_name[index].resize(1);
      output_name[index][0] = parameters->value_string("name","");
    } else if (parameters->type("name") == parameter_list) {
      int size = parameters->list_length("name");
      TRACE1("size = %d",size);
      if (size > 0) output_name[index].resize(size);
      for (int i=0; i<size; i++) {
	output_name[index][i] = parameters->list_value_string(i,"name","");
      }
    }
    //  output_axis
    //  output_colormap
    //  output_colormap_alpha
    if (parameters->type("field_list") == parameter_list) {
      int length = parameters->list_length("field_list");
      output_field_list[index].resize(length);
      for (int i=0; i<length; i++) {
	output_field_list[index][i] = parameters->list_value_string(i,"field_list","");
      }
    }
      
    //  output_schedule

    ASSERT1("Config::read",
	   "'schedule' is not defined for output file group %s",
	    output_file_groups[index].c_str(),
	    (parameters->type("schedule") != parameter_unknown));

    int length = parameters->list_length("schedule");
    ASSERT1("Config::read","Incorrect 'schedule' for Output group %s",
	    output_file_groups[index].c_str(), (length >= 3));

    output_schedule_var[index]  = parameters->list_value_string(0,"schedule");
    output_schedule_type[index] = parameters->list_value_string(1,"schedule");

    bool var_is_int = true;

    if (output_schedule_var[index] == "cycle") {
      var_is_int = true;
    } else if (output_schedule_var[index] == "time") {
      var_is_int = false;
    } else {
      ERROR2 ("Config::read",
	      "Schedule variable %s is not recognized for output file group %s",
	      output_schedule_var[index].c_str(),output_file_groups[index].c_str());
    }

    const int    max_int    = std::numeric_limits<int>::max();
    const double max_double = std::numeric_limits<double>::max();

    if (output_schedule_type[index] == "interval") {
      if (length == 3) { // step
	if (var_is_int) {
	  output_schedule_start[index] = 0;
	  output_schedule_step[index]  = parameters->list_value_integer(2,"schedule");
	  output_schedule_stop[index]  = max_int;
	} else {
	  output_schedule_start[index] = 0.0;
	  output_schedule_step[index]  = parameters->list_value_float(2,"schedule");
	  output_schedule_stop[index]  = max_double;
	}
      } else if (length == 4) { // start, step
	if (var_is_int) {
	  output_schedule_start[index] = parameters->list_value_integer(2,"schedule");
	  output_schedule_step[index]  = parameters->list_value_integer(3,"schedule");
	  output_schedule_stop[index]  = max_int;
	} else {
	  output_schedule_start[index] = parameters->list_value_float(2,"schedule");
	  output_schedule_step[index]  = parameters->list_value_float(3,"schedule");
	  output_schedule_stop[index]  = max_double;
	}
      } else if (length == 5) { // start, step, stop
	if (var_is_int) {
	  output_schedule_start[index] = parameters->list_value_integer(2,"schedule");
	  output_schedule_step[index]  = parameters->list_value_integer(3,"schedule");
	  output_schedule_stop[index]  = parameters->list_value_integer(4,"schedule");
	} else {
	  output_schedule_start[index] = parameters->list_value_float(2,"schedule");
	  output_schedule_step[index]  = parameters->list_value_float(3,"schedule");
	  output_schedule_stop[index]  = parameters->list_value_float(4,"schedule");
	}
      }
    } else if (output_schedule_type[index] == "list") {
      output_schedule_list[index].resize(length-2);
      for (int i=2; i<length; i++) {
	if (var_is_int) {
	  output_schedule_list[index][i-2] = parameters->list_value_integer(i,"schedule");
	} else {
	  output_schedule_list[index][i-2] = parameters->list_value_float(i,"schedule");
	}
      }
    } else {
      ERROR2 ("Config::read",
	      "Schedule type %s is not recognized for output file group %s",
	      output_schedule_type[index].c_str(),output_file_groups[index].c_str());
    }

    // Image 

    TRACE2 ("output_type[%d] = %s",index,output_type[index].c_str());

    if (output_type[index] == "image") {

      output_image_axis[index] = "z";

      if (parameters->type("axis") != parameter_unknown) {
	std::string axis = parameters->value_string("axis");
	ASSERT2("Problem::initialize_output",
		"Output %s axis %d must be \"x\", \"y\", or \"z\"",
		output_file_groups[index].c_str(), axis.c_str(),
		axis=="x" || axis=="y" || axis=="z");
      } 

      if (parameters->type("colormap") == parameter_list) {
	int size = parameters->list_length("colormap");
	output_image_colormap[index].resize(size);
	for (int i=0; i<size; i++) {
	  output_image_colormap[index][i] = parameters->list_value_float(i,"colormap",0.0);
	}
      }

      if (parameters->type("colormap_alpha") == parameter_list) {
	int size = parameters->list_length("colormap_alpha");
	output_image_colormap_alpha[index].resize(size);
	for (int i=0; i<size; i++) {
	  output_image_colormap_alpha[index][i] = 
	    parameters->list_value_float(i,"colormap_alpha",0.0);
	}
      }

    }

  }  

  stopping_cycle = parameters->value_integer
    ( "Stopping:cycle" , std::numeric_limits<int>::max() );
  stopping_time  = parameters->value_float
    ( "Stopping:time" , std::numeric_limits<double>::max() );

  testing_cycle_final = parameters->value_integer("Testing:cycle_final",0);
  testing_time_final  = parameters->value_float  ("Testing:time_final", 0.0);

  timestep_type = parameters->value_string("Timestep:type","default");

  TRACE("END   Config::read()");
}

//----------------------------------------------------------------------

void Config::evaluate_float 
(
 std::string string_expr,
 int         n, 
 double    * result, 
 double    * deflt,
 double    * x, 
 double    * y, 
 double    * z, 
 double    * t) throw()
/// @param   string_expr String containing the float-expression to evaluate
/// @param   n         Length of variable arrays
/// @param   result    Output array of evaluated floating point parameters values if it exists, or deflt if not
/// @param   deflt     Array of default values
/// @param   x         Array of X values
/// @param   y         Array of Y values
/// @param   z         Array of Z values
/// @param   t         Array of T values
{

  // generate expression

  // evaluate expression

  Param * param = parameter_(parameter);
  ASSERT1 ("Config::evaluate_float",
	   "Parameter %s is not a floating-point expression", parameter.c_str(),
	   ( ! param || param->is_float_expr()));
  if (param != NULL) {
    param->evaluate_float(param->value_expr_,n,result,x,y,z,t);
  } else {
    for (int i=0; i<n; i++) result[i] = deflt[i];
  }
  // char deflt_string[MAX_PARAMETER_FILE_WIDTH];
  // sprintf_expression (param->value_expr_,deflt_string);
  // monitor_access_(parameter,deflt_string);
}

//----------------------------------------------------------------------

void Config::evaluate_logical 
(
 std::string string_expr,
 int         n, 
 bool      * result, 
 bool      * deflt,
 double    * x, 
 double    * y, 
 double    * z, 
 double    * t) throw()
/// @param   string_expr String containing the logical-expression to evaluate
/// @param   n         Length of variable arrays
/// @param   result    Output array of evaluated logical parameters values if it exists, or deflt if not
/// @param   deflt     Array of default values
/// @param   x         Array of X values
/// @param   y         Array of Y values
/// @param   z         Array of Z values
/// @param   t         Array of T values
{
  Param * param = parameter_(parameter);
  ASSERT1 ("Config::evaluate_logical",
	   "Parameter %s is not a logical expression", parameter.c_str(),
	   (! param || param->is_logical_expr()));
  if (param != NULL) {
    param->evaluate_logical(param->value_expr_,n,result,x,y,z,t);
  } else {
    for (int i=0; i<n; i++) result[i] = deflt[i];
  }
  // char deflt_string[MAX_PARAMETER_FILE_WIDTH];
  // sprintf_expression (param->value_expr_,deflt_string);
  // monitor_access_(parameter,deflt_string);
}

//----------------------------------------------------------------------

void Config::evaluate_float_
(struct node_expr * node, 
 int                n, 
 double *           result, 
 double *           x, 
 double *           y, 
 double *           z, 
 double *           t)
/// @param node Head node of the tree defining the floating-point expression
/// @param n Length of the result buffer
/// @param result Array in which to store the expression evaluations
/// @param x Array of X spatial values
/// @param y Array of Y spatial values
/// @param z Array of Z spatial values
/// @param t Array of time values
{
  double * left  = NULL;
  double * right = NULL;

  value_accessed_ = true;

  if (node->left) {
    left = new double [n];
    evaluate_float(node->left,n,left,x,y,z,t);
  }
  if (node->right) {
    right = new double [n];
    evaluate_float(node->right,n,right,x,y,z,t);
  }
      
  int i;
  switch (node->type) {
  case enum_node_operation:
    switch (node->op_value) {
    case enum_op_add: for (i=0; i<n; i++) result[i] = left[i] + right[i]; break;
    case enum_op_sub: for (i=0; i<n; i++) result[i] = left[i] - right[i]; break;
    case enum_op_mul: for (i=0; i<n; i++) result[i] = left[i] * right[i]; break;
    case enum_op_div: for (i=0; i<n; i++) result[i] = left[i] / right[i]; break;
    default:
    case enum_op_le:
    case enum_op_lt:
    case enum_op_ge:
    case enum_op_gt:
    case enum_op_eq:
    case enum_op_ne:
    case enum_op_and:
    case enum_op_or:
      ERROR1("Config::evaluate_float",
	     "logical operator %d in floating-point expression",
	     node->op_value);
      break;
    }
    break;
  case enum_node_float:
    for (i=0; i<n; i++) result[i] = node->float_value;
    break;
  case enum_node_integer:
    for (i=0; i<n; i++) result[i] = double(node->integer_value);
    break;
  case enum_node_variable:
    switch (node->var_value) {
    case 'x':	for (i=0; i<n; i++) result[i] = x[i]; break;
    case 'y':	for (i=0; i<n; i++) result[i] = y[i]; break;
    case 'z':	for (i=0; i<n; i++) result[i] = z[i]; break;
    case 't':	for (i=0; i<n; i++) result[i] = t[i]; break;
    default:
      ERROR1("Config::evaluate_float",
	     "unknown variable %c in floating-point expression",
	     node->var_value);
      break;
    }
    break;
  case enum_node_function:
    for (i=0; i<n; i++) result[i] = (*(node->fun_value))(left[i]);
    break;
  case enum_node_unknown:
  default:
    ERROR1("Config::evaluate_float",
	   "unknown expression type %d",
	   node->type);
    break;
  }

  delete [] left;
  delete [] right;
}

//----------------------------------------------------------------------

void Config::evaluate_logical_
(struct node_expr * node, 
 int                n, 
 bool   *           result, 
 double *           x, 
 double *           y, 
 double *           z, 
 double *           t)
/// @param node Head node of the tree defining the floating-point expression
/// @param n Length of the result buffer
/// @param result Array in which to store the expression evaluations
/// @param x Array of X spatial values
/// @param y Array of Y spatial values
/// @param z Array of Z spatial values
/// @param t Array of time values
{
  double * left_float  = NULL;
  double * right_float = NULL;
  bool * left_logical  = NULL;
  bool * right_logical = NULL;

  value_accessed_ = true;

  // Recurse on left subtree

  if (node->left && node->left->type == enum_node_operation) {
    // left node is an operation
    if (node->op_value == enum_op_and || 
	node->op_value == enum_op_or) {
      // left node is a logical operation
      left_logical = new bool [n];
      evaluate_logical(node->left,n,left_logical,x,y,z,t);
    } else {
      // left node is a floating-point operation
      left_float = new double [n];
      evaluate_float(node->left,n,left_float,x,y,z,t);
    }
  } else {
    // left node is a floating-point operation
    left_float = new double [n];
    evaluate_float(node->left,n,left_float,x,y,z,t);
  }

  // Recurse on left subtree

  if (node->right && node->right->type == enum_node_operation) {
    // right node exists
    // right node is an operation
    if (node->op_value == enum_op_and || 
	node->op_value == enum_op_or) {
      // right node is a logical operation
      right_logical = new bool [n];
      evaluate_logical(node->right,n,right_logical,x,y,z,t);
    } else {
      // right node is a floating-point operation
      right_float = new double [n];
      evaluate_float(node->right,n,right_float,x,y,z,t);
    }
  } else {
    // right node is a floating-point operation
    right_float = new double [n];
    evaluate_float(node->right,n,right_float,x,y,z,t);
  }
      
  int i;
  if (node->type == enum_node_operation) {
    switch (node->op_value) {
    case enum_op_le:
      for (i=0; i<n; i++) result[i] = left_float[i] <= right_float[i];
      break;
    case enum_op_lt:
      for (i=0; i<n; i++) result[i] = left_float[i] <  right_float[i];
      break;
    case enum_op_ge:
      for (i=0; i<n; i++) result[i] = left_float[i] >= right_float[i];
      break;
    case enum_op_gt:
      for (i=0; i<n; i++) result[i] = left_float[i] >  right_float[i];
      break;
    case enum_op_eq:
      // warning: comparing equality of doubles
      for (i=0; i<n; i++) result[i] = left_float[i] == right_float[i];
      break;
    case enum_op_ne:
      // warning: comparing inequality of doubles
      for (i=0; i<n; i++) result[i] = left_float[i] != right_float[i];
      break;
    case enum_op_and:
      for (i=0; i<n; i++) result[i] = left_logical[i] && right_logical[i];
      break;
    case enum_op_or:
      for (i=0; i<n; i++) result[i] = left_logical[i] || right_logical[i];
      break;
    default:
      ERROR1("Config::evaluate_logical",
	     "unknown expression type %d",
	     node->type);
      break;
    }
  }

  delete [] left_float;
  delete [] right_float;
  delete [] left_logical;
  delete [] right_logical;
}

//----------------------------------------------------------------------

void Config::dealloc_node_expr_ (struct node_expr * p)
/// @param p Head node of the tree defining the floating-point 
/// expression to deallocate
{
  if (p->left != NULL)  dealloc_node_expr_(p->left);
  if (p->right != NULL) dealloc_node_expr_(p->right);
  free (p->function_name);
  free (p);
};

//======================================================================

