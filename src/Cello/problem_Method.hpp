// See LICENSE_CELLO file for license and copyright information

/// @file     problem_Method.hpp 
/// @author   James Bordner (jobordner@ucsd.edu) 
/// @date     Mon Jul 13 11:11:47 PDT 2009 
/// @brief    [\ref Problem] Declaration for the Method class

#ifndef PROBLEM_METHOD_HPP
#define PROBLEM_METHOD_HPP

class Method : public PUP::able 
{
  /// @class    Method
  /// @ingroup  Method
  /// @brief    [\ref Method] Interface to an application method / analysis / visualization function.

public: // interface

  /// Create a new Method
  Method () throw()
  : rank_(0),
    nx_(0),ny_(0),nz_(0),
    gx_(0),gy_(0),gz_(0),
    hx_(0),hy_(0),hz_(0),
    dt_(0),
    xm_(0),xp_(0),ym_(0),yp_(0),zm_(0),zp_(0)
  {}

  /// Destructor
  virtual ~Method() throw()
  {}

  /// Charm++ PUP::able declarations
  PUPable_abstract(Method);

  /// CHARM++ Pack / Unpack function
  void pup (PUP::er &p)
  { TRACEPUP;
    PUP::able::pup(p);
    WARNING ("Method::pup()","Skipping Method: attributes refreshed when needed");
    // p | rank_;
    // p | mx_; p | my_; p | mz_;
    // p | nx_; p | ny_; p | nz_;
    // p | gx_; p | gy_; p | gz_;
    // p | hx_; p | hy_; p | hz_;
    // p | dt_;
    // p | xm_; p | ym_; p | zm_;
    // p | xp_; p | yp_; p | zp_;
    // p | precision_;
    // p | field_name_;
    // p | field_array_;
    // p | field_id_;
  }

public: // virtual functions

  /// Apply the method to advance a block one timestep 

  virtual void compute ( CommBlock * comm_block) throw() = 0; 

  /// Compute maximum timestep for this method
  virtual double timestep (CommBlock * comm_block) throw() 
  { return std::numeric_limits<double>::max(); }

protected: // functions

  /// Get CommBlock attributes that typical methods will need
  void initialize_(CommBlock * comm_block) throw();

  /// Return the rank of the initialized CommBlock
  int rank() const 
  { return rank_; }

  /// Dimensions of the initialized CommBlock array
  void array_dimension (int id, int *mx, int *my, int *mz) const {
    ASSERT2 ("Method::dimensions()",
	    "id out of bounds: %d not between 0 and %d",
	     id,field_array_.size()-1,
	     0 <= id && id < (int)field_array_.size());
    if (mx) (*mx) = mx_.at(id);
    if (my) (*my) = my_.at(id);
    if (mz) (*mz) = mz_.at(id);
  }

  /// Size of the initialized CommBlock array (excluding ghost zones)
  void array_size (int *nx, int *ny, int *nz) const {
    if (nx) (*nx) = nx_;
    if (ny) (*ny) = ny_;
    if (nz) (*nz) = nz_;
  }

  void ghost_depth (int id, int *gx, int *gy, int *gz) const {
    ASSERT2 ("Method::ghost_depth()",
	    "id out of bounds: %d not between 0 and %d",
	     id,field_array_.size()-1,
	     0 <= id && id < (int)field_array_.size());
    if (gx) (*gx) = gx_.at(id);
    if (gy) (*gy) = gy_.at(id);
    if (gz) (*gz) = gz_.at(id);
  }

  void cell_width (double * hx, double * hy, double * hz) const
  {
    if (hx) (*hx) = hx_;
    if (hy) (*hy) = hy_;
    if (hz) (*hz) = hz_;
  }

  double time_step() const
  { return dt_; }

  void lower (double * xm, double * ym, double * zm) const
  {
    if (xm) (*xm) = xm_;
    if (ym) (*ym) = ym_;
    if (zm) (*zm) = zm_;
  }

   void upper (double * xp, double * yp, double * zp) const
   {
     if (xp) (*xp) = xp_;
     if (yp) (*yp) = yp_;
     if (zp) (*zp) = zp_;
   }

  int num_fields() const
  { return field_name_.size(); }

  std::string field_name (int id) const
  {
    ASSERT2 ("Method::field_name()",
	    "id out of bounds: %d not between 0 and %d",
	     id,field_name_.size()-1,
	     0 <= id && id < (int)field_name_.size());
    return field_name_.at(id);
  }
  std::string field_name (int id)
  {
    ASSERT2 ("Method::field_name()",
	    "id out of bounds: %d not between 0 and %d",
	     id,field_name_.size()-1,
	     0 <= id && id < (int)field_name_.size());
    return field_name_.at(id);
  }

  void * field_array (int id) const
  {
    ASSERT2 ("Method::field_array()",
	    "id out of bounds: %d not between 0 and %d",
	     id,field_array_.size()-1,
	     0 <= id && id < (int)field_array_.size());
    return field_array_.at(id);
  }
  void * field_array (int id)
  {
    ASSERT2 ("Method::field_array()",
	    "id out of bounds: %d not between 0 and %d",
	     id,field_array_.size()-1,
	     0 <= id && id < (int)field_array_.size());
    return field_array_.at(id);
  }

  int field_id (std::string field) const {
    if (field_id_.find(field) != field_id_.end()) {
      return field_id_.at(field);
    } else return -1;
  }

  int field_precision(int id) const
  { 
    ASSERT2 ("Method::field_precision()",
	    "id out of bounds: %d not between 0 and %d",
	     id,field_array_.size()-1,
	     0 <= id && id < (int)field_array_.size());
    return field_precision_.at(id);
  }

 private: // attributes (deliberately not accessible to derived classes)

   /// Dimensional rank of the CommBlock: 1, 2, or 3
   int rank_;

   /// Number of field variables in the CommBlock
   int field_count_;

   /// Dimensionality of the CommBlock array including ghosts
   std::vector<int> mx_,my_,mz_;

  /// Size of the CommBlock array excluding ghost zones
  int nx_,ny_,nz_;

  /// Number of ghost zones for each field variable
  std::vector<int> gx_, gy_, gz_;

  /// Cell widths
  double hx_,hy_,hz_;

  /// Time step
  double dt_;

  /// Extents of the CommBlock excluding ghost zones
  double xm_,xp_,ym_,yp_,zm_,zp_;

  /// Names of the CommBlock field variables
  std::vector<std::string> field_name_;

  /// Starting addresses of the CommBlock field variables
  std::vector<void *> field_array_;

  /// CommBlock field variable id's
  std::map<std::string,int> field_id_;

  /// Precision for each CommBlock field variables
  std::vector<int> field_precision_;

};

#endif /* PROBLEM_METHOD_HPP */
