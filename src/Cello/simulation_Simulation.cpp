// See LICENSE_CELLO file for license and copyright information

/// @file      simulation_Simulation.cpp
/// @author    James Bordner (jobordner@ucsd.edu)
/// @date      2010-11-10
/// @brief     Implementation of the Simulation class

#include "cello.hpp"

#include "main.hpp"

#include "simulation.hpp"
#include "simulation_charm.hpp"

#ifdef CONFIG_USE_CHARM
Simulation::Simulation
(
 const char *   parameter_file,
 int n,
 CProxy_BlockReduce proxy_block_reduce,
 GroupProcess * group_process
 ) throw()
#else 
Simulation::Simulation
(
 const char *   parameter_file,
 GroupProcess * group_process
 ) throw()
#endif

/// Initialize the Simulation object
: factory_(0),
  parameters_(0),
  parameter_file_(parameter_file),
  group_process_(group_process),
  is_group_process_new_(false),
#ifdef CONFIG_USE_CHARM
  proxy_block_reduce_(proxy_block_reduce),
  index_output_(0),
#endif
  dimension_(0),
  cycle_(0),
  time_(0.0),
  dt_(0),
  stop_(false),
  performance_simulation_(0),
  performance_cycle_(0),
  monitor_(0),
  hierarchy_(0),
  field_descr_(0)
{

  if (!group_process_) {
    group_process_ = GroupProcess::create();
    is_group_process_new_ = true;
  }

  performance_simulation_ = new Performance;
  performance_cycle_      = new Performance;

  monitor_ = Monitor::instance();
  monitor_->set_process_rank(group_process_->rank());
  monitor_->set_active(group_process_->is_root());

  parameters_ = new Parameters(parameter_file,monitor_);
}

//----------------------------------------------------------------------

Simulation::~Simulation() throw()
{
  deallocate_();
}

//----------------------------------------------------------------------

void Simulation::initialize() throw()
{
  performance_simulation_->start();
  performance_cycle_->start();

  // Initialize parameters

  initialize_simulation_();

  // INITIALIZE SIMULATION COMPONENTS
  // (warning: initialization may be order dependent)

  // Initialize data
  initialize_data_descr_();

  // Initialize Mesh hierarchy
  initialize_hierarchy_();

  // Initialize boundary conditions
  problem_->initialize_boundary(parameters_);

  // Initialize initial conditions
  problem_->initialize_initial(parameters_,group_process_);

  // initialize stopping criteria
  problem_->initialize_stopping(parameters_);

  // initialize timestep approach
  problem_->initialize_timestep(parameters_);

  // Initialize list of output objects
  problem_->initialize_output
    (parameters_,field_descr_,group_process_,hierarchy_,factory());

  // Initialize list of methods
  problem_->initialize_method(parameters_);

  initialize_parallel_();

}

//----------------------------------------------------------------------

void Simulation::finalize() throw()
{
  performance_simulation_->stop();
  performance_cycle_->stop();
  deallocate_();
}

//======================================================================

void Simulation::initialize_simulation_() throw()
{

  //--------------------------------------------------
  // parameter: Mesh    : root_rank
  // parameter: Initial : cycle
  // parameter: Initial : time
  //--------------------------------------------------

  dimension_ = parameters_->value_integer("Mesh:root_rank",0);
  
  ASSERT ("Simulation::initialize_simulation_()", 
	  "Parameter 'Mesh:root_rank' must be specified",
	  dimension_ != 0);
  
  ASSERT ("Simulation::initialize_simulation_()", 
	  "Parameter 'Mesh:root_rank' must be 1, 2, or 3",
	  (1 <= dimension_) && (dimension_ <= 3));

  cycle_ = parameters_->value_integer("Initial:cycle",0);
  time_  = parameters_->value_float  ("Initial:time",0);

}

//----------------------------------------------------------------------

