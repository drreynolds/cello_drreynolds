// See LICENSE_CELLO file for license and copyright information

/// @file      simulation_Simulation.cpp
/// @author    James Bordner (jobordner@ucsd.edu)
/// @date      2010-11-10
/// @brief     Implementation of the Simulation class

#include "cello.hpp"

#include "main.hpp"

#include "simulation.hpp"
#include "charm_simulation.hpp"

Simulation::Simulation
(
 const char *   parameter_file,
 int            n,
 const GroupProcess * group_process
 ) throw()
/// Initialize the Simulation object
: factory_(0),
  parameters_(0),
  parameter_file_(parameter_file),
  group_process_((GroupProcess *)group_process),
  is_group_process_new_(false),
  dimension_(0),
  cycle_(0),
  time_(0.0),
  dt_(0),
  stop_(false),
  config_(0),
  problem_(0),
  timer_(),
  performance_(NULL),
  performance_name_(""),
  performance_stride_(1),
  monitor_(0),
  hierarchy_(0),
  field_descr_(0)
{

  if (!group_process_) {
    group_process_ = GroupProcess::create();
    is_group_process_new_ = true;
  }

  monitor_ = Monitor::instance();
  monitor_->set_process_rank(group_process_->rank());
  //  monitor_->set_active(group_process_->is_root());
#ifdef CELLO_DEBUG
  monitor_->set_active(true);
#else
  monitor_->set_active(group_process_->is_root());
#endif

  monitor_->print("test","testing");
  parameters_ = new Parameters(parameter_file,monitor_);

}

//----------------------------------------------------------------------

#ifdef CONFIG_USE_CHARM

Simulation::Simulation()
{ TRACE("Simulation()"); }

#endif

//----------------------------------------------------------------------

#ifdef CONFIG_USE_CHARM
void Simulation::pup (PUP::er &p)
{
  // NOTE: change this function whenever attributes change

  TRACEPUP;

  CBase_Simulation::pup(p);
  bool up = p.isUnpacking();

  p | factory_; // PUP::able

  // if (up) parameters_ = new Parameters;
  // p | * parameters_;

  WARNING("Simulation::pup","config_ needs to be PUP::able");
  p | config_;

  p | parameter_file_;

  if (up) group_process_ = GroupProcess::create();

  p | is_group_process_new_;
  p | dimension_; 
  p | cycle_;
  p | time_;
  p | dt_;
  p | stop_;

  if (up) problem_ = new Problem;
  p | * problem_;

  p | *performance_;

  p | performance_name_;
  p | performance_stride_;

  if (up) monitor_ = Monitor::instance();

  if (up) hierarchy_ = new Hierarchy;
  p | *hierarchy_;

  if (up) field_descr_ = new FieldDescr;
  p | *field_descr_;
}

#endif

//----------------------------------------------------------------------

#ifdef CONFIG_USE_CHARM

Simulation::Simulation (CkMigrateMessage *m)
  : CBase_Simulation(m)
{ TRACE("Simulation(CkMigrateMessage)"); }

#endif

//----------------------------------------------------------------------

Simulation::~Simulation() throw()
{
  deallocate_();
}

//----------------------------------------------------------------------

void Simulation::initialize() throw()
{
  TRACE("Simulation::initialize calling Simulation::initialize_config_()");
  initialize_config_();
  initialize_monitor_();
  initialize_performance_();
  initialize_simulation_();

  initialize_data_descr_();

  problem_->initialize_boundary(config_);
  problem_->initialize_initial (config_,parameters_,field_descr_,
				group_process_);
  problem_->initialize_refine  (config_,field_descr_);
  problem_->initialize_stopping(config_);
  problem_->initialize_timestep(config_);
  problem_->initialize_output  (config_,field_descr_,group_process_,factory());
  problem_->initialize_method  (config_);
  problem_->initialize_prolong (config_);
  problem_->initialize_restrict (config_);

  initialize_hierarchy_();

}

//----------------------------------------------------------------------

void Simulation::finalize() throw()
{
  DEBUG0;

  performance_->stop_region(perf_simulation);

  PARALLEL_PRINTF("Tracing Simulation::finalize %d\n",group_process_->rank());

  performance_->end();


}

