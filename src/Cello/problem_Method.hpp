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
#ifdef TEMP_NEW_REFRESH
    p | refresh_;
#else
    p | index_refresh_;
#endif
  }

public: // virtual functions

  /// Apply the method to advance a block one timestep 

  virtual void compute ( Block * block) throw() = 0; 

  /// Return the name of this Method
  virtual std::string name () throw () = 0;

  /// Compute maximum timestep for this method
  virtual double timestep (Block * block) const throw() 
  { return std::numeric_limits<double>::max(); }

  /// Resume computation after a reduction
  virtual void compute_resume ( Block * block,
				CkReductionMsg * msg) throw()
  {
    /* This function intentionally empty */
  }

  /// Append a Refresh index to the list of indices
#ifndef TEMP_NEW_REFRESH
  void add_index_refresh (int index) {
    index_refresh_.push_back(index);
  }
#endif

  /// Return the ith refresh index in 'index', and return whether i is in range
#ifndef TEMP_NEW_REFRESH
  int index_refresh (size_t i) const
  { const bool in_range = (i < index_refresh_.size());
    return in_range ? index_refresh_[i] : -1;
  }
#endif

#ifndef TEMP_NEW_REFRESH
  int num_refresh() const {
    return index_refresh_.size();
  }
#endif

#ifdef TEMP_NEW_REFRESH
  Refresh * refresh() { return refresh_; }
#endif

protected: // functions

  /// Perform vector copy X <- Y
  template <class T>
  void copy_ (T * X, const T * Y,
	      int mx, int my, int mz,
	      bool is_active) const throw()
  {
    if (! is_active ) return;
    const int m = mx*my*mz;
    for (int i=0; i<m; i++) X[i] = Y[i];
  }

protected: // attributes

#ifdef TEMP_NEW_REFRESH
  ///  Refresh object
  Refresh * refresh_;
#else
  /// Indices of Refresh objects associated with this method
  std::vector<int> index_refresh_;
#endif


};

#endif /* PROBLEM_METHOD_HPP */
