// See LICENSE_CELLO file for license and copyright information

/// @file     method_InitialDefault.cpp
/// @author   James Bordner (jobordner@ucsd.edu)
/// @date     2011-02-16
/// @brief    Implementation of the InitialDefault class

#include "cello.hpp"

#include "problem.hpp"

//----------------------------------------------------------------------

InitialDefault::InitialDefault
(Parameters * parameters,
 int cycle, double time) throw ()
  : Initial (cycle, time),
    parameters_(parameters)
{
}

//----------------------------------------------------------------------

#ifdef CONFIG_USE_CHARM

void InitialDefault::pup (PUP::er &p)
{
  // NOTE: update whenever attributes change

  TRACEPUP;

  Initial::pup(p);

  bool up = p.isUnpacking();

  if (up) parameters_ = new Parameters;
  p | *parameters_;
}

#endif

//----------------------------------------------------------------------

void InitialDefault::enforce_block
(
 CommBlock        * comm_block,
 const FieldDescr * field_descr,
 const Hierarchy  * hierarchy
 ) throw()
{
  // Initialize Fields according to parameters

  ASSERT("InitialDefault::enforce_block",
	 "CommBlock does not exist",
	 comm_block != NULL);
  
  //--------------------------------------------------
  parameters_->group_set(0,"Initial");
  //--------------------------------------------------

  FieldBlock *       field_block = comm_block->block()->field_block();

  double *value=0, *vdeflt=0, *x=0, *y=0, *z=0, *t=0;
  bool * mask=0, *rdeflt=0;
  int n, nx=0,ny=0,nz=0;

  for (int index_field = 0;
       index_field < field_descr->field_count();
       index_field++) {
    
    std::string field_name = field_descr->field_name(index_field);

    //--------------------------------------------------
    parameters_->group_set(1,field_name);
    //--------------------------------------------------

    // If Initial:<field_name>:value is a list, try parsing it

    // parameter: Initial : <field> : value

    parameter_type parameter_type = parameters_->type("value");

    if (parameter_type == parameter_float) {

      field_block->clear(field_descr,parameters_->value_float("value",0.0), 
			 index_field);

    } else if (parameter_type == parameter_list) {

      // Check parameter length

      int list_length = parameters_->list_length("value");

      ASSERT1("InitialDefault::enforce_block",
	     "Length of list parameter Initial:%s:value must be odd",
	     field_name.c_str(),
	     (list_length % 2) == 1);

      // Allocate arrays if needed
      if (value == NULL) {
	allocate_xyzt_(comm_block,index_field,field_descr,
		       &nx,&ny,&nz,
		       &value, &vdeflt,
		       &mask,&rdeflt,
		       &x,&y,&z,&t);
      }

      // Evaluate last non-conditional equation in list

      n = nx*ny*nz;

      evaluate_float_ (field_block, list_length-1, field_name, 
			n, value,vdeflt,x,y,z,t);

      for (int i=0; i<n; i++) mask[i] = true;

      copy_values_ (field_descr,field_block,value, mask,index_field,nx,ny,nz);

      // Evaluate conditional equations in list

      for (int index_list=0; index_list < list_length-1; index_list+=2) {

	evaluate_float_ (field_block, index_list, field_name, 
			  n, value,vdeflt,x,y,z,t);

	evaluate_logical_ 
	  (hierarchy,comm_block,field_block, index_list+1, field_name, 
	   field_descr, n, mask,rdeflt,x,y,z,t);

	copy_values_ (field_descr,field_block,value, mask,index_field,nx,ny,nz);

      }

    } else if (parameter_type == parameter_unknown) {
      WARNING1("InitialDefault::enforce_block",  
	       "Uninitialized field %s",
	       field_name.c_str());
      
    } else {
      ERROR2("InitialDefault::enforce_block",
	     "Illegal parameter type %s when initializing field %s",
	     parameter_type_name[parameter_type],field_name.c_str());
    }
  }
  // Deallocate arrays if needed
  if (value != NULL) {
    delete [] value;
    delete [] vdeflt;
    delete [] mask;
    delete [] rdeflt;
    delete [] x;
    delete [] y;
    delete [] z;
    delete [] t;
  }
}

