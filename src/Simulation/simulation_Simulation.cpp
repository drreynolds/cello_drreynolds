// $Id$
// See LICENSE_CELLO file for license and copyright information

/// @file      simulation_Simulation.cpp
/// @author    James Bordner (jobordner@ucsd.edu)
/// @date      2010-11-10
/// @brief     Implementation of the Simulation class

#include "cello.hpp"

#include "simulation.hpp"

/// Initialize the Simulation object
Simulation::Simulation
(
 Factory *      factory,
 Parameters *   parameters,
 GroupProcess * group_process
)
  : factory_(factory),
    dimension_(0),
    group_process_(group_process),
    parameters_(parameters),
    mesh_(0),
    field_descr_(0),
    stopping_(0),
    timestep_(0),
    initial_(0),
    boundary_(0),
    output_list_(),
    method_list_()
{
  ASSERT("Simulation::Simulation","Parameters is NULL", parameters != NULL);

  lower_[0] = 0.0;
  lower_[1] = 0.0;
  lower_[2] = 0.0;

  upper_[0] = 1.0;
  upper_[1] = 1.0;
  upper_[2] = 1.0;

}

//----------------------------------------------------------------------

Simulation::~Simulation() throw()
{
  deallocate_();
}

//----------------------------------------------------------------------

Simulation::Simulation(const Simulation & simulation) throw()
{
  INCOMPLETE("Simulation::Simulation(simulation)");
}

//----------------------------------------------------------------------


Simulation & Simulation::operator= (const Simulation & simulation) throw()
{
  INCOMPLETE("Simulation::operatior = (simulation)");
  return (*this);
}

//----------------------------------------------------------------------

void Simulation::initialize() throw()
{

  // Initialize parameters

  initialize_simulation_();

  // Initialize simulation components

  initialize_data_();
  initialize_mesh_();

  initialize_stopping_();
  initialize_timestep_();
  initialize_initial_();
  initialize_boundary_();
  initialize_output_();
  initialize_method_();

}

//----------------------------------------------------------------------

void Simulation::finalize() throw()
{
  deallocate_();
}

//----------------------------------------------------------------------

int Simulation::dimension() const throw()
{
  return dimension_;
}

//----------------------------------------------------------------------

void Simulation::lower (double * xm, double *ym, double *zm) const throw()
{
  if (xm) *xm = lower_[0];
  if (ym) *ym = lower_[1];
  if (zm) *zm = lower_[2];
}

//----------------------------------------------------------------------

void Simulation::upper (double * xp, double *yp, double *zp) const throw()
{
  if (xp) *xp = upper_[0];
  if (yp) *yp = upper_[1];
  if (zp) *zp = upper_[2];
}

//----------------------------------------------------------------------

Mesh * Simulation::mesh() const throw()
{
  return mesh_;
}
  
//----------------------------------------------------------------------

FieldDescr * Simulation::field_descr() const throw()
{
  return field_descr_;
}

//----------------------------------------------------------------------

Stopping * Simulation::stopping() const throw() 
{ return stopping_; }

//----------------------------------------------------------------------

Timestep * Simulation::timestep() const throw() 
{ return timestep_; }

//----------------------------------------------------------------------

Initial * Simulation::initial() const throw() 
{ return initial_; }

//----------------------------------------------------------------------

Boundary * Simulation::boundary() const throw() 
{ return boundary_; }

//----------------------------------------------------------------------

int Simulation::num_output() const throw()
{ return output_list_.size(); }

//----------------------------------------------------------------------

Output * Simulation::output(int i) const throw()
{ return output_list_[i]; }

//----------------------------------------------------------------------

int Simulation::num_method() const throw()
{ return method_list_.size(); }

//----------------------------------------------------------------------

Method * Simulation::method(int i) const throw()
{ return method_list_[i]; }

//----------------------------------------------------------------------

Factory * Simulation::factory () const throw()
{ return factory_; }

//======================================================================

