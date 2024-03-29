// See LICENSE_CELLO file for license and copyright information

/// @file     enzo_EnzoMethodGravityMlat.hpp
/// @author   James Bordner (jobordner@ucsd.edu) 
/// @date     2015-04-24 22:32:18
/// @brief    [\ref Enzo] Declaration of EnzoMethodGravityMlat
///
/// Multigrid on adaptive meshes for solving for self-gravity on field
/// data.

#ifndef ENZO_ENZO_METHOD_GRAVITY_MLAT_HPP
#define ENZO_ENZO_METHOD_GRAVITY_MLAT_HPP

class EnzoMethodGravityMlat : public Method {

  /// @class    EnzoMethodGravityMlat
  /// @ingroup  Enzo
  ///
  /// @brief [\ref Enzo] Demonstration method to solve self-gravity
  /// using Multigrid.

public: // interface

  /// Create a new EnzoMethodGravityMlat object
  EnzoMethodGravityMlat
  (const FieldDescr * field_descr, int rank,
   double grav_const,
   int iter_max, 
   double res_tol,
   int monitor_iter,
   std::string smoother,
   bool is_singular,
   Restrict * restrict,
   Prolong * prolong,
   int level_min,
   int level_max);

  EnzoMethodGravityMlat() {};

  /// Charm++ PUP::able declarations
  PUPable_decl(EnzoMethodGravityMlat);
  
  /// Charm++ PUP::able migration constructor
  EnzoMethodGravityMlat (CkMigrateMessage *m) {}

  /// Destructor
  ~EnzoMethodGravityMlat () throw();

  /// CHARM++ Pack / Unpack function
  void pup (PUP::er &p)
  {

    // NOTE: change this function whenever attributes change

    TRACEPUP;

    Method::pup(p);

    p | A_;
    p | smooth_;
    p | restrict_;
    p | prolong_;
    p | is_singular_;
    p | rank_;
    p | grav_const_;
    p | iter_max_;
    p | res_tol_;
    p | monitor_iter_;
    p | rr_;
    p | rr0_;
    p | rr_min_;
    p | rr_max_;

    p | idensity_;
    p | ipotential_;
    p | ib_;
    p | id_;
    p | ir_;
    p | ix_;
    p | iy_;

    p | iter_;
    p | level_min_;
    p | level_max_;

    p | precision_;
  }

  /// Solve for the gravitational potential
  virtual void compute( Block * block) throw();

  virtual std::string name () throw () 
  { return "gravity_mg"; }

  void compute_correction(EnzoBlock * enzo_block) throw();

protected: // methods

  template <class T>
  void enter_solver_(EnzoBlock * enzo_block) throw();
  template <class T>
  void begin_cycle_(EnzoBlock * enzo_block) throw();
  void send_faces_(EnzoBlock * enzo_block) throw();
  int determine_count_(EnzoBlock * enzo_block) throw();
  void receive_face_() throw();
  /// Apply pre-smoothing on the current level
  void pre_smooth_(EnzoBlock * enzo_block) throw();
  void do_restrict_() throw();
  void evaluate_b_(EnzoBlock * enzo_block) throw();
  /// Solve the coarse-grid equation A*C = R
  void solve_coarse_(EnzoBlock * enzo_block) throw();
  void coarse_solved_() throw();
  /// Prolong the correction C to the next-finer level
  void do_prolong_() throw();

  /// Apply post-smoothing to the current level
  void post_smooth_(EnzoBlock * enzo_block) throw();
  void end_cycle_(EnzoBlock * enzo_block) throw();
  template <class T>
  void exit_solver_(EnzoBlock * enzo_block, int retval) throw();
  
  void monitor_output_(EnzoBlock * enzo_block) throw();

protected: // attributes

  /// Matrix
  Matrix * A_;

  /// Smoother
  Compute * smooth_;

  /// Restriction
  Restrict * restrict_;

  /// Prolongation
  Prolong * prolong_;

  /// Whether you need to subtract of the nullspace of A from b, e.g. fully
  /// periodic or Neumann problems
  bool is_singular_;

  /// Dimensionality of the problem
  int rank_;

  /// Gas constant, e.g. 6.67384e-8 (cgs)
  double grav_const_;

  /// Maximum number of MG iterations
  int iter_max_;

  /// Convergence tolerance on the residual reduction rz_ / rz0_
  double res_tol_;

  /// How often to display progress
  int monitor_iter_;

  /// Current residual
  long double rr_;

  /// Initial residual
  long double rr0_;

  /// Minimum residual
  long double rr_min_;

  /// Maximum residual
  long double rr_max_;

  /// Density and potential field id's

  int idensity_;
  int ipotential_;

  /// MG vector id's
  int ib_;
  int id_;
  int ir_;
  int ix_;
  int iy_;

  /// Current MG iteration
  int iter_;

  /// Minimum refinement level (may be < 0)
  int level_min_;

  /// Maximum refinement level
  int level_max_;

  /// Precision of fields
  int precision_;
};

#endif /* ENZO_ENZO_METHOD_GRAVITY_MLAT_HPP */
