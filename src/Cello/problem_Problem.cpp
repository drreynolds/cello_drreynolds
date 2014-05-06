// See LICENSE_CELLO file for license and copyright information

/// @file     problem_Problem.cpp
/// @author   James Bordner (jobordner@ucsd.edu)
/// @date     2012-03-03
/// @brief    Implementation of the Problem container class

#include "problem.hpp"

//----------------------------------------------------------------------

Problem::Problem() throw()
  : stopping_(0),
    index_output_(0)
{
  
}

//----------------------------------------------------------------------

Problem::~Problem() throw()
{
  deallocate_();
}

//----------------------------------------------------------------------

void Problem::pup (PUP::er &p)
{
  // NOTE: change this function whenever attributes change

  TRACEPUP;

  PUP::able::pup(p);

  bool pk = p.isPacking();
  bool up = p.isUnpacking();

  int n;

  if (pk) n=boundary_list_.size();
  p | n;
  if (up) boundary_list_.resize(n);
  for (int i=0; i<n; i++) {
    p | boundary_list_[i]; // PUP::able
  }

  if (pk) n=initial_list_.size();
  p | n;
  if (up) initial_list_.resize(n);
  for (int i=0; i<n; i++) {
    p | initial_list_[i]; // PUP::able
  }

  if (pk) n=refine_list_.size();
  p | n;
  if (up) refine_list_.resize(n);
  for (int i=0; i<n; i++) {
    p | refine_list_[i]; // PUP::able
  }

  if (up) stopping_ = new Stopping;
  p | *stopping_;

  // if (pk) n=timestep_list_.size();
  // p | n;
  // if (up) timestep_list_.resize(n);
  // for (int i=0; i<n; i++) {
  //   p | timestep_list_[i]; // PUP::able
  // }

  if (pk) n=method_list_.size();
  p | n;
  if (up) method_list_.resize(n);
  for (int i=0; i<n; i++) {
    p | method_list_[i]; // PUP::able
  }

  if (pk) n=output_list_.size();
  p | n;
  if (up) output_list_.resize(n);
  for (int i=0; i<n; i++) {
    p | output_list_[i]; // PUP::able
  }

  p | prolong_; // PUP::able
  p | restrict_; // PUP::able

  p | index_refine_;
  p | index_output_;
  p | index_boundary_;
}

//----------------------------------------------------------------------

void Problem::initialize_boundary(Config * config, 
				  Parameters * parameters) throw()
{
  for (int index=0; index < config->num_boundary; index++) {

    std::string type = config->boundary_type[index];

    Boundary * boundary = create_boundary_(type,index,config,parameters);

    ASSERT1("Problem::initialize_boundary",
	  "Boundary type %s not recognized",
	  type.c_str(),  boundary != NULL);

    boundary_list_.push_back(boundary);
  }

}

//----------------------------------------------------------------------

void Problem::initialize_initial(Config * config,
				 Parameters * parameters,
				 const FieldDescr * field_descr,
				 const GroupProcess * group_process) throw()
{

  Initial * initial = create_initial_
    (config->initial_type,config,parameters,field_descr,group_process);

  initial_list_.push_back( initial );

  ASSERT1("Problem::initialize_initial",
	  "Initial type %s not recognized",
	  config->initial_type.c_str(),
	  initial != NULL);

}

//----------------------------------------------------------------------

void Problem::initialize_refine(Config * config,
				Parameters * parameters,
				const FieldDescr * field_descr) throw()
{
  for (int i=0; i<config->num_mesh; i++) {

    std::string name = config->mesh_type[i];

    Refine * refine = create_refine_ 
      (name,config,parameters,field_descr,i);

    if (refine) {
      refine_list_.push_back( refine );
    } else {
      ERROR1("Problem::initialize_refine",
	     "Cannot create Mesh type %s",name.c_str());
    }
  }
}

//----------------------------------------------------------------------

