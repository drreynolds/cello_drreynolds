#ifndef PATCH_HPP
#define PATCH_HPP

#include "parallel.def"

enum face_type {
  face_xm,
  face_xp,
  face_ym,
  face_yp,
  face_zm,
  face_zp
};

#include "test_jacobi.decl.h"

class Patch : public CBase_Patch
{
  
public:

  Patch(int patch_count, int patch_size, CProxy_Main main_proxy) ;
  ~Patch() ;

  Patch(CkMigrateMessage *) ;

public: // entry methods

  void p_evolve();
  void p_receive(int axis, int face, int n, double * buffer);

private: // functions

  void initialize_();

  void allocate_(int n);
  void compute_();

  void face_to_buffer_ (int axis, int face, double * buffer);
  void buffer_to_ghost_(int axis, int face, double * buffer);
  void print_ ();


private: // attributes

  CProxy_Main main_proxy_;

  int block_count_;
  int block_size_;

  double * values_;

  int cycle_values_;
  int cycle_ghosts_[6];

  int count_receive_;

};

#endif /* PATCH_HPP */
