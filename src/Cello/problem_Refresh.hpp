// See LICENSE_CELLO file for license and copyright information

/// @file     problem_Refresh.hpp
/// @author   James Bordner (jobordner@ucsd.edu)
/// @date     2014-11-04 22:24:46
/// @brief    [\ref Problem] Declaration of the Refresh class
///

#ifndef PROBLEM_REFRESH_HPP
#define PROBLEM_REFRESH_HPP

class Refresh : public PUP::able {

  /// @class    Refresh
  /// @ingroup  Problem
  /// @brief    [\ref Problem] 

public: // interface

  /// empty constructor for charm++ pup()
  Refresh() throw() { }

  /// Create a Refresh object
  Refresh(int ghost_depth,
	  int min_face_rank) throw()
    : field_list_(),
      ghost_depth_(ghost_depth),
      min_face_rank_(min_face_rank),
      sync_(),
      active_(true)

  {  }

  /// CHARM++ PUP::able declaration
  PUPable_decl(Refresh);

  /// CHARM++ migration constructor for PUP::able
  Refresh (CkMigrateMessage *m) : PUP::able(m)
  {  }

  /// CHARM++ Pack / Unpack function
  void pup (PUP::er &p)
  {

    PUP::able::pup(p);

    p | field_list_;
    p | ghost_depth_;
    p | min_face_rank_;
    p | sync_type_;
    p | sync_;
    p | active_;
    p | callback_;
  }

  /// Add the given field to the list

  void add_field(int id_field) {
    field_list_.push_back(id_field);
  }

  /// All fields are refreshed

  void add_all_fields(int num_fields) {
    field_list_.clear();
    for (int i=0; i<num_fields; i++) {
      field_list_.push_back(i);
    }
  }

  /// Return the list of fields participating in the Refresh operation

  std::vector<int> & field_list() {
    return field_list_;
  }

  // Whether this Block is participating in the Refresh operation

  void set_active (bool active) 
  { active_ = active; }

  bool active () const 
  { return active_; }
    
  /// Set or return the current minimum rank (dimension) of faces to
  /// refresh e.g. 0: everything, 1: omit corners, 2: omit corners and
  /// edges

  void set_min_face_rank (int min_face_rank)
  { min_face_rank_ = min_face_rank; }

  int min_face_rank() const 
  { return min_face_rank_; }

  /// Set or return the data field ghost depth

  void set_ghost_depth(int ghost_depth)
  { ghost_depth_ = ghost_depth; }

  int ghost_depth() const
  { return ghost_depth_; }

  Sync & sync() 
  {  return sync_; }

  void set_sync_type(int sync_type) 
  { sync_type_ = sync_type; }

  int sync_type() const 
  { return sync_type_; }

  int callback() const { return callback_; };

  void set_callback(int callback) 
  { callback_ = callback; }

private: // functions


private: // attributes

  // NOTE: change pup() function whenever attributes change

  /// Indicies of fields to include
  std::vector <int> field_list_;

  /// Ghost zone depth
  int ghost_depth_;

  /// minimum face field rank to refresh (0 = corners, 1 = edges, etc.)
  int min_face_rank_;

  /// Synchronization type
  int sync_type_;

  /// Counter for synchronization
  Sync sync_;

  /// Whether the Refresh object is active for the block
  bool active_;

  /// Callback after the refresh operation
  int callback_;
};

#endif /* PROBLEM_REFRESH_HPP */