void Problem::initialize_stopping(Config * config) throw()
{
  stopping_ = create_stopping_("default",config);

  ASSERT("Problem::initialize_stopping",
	  "Stopping object not successfully created",
	  stopping_ != NULL);
}

//----------------------------------------------------------------------

void Problem::initialize_prolong(Config * config) throw()
{
  prolong_ = create_prolong_(config->prolong_type,config);

  ASSERT1("Problem::initialize_prolong",
	  "Prolong type %s not recognized",
	  config->prolong_type.c_str(),
	  prolong_ != NULL);
}

//----------------------------------------------------------------------

void Problem::initialize_restrict(Config * config) throw()
{
  restrict_ = create_restrict_(config->restrict_type,config);

  ASSERT1("Problem::initialize_restrict",
	  "Restrict type %s not recognized",
	  config->restrict_type.c_str(),
	  restrict_ != NULL);
}

//----------------------------------------------------------------------

void Problem::initialize_output
(Config * config,
 const FieldDescr * field_descr,
 const GroupProcess * group_process,
 const Factory * factory) throw()
{

  for (int index=0; index < config->num_output; index++) {

    std::string type       = config->output_type[index];

    Output * output = create_output_ 
      (type,index, config,field_descr,group_process,factory);

    if (output == NULL) {
      ERROR2("Problem::initialize_output",
	     "Unknown parameter type Output:%s:type = %s",
	     config->output_list[index].c_str(),type.c_str());
    }

    if (config->output_name[index].size() > 0) {
      std::string file_name = config->output_name[index][0];

      std::vector<std::string> file_args;

      for (size_t i=1; i<config->output_name[index].size(); i++) {
	file_args.push_back(config->output_name[index][i]);
      }

      output->set_filename (file_name,file_args);
    }


    if (config->output_field_list[index].size() > 0) {

	ItFieldList * it_field = new ItFieldList;
	int length = config->output_field_list[index].size();
	for (int i=0; i<length; i++) {
	  std::string field_name = config->output_field_list[index][i];
	  int field_index = field_descr->field_id(field_name);
	  it_field->append(field_index);
	}
	
	output->set_it_field(it_field);

    } else {

      int field_count = field_descr->field_count();
      ItFieldRange * it_field = new ItFieldRange(field_count);

      output->set_it_field(it_field);

    }


    //--------------------------------------------------
    // Scheduling parameters
    //--------------------------------------------------


    output->set_schedule
      (Schedule::create( config->output_schedule_var[index],
			config->output_schedule_type[index],
			config->output_schedule_start[index],
			config->output_schedule_stop[index],
			config->output_schedule_step[index],
			 config->output_schedule_list[index]));


    //--------------------------------------------------
    // Image parameters
    //--------------------------------------------------

    OutputImage * output_image = dynamic_cast<OutputImage *> (output);

    if (output != NULL) {

      // AXIS

      std::string axis = config->output_image_axis[index];

      if (axis == "x") output_image->set_axis(axis_x);
      if (axis == "y") output_image->set_axis(axis_y);
      if (axis == "z") output_image->set_axis(axis_z);

      // COLORMAP

      int n = config->output_image_colormap[index].size() / 3;

      if (n > 0) {
	double * r = new double [n];
	double * g = new double [n];
	double * b = new double [n];

	for (int i=0; i<n; i++) {

	  int ir=3*i+0;
	  int ig=3*i+1;
	  int ib=3*i+2;

	  r[i] = config->output_image_colormap[index][ir];
	  g[i] = config->output_image_colormap[index][ig];
	  b[i] = config->output_image_colormap[index][ib];

	}

	output_image->set_colormap(n,r,g,b);

	delete r;
	delete g;
	delete b;

      }

    }

    // Add the initialized Output object to the Simulation's list of
    // output objects

    output_list_.push_back(output); 

  } // (for index)

}

//----------------------------------------------------------------------