void Simulation::initialize_data_descr_() throw()
{

  field_descr_ = new FieldDescr;

  //--------------------------------------------------
  // parameter: Field : fields
  //--------------------------------------------------

  // Add data fields

  int i;
  for (i=0; i<parameters_->list_length("Field:fields"); i++) {
    field_descr_->insert_field
      (parameters_->list_value_string(i, "Field:fields"));
  }

  // Define default ghost zone depth for all fields, default value of 1

  //--------------------------------------------------
  // parameter: Field : ghosts
  //--------------------------------------------------

  int gx = 0;
  int gy = 0;
  int gz = 0;

  if (parameters_->type("Field:ghosts") == parameter_integer) {
    gx = gy = gz = parameters_->value_integer("Field:ghosts",0);
    if (dimension_ < 2) gy = 0;
    if (dimension_ < 3) gz = 0;
  } else if (parameters_->type("Field:ghosts") == parameter_list) {
    gx = parameters_->list_value_integer(0,"Field:ghosts",0);
    gy = parameters_->list_value_integer(1,"Field:ghosts",0);
    gz = parameters_->list_value_integer(2,"Field:ghosts",0);
  }

  for (i=0; i<field_descr_->field_count(); i++) {
    field_descr_->set_ghosts(i,gx,gy,gz);
  }

  // Set face dimensions to refresh

  //--------------------------------------------------
  // parameter: Field : refresh_faces
  // parameter: Field : refresh_edges
  // parameter: Field : refresh_corners
  //--------------------------------------------------

  // Refresh face ghost zones
  if (parameters_->type("Field:refresh_faces") == parameter_logical) {
    bool refresh_faces = 
      parameters_->value_logical ("Field:refresh:faces",true);
    field_descr_->set_refresh_face(2,refresh_faces);
  }

  // Refresh edge ghost zones
  if (parameters_->type("Field:refresh_edges") == parameter_logical) {
    bool refresh_edges = 
      parameters_->value_logical ("Field:refresh:edges",false);
    field_descr_->set_refresh_face(1,refresh_edges);
  }

  // Refresh corner ghost zones
  if (parameters_->type("Field:refresh_corners") == parameter_logical) {
    bool refresh_corners = 
      parameters_->value_logical ("Field:refresh:corners",false);
    field_descr_->set_refresh_face(0,refresh_corners);
  }
  
  //--------------------------------------------------
  // parameter: Field : precision
  //--------------------------------------------------

  std::string precision_str = 
    parameters_->value_string("Field:precision","default");

  precision_enum precision = precision_unknown;

  if (precision_str == "default")
    precision = precision_default;
  else if (precision_str == "single")
    precision = precision_single;
  else if (precision_str == "double")
    precision = precision_double;
  else if (precision_str == "quadruple")
    precision = precision_quadruple;
  else {
    ERROR1 ("Simulation::initialize_data_descr_()", 
	    "Unknown precision %s",
	    precision_str.c_str());
  }

  for (i=0; i<field_descr_->field_count(); i++) {
    field_descr_->set_precision(i,precision);
  }

  //--------------------------------------------------
  // parameter: Field : alignment
  //--------------------------------------------------

  int alignment = parameters_->value_integer("Field:alignment",8);

  field_descr_->set_alignment (alignment);
  
  //--------------------------------------------------
  // parameter: Field : padding
  //--------------------------------------------------

  int padding = parameters_->value_integer("Field:padding",0);

  field_descr_->set_padding (padding);

  //--------------------------------------------------
  // parameter: Field : <field_name> : centering
  //--------------------------------------------------

  for (int i=0; i<field_descr_->field_count(); i++) {

    std::string field_name = field_descr_->field_name(i);

    std::string param_name = 
      std::string("Field:") + field_name + ":centering";
    
    if (parameters_->type(param_name) != parameter_unknown) {

      bool valid = parameters_->type(param_name) == parameter_list;
      valid = valid && parameters_->list_length(param_name) == dimension_;
      for (int i=0; i<dimension_; i++) {
	valid = valid && 
	  (parameters_->list_type(i,param_name) == parameter_logical);
      }
      
      ASSERT2 ("Simulation::initialize_data_descr_()",
	       "Parameter '%s' must be a list of logical values with length %d",
	       param_name.c_str(), dimension_, valid);

      int id_field = field_descr_->field_id(field_name);

      bool cx,cy,cz;

      cx = (dimension_ >= 1) ? 
	parameters_->list_value_logical(0,param_name,true) : true;
      cy = (dimension_ >= 2) ? 
	parameters_->list_value_logical(1,param_name,true) : true;
      cz = (dimension_ >= 3) ? 
	parameters_->list_value_logical(2,param_name,true) : true;

      field_descr_->set_centering (id_field, cx,cy,cz);

    }
  }
}
//----------------------------------------------------------------------