void Simulation::initialize_simulation_() throw()
{

  //--------------------------------------------------
  parameters_->set_current_group("Physics");
  //--------------------------------------------------

  // parameter: Physics::dimensions

  dimension_ = parameters_->value_integer("dimensions",0);


  //--------------------------------------------------
  parameters_->set_current_group("Domain");
  //--------------------------------------------------

  // parameter: Domain::dimensions
  // parameter: Domain::extent

  // get extent_length and check consistency

  int extent_length = parameters_->list_length("extent");

  bool valid_extent_length = ((extent_length == 2) ||
			      (extent_length == 4) ||
			      (extent_length == 6));
  ASSERT ("Simulation::initialize_simulation_",
	  "Parameter Domain:extent list must have length 2, 4, or 6",
	  valid_extent_length);

  ASSERT ("Simulation::initialize_simulation_",
	  "Parameter mismatch between Physics:dimensions and Domain:extent",
	  (dimension_ == extent_length / 2));

  for (int i=0; i<extent_length/2; i++) {

    lower_[i] = parameters_->list_value_scalar(2*i,   "extent", 0.0);
    upper_[i] = parameters_->list_value_scalar(2*i+1, "extent", 1.0);

    if (lower_[i] > upper_[i]) {
      char error_message[ERROR_LENGTH];
      sprintf (error_message,
	       "Parameter Domain::extent[%d]==%g not lower than "
	       "Domain::extent[%d]==%g",
	       2*i,lower_[i],2*i+1,upper_[i]);
      ERROR ("Simulation::Simulation", error_message);
    }
  }

}

//----------------------------------------------------------------------

void Simulation::initialize_data_() throw()
{

  /* MEMORY LEAK */
  field_descr_ = new FieldDescr;

  //--------------------------------------------------
  parameters_->set_current_group("Field");
  //--------------------------------------------------

  // parameter: Field::fields

  // Add data fields

  int i;
  for (i=0; i<parameters_->list_length("fields"); i++) {
    field_descr_->insert_field
      (parameters_->list_value_string(i, "fields"));
  }

  // Define default ghost zone depth for all fields, default value of 1

  // @@@ WARNING: REPEATED CODE: SEE enzo_EnzoBlock.cpp

  // parameter: Field::ghosts

  int gx = 1;
  int gy = 1;
  int gz = 1;

  if (parameters_->type("ghosts") == parameter_integer) {
    gx = gy = gz = parameters_->value_integer("ghosts",1);
  } else if (parameters_->type("ghosts") == parameter_list) {
    gx = parameters_->list_value_integer(0,"ghosts",1);
    gy = parameters_->list_value_integer(1,"ghosts",1);
    gz = parameters_->list_value_integer(2,"ghosts",1);
  }

  if (dimension_ < 2) gy = 0;
  if (dimension_ < 3) gz = 0;

  for (i=0; i<field_descr_->field_count(); i++) {
    field_descr_->set_ghosts(i,gx,gy,gz);
    int gx2,gy2,gz2;
    field_descr_->ghosts(i,&gx2,&gy2,&gz2);
  }

  // Set precision

  // parameter: Field::precision

  std::string precision_str = parameters_->value_string("precision","default");

  precision_enum precision = precision_default;

  if (precision_str == "single")
    precision = precision_single;
  else if (precision_str == "double")
    precision = precision_double;
  else if (precision_str == "quadruple")
    precision = precision_quadruple;

  for (i=0; i<field_descr_->field_count(); i++) {
    field_descr_->set_precision(i,precision);
  }
}

//----------------------------------------------------------------------