void Problem::initialize_method(Config * config) throw()
{
  for (size_t i=0; i<config->method_list.size(); i++) {

    std::string name = config->method_list[i];

    Method * method = create_method_(name, config);

    if (method) {

      method_list_.push_back(method); 

    } else {
      ERROR1("Problem::initialize_method",
	     "Unknown Method %s",name.c_str());
    }
  }
}

//======================================================================

void Problem::deallocate_() throw()
{
  for (size_t i=0; i<boundary_list_.size(); i++) {
    delete boundary_list_[i];    boundary_list_[i] = 0;
  }

  for (size_t i=0; i<initial_list_.size(); i++) {
    delete initial_list_[i];    initial_list_[i] = 0;
  }
  for (size_t i=0; i<refine_list_.size(); i++) {
    delete refine_list_[i];    refine_list_[i] = 0;
  }
  delete stopping_;      stopping_ = 0;
  // for (size_t i=0; i<timestep_list_.size(); i++) {
  //   delete timestep_list_[i];     timestep_list_[i] = 0;
  // }
  for (size_t i=0; i<output_list_.size(); i++) {
    delete output_list_[i];    output_list_[i] = 0;
  }
  for (size_t i=0; i<method_list_.size(); i++) {
    delete method_list_[i];    method_list_[i] = 0;
  }
}

//----------------------------------------------------------------------

Boundary * Problem::create_boundary_
(
 std::string type,
 int index,
 Config * config,
 Parameters * parameters
 ) throw ()
{
  if (type == "inflow") {
    std::string param_str = 
      "Boundary:" + config->boundary_list[index] + ":value";
    int param_type = parameters->type(param_str);
    if (! (param_type == parameter_list ||
	   param_type == parameter_float ||
	   param_type == parameter_float_expr)) {
      ERROR2("Problem::create_boundary_()",
	     "Parameter %s is of incorrect type %d",
	     param_str.c_str(),param_type);
    }
    Value * value = new Value (parameters, param_str);
    axis_enum axis = (axis_enum) config->boundary_axis[index];
    face_enum face = (face_enum) config->boundary_face[index];

    return new BoundaryValue (axis,face,value,
			      config->boundary_field_list[index]);
  }
  return NULL;
}

//----------------------------------------------------------------------

Initial * Problem::create_initial_
(
 std::string  type,
 Config * config,
 Parameters * parameters,
 const FieldDescr * field_descr,
 const GroupProcess * group_process
 ) throw ()
{ 
  //--------------------------------------------------
  // parameter: Initial : cycle
  // parameter: Initial : time
  //--------------------------------------------------

  if (type == "file" || type == "checkpoint") {
    return new InitialFile   (parameters,group_process,
			      config->initial_cycle,
			      config->initial_time);;
  } else if (type == "value") {
    return new InitialValue(parameters,field_descr,
			    config->initial_cycle,
			    config->initial_time);
  }
  return NULL;
}

//----------------------------------------------------------------------

Refine * Problem::create_refine_
(
 std::string        type,
 Config *           config,
 Parameters *       parameters,
 const FieldDescr * field_descr,
 int                index
 ) throw ()
{ 

  printf ("Problem::create_refine_ %s\n",type.c_str());
  if (type == "slope") {

    return new RefineSlope 
      (field_descr,
       config->mesh_min_refine[index],
       config->mesh_max_coarsen[index],
       config->mesh_field_list[index]);

  } else if (type == "mask") {

    std::string param_str = "Mesh:" + config->mesh_list[index] + ":value";

    return new RefineMask (parameters, param_str);

  } else if (type == "mass") {

    double root_cell_volume = 1.0;
    for (int i=0; i<config->mesh_root_rank; i++) {
      double upper = config->domain_upper[i] ;
      double lower = config->domain_lower[i];
      int     root = config->mesh_root_size[i];

      root_cell_volume *= (upper - lower) / (root);

    }

    return new RefineMass (config->mesh_min_refine[index],
			   config->mesh_max_coarsen[index],
			   config->mesh_level_exponent[index],
			   root_cell_volume);
  }
  return NULL;
}

