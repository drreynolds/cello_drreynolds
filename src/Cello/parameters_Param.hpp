// See LICENSE_CELLO file for license and copyright information

/// @file     parameters_Param.hpp
/// @author   James Bordner (jobordner@ucsd.edu)
/// @date     Sun Oct 11 14:55:25 PDT 2009
/// @brief    [\ref Parameters] Interface for the Param class

#ifndef PARAMETERS_PARAM_HPP
#define PARAMETERS_PARAM_HPP

//----------------------------------------------------------------------

/// @brief Print a parameter expression
extern "C" { 
  void print_expression(struct node_expr * node,
		      FILE * fp = stdout);
}

/// @brief Print a parameter expression
extern "C" { 
  void sprintf_expression(struct node_expr * node,
			  char * buffer);
}

/// @brief Print a parameter list
extern "C" { 
  void cello_parameters_print_list(struct param_struct * head, int level);
}

typedef std::vector<class Param *> list_type;

//----------------------------------------------------------------------

class Param {

  /// @class    Param
  /// @ingroup  Parameters
  /// @brief    [\ref Parameters] Represent and evaluate various
  /// parameter types and expressions

  friend class Parameters;

public: // interface

  /// Initialize a Param object
  Param () 
    : type_(parameter_unknown),
      value_accessed_(false)
  {};

  /// Delete a Param object
  ~Param () 
  { dealloc_(); };

  /// Copy constructor
  Param(const Param & param) throw()
  { INCOMPLETE("Param::Param"); };

  /// Assignment operator
  Param & operator= (const Param & param) throw()
  { INCOMPLETE("Param::operator =");
    return *this; };

  /// CHARM++ Pack / Unpack function
  inline void pup (PUP::er &p)
  {
    TRACEPUP;
    // NOTE: change this function whenever attributes change
    p | type_;
    p | value_accessed_;
    if (type_ == parameter_integer) {
      p | value_integer_;
    } else if (type_ == parameter_float) {
      p | value_float_;
    } else if (type_ == parameter_logical) {
      p | value_logical_; 
    } else if (type_ == parameter_string) {
      // *value_string_;
      WARNING("Param::pup","skipping value_string");
    } else if (type_ == parameter_list) {
      // *value_list_;
      WARNING("Param::pup","skipping value_list");
    } else if (type_ == parameter_logical_expr) {
      // *value_expr_;
      WARNING("Param::pup","skipping value_logical_expr");
    } else if (type_ == parameter_float_expr) {
      // *value_expr_;
      WARNING("Param::pup","skipping value_float_expr");
    } else if (type_ == parameter_unknown) {
      ERROR("Param::pup","parameter type is unknown");
    }
  }

  /// Evaluate a floating-point expression given vectos x,y,z,t
  void evaluate_float  
  ( int                n, 
    double *           result, 
    double *           x, 
    double *           y, 
    double *           z, 
    double             t,
    struct node_expr * node = 0 );

  /// Evaluate a logical expression given vectos x,y,z,t
  void evaluate_logical  
  ( int                n, 
    bool *             result, 
    double *           x, 
    double *           y, 
    double *           z, 
    double             t,
    struct node_expr * node = 0);

  /// Set the parameter type and value
  void set(struct param_struct * param);

  /// Write the parameter to the file
  void write(FILE * file_pointer,
	     std::string parameter);

  /// Return whether the parameter has the given type
  bool is_type (parameter_enum type) { return type_ == type; }

  bool accessed () const { return value_accessed_; }

  void set_accessed () { value_accessed_ = true; }

  /// Get an integer parameter
  int get_integer () 
  { value_accessed_ = true; return value_integer_; }

  /// Get a floating-point (double) parameter
  double get_float  () 
  { value_accessed_ = true; return value_float_; }

  /// Get a logical parameter
  int get_logical ()    
  { value_accessed_ = true; return value_logical_; }

  /// Get a string parameter (note that string is aliased)
  const char * get_string () 
  { value_accessed_ = true; return value_string_; }

  /// Convert the parameter value into a string
  std::string value_to_string ();

  /// Return the type of the parameter
  parameter_type type() const { return type_; } 

  //----------------------------------------------------------------------

private: // functions

  /// Set an integer parameter
  void set_integer_ (int value)
  { 
    type_ = parameter_integer; 
    value_integer_ = value; 
  };

  /// Set a floating-point (double) parameter
  void set_float_  (double value) 
  { 
    type_ = parameter_float; 
    value_float_ = value; 
  };

  /// Set a logical parameter
  void set_logical_ (int value)    
  { 
    type_ = parameter_logical; 
    value_logical_ = (value != 0); 
  };

  /// Set a string parameter (note that string is aliased)
  void set_string_ (char * value) 
  { 
    type_ = parameter_string; 
    value_string_ = value;
  };

  /// Set a list parameter given a list of parameter values
  ///
  /// Lists are bounded by "sentinel" types: the list values are
  /// taken to be between the first sentinel and the next sentinel

  void set_list_ (struct param_struct * value) 
  { 
    type_ = parameter_list; 
    value_list_ = new list_type;
    value = value->next; // Skip sentinel
    while (value->type != enum_parameter_sentinel) {
      Param * param = new Param;
      param->set(value);
      value_list_->push_back(param);
      value = value->next;
    }
  };

  /// Set a floating-point expression parameter
  void set_float_expr_ (struct node_expr * value)
  { 
    type_ = parameter_float_expr;
    value_expr_     = value; 
  };

  /// Set a logical expression parameter
  void set_logical_expr_ (struct node_expr * value)
  { 
    type_ = parameter_logical_expr;
    value_expr_     = value; 
  };

  /// Deallocate the parameter
  void dealloc_();

  /// Deallocate a string
  void dealloc_string_() { free (value_string_); } 

  /// Deallocate a list of parameters
  void dealloc_list_     (list_type *value_list_);

  /// Deallocate an expression parameter
  void dealloc_node_expr_ (struct node_expr * p);

  ///
  void write_float_expr_(FILE * file_pointer,
			  struct node_expr * value_expr_);

  //----------------------------------------------------------------------

private: // attributes

  /// Parameter type
  parameter_type type_;

  /// Whether parameter value has been accessed
  bool value_accessed_;

  /// Type definition for a list of parameters

  /// Value of the parameter, with type depending on type_
  union {
    int                value_integer_; 
    double             value_float_; 
    bool               value_logical_; 
    char *             value_string_;
    list_type *        value_list_;
    struct node_expr * value_expr_;
  };

};

//----------------------------------------------------------------------

#endif /* PARAMETERS_PARAM_HPP */

