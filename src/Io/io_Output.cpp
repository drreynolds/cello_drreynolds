// See LICENSE_CELLO file for license and copyright information

/// @file     io_Output.cpp
/// @author   James Bordner (jobordner@ucsd.edu)
/// @bug      time interval scheduling gets confused if multiple outputs scheduled per timestep
/// @date     Wed Mar 16 09:53:31 PDT 2011
/// @brief    Implementation of the Output class

#include "cello.hpp"

#include "io.hpp"

//----------------------------------------------------------------------

Output::Output (Simulation * simulation) throw()
  : simulation_(simulation),
    file_(0),           // Initialization deferred
    schedule_(new Schedule),
    process_stride_(1), // default one file per process
    process_(0),        // initialization below
#ifdef CONFIG_USE_CHARM
    count_reduce_(0),
#endif
    cycle_(0),
    count_output_(0),
    time_(0),
    file_name_(""),     // set_filename()
    file_args_(),       // set_filename()
    it_field_(0)        // set_it_field()
{

  GroupProcess * group_process = GroupProcess::create();
  process_  = group_process->rank();
  delete group_process;
}

//----------------------------------------------------------------------

Output::~Output () throw()
{
  delete schedule_;
  schedule_ = 0;
  delete file_;
  file_ = 0;
  delete it_field_;
  it_field_ = 0;
}
//----------------------------------------------------------------------

void Output::set_filename (std::string filename,
			   std::vector<std::string> fileargs) throw()
{
  file_name_ = filename; 
  file_args_ = fileargs;
}

//----------------------------------------------------------------------

std::string Output::expand_file_name () const throw()
{
  char buffer_curr[CELLO_STRING_LENGTH];
  char buffer_next[CELLO_STRING_LENGTH];

  // Error check no \% in file name

  ASSERT1 ("Output::expand_file_name",
	   "File name %s cannot contain '\\%%'",
	   file_name_.c_str(),
	   file_name_.find("\\%") == std::string::npos);

  // Error check variable count equals format conversion specifier count

  std::string file_rest = file_name_;
  size_t count = 0;
  size_t pos = 0;
  size_t len;
  while ((pos = file_rest.find("%")) != std::string::npos) {
    count ++;
    len = file_rest.size();
    file_rest = file_rest.substr(pos+1,len-pos-1);
  }

  ASSERT3 ("Output::expand_file_name",
	   "The number of format conversion specifiers %d "
	   "associated with file name %s "
	   "must equal the number of variables %d",
	    count, file_name_.c_str(),file_args_.size(),
	   file_args_.size() == count);

  // loop through file_args_[] from the right and replace 
  // format strings with variable values

  std::string file_left  = file_name_;
  std::string file_middle = "";
  std::string file_right = "";

  for (size_t i=0; i<file_args_.size(); i++) {

    // visit variables from right to left
    const std::string & file_arg = file_args_[file_args_.size() - i - 1];

    size_t pos = file_left.rfind("%");
    size_t len = file_left.size();

    file_middle = file_left.substr(pos,len-pos);
    file_left  = file_left.substr(0,pos);

    strcpy (buffer_curr, file_middle.c_str());
    
    if (file_arg == "cycle") {
      sprintf (buffer_next,buffer_curr, cycle_);
    } else if (file_arg == "time") {
      sprintf (buffer_next,buffer_curr, time_);
    } else if (file_arg == "count") {
      sprintf (buffer_next,buffer_curr, count_output_);
    } else if (file_arg == "proc") {
      sprintf (buffer_next,buffer_curr, process_);
    } else {
      char buffer[CELLO_STRING_LENGTH];
      sprintf (buffer,"Unknown file variable #%d '%s' for file '%s'",
	       int(i),file_arg.c_str(),file_name_.c_str());
      ERROR("Output::expand_file_name",buffer);
    }

    file_right = std::string(buffer_next) + file_right;

  }

  return file_left + file_right;
}

//----------------------------------------------------------------------

bool Output::is_scheduled (int cycle, double time)
{
  cycle_ = cycle;
  time_  = time;
  return schedule()->write_this_cycle(cycle_,time_);
}

//----------------------------------------------------------------------

double Output::update_timestep (double time, double dt) const
{
  return schedule_->update_timestep(time,dt); 
}

//----------------------------------------------------------------------
