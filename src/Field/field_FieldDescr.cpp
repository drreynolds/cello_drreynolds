// $Id: field_FieldDescr.cpp 1262 2010-03-03 15:44:05Z bordner $
// See LICENSE_CELLO file for license and copyright information

/// @file     field_FieldDescr.cpp
/// @author   James Bordner (jobordner@ucsd.edu)
/// @date     Thu Feb 25 16:20:17 PST 2010
/// @brief    Implementation of the FieldDescr class

#include "error.hpp"
#include "field.hpp"

//----------------------------------------------------------------------

FieldDescr::FieldDescr () throw ()
  : alignment_(0),
    padding_(0),
    courant_(1),
    field_name_(),
    field_id_(),
    group_name_(),
    group_id_(),
    field_in_group_(),
    precision_(),
    centering_(),
    ghosts_(),
    min_value_(),
    max_value_(),
    min_action_(),
    max_action_()
{
}

//----------------------------------------------------------------------

FieldDescr::~FieldDescr() throw()
{
  for (size_t i=0; i<centering_.size(); i++)  delete centering_[i];
  for (size_t i=0; i<ghosts_.size();    i++)  delete ghosts_[i];
}

//----------------------------------------------------------------------

FieldDescr::FieldDescr(const FieldDescr & field_descr) throw()
{
  alignment_      = field_descr.alignment_;
  padding_        = field_descr.padding_;
  courant_        = field_descr.courant_;
  field_name_     = field_descr.field_name_;
  field_id_       = field_descr.field_id_;
  group_name_     = field_descr.group_name_;
  group_id_       = field_descr.group_id_;
  field_in_group_ = field_descr.field_in_group_;
  precision_      = field_descr.precision_;
  for (size_t i=0; i<field_descr.centering_.size(); i++) {
    centering_.push_back(new bool[3]);
    centering_[0] = field_descr.centering_[0];
    centering_[1] = field_descr.centering_[1];
    centering_[2] = field_descr.centering_[2];
  }
  for (size_t i=0; i<field_descr.ghosts_.size(); i++) {
    ghosts_.push_back(new int[3]);
    ghosts_[0] = field_descr.ghosts_[0];
    ghosts_[1] = field_descr.ghosts_[1];
    ghosts_[2] = field_descr.ghosts_[2];
  }
  centering_      = field_descr.centering_;
  ghosts_         = field_descr.ghosts_;
  min_value_      = field_descr.min_value_;
  max_value_      = field_descr.max_value_;
  min_action_     = field_descr.min_action_;
  max_action_     = field_descr.max_action_;
}

//----------------------------------------------------------------------

FieldDescr & FieldDescr::operator= (const FieldDescr & field_descr) throw()
{
  alignment_      = field_descr.alignment_;
  padding_        = field_descr.padding_;
  courant_        = field_descr.courant_;
  field_name_     = field_descr.field_name_;
  field_id_       = field_descr.field_id_;
  group_name_     = field_descr.group_name_;
  group_id_       = field_descr.group_id_;
  field_in_group_ = field_descr.field_in_group_;
  precision_      = field_descr.precision_;
  centering_.clear();
  for (size_t i=0; i<field_descr.centering_.size(); i++) {
    centering_.push_back(new bool[3]);
    centering_[0] = field_descr.centering_[0];
    centering_[1] = field_descr.centering_[1];
    centering_[2] = field_descr.centering_[2];
  }
  ghosts_.clear();
  for (size_t i=0; i<field_descr.ghosts_.size(); i++) {
    ghosts_.push_back(new int[3]);
    ghosts_[0] = field_descr.ghosts_[0];
    ghosts_[1] = field_descr.ghosts_[1];
    ghosts_[2] = field_descr.ghosts_[2];
  }
  centering_      = field_descr.centering_;
  ghosts_         = field_descr.ghosts_;
  min_value_      = field_descr.min_value_;
  max_value_      = field_descr.max_value_;
  min_action_     = field_descr.min_action_;
  max_action_     = field_descr.max_action_;

  return *this;
}

//----------------------------------------------------------------------

int FieldDescr::field_count() const throw()
{
  return field_name_.size();
}

//----------------------------------------------------------------------

std::string FieldDescr::field_name(size_t id_field) const throw(std::out_of_range)
{ 
  return field_name_.at(id_field);
}

//----------------------------------------------------------------------

int FieldDescr::field_id(const std::string name) const throw(std::out_of_range)
{
  return field_id_.at(name);
}

//----------------------------------------------------------------------