//----------------------------------------------------------------------

Stopping * Problem::create_stopping_ 
(
 std::string  type,
 Config * config
 ) throw ()
/// @param type   Type of the stopping method to create (ignored)
/// @param stop_cycle  Stopping cycle
/// @param stop_time  Stopping time
{
  return new Stopping(config->stopping_cycle,
		      config->stopping_time);
}

//----------------------------------------------------------------------

// Timestep * Problem::create_timestep_ 
// (
//  std::string  type,
//  Config * config
//  ) throw ()
// { 
//   // No default timestep
//   return NULL;
// }

//----------------------------------------------------------------------

Method * Problem::create_method_ 
( std::string  name, Config * config) throw ()
{
  TRACE1("Problem::create_method %s",name.c_str());
  // No default method
  return NULL;
}


//----------------------------------------------------------------------

Output * Problem::create_output_ 
(
 std::string    name,
 int index,
 Config *  config,
 const FieldDescr * field_descr,
 const GroupProcess * group_process,
 const Factory * factory
 ) throw ()
/// @param name          Name of Output object to create
/// @param group_process Image output needs group process size
{ 

  Output * output = NULL;

  if (name == "image") {

    //--------------------------------------------------
    // parameter: Mesh : root_size
    // parameter: Mesh : root_blocks
    //--------------------------------------------------

    int nx = config->mesh_root_size[0];
    int ny = config->mesh_root_size[1];
    int nz = config->mesh_root_size[2];

    int nbx = config->mesh_root_blocks[0];
    int nby = config->mesh_root_blocks[1];
    int nbz = config->mesh_root_blocks[2];

    // NOTE: assumes cube for non-z axis images

    std::string image_type       = config->output_image_type[index];
    int         image_size_x     = config->output_image_size[index][0];
    int         image_size_y     = config->output_image_size[index][1];
    int         image_block_size = config->output_image_block_size[index];
    bool        image_ghost      = config->output_image_ghost[index];
    bool        image_log        = config->output_image_log[index];
    int         image_face_rank  = config->output_image_face_rank[index];
    int         max_level        = config->mesh_max_level;
    std::string image_reduce_type = config->output_image_reduce_type[index];
    std::string image_mesh_color  = config->output_image_mesh_color[index];
    bool        image_specify_bounds =
      config->output_image_specify_bounds[index];
    double      image_min = config->output_image_min[index];
    double      image_max = config->output_image_max[index];

    output = new OutputImage (field_descr,
			      index,factory,group_process->size(),
			      nx,ny,nz, 
			      nbx,nby,nbz, 
			      max_level,
			      image_type,
			      image_size_x,image_size_y,
			      image_reduce_type,
			      image_mesh_color,
			      image_block_size,
			      image_face_rank,
			      image_log,
			      image_ghost,
			      image_specify_bounds,
			      image_min, image_max);

  } else if (name == "data") {

    output = new OutputData (index,factory,config);

  } else if (name == "checkpoint") {

    output = new OutputCheckpoint (index,factory,config,group_process->size());

  }

  return output;

}

//----------------------------------------------------------------------

Prolong * Problem::create_prolong_ ( std::string  name ,
				     Config * config) throw ()
{
  Prolong * prolong = 0;

  if (name == "linear") {

    prolong = new ProlongLinear;

  } else {
    
    ERROR1("Problem::create_prolong_",
	  "Unrecognized Field:prolong parameter %s",name.c_str());

  }

  return prolong;
  
}

//----------------------------------------------------------------------

Restrict * Problem::create_restrict_ ( std::string  name ,
				     Config * config) throw ()
{
  Restrict * restrict = 0;

  if (name == "linear") {

    restrict = new RestrictLinear;

  } else {
    
    ERROR1("Problem::create_restrict_",
	  "Unrecognized Field:restrict parameter %s",name.c_str());

  }

  return restrict;
  
}

//----------------------------------------------------------------------
