// See LICENSE_CELLO file for license and copyright information

/// @file     test_Parameters.cpp
/// @author   James Bordner (jobordner@ucsd.edu)
/// @date     Thu Feb 21 16:04:03 PST 2008
/// @brief    Program implementing unit tests for the Parameters class
//----------------------------------------------------------------------

#include <fstream>

#include "main.hpp" 
#include "test.hpp"

#include "parameters.hpp"

//----------------------------------------------------------------------

/// @def      CLOSE
/// @brief    Local definition for testing whether two scalars are close
#define MACH_EPS cello::machine_epsilon(default_precision)
#define CLOSE(a,b) ( cello::err_rel(a,b) < 2*MACH_EPS )

//----------------------------------------------------------------------

void generate_input()
{
  std::fstream fp;

  fp.open ("test.in",std::fstream::out);

  // Groups
  // 
  // Logical:
  // Integer:
  // Float:
  // Float:const_float_1
  // Float:const_float_2
  // String
  // Float_expr
  // Float_expr:var_float_1
  // Float_expr:var_float_2
  // Logical_expr
  // Logical_expr:var_logical
  // List
  
  fp << "Logical {\n";
  fp << "  logical_1_true  = true;\n";
  fp << "  logical_2_false = false;\n";
  fp << "}\n";
  fp << "   \n";
  fp << "Integer {\n";
  fp << "  integer_1_1 = 1;\n";
  fp << "  integer_2_m37 = -37;\n";
  fp << "}\n";

  fp << "Float {\n";
  fp << "  float_1_1p5 = 1.5;\n";
  fp << "  test_m37_25 = -37.25;\n";
  fp << "}\n";

  fp << "Float {\n";
  fp << "  group_float_1 {\n";
  fp << "    num1 = 24.5 + 6.125;\n";
  fp << "    num2 = 24.5 - 6.125;\n";
  fp << "    num3 = 24.5 * 6.125;\n";
  fp << "    num4 = 24.5 / 6.125;\n";
  fp << "  };\n";
  fp << "  const_float_2 {\n";
  fp << "    num1 = 24.5 + 6.125*2.0;\n";
  fp << "    num2 = 24.5*3.0 - 6.125;\n";
  fp << "    num3 = (24.5 + 6.125*2.0 - (24.5*3.0 - 6.125));\n";
  fp << "  }\n";
  fp << "}\n";

  fp << "String {\n";
  fp << "  str1 = \"testing\";\n";
  fp << "  str2 = \"one\";\n";
  fp << "}\n";

  fp << "Float_expr {\n";
  fp << "  var_float_1 {\n";
  fp << "    num1 = x;\n";
  fp << "    num2 = x + 3.0;\n";
  fp << "    num3 = x+y+z+t;\n";
  fp << "  }\n";
  fp << "}\n";

  fp << " Float_expr {\n";
  fp << "    var_float_2 {\n";
  fp << "       num1 = sin(x);\n";
  fp << "       num2 = atan(y/3.0+3.0*t);\n";
  fp << "     }\n";
  fp << "  }\n";

  fp << " Logical_expr {\n";
  fp << "  var_logical {\n";
  fp << "    num1 = x < y;\n";
  fp << "    num2 = x + y >= t + 3.0;\n";
  fp << "    num3 = x == y;\n";
  fp << "  }\n";
  fp << "}\n";

  fp << " List {\n";
  fp << "  num1 = [1.0, true, -37, \"string\", x-y+2.0*z, x+y+t > 0.0 ];\n";
  fp << "}\n";

  fp.close();

}

//======================================================================

