// See LICENSE_CELLO file for license and copyright information

/// @file     mesh_ItNeighbor.hpp
/// @author   James Bordner (jobordner@ucsd.edu)
/// @date     2013-06-11
/// @brief    [\ref Mesh] Declaration of the ItNeighbor class
///

#ifndef MESH_ITNEIGHBOR_HPP
#define MESH_ITNEIGHBOR_HPP

class ItNeighbor {

  /// @class    ItNeighbor
  /// @ingroup  Mesh
  /// @brief    [\ref Mesh] 

public: // interface

  /// Constructor
  ItNeighbor(int rank,
	 int rank_limit,
	 bool periodic[3][2],
	 int n3[3],
	 Index index,
	 const int * ic3=0,
	 const int * of3=0) throw();

  /// Destructor
  ~ItNeighbor() throw();

  /// CHARM++ Pack / Unpack function
  inline void pup (PUP::er &p)
  {
    // NOTE: change this function whenever attributes change
    TRACEPUP;
    PUParray(p,of3_,3);
    p | ic3_;
    p | ipf3_;
    p | index_;
    p | rank_;
    p | rank_limit_;
    for (int i=0; i<3; i++) {
      PUParray (p,periodicity_[i],2);
    }
    PUParray (p,n3_,3);
    p | index_;
  }

  /// Reduce another value
  bool next (int of3[3]) throw();

  /// Reset the Iterator to the beginning
  void reset() throw();

  bool is_reset() const;

  /// Return the current value of the reduction operator
  void value(int of3[3]) const throw();

private: // functions

  /// go to the next face
  void increment_();


  /// go to the first face
  void set_first_();

  /// Whether the current face rank is valid
  bool valid_() const;

private: // attributes

  // NOTE: change pup() function whenever attributes change

  /// Current face
  int of3_[3];

  /// Adjacency child
  std::vector<int> ic3_;

  /// Parent face
  std::vector<int> ipf3_;

  /// simulation rank
  int rank_;

  /// face rank limit
  int rank_limit_;

  /// Periodicity
  int periodicity_[3][2];

  /// Size of the hierarchy forest
  int n3_[3];

  /// Index
  Index index_;

};

#endif /* MESH_ITNEIGHBOR_HPP */