void Simulation::initialize_hierarchy_() throw()
{

  ASSERT("Simulation::initialize_hierarchy_",
	 "data must be initialized before hierarchy",
	 field_descr_ != NULL);

  //----------------------------------------------------------------------
  // Create and initialize Hierarchy
  //----------------------------------------------------------------------

  hierarchy_ = factory()->create_hierarchy();

  // Domain extents

  //--------------------------------------------------
  // parameter: Domain : lower
  // parameter: Domain : upper
  //--------------------------------------------------

  ASSERT ("Simulation::initialize_simulation_",
	  "Parameter Domain:lower list length must match Physics::dimension",
	  (parameters_->list_length("Domain:lower") == dimension_));

  ASSERT ("Simulation::initialize_simulation_",
	  "Parameter Domain:upper list length must match Physics::dimension",
	  (parameters_->list_length("Domain:upper") ==  dimension_));

  double lower[3];
  double upper[3];

  for (int i=0; i<3; i++) {
    lower[i] = parameters_->list_value_float(i, "Domain:lower", 0.0);
    upper[i] = parameters_->list_value_float(i, "Domain:upper", 0.0);
    ASSERT ("Simulation::initialize_simulation_",
	    "Domain:lower may not be greater than Domain:upper",
	    lower[i] <= upper[i]);
  }

  hierarchy_->set_lower(lower[0], lower[1], lower[2]);
  hierarchy_->set_upper(upper[0], upper[1], upper[2]);

  //----------------------------------------------------------------------
  // Create and initialize root Patch in Hierarchy
  //----------------------------------------------------------------------

  //--------------------------------------------------
  // parameter: Mesh : root_size
  // parameter: Mesh : root_blocks
  //--------------------------------------------------

  int root_size[3];

  root_size[0] = parameters_->list_value_integer(0,"Mesh:root_size",1);
  root_size[1] = parameters_->list_value_integer(1,"Mesh:root_size",1);
  root_size[2] = parameters_->list_value_integer(2,"Mesh:root_size",1);

  int root_blocks[3];

  root_blocks[0] = parameters_->list_value_integer(0,"Mesh:root_blocks",1);
  root_blocks[1] = parameters_->list_value_integer(1,"Mesh:root_blocks",1);
  root_blocks[2] = parameters_->list_value_integer(2,"Mesh:root_blocks",1);

#ifndef CONFIG_USE_CHARM
  ASSERT4 ("Simulation::initialize_simulation_",
	   "Product of Mesh:root_blocks = [%d %d %d] must equal MPI_Comm_size = %d",
	   root_blocks[0],root_blocks[1],root_blocks[2], group_process_->size(),
	   root_blocks[0]*root_blocks[1]*root_blocks[2]==group_process_->size());
#endif

  std::string type = parameters_->value_string("Initial:type","default");

  // Don't allocate blocks if reading data from files
  bool allocate_blocks = ! ( type == "file" || type == "restart" );

  hierarchy_->create_root_patch
    (group_process_,dimension_,
     field_descr_,
     root_size[0],root_size[1],root_size[2],
     root_blocks[0],root_blocks[1],root_blocks[2],
     allocate_blocks);

}

//----------------------------------------------------------------------

void Simulation::initialize_parallel_() throw()
{
}
//----------------------------------------------------------------------

void Simulation::deallocate_() throw()
{
  delete factory_;       factory_     = 0;
  delete parameters_;    parameters_  = 0;
  delete performance_simulation_;   performance_simulation_ = 0;
  delete performance_cycle_;   performance_cycle_ = 0;
  if (is_group_process_new_)
    { delete group_process_; group_process_ = 0; }
  delete hierarchy_;     hierarchy_ = 0;
  delete field_descr_;   field_descr_ = 0;
}

//----------------------------------------------------------------------

const Factory * Simulation::factory() const throw()
{
  if (factory_ == NULL) factory_ = new Factory;
  return factory_;
}

//======================================================================


#ifdef CONFIG_USE_CHARM

Simulation::Simulation() 
{
}

#endif

//----------------------------------------------------------------------

#ifdef CONFIG_USE_CHARM

Simulation::Simulation (CkMigrateMessage *m) 
{
}

#endif

//----------------------------------------------------------------------

#ifdef CONFIG_USE_CHARM