//======================================================================

void InitialDefault::allocate_xyzt_
(
 CommBlock * comm_block,
 int index_field,
 const FieldDescr * field_descr,
 int * nx, int * ny, int * nz,
 double ** value, double ** vdeflt,
 bool   ** mask,bool   ** rdeflt,
 double ** x, double ** y, double ** z, double ** t
 ) throw()
{

  FieldBlock * field_block = comm_block->block()->field_block();

  // Get field size

  field_block->size(nx,ny,nz);

  int gx,gy,gz;
  field_descr->ghosts(index_field,&gx,&gy,&gz);
  (*nx) += 2*gx;
  (*ny) += 2*gy;
  (*nz) += 2*gz;

  int n = (*nx)*(*ny)*(*nz);

  // Allocate double arrays

  (*value)  = new double [n];
  (*vdeflt) = new double [n];
  (*mask)   = new bool [n];
  (*rdeflt) = new bool [n];
  (*x)      = new double [n];
  (*y)      = new double [n];
  (*z)      = new double [n];
  (*t)      = new double [n];

  double xm, xp, ym, yp, zm, zp;

  comm_block->block()->lower(&xm,&ym,&zm);
  comm_block->block()->upper(&xp,&yp,&zp);

  double hx,hy,hz;
  field_block->cell_width(xm,xp,&hx,
			  ym,yp,&hy,
			  zm,zp,&hz);

  double time = comm_block->time();
  // Initialize arrays
  for (int iz=0; iz<(*nz); iz++) {
    for (int iy=0; iy<(*ny); iy++) {
      for (int ix=0; ix<(*nx); ix++) {
	int i=ix + (*nx)*(iy + (*ny)*iz);
	(*value)[i]  = 0.0;
	(*vdeflt)[i] = 0.0;
	(*mask)[i]   = false;
	(*rdeflt)[i] = false;
	(*x)[i]      = xm + (ix-gx)*hx;
	(*y)[i]      = ym + (iy-gy)*hy;
	(*z)[i]      = zm + (iz-gz)*hz;
	(*t)[i]      = time;
      }
    }
  }
}

//----------------------------------------------------------------------

void InitialDefault::copy_values_ 
(
 const FieldDescr * field_descr,
 FieldBlock *       field_block,
 double *           value, 
 bool *             mask,
 int                index_field,
 int nx, int ny, int nz
 ) throw()
{

  // Copy the floating-point values to the field where logical values are true

  void * field = field_block->field_unknowns(field_descr,index_field);

  // Determine allocated array size

  int gx,gy,gz;

  field_descr->ghosts(index_field,&gx,&gy,&gz);

  int offset = gx + nx*(gy + ny*gz);

  // Copy evaluated values to field values

  //  @@@@ BUG: PNG image input
  //  @@@@ IC's SMALLER BUT CENTERED IF OFFSET SET TO 0
  //  @@@@ IC's SHIFTED TO LOWER LEFT
  //  mask[0][0],[1][1],[2][2] == 1

  precision_type precision = field_descr->precision(index_field);
  TRACE3("nx ny nz = %d %d %d",nx,ny,nz);
  switch (precision) {
  case precision_single:
    for (int iz = 0; iz<nz; iz++) {
      for (int iy = 0; iy<ny; iy++) {
	for (int ix = 0; ix<nx; ix++) {
	  int i = ix + nx*(iy + ny*iz);
	  if (mask[i]) 
	    ((float *) field - offset)[i] = (float) value[i];
	}
      }
    }
    break;
  case precision_double:
    for (int iz = 0; iz<nz; iz++) {
      for (int iy = 0; iy<ny; iy++) {
	for (int ix = 0; ix<nx; ix++) {
	  int i = ix + nx*(iy + ny*iz);
	  if (mask[i]) 
	    ((double *) field - offset)[i] = (double) value[i];
	}
      }
    }
    break;
  case precision_quadruple:
    for (int iz = 0; iz<nz; iz++) {
      for (int iy = 0; iy<ny; iy++) {
	for (int ix = 0; ix<nx; ix++) {
	  int i = ix + nx*(iy + ny*iz);
	  if (mask[i]) 
	    ((long double *) field - offset)[i] = (long double) value[i];
	}
      }
    }
    break;
  default:
    break;
  }
}