PARALLEL_MAIN_BEGIN
{

  PARALLEL_INIT;

  GroupProcess * parallel = GroupProcess::create();

  unit_init (parallel->rank(), parallel->size());

  unit_class("Parameters");

  Monitor::instance()->set_active(false);

  //----------------------------------------------------------------------
  // test parameter
  //----------------------------------------------------------------------

  Parameters * parameters = new Parameters;

  // Generate test.in to make sure it exists

  generate_input();

  unit_func("read");

  parameters->read ( "test.in" );
  unit_assert(true);

  unit_func("write");

  parameters->write ( "test.out" );
  unit_assert(unit_incomplete);

  unit_func("group_push");

  parameters->group_push("Group");

  unit_assert(parameters->group(0) == "Group");
  unit_assert(parameters->group_depth() == 1);

  parameters->group_push("subgroup_1");

  unit_assert(parameters->group_depth() == 2);
  unit_assert(parameters->group(0) == "Group");
  unit_assert(parameters->group(1) == "subgroup_1");

  unit_func("group_pop");

  parameters->group_pop("subgroup_1");

  unit_assert(parameters->group(0) == "Group");
  unit_assert(parameters->group_depth() == 1);

  parameters->group_push("subgroup_2");

  unit_assert(parameters->group_depth() == 2);
  unit_assert(parameters->group(0) == "Group");
  unit_assert(parameters->group(1) == "subgroup_2");

  parameters->group_pop();
  parameters->group_pop();

  unit_assert(parameters->group_depth() == 0);

  parameters->group_push("Group2");

  unit_assert(parameters->group(0) == "Group2");
  unit_assert(parameters->group_depth() == 1);

  parameters->group_pop();

  // value_logical()

  unit_func("value_logical");

  parameters->group_set(0,"Logical");
  
  unit_assert (parameters->value_logical("logical_1_true")  == true);
  unit_assert (parameters->value_logical("logical_2_false") == false);
  unit_assert (parameters->value_logical("none",true) == true);
  unit_assert (parameters->value_logical("none",false) == false);

  unit_assert(parameters->value_logical("Logical:logical_1_true") == true);
  unit_assert(parameters->value_logical("Logical:logical_2_false") == false);

  // bool l,ld;

  // parameters->value("logical_1_true",parameter_logical,&l);
  // unit_assert (l == true);

  // parameters->value("logical_2_false",parameter_logical,&l);
  // unit_assert (l == false);

  // ld = true;
  // parameters->value("none",parameter_logical,&l,&ld);
  // unit_assert (l == ld);

  // ld = false;
  // parameters->value("none",parameter_logical,&l,&ld);
  // unit_assert (l == ld);

  // set_logical()

  unit_func("set_logical");

  parameters->set_logical("logical_1_true",false);
  unit_assert (parameters->value_logical("logical_1_true") == false);
  parameters->set_logical("none",true);
  unit_assert (parameters->value_logical("none") == true);
  parameters->set_logical("none_l1",true);
  unit_assert (parameters->value_logical("none_l1") == true);
  parameters->set_logical("none_l2",false);
  unit_assert (parameters->value_logical("none_l2") == false);

  // value_integer()

  unit_func("value_integer");

  parameters->group_set(0,"Integer");
  
  unit_assert (parameters->value_integer("integer_1_1")  == 1);
  unit_assert (parameters->value_integer("integer_2_m37") == -37);
  unit_assert (parameters->value_integer("none",58) == 58);

  // int i,id;
  // parameters->value("integer_1_1",parameter_integer,&i);
  // unit_assert (i == 1);
  // parameters->value("test_37",parameter_integer,&i);
  // unit_assert (i == 37);
  // id = 58;
  // parameters->value("none",parameter_integer,&i,&id);
  // unit_assert (i == id);

  // set_integer()

  unit_func("set_integer");

  parameters->set_integer("integer_1_1",2);
  unit_assert (parameters->value_integer("integer_1_1") == 2);
  parameters->set_integer("none",3);
  unit_assert (parameters->value_integer("none") == 3);
  parameters->set_integer("none2",4);
  unit_assert (parameters->value_integer("none2") == 4);


  // value_float()
  
  unit_func("value_float");

  parameters->group_set(0,"Float");
  
  unit_assert (parameters->value_float("float_1_1p5")  == 1.5);
  unit_assert (parameters->value_float("test_m37_25") == -37.25);
  unit_assert (parameters->value_float("none",58.75) == 58.75);

  // double d,dd;
  
  // parameters->value("float_1_1p5",parameter_float,&d);
  // unit_assert (d  == 1.5);
  // parameters->value("test_37_25",parameter_float,&d);
  // unit_assert (d == 37.25);
  // dd = 58.75;
  // parameters->value("none",parameter_float,&d,&dd);
  // unit_assert (d == dd);

  // set_float()

  unit_func("set_float");

  parameters->set_float("float_1_1p5",27.0);
  unit_assert (parameters->value_float("float_1_1p5") == 27.0);
  parameters->set_float("none_s",1.5);
  unit_assert (parameters->value_float("none_s") == 1.5);

  // Constant float expressions
  // subgroups

  unit_func("value_float");

  parameters->group_set(0,"Float");
  parameters->group_set(1,"group_float_1");

  unit_assert(parameters->value_float("num1") == 30.625);
  unit_assert(parameters->value_float("num2") == 18.375);
  unit_assert(parameters->value_float("num3") == 150.0625);
  unit_assert(parameters->value_float("num4") == 4.0000000000);

  unit_assert(parameters->value_float("Float:group_float_1:num1") == 30.625);
  unit_assert(parameters->value_float("Float:group_float_1:num2") == 18.375);
  unit_assert(parameters->value_float("Float:group_float_1:num3") == 150.0625);
  unit_assert(parameters->value_float("Float:group_float_1:num4") == 4.0000000000);

  parameters->group_set(1,"const_float_2");

  unit_assert(parameters->value_float("num1") == 36.750);
  unit_assert(parameters->value_float("num2") == 67.375);
  unit_assert(parameters->value_float("num3") == -30.625);

  // Strings

  unit_func("value_string");

  parameters->group_set(0,"String");

  unit_assert(parameters->group_depth()==1);
  unit_assert(parameters->group(0)=="String");

  unit_assert(strcmp(parameters->value_string("str1"),"testing")==0);
  unit_assert(strcmp(parameters->value_string("str2","blah"),"one")==0);
  unit_assert(strcmp(parameters->value_string("none","blah"),"blah")==0);

  // const char *s, *sd = "blah";
  // parameters->value("str1",parameter_string,&s);
  // unit_assert(strcmp(s,"testing")==0);
  // parameters->value("str2",parameter_string,&s,&sd);
  // unit_assert(strcmp(s,"one")==0);
  // parameters->value("none",parameter_string,&s,&sd);
  // unit_assert(strcmp(s,"blah")==0);

  // set_string()

  unit_func("set_string");

  parameters->set_string("str1","yahoo");
  unit_assert (strcmp(parameters->value_string("str1"),"yahoo")==0);
  parameters->set_string("none_str","hello");
  unit_assert (strcmp(parameters->value_string("none_str"),"hello")==0);

  // Variable float expressions

  unit_func("evaluate_float");

  double x[] = { 1, 2, 3};
  double y[] = {5 , 4, 3};
  double z[] = {8, 9, 10};
  double t[] = {-1, 2, -7};
  double values_float[] = {0,0,0};
  double deflts_float[] = {-1,-2,-3};

  parameters->group_set(0,"Float_expr");
  parameters->group_set(1,"var_float_1");

  parameters->evaluate_float("num1",3,values_float,deflts_float,x,y,z,t);
  unit_assert (values_float[0]==x[0]);
  unit_assert (values_float[1]==x[1]);
  unit_assert (values_float[2]==x[2]);

  
  parameters->evaluate_float("num2",3,values_float,deflts_float,x,y,z,t);
  unit_assert (values_float[0]==x[0]+3.0);
  unit_assert (values_float[1]==x[1]+3.0);
  unit_assert (values_float[2]==x[2]+3.0);

  parameters->evaluate_float("num3",3,values_float,deflts_float,x,y,z,t);
  unit_assert (values_float[0]==x[0]+y[0]+z[0]+t[0]);
  unit_assert (values_float[1]==x[1]+y[1]+z[1]+t[1]);
  unit_assert (values_float[2]==x[2]+y[2]+z[2]+t[2]);

  parameters->group_set(1,"var_float_2");

  parameters->evaluate_float("num1",3,values_float,deflts_float,x,y,z,t);
  unit_assert (CLOSE(values_float[0],sin(x[0])));
  unit_assert (CLOSE(values_float[1],sin(x[1])));
  unit_assert (CLOSE(values_float[2],sin(x[2])));

  parameters->evaluate_float("num2",3,values_float,deflts_float,x,y,z,t);
  unit_assert (CLOSE(values_float[0],atan(y[0]/3.0+3*t[0])));
  unit_assert (CLOSE(values_float[1],atan(y[1]/3.0+3*t[1])));
  unit_assert (CLOSE(values_float[2],atan(y[2]/3.0+3*t[2])));

  // Logical expressions

  unit_func("evaluate_logical");

  bool values_logical[] = {false, false, false};
  bool deflts_logical[] = {true, false,true};

  parameters->group_set(0,"Logical_expr");
  parameters->group_set(1,"var_logical");

  parameters->evaluate_logical("num1",3,values_logical,deflts_logical,x,y,z,t);
  unit_assert (values_logical[0] == (x[0] < y[0]));
  unit_assert (values_logical[1] == (x[1] < y[1]));
  unit_assert (values_logical[2] == (x[2] < y[2]));

  parameters->evaluate_logical("num2",3,values_logical,deflts_logical,x,y,z,t);
  unit_assert (values_logical[0] == (x[0] + y[0] >= t[0] + 3.0));
  unit_assert (values_logical[1] == (x[1] + y[1] >= t[1] + 3.0));
  unit_assert (values_logical[2] == (x[2] + y[2] >= t[2] + 3.0));

  parameters->evaluate_logical("num3",3,values_logical,deflts_logical,x,y,z,t);
  unit_assert (values_logical[0] == (x[0] == y[0]));
  unit_assert (values_logical[1] == (x[1] == y[1]));
  unit_assert (values_logical[2] == (x[2] == y[2]));

  // Lists

  parameters->group_set(0,"List");

  unit_func("list_length");
  unit_assert(parameters->list_length("num1") == 6);

  unit_func("list_value_float");
  unit_assert(parameters->list_value_float(0,"num1") == 1.0);

  unit_func("list_value_logical");
  unit_assert(parameters->list_value_logical(1,"num1") == true);

  unit_func("list_value_integer");
  unit_assert(parameters->list_value_integer(2,"num1") == -37);

  unit_func("list_value_string");
  unit_assert(strcmp(parameters->list_value_string(3,"num1"),"string")==0);

  unit_func("list_evaluate_float");
  parameters->list_evaluate_float(4,"num1",3,values_float,deflts_float,x,y,z,t);
  unit_assert (values_float[0] == (x[0]-y[0]+2.0*z[0]));
  unit_assert (values_float[1] == (x[1]-y[1]+2.0*z[1]));
  unit_assert (values_float[2] == (x[2]-y[2]+2.0*z[2]));

  unit_func("list_evaluate_logical");
  parameters->list_evaluate_logical(5,"num1",3,values_logical,deflts_logical,x,y,z,t);
  unit_assert (values_logical[0] == (x[0]+y[0]+t[0] > 0 ));
  unit_assert (values_logical[1] == (x[1]+y[1]+t[1] > 0 ));
  unit_assert (values_logical[2] == (x[2]+y[2]+t[2] > 0 ));

  // group_count(), group(i)

  unit_func("child_count");

  delete parameters;
  parameters = new Parameters;
  parameters->read ("test.in");
  const int NUM_GROUPS = 7;
  struct {
    const char * group;
    int count;
  } child_count[NUM_GROUPS] = {
    {"Float",       4},
    {"Float_expr",  2},
    {"Integer",     2},
    {"List",        1},
    {"Logical",     2},
    {"Logical_expr",1},
    {"String",      2}
  };

  parameters->group_clear();
  int num_groups = parameters->group_count();
  unit_assert (num_groups == NUM_GROUPS);

  for (int i=0; i<NUM_GROUPS; i++) {
    parameters->group_set(0,child_count[i].group);
    unit_assert (parameters->group_count() == child_count[i].count);
  }
      

  unit_finalize();

  PARALLEL_EXIT;
}

PARALLEL_MAIN_END