int FieldDescr::group_count() const throw()
{
  return group_name_.size(); 
}

//----------------------------------------------------------------------

std::string FieldDescr::group_name(int id_group) const throw(std::out_of_range)
{
  return group_name_.at(id_group);
}

//----------------------------------------------------------------------

int FieldDescr::group_id(const std::string name) const throw(std::out_of_range)
{
  return group_id_.at(name);
}

//----------------------------------------------------------------------

bool FieldDescr::field_in_group(int id_field, int id_group) const throw(std::out_of_range)
{
  set_int_type t = field_in_group_.at(id_field);
  return t.find(id_group) != t.end();
}

//----------------------------------------------------------------------

int FieldDescr::alignment() const throw()
{
  return alignment_;
}

//----------------------------------------------------------------------

int FieldDescr::padding() const throw()
{
  return padding_;
}

//----------------------------------------------------------------------

void FieldDescr::centering
(
 int id_field,
 bool * cx, 
 bool * cy, 
 bool * cz
) const throw(std::out_of_range)
{
  *cx = centering_.at(id_field)[0];
  *cy = centering_.at(id_field)[1];
  *cz = centering_.at(id_field)[2];
}

//----------------------------------------------------------------------

void FieldDescr::ghosts
(
 int id_field,
 int * gx, 
 int * gy, 
 int * gz
) const throw(std::out_of_range)
{
  *gx = ghosts_.at(id_field)[0];
  *gy = ghosts_.at(id_field)[1];
  *gz = ghosts_.at(id_field)[2];
}

//----------------------------------------------------------------------

precision_type FieldDescr::precision(int id_field) const throw(std::out_of_range)
{
  return precision_.at(id_field);
}

//----------------------------------------------------------------------

void FieldDescr::insert_field(std::string field_name) throw()
{

  int id = field_count();

  // Insert field name and id

  field_name_.push_back(field_name);

  field_id_[field_name] = id;

  // Initialize attributes with default values

  precision_type precision = precision_default;

  bool * centered = new bool[3];
  centered[0] = true;
  centered[1] = true;
  centered[2] = true;

  int * ghosts = new int [3];
  ghosts[0] = 1;
  ghosts[1] = 1;
  ghosts[2] = 1;

  set_int_type a;
  field_in_group_.push_back(a);
  precision_. push_back(precision);
  centering_. push_back(centered);
  ghosts_.    push_back(ghosts);
  min_value_. push_back(0);
  max_value_. push_back(0);
  min_action_.push_back(field_action_none);
  max_action_.push_back(field_action_none);
}

//----------------------------------------------------------------------

void FieldDescr::insert_group(std::string group_name) throw()
{
  int id = group_count();
  group_name_.push_back(group_name);
  group_id_[group_name] = id;
}

//----------------------------------------------------------------------

void FieldDescr::set_field_in_group(int id_field, int id_group) throw(std::out_of_range)
{
  field_in_group_.at(id_field).insert(id_group);
}

//----------------------------------------------------------------------

void FieldDescr::set_alignment(int alignment) throw()
{
}

//----------------------------------------------------------------------

void FieldDescr::set_padding(int padding) throw()
{
  padding_ = padding;
}

//----------------------------------------------------------------------

void FieldDescr::set_courant(double courant) throw()
{
  courant_ = courant;
}

//----------------------------------------------------------------------

void FieldDescr::set_precision(int id_field, precision_type precision) throw(std::out_of_range)
{
  precision_.at(id_field) = precision;
}

//----------------------------------------------------------------------

void FieldDescr::set_centering(int id_field, bool cx, bool cy, bool cz) throw(std::out_of_range)
{
  centering_.at(id_field)[0] = cx;
  centering_.at(id_field)[1] = cy;
  centering_.at(id_field)[2] = cz;
}

//----------------------------------------------------------------------

void FieldDescr::set_ghosts(int id_field, int gx, int gy, int gz) throw(std::out_of_range)
{
  ghosts_.at(id_field)[0] = gx;
  ghosts_.at(id_field)[1] = gy;
  ghosts_.at(id_field)[2] = gz;
}

//----------------------------------------------------------------------

void FieldDescr::set_minimum (int id_field, double min_value, field_action min_action) throw(std::out_of_range)
{
  min_value_.at(id_field)  = min_value;
  min_action_.at(id_field) = min_action;
}

//----------------------------------------------------------------------

void FieldDescr::set_maximum (int id_field, double max_value, field_action max_action) throw(std::out_of_range)
{
  max_value_.at(id_field)  = max_value;
  max_action_.at(id_field) = max_action;
}
