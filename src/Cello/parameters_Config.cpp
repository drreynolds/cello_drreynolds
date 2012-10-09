// See LICENSE_CELLO file for license and copyright information

/// @file     parameters_Classname.cpp
/// @author   James Bordner (jobordner@ucsd.edu)
/// @date     2012-10-03
/// @brief    Implementation of the Config class 

#include "cello.hpp"
#include "parameters.hpp"

//----------------------------------------------------------------------

Config::Config() throw ()
{
}

//----------------------------------------------------------------------

Config::~Config() throw ()
{
}

//----------------------------------------------------------------------

Config::Config(const Config & config) throw ()
/// @param     config  Object being copied
{
}

//----------------------------------------------------------------------

Config & Config::operator= (const Config & config) throw ()
/// @param     config  Source object of the assignment
/// @return    The target assigned object
{
  INCOMPLETE("Config::operator=");
  return *this;
}

//----------------------------------------------------------------------

#ifdef CONFIG_USE_CHARM

void Config::pup (PUP::er &p)
{
  TRACEPUP;
  // NOTE: change this function whenever attributes change
  p | boundary_type;

  PUParray(p,domain_lower,3);
  PUParray(p,domain_upper,3);

  p | enzo_ppm_density_floor;
  p | enzo_ppm_diffusion;
  p | enzo_ppm_dual_energy;
  p | enzo_ppm_dual_energy_eta_1;
  p | enzo_ppm_dual_energy_eta_2;
  p | enzo_ppm_flattening;
  p | enzo_ppm_minimum_pressure_support_parameter;
  p | enzo_ppm_number_density_floor;
  p | enzo_ppm_pressure_floor;
  p | enzo_ppm_pressure_free;
  p | enzo_ppm_steepening;
  p | enzo_ppm_temperature_floor;
  p | enzo_ppm_use_minimum_pressure_support;

  p | field_alignment;
  PUParray(p,field_centering,MAX_FIELDS);
  p | field_courant;
  p | field_fields;
  PUParray(p,field_ghosts,3);
  p | field_padding;
  p | field_precision;
  p | field_refresh_corners;
  p | field_refresh_edges;

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

  p | output_file_groups;
  PUParray (p,output_axis,MAX_FILE_GROUPS);
  PUParray (p,output_colormap,MAX_FILE_GROUPS);
  PUParray (p,output_colormap_alpha,MAX_FILE_GROUPS);
  PUParray (p,output_field_list,MAX_FILE_GROUPS);
  PUParray (p,output_name,MAX_FILE_GROUPS);
  PUParray (p,output_dir,MAX_FILE_GROUPS);
  PUParray (p,output_schedule,MAX_FILE_GROUPS);
  PUParray (p,output_type,MAX_FILE_GROUPS);

  p | physics_cosmology;
  p | physics_cosmology_comoving_box_size;
  p | physics_cosmology_hubble_constant_now;
  p | physics_cosmology_initial_redshift;
  p | physics_cosmology_max_expansion_rate;
  p | physics_cosmology_omega_lamda_now;
  p | physics_cosmology_omega_matter_now;
  p | physics_gamma;

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
  boundary_type = parameters->value_string("Boundary:type","");

  for (int i=0; i<3; i++)  {
    domain_lower[i] = parameters->list_value_float(i, "Domain:lower", 0.0);
    domain_upper[i] = parameters->list_value_float(i, "Domain:upper", 0.0);
  }

  double floor_default = 1e-6;

  enzo_ppm_density_floor = parameters->value_float
    ("Enzo:ppm:density_floor",  floor_default);
  enzo_ppm_diffusion = parameters->value_logical 
    ("Enzo:ppm:diffusion",  false);
  enzo_ppm_dual_energy = parameters->value_logical 
    ("Enzo:ppm:dual_energy",false);
  enzo_ppm_dual_energy_eta_1 = parameters->value_float
    ("Enzo:ppm:dual_energy_eta_1", 0.001);
  enzo_ppm_dual_energy_eta_2 = parameters->value_float
    ("Enzo:ppm:dual_energy_eta_2", 0.1);
  enzo_ppm_flattening = parameters->value_integer
    ("Enzo:ppm:flattening", 3);
  enzo_ppm_minimum_pressure_support_parameter = parameters->value_integer
    ("Enzo:ppm:minimum_pressure_support_parameter",100);
  enzo_ppm_number_density_floor = parameters->value_float
    ("Enzo:ppm:number_density_floor", floor_default);
  enzo_ppm_pressure_floor = parameters->value_float
    ("Enzo:ppm:pressure_floor", floor_default);
  enzo_ppm_pressure_free = parameters->value_logical
    ("Enzo:ppm:pressure_free",false);
  enzo_ppm_steepening = parameters->value_logical 
    ("Enzo:ppm:steepening", false);
  enzo_ppm_temperature_floor = parameters->value_float
    ("Enzo:ppm:temperature_floor", floor_default);
  enzo_ppm_use_minimum_pressure_support = parameters->value_logical
    ("Enzo:ppm:use_minimum_pressure_support",false);

  num_fields = parameters->list_length("Field:fields"); 

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

  //  field_padding;
  //  field_precision;
  //  field_refresh_corners;
  //  field_refresh_edges;

  mesh_root_rank = parameters->value_integer("Mesh:root_rank",0);

  if (mesh_root_rank < 2) field_ghosts[1] = 0;
  if (mesh_root_rank < 3) field_ghosts[2] = 0;
  

  initial_cycle = parameters->value_integer("Initial:cycle",0);
  initial_time  = parameters->value_float  ("Initial:time",0.0);
  //  initial_type;
  //  initial_name;
  //  initial_value

  //  mesh_root_blocks
  //  mesh_root_size

  //  method_sequence;

  monitor_debug = parameters->value_logical("Monitor:debug",false);

  //  output_file_groups;
  //  output_axis
  //  output_colormap
  //  output_colormap_alpha
  //  output_field_list
  //  output_name
  //  output_dir
  //  output_schedule
  //  output_type

  // RENAME physics_ as enzo_

  physics_cosmology = parameters->value_logical ("Physics:cosmology",false);
  physics_cosmology_comoving_box_size = parameters->value_float
    ("Physics:cosmology:comoving_box_size", 64.0);
  physics_cosmology_hubble_constant_now = parameters->value_float
    ("Physics:cosmology:hubble_constant_now",0.701);
  physics_cosmology_initial_redshift = parameters->value_float
    ("Physics:cosmology:initial_redshift",  20.0);;
  physics_cosmology_max_expansion_rate = parameters->value_float
    ("Physics:cosmology:max_expansion_rate", 0.01);
  physics_cosmology_omega_lamda_now = parameters->value_float
    ("Physics:cosmology:omega_lambda_now",   0.721);
  physics_cosmology_omega_matter_now = parameters->value_float
    ("Physics:cosmology:omega_matter_now",   0.279);
  physics_gamma = parameters->value_float ("Physics:gamma",5.0/3.0);

  //  stopping_cycle;
  //  stopping_time;

  //  testing_cycle_final;
  //  testing_time_final;

  //  timestep_type;

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