void Simulation::initialize_mesh_() throw()
{

  ASSERT("Simulation::initialize_mesh_",
	 "data must be initialized before mesh",
	 field_descr_ != NULL);

  //--------------------------------------------------
  parameters_->set_current_group ("Mesh");
  //--------------------------------------------------

  int root_size[3];

  // parameter: Mesh::root_size

  root_size[0] = parameters_->list_value_integer(0,"root_size",1);
  root_size[1] = parameters_->list_value_integer(1,"root_size",1);
  root_size[2] = parameters_->list_value_integer(2,"root_size",1);

  int root_blocks[3];

  // parameter: Mesh::root_blocks

  root_blocks[0] = parameters_->list_value_integer(0,"root_blocks",1);
  root_blocks[1] = parameters_->list_value_integer(1,"root_blocks",1);
  root_blocks[2] = parameters_->list_value_integer(2,"root_blocks",1);

  mesh_ = factory()->create_mesh 
    (group_process_,
     root_size[0],root_size[1],root_size[2],
     root_blocks[0],root_blocks[1],root_blocks[2]);

  // Allocate and insert the root patch, using all processes

  Patch * root_patch = factory()->create_patch
    (group_process_,
     root_size[0],root_size[1],root_size[2],
     root_blocks[0],root_blocks[1],root_blocks[2]);

  mesh_->insert_patch(root_patch);

  // Domain extents

  //--------------------------------------------------
  parameters_->set_current_group("Domain");
  //--------------------------------------------------

  double domain_lower[3];

  // parameter: Domain::extent

  domain_lower[0] = parameters_->list_value_scalar(0,"extent",0.0);
  domain_lower[1] = parameters_->list_value_scalar(2,"extent",0.0);
  domain_lower[2] = parameters_->list_value_scalar(4,"extent",0.0);

  mesh_->set_lower(domain_lower[0],
		   domain_lower[1],
		   domain_lower[2]);

  double domain_upper[3];

  domain_upper[0] = parameters_->list_value_scalar(1,"extent",1.0);
  domain_upper[1] = parameters_->list_value_scalar(3,"extent",1.0);
  domain_upper[2] = parameters_->list_value_scalar(5,"extent",1.0);

  mesh_->set_upper(domain_upper[0],
		   domain_upper[1],
		   domain_upper[2]);

  // Create the root patch

  root_patch->set_lower(domain_lower[0],
			domain_lower[1],
			domain_lower[2]);

  root_patch->set_upper(domain_upper[0],
			domain_upper[1],
			domain_upper[2]);

  root_patch->allocate_blocks(field_descr_);

  // Parallel layout of the root patch
  
  Layout * layout = root_patch->layout();

  //--------------------------------------------------
  parameters_->set_current_group("Mesh");
  //--------------------------------------------------

  // parameter: Mesh::root_process_first
  // parameter: Mesh::root_process_count

  int process_first = parameters_->value_integer("root_process_first",0);
  int process_count = parameters_->value_integer("root_process_count",1);

  layout->set_process_range(process_first, process_count);

  // Mesh AMR settings

  // parameter: Mesh::refine
  // parameter: Mesh::max_level
  // parameter: Mesh::balanced
  // parameter: Mesh::backfill
  // parameter: Mesh::coalesce

  mesh_->set_refine_factor (parameters_->value_integer("refine",    2));
  mesh_->set_max_level     (parameters_->value_integer("max_level", 0));
  mesh_->set_balanced      (parameters_->value_logical("balanced",  true));
  mesh_->set_backfill      (parameters_->value_logical("backfill",  true));
  mesh_->set_coalesce      (parameters_->value_logical("coalesce",  true));

}

//----------------------------------------------------------------------

void Simulation::initialize_stopping_() throw()
{
  stopping_ = create_stopping_("ignored");
}

//----------------------------------------------------------------------

void Simulation::initialize_timestep_() throw()
{
  timestep_ = create_timestep_("ignored");
}

//----------------------------------------------------------------------

void Simulation::initialize_initial_() throw()
{
  //--------------------------------------------------
  parameters_->set_current_group("Initial");
  //--------------------------------------------------

  // parameter: Initial::problem

  std::string name = parameters_->value_string("problem","default");

  initial_ = create_initial_(name);

  if (initial_ == 0) {
    initial_ = new InitialDefault(parameters_);
  }
}

