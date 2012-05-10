// See LICENSE_CELLO file for license and copyright information

/// @file     lcaperf_Lcaperf.hpp
/// @author   James Bordner (jobordner@ucsd.edu)
/// @date     yyyy-mm-dd
/// @brief    [\ref Lcaperf] Declaration of the Lcaperf class
///

#ifndef LCAPERF_LCAPERF_HPP
#define LCAPERF_LCAPERF_HPP

//----------------------------------------------------------------------

namespace lca {

class LcaPerf {

 public: // functions

  /// LcaPerf constructor
  LcaPerf();

  /// LcaPerf destructor
  ~LcaPerf();

  /// Add a region name: used for printing
  void new_region (const char * region_name);

  /// Create a new attribute of the given type
  void new_attribute (const char * attribute_name,
		      int          attribute_type);

  /// Return the value of the attribute of the given type
  void attribute (const char        * attribute_name,
		  const void        * attribute_value,
		  int                 attribute_type);

  /// Delete the named attribute
  void delete_attribute (const char * attribute_name);

  /// Create a new counter of the given type
  void new_counter  (const char *      counter_name,
		     counter_type counter_type);

  /// Increment the given user counter
  void increment (const char * counter_name,
		  long long    value);

  /// Assign the value to the user counter
  void assign    (const char * counter_name,
		  long long    value);

  /// Delete the named counter of the given type
  void delete_counter (const char * counter_name);

  /// Initialize lcaperf
  void initialize (const char * filename = 0);

  /// Finalize lcaperf
  void finalize ();

  /// Begin collecting performance data in files with the give suffix
  void begin ();

  /// Stop collecting performance data in files with the given suffix
  void end   ();

  /// Return the counter value for the currently assigned attribute
  long long value (const char * set,
		   const char * key,
		   const char * counter);

  /// Identify the beginning of a code region
  void start     (const char * region_name);

  /// Identify the end of a code region
  void stop      (const char * region_name);

  //--------------------------------------------------

  /// Print the header for subsequent print()'s
  void header ();

  /// Print all keys and associated counter values
  void print ();

  //----------------------------------------------------------------------

 protected: // attributes

  /// Attributes object
  Attributes     attributes_;

  /// List of Counter object pointers
  std::map<std::string,Counters *> counters_;

  /// List of Regions encountered
  std::vector<std::string> regions_;

};

}

#endif