void Simulation::refresh() throw()
{

  //--------------------------------------------------
  // Monitor
  //--------------------------------------------------

  monitor_output();

  //--------------------------------------------------
  // Stopping
  //--------------------------------------------------

  if (stop_) {

    performance_output(performance_simulation_,"simulation");

    proxy_main.p_exit(CkNumPes());

  } else {

    //--------------------------------------------------
    // Compute
    //--------------------------------------------------

    ItPatch it_patch(hierarchy_);
    Patch * patch;
    while (( patch = ++it_patch )) {
      if (patch->blocks_allocated()) {
	patch->block_array().p_compute(cycle_, time_, dt_);
      }
    }
  }

}

#endif

#ifndef CONFIG_USE_CHARM

//----------------------------------------------------------------------
// NOT CHARM
//----------------------------------------------------------------------

void Simulation::scheduled_output()

{
  size_t index_output = 0;
  while (Output * output = problem()->output(index_output++)) {

    if (output->is_scheduled(cycle_,time_)) {

      output->init();

      output->open();

      output->write_simulation(this);

      //--------------------------------------------------
      int ip       = group_process_->rank();
      int ip_writer = output->process_writer();

      int n=1;  char * buffer = 0;

      if (ip == ip_writer) { // process is writer
	int ip1 = ip+1;
	int ip2 = ip_writer+output->process_stride();
	for (int ip_remote=ip1; ip_remote<ip2; ip_remote++) {

	  // receive size

	  void * handle_recv;
	  handle_recv = group_process_->recv_begin(ip_remote,&n,sizeof(n));
	  group_process_->wait(handle_recv);
	  group_process_->send_end(handle_recv);

	  // allocate buffer

	  buffer = new char [n];

	  // receive buffer

	  handle_recv = group_process_->recv_begin(ip_remote,buffer,n);
	  group_process_->wait(handle_recv);
	  group_process_->recv_end(handle_recv);
	  
	  // update

	  output->update_remote(n,buffer);

	  // deallocate
	  output->cleanup_remote(&n,&buffer);
	}
      } else { // process is not writer

	// send data to writer

	output->prepare_remote(&n,&buffer);

	// send size

	void * handle_send;

	handle_send = group_process_->send_begin(ip_writer,&n,sizeof(n));
	group_process_->wait(handle_send);
	group_process_->send_end(handle_send);

	// send buffer

	handle_send = group_process_->send_begin(ip_writer,buffer,n);
	group_process_->wait(handle_send);
	group_process_->send_end(handle_send);

      }
      //--------------------------------------------------

      output->close();
      output->finalize();
    }
  }
}

#endif

//----------------------------------------------------------------------

void Simulation::update_cycle(int cycle, double time, double dt, double stop) 
{
  cycle_ = cycle;
  time_  = time;
  dt_    = dt;
  stop_  = stop;
}

//----------------------------------------------------------------------

void Simulation::monitor_output() const 
{

  monitor_->  print("", "-------------------------------------");

  monitor_-> print("Simulation", "cycle %04d", cycle_);
  monitor_-> print("Simulation", "time-sim %15.12f",time_);
  monitor_-> print("Simulation", "dt %15.12g", dt_);

#ifdef CONFIG_USE_MEMORY

  Memory * memory = Memory::instance();

  monitor_->print("Memory","bytes-curr %lld", memory->bytes());
  monitor_->print("Memory","bytes-high %lld", memory->bytes_high());

  memory->reset_high();

#endif

  performance_output(performance_cycle_);

}


//----------------------------------------------------------------------

void Simulation::performance_output(Performance * performance,
				    const char * region) const
{
  monitor_->print ("Performance","time-cycle-real %f",
		   performance_cycle_->time());
  monitor_->print ("Performance","time-accum-real %f",
		   performance_simulation_->time());

#ifdef CONFIG_USE_PAPI

  bool save_active = monitor_->is_active();
  monitor_->set_active(true);

  Papi * papi = performance->papi();

  papi->update();

  double time_real   = papi->time_real();
  double time_proc   = papi->time_proc();
  double gflop_count = papi->flop_count()*1e-9;
  double gflop_rate  = gflop_count / time_real;

  monitor_->print ("Performance","%s time-real-papi   %f", region, time_real);
  monitor_->print ("Performance","%s time-proc-papi   %f", region, time_proc);
  monitor_->print ("Performance","%s gflop-count-papi %f", region, gflop_count);
  monitor_->print ("Performance","%s gflop-rate-papi  %f", region, gflop_rate);

  monitor_->set_active(save_active);

#endif

}
//======================================================================
