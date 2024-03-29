// See LICENSE_CELLO file for license and copyright information

/// @file     mesh_RefineMass.cpp
/// @author   James Bordner (jobordner@ucsd.edu)
/// @date     2013-04-23
/// @brief    Implementation of RefineMass class

#include "mesh.hpp"

//----------------------------------------------------------------------

RefineMass::RefineMass
(
 double min_refine,
 double max_coarsen,
 double level_exponent,
 double root_cell_volume,
 std::string output) throw ()
  : Refine(min_refine,max_coarsen,output),
    level_exponent_(level_exponent)
  // ENZO non-cosmology

  //    MinimumMassForRefinement[0] = MinimumOverDensityForRefinement[0];
  //    for (int dim = 0; dim < MetaData.TopGridRank; dim++)
  //      MinimumMassForRefinement[0] *=(DomainRightEdge[dim]-DomainLeftEdge[dim])/
  //        float(MetaData.TopGridDims[dim]);

  // ENZO Cosmology
  //      MinimumMassForRefinement[i] = CosmologySimulationOmegaBaryonNow/
  //	                            OmegaMatterNow;
  //      if (CellFlaggingMethod[i] == 4)
  //	MinimumMassForRefinement[i] = CosmologySimulationOmegaCDMNow/
  //	                              OmegaMatterNow;
  // 
  //      MinimumMassForRefinement[i] *= MinimumOverDensityForRefinement[i];
  //      for (dim = 0; dim < MetaData.TopGridRank; dim++)
  //	MinimumMassForRefinement[i] *=
  //	  (DomainRightEdge[dim]-DomainLeftEdge[dim])/
  //	  float(MetaData.TopGridDims[dim]);
{
  TRACE("RefineSlope::RefineSlope");
  WARNING ("RefineMass::RefineMass()",
	   "Assuming non-Cosmology problem for RefineMass");

}

//----------------------------------------------------------------------

int RefineMass::apply 
(
 Block * block,
 const FieldDescr * field_descr
 ) throw ()
{
  FieldData * field_data = block->data()->field_data();
  int level = 0;

  WARNING ("RefineMass::RefineMass()",
	   "Assuming refinement factor == 2");
  WARNING ("RefineMass::RefineMass()",
	   "Assuming level==0 for level_exponent");

  double mass_min_refine  = min_refine_ * pow(2.0,level*level_exponent_);
  double mass_max_coarsen = max_coarsen_* pow(2.0,level*level_exponent_);

  int nx,ny,nz;
  field_data->size(&nx,&ny,&nz);

  int gx,gy,gz;
  field_descr->ghost_depth(0, &gx,&gy,&gz);

  precision_type precision = field_descr->precision(0);

  void * void_array  = field_data->values(0);
  void * void_output = initialize_output_(field_data);

  //  int num_fields = field_descr->field_count();

  bool all_coarsen = true;
  bool any_refine = false;

  const int d3[3] = {1,nx,nx*ny};

  switch (precision) {
  case precision_single:
    {
      float * array  = (float*)void_array;
      float * output = (float*)void_output;
      float mass;
      for (int axis=0; axis<3; axis++) {
	int d = d3[axis];
	for (int ix=0; ix<nx; ix++) {
	  for (int iy=0; iy<ny; iy++) {
	    for (int iz=0; iz<nz; iz++) {
	      int i = (gx+ix) + nx*((gy+iy) + ny*(gz+iz));
	      mass = (array[i]) ? (array[i+d] - array[i-d]) / array[i] : 0.0;
	      if (mass > mass_min_refine)  any_refine  = true;
	      if (mass > mass_max_coarsen) all_coarsen = false;
	      if (output) {
		if (mass > mass_max_coarsen) output[i] =  0;
		if (mass > mass_min_refine)  output[i] = +1;
	      }
	    }
	  }
	}
      }
    }
    break;
  case precision_double:
    {
      double * array  = (double*)void_array;
      double * output = (double*)void_output;
      double mass;

      for (int axis=0; axis<3; axis++) {
	int d = d3[axis];
	for (int ix=0; ix<nx; ix++) {
	  for (int iy=0; iy<ny; iy++) {
	    for (int iz=0; iz<nz; iz++) {
	      int i = (gx+ix) + nx*((gy+iy) + ny*(gz+iz));
	      mass = (array[i]) ? (array[i+d] - array[i-d]) / array[i] : 0.0;
	      if (mass > mass_min_refine)  any_refine  = true;
	      if (mass > mass_max_coarsen) all_coarsen = false;
	      if (output) {
		if (mass > mass_max_coarsen) output[i] =  0;
		if (mass > mass_min_refine)  output[i] = +1;
	      }
	    }
	  }
	}
      }
    }
    break;
  default:
    ERROR2("RefineMass::apply",
	   "Unknown precision %d for field %d",
	   precision,0);
    break;
  }

  return 
    any_refine ?  adapt_refine : (all_coarsen ? adapt_coarsen : adapt_same) ;

}


//======================================================================