//======================================================================

void Simulation::initialize_simulation_() throw()
{

  dimension_ = config_->mesh_root_rank;
  
  ASSERT ("Simulation::initialize_simulation_()", 
	  "Parameter 'Mesh:root_rank' must be specified",
	  dimension_ != 0);
  
  ASSERT ("Simulation::initialize_simulation_()", 
	  "Parameter 'Mesh:root_rank' must be 1, 2, or 3",
	  (1 <= dimension_) && (dimension_ <= 3));

  cycle_ = config_->initial_cycle;
  time_  = config_->initial_time;
  dt_ = 0;
}

//----------------------------------------------------------------------

void Simulation::initialize_performance_() throw()
{

  performance_ = new Performance (config_);

  performance_->new_region(perf_simulation, "simulation");
  performance_->new_region(perf_cycle,      "cycle");
  performance_->new_region(perf_initial,    "initial");
  performance_->new_region(perf_adapt,      "adapt");
  performance_->new_region(perf_refresh,    "refresh");
  performance_->new_region(perf_compute,    "compute");
  performance_->new_region(perf_output,     "output");

  performance_name_   = config_->performance_name;
  performance_stride_ = config_->performance_stride;

  timer_.start();

  for (size_t i=0; i<config_->performance_papi_counters.size(); i++) {
    performance_->new_counter(counter_type_papi, config_->performance_papi_counters[i]);
  }
  performance_->begin();

  performance_->start_region(perf_simulation);

}

//----------------------------------------------------------------------

void Simulation::initialize_config_() throw()
{
  TRACE("BEGIN Simulation::initialize_config_");
  if (config_ == NULL) {
    config_ = new Config;
    TRACE("Simulation::initialize_config_ calling Config::read()");
    config_->read(parameters_);
  }
  TRACE("END   Simulation::initialize_config_");
}

//----------------------------------------------------------------------

void Simulation::initialize_monitor_() throw()
{
  bool debug = config_->monitor_debug;
  monitor_->set_active("DEBUG",debug);
 
}

//----------------------------------------------------------------------