//----------------------------------------------------------------------

void InitialDefault::evaluate_float_ 
(FieldBlock * field_block, int index_list, std::string field_name, 
 int n, double * value, double * deflt,
 double * x, double * y, double * z, double * t) throw ()
{

  // parameter: Initial : <field> : value

  parameter_type value_type = 
    parameters_->list_type(index_list,"value");

  if (value_type != parameter_float_expr &&
      value_type != parameter_float) {
	  	      
    ERROR1("InitialDefault::evaluate_float_", 
	   "Odd-index elements of %s must be floating-point expressions",
	   field_name.c_str());
  }

  // Evaluate the floating-point expression

  if (value_type == parameter_float) {
    double v = parameters_->list_value_float(index_list,"value",0.0);
    for (int i=0; i<n; i++) value[i] = v;
  } else {
    parameters_->list_evaluate_float
      (index_list,"value",n,value,deflt,x,y,z,t);
  }
}

//----------------------------------------------------------------------

void InitialDefault::evaluate_logical_ 
(const Hierarchy * hierarchy,
 const CommBlock * comm_block,
 FieldBlock * field_block, 
 int index_list, std::string field_name, 
 const FieldDescr * field_descr,
 int n, bool * mask, bool * deflt,
 double * x, double * y, double * z, double * t) throw ()
{

  // parameter: Initial : <field> : value

  parameter_type value_type = 
    parameters_->list_type(index_list,"value");

  bool v;
  const char * file;
  int nxb,nyb,nzb;
  int nx,ny;

  switch (value_type) {
  case parameter_logical:
    v = parameters_->list_value_logical(index_list,"value",false);
    for (int i=0; i<n; i++) mask[i] = v;
    break;
  case parameter_logical_expr:
    parameters_->list_evaluate_logical
      (index_list,"value",n,mask,deflt,x,y,z,t);
    break;
  case parameter_string:
    field_block->size(&nxb,&nyb,&nzb);
    ASSERT1("InitialDefault::evaluate_logical",
	   "mask file %s requires problem to be 2D",
	    field_name.c_str(),
	    nzb == 1);
    file = parameters_->list_value_string(index_list,"value","default");
    create_png_mask_(mask,hierarchy,comm_block,field_descr,file,nxb,nyb,&nx,&ny);
    break;
  default:
    ERROR3("InitialDefault::evaluate_logical",
	   "Even-index element %d of %s is of illegal type %d",
	   index_list,field_name.c_str(),value_type);
    break;
  }
}

//----------------------------------------------------------------------

