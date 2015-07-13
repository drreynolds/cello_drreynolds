// See LICENSE_CELLO file for license and copyright information

/// @file     enzo_EnzoMatrixIdentity.cpp
/// @author   James Bordner (jobordner@ucsd.edu)
/// @author   Daniel R. Reynolds (reynolds@smu.edu)
/// @date     2015-04-02
/// @brief    Implementation of the discrete Identity operator EnzoMatrixIdentity

#include "enzo.hpp"

//======================================================================

void EnzoMatrixIdentity::matvec (int id_y, int id_x, Block * block) throw()
{
  Data * data = block->data();
  Field field = data->field();

  int mx,my,mz;
  field.dimensions(0,&mx,&my,&mz);
  m_ = mx*my*mz;

  int precision = field.precision(0);

  void * X = field.values(id_x);
  void * Y = field.values(id_y);

  if      (precision == precision_single)    
    matvec_((float *)(Y),(float *)(X));
  else if (precision == precision_double)    
    matvec_((double *)(Y),(double *)(X));
  else if (precision == precision_quadruple) 
    matvec_((long double *)(Y),(long double *)(X));
  else 
    ERROR1("EnzoMatrixIdentity::matvec()", "precision %d not recognized", precision);
}

//----------------------------------------------------------------------

void EnzoMatrixIdentity::diagonal (int id_x, Block * block) throw()
{
  Data * data = block->data();
  Field field = data->field();

  int mx,my,mz;
  field.dimensions(0,&mx,&my,&mz);
  m_ = mx*my*mz;

  int precision = field.precision(0);

  void * X = field.values(id_x);

  if      (precision == precision_single)    
    diagonal_((float *)(X));
  else if (precision == precision_double)    
    diagonal_((double *)(X));
  else if (precision == precision_quadruple) 
    diagonal_((long double *)(X));
  else 
    ERROR1("EnzoMatrixIdentity::diagonal()", "precision %d not recognized", precision);
}

//----------------------------------------------------------------------

template <class T>
void EnzoMatrixIdentity::matvec_ (T * Y, T * X) const throw()
{
  for (int i=0; i<m_; i++) Y[i] = X[i];
}

//----------------------------------------------------------------------

template <class T>
void EnzoMatrixIdentity::diagonal_ (T * X) const throw()
{
  for (int i=0; i<m_; i++) X[i] = 1.0;
}