void Simulation::initialize_data_descr_() throw()
{

  field_descr_ = new FieldDescr;

  //--------------------------------------------------
  // parameter: Field : fields
  //--------------------------------------------------

  // Add data fields

  for (size_t i=0; i<config_->field_fields.size(); i++) {
    field_descr_->insert_field (config_->field_fields[i]);
  }

  // Define default ghost zone depth for all fields, default value of 1

  int gx = config_->field_ghosts[0];
  int gy = config_->field_ghosts[1];
  int gz = config_->field_ghosts[2];

  for (int i=0; i<field_descr_->field_count(); i++) {
    field_descr_->set_ghosts (i,gx,gy,gz);
  }

  // Default precision

  for (int i=0; i<field_descr_->field_count(); i++) {
    field_descr_->set_precision(i,config_->field_precision);
  }

  //--------------------------------------------------
  // parameter: Field : alignment
  //--------------------------------------------------

  int alignment = config_->field_alignment;

  ASSERT1 ("Simulation::initialize_data_descr_",
	  "Illegal Field:alignment parameter value %d",
	   alignment,
	   1 <= alignment );
	  
  field_descr_->set_alignment (alignment);
  
  field_descr_->set_padding (config_->field_padding);


  for (int i=0; i<field_descr_->field_count(); i++) {

    std::string field_name = field_descr_->field_name(i);

    bool cx = config_->field_centering[0][i];
    bool cy = config_->field_centering[1][i];
    bool cz = config_->field_centering[2][i];

    field_descr_->set_centering(i,cx,cy,cz);

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

  const int refinement = 2;
  hierarchy_ = factory()->create_hierarchy 
    (
#ifndef CONFIG_USE_CHARM
     this,
#endif
     dimension_,refinement, 0, group_process_->size());

  // Domain extents

  hierarchy_->set_lower
    (config_->domain_lower[0], 
     config_->domain_lower[1], 
     config_->domain_lower[2]);
  hierarchy_->set_upper
    (config_->domain_upper[0], 
     config_->domain_upper[1], 
     config_->domain_upper[2]);

  //----------------------------------------------------------------------
  // Create and initialize root Patch in Hierarchy
  //----------------------------------------------------------------------

  //--------------------------------------------------
  // parameter: Mesh : root_size
  // parameter: Mesh : root_blocks
  //--------------------------------------------------

  hierarchy_->set_root_size(config_->mesh_root_size[0],
			    config_->mesh_root_size[1],
			    config_->mesh_root_size[2]);

  // Don't allocate blocks if reading data from files


#ifdef CONFIG_USE_CHARM
  bool allocate_blocks = (group_process()->is_root());
#else
  bool allocate_blocks = true;
#endif

  bool allocate_data = ! ( config_->initial_type == "file" || 
			   config_->initial_type == "restart" );

  hierarchy_->create_forest
    (field_descr_,
     config_->mesh_root_size[0],
     config_->mesh_root_size[1],
     config_->mesh_root_size[2],
     config_->mesh_root_blocks[0],
     config_->mesh_root_blocks[1],
     config_->mesh_root_blocks[2],
     allocate_blocks,
     allocate_data);
}

//----------------------------------------------------------------------

void Simulation::deallocate_() throw()
{
  delete factory_;       factory_     = 0;
  delete parameters_;    parameters_  = 0;
  if (is_group_process_new_)
    { delete group_process_; group_process_ = 0; }
  delete hierarchy_;     hierarchy_ = 0;
  delete field_descr_;   field_descr_ = 0;
  delete performance_;   performance_ = 0;
}

//----------------------------------------------------------------------

const Factory * Simulation::factory() const throw()
{
  DEBUG("Simulation::factory()");
  if (factory_ == NULL) factory_ = new Factory;
  return factory_;
}

//======================================================================

void Simulation::update_state(int cycle, double time, double dt, double stop) 
{
  DEBUG4 ("Simulation::update_state cycle %d time %f dt %f stop %f",
	  cycle,time,dt,stop);
 
  cycle_ = cycle;
  time_  = time;
  dt_    = dt;
  stop_  = stop;
}

//----------------------------------------------------------------------

void Simulation::monitor_output()
{
  TRACE("Simulation::monitor_output()");

  monitor_-> print("", "-------------------------------------");
  monitor_-> print("Simulation", "cycle %04d", cycle_);
  monitor_-> print("Simulation", "time-sim %15.12f",time_);
  monitor_-> print("Simulation", "dt %15.12g", dt_);

  performance_output ();

  Memory * memory = Memory::instance();
  memory->reset_high();


}


//----------------------------------------------------------------------

void Simulation::performance_output()
{
  TRACE("Simulation::performance_output()");

  int num_regions  = performance_->num_regions();
  int num_counters =  performance_->num_counters();
  long long * counters = new long long [num_counters];

  for (int ir = 0; ir < num_regions; ir++) {

    performance_->region_counters(ir,counters);

    for (int ic = 0; ic < num_counters; ic++) {
    
      int perf_counter = performance_->index_to_id(ic);

      monitor_->print("Performance","%s %s %lld",
	      performance_->region_name(ir).c_str(),
	      performance_->counter_name(perf_counter).c_str(),
	      counters[ic]);  
    }
  }

  delete [] counters;

}


//----------------------------------------------------------------------

void Simulation::performance_write()
{
  TRACE("Simulation::performance_write()");

  PARALLEL_PRINTF("Tracing Simulation::performance_write %d\n",group_process_->rank());

  if (performance_name_ != "" && (group_process_->rank() % performance_stride_) == 0) {

    char filename[30];
    sprintf (filename,performance_name_.c_str(),group_process_->rank());
    FILE * fp = fopen(filename,"w");

    int num_regions  = performance_->num_regions();
    int num_counters =  performance_->num_counters();
    long long * counters = new long long [num_counters];

    for (int ir = 0; ir < num_regions; ir++) {

      performance_->region_counters(ir,counters);

      for (int ic = 0; ic < num_counters; ic++) {
    
	int perf_counter = performance_->index_to_id(ic);

	fprintf (fp,"%s %s %lld\n",
		 performance_->region_name(ir).c_str(),
		 performance_->counter_name(perf_counter).c_str(),
		 counters[ic]);  
      }
    }

    delete [] counters;
    fclose(fp);
    
  }

}

//======================================================================