void InitialDefault::create_png_mask_
(
 bool            * mask,
 const Hierarchy * hierarchy,
 const CommBlock * comm_block,
 const FieldDescr * field_descr,
 const char      * pngfile,
 int               nxb,
 int               nyb,
 int             * nx,
 int             * ny
 )
{
  int gx,gy,gz;
  field_descr->ghosts(0,&gx,&gy,&gz);
  nxb += 2*gx;
  nyb += 2*gy;

  pngwriter png;

  // Open the PNG file

  png.readfromfile(pngfile);

  // Get the PNG file size

  (*nx) = png.getwidth();
  (*ny) = png.getheight();

  // Clear the block mask

  int nb = nxb*nyb;
  for (int i=0; i<nb; i++) mask[i] = false;

  // Get the hierarchy's lower and upper extents

  double lower_h[3];
  hierarchy->lower(&lower_h[0],&lower_h[1],&lower_h[2]);
  double upper_h[3];
  hierarchy->upper(&upper_h[0],&upper_h[1],&upper_h[2]);

  // Get the block's lower and upper extents

  double lower_b[3];
  comm_block->block()->lower(&lower_b[0],&lower_b[1],&lower_b[2]);

  double upper_b[3];
  comm_block->block()->upper(&upper_b[0],&upper_b[1],&upper_b[2]);

  // get the block's cell width

  double hb[3];
  comm_block->block()->field_block()->cell_width 
    (lower_b[0],upper_b[0],&hb[0],
     lower_b[1],upper_b[1],&hb[1],
     lower_b[2],upper_b[2],&hb[2]);

  // Get the hierarchy's size including ghosts

  double size_h[3];
  size_h[0] = upper_h[0]-lower_h[0] + 2*gx*hb[0];
  size_h[1] = upper_h[1]-lower_h[1] + 2*gy*hb[1];

  // get the offset between the block and the hierarchy

  double offset_b[3];
  offset_b[0] = lower_b[0]-lower_h[0];
  offset_b[1] = lower_b[1]-lower_h[1];

  // Compute the block mask from the image pixel values
  // 0
  // ++---++
  //    0
  //    ++---++
  // 0  3
  // ++------++

  TRACE3("block lower %f %f %f",lower_b[0],lower_b[1],lower_b[2]);
  TRACE3("block upper %f %f %f",upper_b[0],upper_b[1],upper_b[2]);
  TRACE3("domain lower %f %f %f",lower_h[0],lower_h[1],lower_h[2]);
  TRACE3("domain upper %f %f %f",upper_h[0],upper_h[1],upper_h[2]);

  TRACE2("nb %d %d",nxb,nyb);
  TRACE3("hb %15.12f %15.12f %15.12f",hb[0],hb[1],hb[2]);
  TRACE3("size %15.12f %15.12f %15.12f",size_h[0],size_h[1],size_h[2]);

  for (int iy_b=0; iy_b<nyb; iy_b++) {
    int iy_h = int((*ny)*(iy_b*hb[1]+offset_b[1])/(size_h[1]));
    for (int ix_b=0; ix_b<nxb; ix_b++) {
      int ix_h = int((*nx)*(ix_b*hb[0]+offset_b[0])/(size_h[0]));
      if (ix_b==0&&iy_b==0)         TRACE2("A %d : %d",ix_h,ix_h);
      if (ix_b==nxb-1&&iy_b==nyb-1) TRACE2("B %d : %d",ix_h,ix_h);
      
      int i_b = ix_b + nxb*(iy_b);

      int r = png.read(ix_h+1,iy_h+1,1);
      int g = png.read(ix_h+1,iy_h+1,2);
      int b = png.read(ix_h+1,iy_h+1,3);

      if (ix_b==0 && iy_b==0) TRACE3("color %d %d %d",r,g,b);
      mask[i_b] = (r+g+b > 0);
      if (ix_b==0 && iy_b==0) {
	TRACE4("MASK 0: %d %d %d  %d",r,g,b,mask[i_b]);
	TRACE2("MASK 0: %d %d",ix_h,iy_h);
      }
      if (ix_b==nxb-1 && iy_b==nyb-1) {
	TRACE4("MASK n: %d %d %d  %d",r,g,b,mask[i_b]);
	TRACE2("MASK n: %d %d",ix_h,iy_h);
      }
    }
  }
  TRACE5("mask %p = %d %d %d %d",mask,mask[0],mask[nxb+1],mask[2*(nxb+1)],mask[3*(nxb+1)]);
  int k = 1024+nxb*(1024);
  TRACE5("mask %p = %d %d %d %d",mask,mask[k+(nxb+1)],mask[k+2*(nxb+1)],mask[k+3*(nxb+1)],mask[k+4*(nxb+1)]);
 
  png.close();
  return;
}