//----------------------------------------------------------------------

void Simulation::initialize_boundary_() throw()
{
  //--------------------------------------------------
  parameters_->set_current_group("Boundary");
  //--------------------------------------------------

  // parameter: Boundary::name

  std::string name = parameters_->value_string("name","");
  boundary_ = create_boundary_(name);
}


//----------------------------------------------------------------------

void Simulation::initialize_output_() throw()
{
  UNTESTED("Simulation::initialize_output_");

  //--------------------------------------------------
  parameters_->set_current_group("Output");
  //--------------------------------------------------

  int num_groups = parameters_->subgroup_count();

  // Create and initialize an Output object for each Output group

  for (int index_group=0; index_group < num_groups; index_group++) {

    std::string group = parameters_->subgroup(index_group);

    parameters_->set_current_subgroup(group);

    // // parameter: Output:<group>:type

    // std::string type = parameters_->value_string("type","unknown");

    // // Error if output type is not defined
    // if (type == "unknown") {
    //   char buffer[ERROR_LENGTH];
    //   sprintf (buffer,"Output:%s:type parameter is undefined",group.c_str());
    //   ERROR("Simulation::initialize_output_",buffer);
    // }

    // parameter: Output:<group>:file_name

    std::string file_name = parameters_->value_string("file_name");

    Output * output = create_output_(file_name);

    // // Error if output type is not recognized
    // if (output == NULL) {
    //   char buffer[ERROR_LENGTH];
    //   sprintf (buffer,"Unrecognized parameter value Output:%s:type = %s",
    // 	       group.c_str(),type.c_str());
    //   ERROR("Simulation::initialize_output_",buffer);
    // }

    // ASSERT("Simulation::initialize_output_",
    // 	   "Bad type for Output 'file_name' parameter",
    // 	   parameters_->type("file_name") == parameter_string);

    bool cycle_interval,cycle_list,time_interval,time_list;

    // parameter: Output:<group>:cycle_interval
    // parameter: Output:<group>:cycle_list
    // parameter: Output:<group>:time_interval
    // parameter: Output:<group>:time_list

    cycle_interval = (parameters_->type("cycle_interval") != parameter_unknown);
    cycle_list     = (parameters_->type("cycle_list")     != parameter_unknown);
    time_interval  = (parameters_->type("time_interval")  != parameter_unknown);
    time_list      = (parameters_->type("time_list")      != parameter_unknown);

    ASSERT("Simulation::initialize_output_",
	   "exactly one of [cycle|time]_[interval|list] must be defined for each Output group",
	   (cycle_interval? 1 : 0 + 
	    cycle_list?     1 : 0 + 
	    time_interval?  1 : 0 + 
	    time_list?      1 : 0) == 1);

    if (cycle_interval) {

      const int max_int = std::numeric_limits<int>::max();
      int cycle_start = 0;
      int cycle_step = 0;
      int cycle_stop = max_int;
      if (parameters_->type("cycle_interval") == parameter_integer) {
	cycle_step = parameters_->value_integer("cycle_interval",1);
      } else if (parameters_->type("cycle_interval") == parameter_list) {
	if (parameters_->list_length("cycle_interval") != 3) {
	  ERROR("Simulation::initialize_output_",
		"Output cycle_interval parameter must be of the form "
		"[cycle_start, cycle_step, cycle_stop");
	}
	cycle_start = 
	  parameters_->list_value_integer (0,"cycle_interval",0);
	cycle_step  = 
	  parameters_->list_value_integer (1,"cycle_interval",1);
	cycle_stop  = 
	  parameters_->list_value_integer (2,"cycle_interval",max_int);
      } else {
	ERROR("Simulation::initialize_output_",
	      "Output cycle_interval is of the wrong type");
      }
      output->set_cycle_interval(cycle_start,cycle_step,cycle_stop);

    } else if (cycle_list) {

      std::vector<int> list;

      if (parameters_->type("cycle_list") == parameter_integer) {
	list.push_back (parameters_->value_integer("cycle_list",0));
      } else if (parameters_->type("cycle_list") == parameter_list) {
	for (int i=0; i<parameters_->list_length("cycle_list"); i++) {
	  int value = parameters_->list_value_integer(i,"cycle_list",0);
	  list.push_back (value);
	  // check monotonicity
	  if (i > 0) {
	    int value_prev = parameters_->list_value_integer(i-1,"cycle_list",0);
	    ASSERT("Simulation::initialize_output_",
		   "Output cycle_list must be monotonically increasing",
		   value_prev < value);
	  }
	}
      }

      output->set_cycle_list(list);

    } else if (time_interval) {

      const double max_double = std::numeric_limits<double>::max();
      double time_start = 0;
      double time_step = 0;
      double time_stop = max_double;
      if (parameters_->type("time_interval") == parameter_scalar) {
	time_step = parameters_->value_scalar("time_interval",1);
      } else if (parameters_->type("time_interval") == parameter_list) {
	if (parameters_->list_length("time_interval") != 3) {
	  ERROR("Simulation::initialize_output_",
		"Output time_interval parameter must be of the form "
		"[time_start, time_step, time_stop");
	}
	time_start = 
	  parameters_->list_value_scalar (0,"time_interval",0);
	time_step  = 
	  parameters_->list_value_scalar (1,"time_interval",1);
	time_stop  = 
	  parameters_->list_value_scalar (2,"time_interval",max_double);
      } else {
	ERROR("Simulation::initialize_output_",
	      "Output time_interval is of the wrong type");
      }
      output->set_time_interval(time_start,time_step,time_stop);

    } else if (time_list) {

      std::vector<double> list;

      if (parameters_->type("time_list") == parameter_scalar) {
	list.push_back (parameters_->value_scalar("time_list",0));
      } else if (parameters_->type("time_list") == parameter_list) {
	for (int i=0; i<parameters_->list_length("time_list"); i++) {
	  double value = parameters_->list_value_scalar(i,"time_list",0.0);
	  list.push_back (value);
	  // check monotonicity
	  if (i > 0) {
	    double value_prev = parameters_->list_value_scalar(i-1,"time_list",0);
	    ASSERT("Simulation::initialize_output_",
		   "Output time_list must be monotonically increasing",
		   value_prev < value);
	  }
	}
      }

      output->set_time_list(list);

    }

    output_list_.push_back(output); 

  } // (for index_group)

}

