// See LICENSE_CELLO file for license and copyright information

/// @file     field_FieldDescr.hpp
/// @author   James Bordner (jobordner@ucsd.edu)
/// @date     2009-11-17
/// @brief    [\ref Field] Declaration for the FieldDescr class

#ifndef FIELD_FIELD_DESCR_HPP
#define FIELD_FIELD_DESCR_HPP

enum field_action_enum {
  field_action_unknown,  // Uninitialized action
  field_action_none,     // Do nothing if range exceeded
  field_action_assign,   // Assign field values to min / max if range exceeded
  field_action_warning,  // Issue warning if range exceeded
  field_action_error,    // Issue error if range exceeded
  field_action_timestep, // Retry with reduced timestep if range exceeded
  field_action_method    // Retry with alternate method if range exceeded
};

class FieldDescr 
{

  /// @class    FieldDescr
  /// @ingroup  Field
  /// @brief    [\ref Field] Interface for the FieldDescr class

public: // functions

  /// Initialize a FieldDescr object
  FieldDescr() throw();

  /// Destructor
  ~FieldDescr() throw();

  /// Copy constructor
  FieldDescr(const FieldDescr & field_descr) throw();

  /// Assignment operator
  FieldDescr & operator= (const FieldDescr & field_descr) throw();

  /// Set membership of a field in a group
  void set_field_in_group(int id_field, int id_group) 
    throw(std::out_of_range);

  /// Set alignment
  void set_alignment(int alignment) throw();

  /// Set padding
  void set_padding(int padding) throw();

  /// Set courant
  void set_courant(double courant) throw();

  /// Set centering for a field
  void set_centering(int id_field, bool cx, bool cy, bool cz) 
    throw(std::out_of_range);

  /// Set ghosts for a field
  void set_ghosts(int id_field, int gx, int gy, int gz) 
    throw(std::out_of_range);

  /// Set whether to update the k-D ghosts (corners=0, edges=1, faces=2)
  void set_refresh_face(int face, bool value) throw();

  /// Return whether to update the k-D ghosts (corners=0, edges=1, faces=2)
  bool refresh_face(int face) const throw();

  /// Set precision for a field
  void set_precision(int id_field, precision_enum precision) 
    throw(std::out_of_range);

  /// Set minimum bound and action
  void set_minimum (int id_field, double min_value,
		    enum field_action_enum min_action) 
    throw(std::out_of_range);

  /// Set maximum bound and action
  void set_maximum (int id_field, double max_value, 
		    enum field_action_enum max_action) 
    throw(std::out_of_range);

  /// Insert a new field
  int insert_field(std::string name_field) throw();

  /// Insert a new group
  void insert_group(std::string name_group) throw();

  //----------------------------------------------------------------------

  /// Return the number of fields
  int field_count() const throw();

  /// Return name of the ith field
  std::string field_name(size_t id_field) const throw(std::out_of_range);

  /// Return whether the field has been inserted
  bool is_field(const std::string name) const throw();

  /// Return the integer handle for the named field
  int field_id(const std::string name) throw();

  /// Return the number of groups
  int group_count() const throw();

  /// Return name of the ith group
  std::string group_name(int id_group) const throw(std::out_of_range);

  /// Return whether the group has been inserted
  bool is_group(const std::string name) const throw();

  /// Return the integer handle for the named group
  int group_id(const std::string name) throw();


  /// Return whether the given field is in the given group
  bool field_in_group(int id_field, int id_group) 
    const throw(std::out_of_range);


  /// alignment in bytes of fields in memory
  int alignment() const throw();

  /// padding in bytes between fields in memory
  int padding() const throw();

  /// courant number for fields
  double courant() const throw();

  /// centering of given field
  void centering(int id_field, bool * cx, bool * cy = 0, bool * cz = 0) const 
    throw(std::out_of_range);

  /// depth of ghost zones of given field
  void ghosts(int id_field, int * gx, int * gy = 0, int * gz = 0) const 
    throw(std::out_of_range);

  /// Set 
  /// precision of given field
  precision_enum precision(int id_field) const 
    throw(std::out_of_range);

  /// Number of bytes per element required by the given field
  int bytes_per_element(int id_field) const throw();

  /// minimum value for the field
  double minimum_value(int id_field) const throw(std::out_of_range);

  /// minimum action for the field
  enum field_action_enum minimum_action(int id_field) const
    throw(std::out_of_range);

  /// maximum value for the field
  double maximum_value(int id_field) const  throw(std::out_of_range);

  /// maximum action for the field
  enum field_action_enum maximum_action(int id_field) const 
    throw(std::out_of_range);

private: // functions

  void copy_(const FieldDescr & field_descr) throw();

private: // attributes

  /// String defining each field
  std::vector<std::string> field_name_;

  /// Integer id for each field.  Inverse mapping of field_name_
  std::map<std::string,int> field_id_;

  /// String defining each group
  std::vector<std::string> group_name_;

  /// Integer id for each group.  Inverse mapping of group_name_
  std::map<std::string,int> group_id_;

  typedef std::set<int> int_set_type;
  /// Set of groups containing each field.  field_in_group_[field][group]
  std::vector<int_set_type> field_in_group_;

  /// alignment of start of each field in bytes
  int alignment_;

  /// padding between fields in bytes
  int padding_;

  /// Courant number for fields
  double courant_;

  /// Precision of each field
  std::vector<precision_enum> precision_;

  /// cell centering for each field
  std::vector<bool *> centering_;

  /// Ghost depth of each field
  std::vector<int *> ghosts_;

  /// Whether to update the k-D ghosts (corners, edges, faces)
  bool refresh_face_[3];

  /// minimum allowed value for each field
  std::vector<double> min_value_;

  /// maximum allowed value for each field
  std::vector<double> max_value_;

  /// what to do if a field violates its minimum value
  std::vector<enum field_action_enum> min_action_;

  /// what to do if a field violates its maximum value
  std::vector<enum field_action_enum> max_action_;

};

#endif /* FIELD_FIELD_DESCR_HPP */

