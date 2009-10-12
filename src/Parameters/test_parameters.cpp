//345678901234567890123456789012345678901234567890123456789012345678901234567890

/** 
 *********************************************************************
 *
 * @file      test_parameters.cpp
 * @brief     Program implementing unit tests for the Parameters class
 * @author    James Bordner
 * @date      Thu Feb 21 16:04:03 PST 2008
 * 
 * $Id: test_parameters.cpp 715 2009-07-08 23:48:09Z bordner $
 * 
 *********************************************************************
 */
 
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "test.hpp"
#include "error.hpp"
#include "parameters.hpp"

#define CLOSE(a,b) ((((a) - (b)) / (fabs(a) + fabs(b))) < 1e-16)

int main(int argc, char **argv)
{

  unit_class ("Parameters");

  unit_open();

  //----------------------------------------------------------------------
  // test parameter
  //----------------------------------------------------------------------

  Parameters * parameters = new Parameters;

  unit_func("read");
  FILE * fp = fopen ("test.in","r");

  parameters->read ( fp );

  // set_group()

  unit_func("set_group");
  parameters->set_group("Group");
  unit_assert(parameters->get_group() == "Group");

  // set_subgroup()
  unit_func("set_subgroup");
  parameters->set_subgroup("subgroup_1");
  unit_assert(parameters->get_subgroup() == "subgroup_1");
  parameters->set_subgroup("subgroup_2");
  unit_assert(parameters->get_subgroup() == "subgroup_2");

  // set_group() without set_subgroup() clears subgroup to ""

  unit_func("set_group");
  parameters->set_group("Group2");
  unit_assert(parameters->get_group() == "Group2");
  unit_assert(parameters->get_subgroup() == "");

  // value_logical()

  bool value_logical;

  unit_func("value_logical");

  parameters->set_group("Logical");
  
  unit_assert (parameters->value_logical("test_true")  == true);
  unit_assert (parameters->value_logical("test_false") == false);
  unit_assert (parameters->value_logical("test_none",true) == true);
  unit_assert (parameters->value_logical("test_none",false) == false);

  // value_integer()

  int value_integer;

  unit_func("value_integer");

  parameters->set_group("Integer");
  
  unit_assert (parameters->value_integer("test_1")  == 1);
  unit_assert (parameters->value_integer("test_37") == 37);
  unit_assert (parameters->value_integer("test_none",58) == 58);

  // value_scalar()
  
  double value_scalar;

  unit_func("value_scalar");

  parameters->set_group("Scalar");
  
  unit_assert (parameters->value_scalar("test_1_5")  == 1.5);
  unit_assert (parameters->value_scalar("test_37_25") == 37.25);
  unit_assert (parameters->value_scalar("test_none",58.75) == 58.75);

  // Constant scalar expressions
  // subgroups

  unit_func("value_scalar");

  parameters->set_group("Scalar");

  parameters->set_subgroup("const_scalar_1");

  unit_assert(parameters->value_scalar("num1") == 30.625);
  unit_assert(parameters->value_scalar("num2") == 18.375);
  unit_assert(parameters->value_scalar("num3") == 150.0625);
  unit_assert(parameters->value_scalar("num4") == 4.0000000000);

  parameters->set_subgroup("const_scalar_2");

  unit_assert(parameters->value_scalar("num1") == 36.750);
  unit_assert(parameters->value_scalar("num2") == 67.375);
  unit_assert(parameters->value_scalar("num3") == -30.625);

  // Variable scalar expressions

  unit_func("evaluate_scalar");

  double x[] = { 1, 2, 3};
  double y[] = {5 , 4, 3};
  double z[] = {8, 9, 10};
  double t[] = {-1, 2, -7};
  double values[] = {0,0,0};
  double deflts[] = {-1,-2,-3};

  parameters->set_group("Scalar_expr");

  parameters->set_subgroup("var_scalar_1");

  parameters->evaluate_scalar("num1",3,values,deflts,x,y,z,t);
  unit_assert (values[0]==x[0]);
  unit_assert (values[1]==x[1]);
  unit_assert (values[2]==x[2]);

  
  parameters->evaluate_scalar("num2",3,values,deflts,x,y,z,t);
  unit_assert (values[0]==x[0]+3.0);
  unit_assert (values[1]==x[1]+3.0);
  unit_assert (values[2]==x[2]+3.0);

  parameters->evaluate_scalar("num3",3,values,deflts,x,y,z,t);
  unit_assert (values[0]==x[0]+y[0]+z[0]+t[0]);
  unit_assert (values[1]==x[1]+y[1]+z[1]+t[1]);
  unit_assert (values[2]==x[2]+y[2]+z[2]+t[2]);

  parameters->set_subgroup("var_scalar_2");

  parameters->evaluate_scalar("num1",3,values,deflts,x,y,z,t);
  unit_assert (CLOSE(values[0],sin(x[0])));
  unit_assert (CLOSE(values[1],sin(x[1])));
  unit_assert (CLOSE(values[2],sin(x[2])));

  parameters->evaluate_scalar("num2",3,values,deflts,x,y,z,t);
  unit_assert (CLOSE(values[0],atan(y[0]/3.0+3*t[0])));
  unit_assert (CLOSE(values[1],atan(y[1]/3.0+3*t[1])));
  unit_assert (CLOSE(values[2],atan(y[2]/3.0+3*t[2])));

  unit_func("evaluate_logical");
  unit_assert(0);

  // Lists

  unit_func("value_list");

  parameters->set_group("List");

  unit_assert(parameters->list_value_scalar(0,"num1") == 1.0);
  unit_assert(parameters->list_value_logical(1,"num1") == true);
  unit_assert(parameters->list_value_integer(2,"num1") == 37);
  unit_assert(parameters->list_value_string(3,"num1") == "string");

  unit_close();

  delete parameters;

}