//----------------------------------------------------------------------

void Simulation::initialize_method_() throw()
{
  //--------------------------------------------------
  parameters_->set_current_group("Method");
  //--------------------------------------------------

  int method_count = parameters_->list_length("sequence");

  if (method_count == 0) {
    ERROR ("Simulation::initialize_method_",
		   "List parameter 'Method sequence' must have length greater than zero");
  }

  for (int i=0; i<method_count; i++) {

    // parameter: Method::sequence

    std::string method_name = parameters_->list_value_string(i,"sequence");

    Method * method = create_method_(method_name);
    if (method) {

      method_list_.push_back(method); 

    } else {
      char error_message[ERROR_LENGTH];
      sprintf (error_message,"Unknown Method %s",method_name.c_str());
      ERROR ("Simulation::initialize_method_",
		     error_message);
    }
  }


  INCOMPLETE("Simulation::initialize_method_");
}

//----------------------------------------------------------------------

void Simulation::deallocate_() throw()
{
  delete mesh_;
  mesh_ = 0;
  delete field_descr_;
  field_descr_ = 0;
  delete stopping_;
  stopping_ = 0;
  delete timestep_;
  timestep_ = 0;
  delete initial_;
  initial_ = 0;
  delete boundary_;
  boundary_ = 0;
  delete factory_;
  factory_ = 0;
  for (size_t i=0; i<method_list_.size(); i++) {
    delete method_list_[i];
    method_list_[i] = 0;
  }
}
