// See LICENSE_CELLO file for license and copyright information

/// @file     parameters_EnzoConfig.cpp
/// @author   James Bordner (jobordner@ucsd.edu)
/// @date     2012-10-03
/// @brief    Implementation of the EnzoConfig class 

#include "cello.hpp"
#include "enzo.hpp"

//----------------------------------------------------------------------

EnzoConfig::EnzoConfig() throw ()
{
}

//----------------------------------------------------------------------

EnzoConfig::~EnzoConfig() throw ()
{
}

//----------------------------------------------------------------------

void EnzoConfig::pup (PUP::er &p)
{
  Config::pup(p);
  TRACEPUP;
  // NOTE: change this function whenever attributes change

  p | ppm_density_floor;
  p | ppm_diffusion;
  p | ppm_dual_energy;
  p | ppm_dual_energy_eta_1;
  p | ppm_dual_energy_eta_2;
  p | ppm_flattening;
  p | ppm_minimum_pressure_support_parameter;
  p | ppm_number_density_floor;
  p | ppm_pressure_floor;
  p | ppm_pressure_free;
  p | ppm_steepening;
  p | ppm_temperature_floor;
  p | ppm_use_minimum_pressure_support;

  p | field_gamma;

  p | cosmology;
  p | cosmology_comoving_box_size;
  p | cosmology_hubble_constant_now;
  p | cosmology_initial_redshift;
  p | cosmology_max_expansion_rate;
  p | cosmology_omega_lamda_now;
  p | cosmology_omega_matter_now;

  PUParray(p,sedov_array,3);
  p | sedov_radius_relative;
  p | sedov_pressure_in;
  p | sedov_pressure_out;
  p | sedov_density;

  p | interpolation_method;

  p | method_heat_alpha;

  // Grackle cooling parameters


}

//----------------------------------------------------------------------

void EnzoConfig::read(Parameters * p) throw()
{

  TRACE("BEGIN EnzoConfig::read()");

  // Read Cello parameters

  
  TRACE("EnzoCharm::read calling Config::read()");
  ((Config*)this) -> read (p);

  double floor_default = 1e-6;

  ppm_density_floor = p->value_float
    ("Method:ppm:density_floor",  floor_default);
  ppm_diffusion = p->value_logical 
    ("Method:ppm:diffusion",  false);
  ppm_dual_energy = p->value_logical 
    ("Method:ppm:dual_energy",false);
  ppm_dual_energy_eta_1 = p->value_float
    ("Method:ppm:dual_energy_eta_1", 0.001);
  ppm_dual_energy_eta_2 = p->value_float
    ("Method:ppm:dual_energy_eta_2", 0.1);
  ppm_flattening = p->value_integer
    ("Method:ppm:flattening", 3);
  ppm_minimum_pressure_support_parameter = p->value_integer
    ("Method:ppm:minimum_pressure_support_parameter",100);
  ppm_number_density_floor = p->value_float
    ("Method:ppm:number_density_floor", floor_default);
  ppm_pressure_floor = p->value_float
    ("Method:ppm:pressure_floor", floor_default);
  ppm_pressure_free = p->value_logical
    ("Method:ppm:pressure_free",false);
  ppm_steepening = p->value_logical 
    ("Method:ppm:steepening", false);
  ppm_temperature_floor = p->value_float
    ("Method:ppm:temperature_floor", floor_default);
  ppm_use_minimum_pressure_support = p->value_logical
    ("Method:ppm:use_minimum_pressure_support",false);


  cosmology = p->value_logical ("Method:cosmology",false);
  cosmology_comoving_box_size = p->value_float
    ("Method:cosmology:comoving_box_size", 64.0);
  cosmology_hubble_constant_now = p->value_float
    ("Method:cosmology:hubble_constant_now",0.701);
  cosmology_initial_redshift = p->value_float
    ("Method:cosmology:initial_redshift",  20.0);;
  cosmology_max_expansion_rate = p->value_float
    ("Method:cosmology:max_expansion_rate", 0.01);
  cosmology_omega_lamda_now = p->value_float
    ("Method:cosmology:omega_lambda_now",   0.721);
  cosmology_omega_matter_now = p->value_float
    ("Method:cosmology:omega_matter_now",   0.279);

  field_gamma = p->value_float ("Field:gamma",5.0/3.0);

  TRACE1("field_gamma = %f",field_gamma);

  sedov_array[0] = p->list_value_integer (0,"Initial:sedov:array",1);
  sedov_array[1] = p->list_value_integer (1,"Initial:sedov:array",1);
  sedov_array[2] = p->list_value_integer (2,"Initial:sedov:array",1);

  sedov_radius_relative = 
    p->value_float("Initial:sedov:radius_relative",0.1);
  sedov_pressure_in = 
    p->value_float("Initial:sedov:pressure_in",1.0);
  sedov_pressure_out = 
    p->value_float("Initial:sedov:pressure_in",1e-5);
  sedov_density = 
    p->value_float("Initial:sedov:density",1.0);

  interpolation_method = p->value_string 
    ("Field:interpolation_method","SecondOrderA");


  method_heat_alpha = p->value_float 
    ("Method:heat:alpha",1.0);

  /// Grackle parameters

  chemistry_data g = set_default_chemistry_parameters();

  method_grackle_gamma = p->value_float
    ("Method:grackle:gamma",g.Gamma);
  
  method_grackle_with_radiative_cooling =p->value_logical
    ("Method:grackle:with_radiative_cooling",g.with_radiative_cooling);

  method_grackle_primordial_chemistry = p->value_logical
    ("Method:grackle:multi_species",g.primordial_chemistry);

  method_grackle_metal_cooling = p->value_logical
    ("Method:grackle:metal_cooling",g.metal_cooling);

  method_grackle_h2_formation_on_dust = p->value_logical
    ("Method:grackle:h2_on_dust",g.h2_on_dust);

  method_grackle_cmb_temperature_floor = p->value_logical
    ("Method:grackle:cmb_temperature_floor",g.cmb_temperature_floor);

  method_grackle_data_file = p->value_string
    ("Method:grackle:data_file",g.grackle_data_file);

  method_grackle_cie_cooling = p->value_integer
    ("Method:grackle:cie_cooling",g.cie_cooling);

  method_grackle_h2_optical_depth_approximation = p->value_integer
    ("Method:grackle:h2_optical_depth_approximation",g.h2_optical_depth_approximation);

  method_grackle_photoelectric_heating = p->value_integer
    ("Method:grackle:photoelectric_heating",g.photoelectric_heating);

  method_grackle_photoelectric_heating_rate = p->value_float
    ("Method:grackle:photoelectric_heating_rate",g.photoelectric_heating_rate);

  method_grackle_UVbackground = p->value_integer
    ("Method:grackle:UVbackground",g.UVbackground);


  //  method_grackle_UVbackground_redshift_on      = 7.0;
  //  method_grackle_ = p->value_
  //    ("Method:grackle:",g.);
  //  method_grackle_UVbackground_redshift_off     = 0.0;
  //  method_grackle_ = p->value_
  //    ("Method:grackle:",g.);
  //  method_grackle_UVbackground_redshift_fullon  = 6.0;
  //  method_grackle_ = p->value_
  //    ("Method:grackle:",g.);
  //  method_grackle_UVbackground_redshift_drop    = 0.0;
  //  method_grackle_ = p->value_
  //    ("Method:grackle:",g.);
  //
  //  method_grackle_Compton_xray_heating   = 0;
  //  method_grackle_ = p->value_integer
  //    ("Method:grackle:",g.);
  //
  //  method_grackle_LWbackground_intensity = 0.0;   // [in units of 10^21 erg/s/cm^2/Hz/sr]
  //  method_grackle_ = p->value_
  //    ("Method:grackle:",g.);
  //  method_grackle_LWbackground_sawtooth_suppression = 0;
  //  method_grackle_ = p->value_integer
  //    ("Method:grackle:",g.);
  //
  //  method_grackle_HydrogenFractionByMass       = 0.76;
  //  method_grackle_ = p->value_
  //    ("Method:grackle:",g.);
  //  /* The DToHRatio is by mass in the code, so multiply by 2. */
  //  method_grackle_DeuteriumToHydrogenRatio     = 2.0*3.4e-5; // Burles & Tytler 1  method_grackle_ = p->value_
  //    ("Method:grackle:",g.);
  //998
  //  method_grackle_SolarMetalFractionByMass     = 0.02041;
  //  method_grackle_ = p->value_
  //    ("Method:grackle:",g.);
  //  method_grackle_NumberOfTemperatureBins      = 600;
  //  method_grackle_ = p->value_integer
  //    ("Method:grackle:",g.);
  //  method_grackle_ih2co                        = 1;
  //  method_grackle_ = p->value_integer
  //    ("Method:grackle:",g.);
  //  method_grackle_ipiht                        = 1;
  //  method_grackle_ = p->value_integer
  //    ("Method:grackle:",g.);
  //  method_grackle_TemperatureStart             = 1.0;
  //  method_grackle_ = p->value_
  //    ("Method:grackle:",g.);
  //  method_grackle_TemperatureEnd               = 1.0e9;
  //  method_grackle_ = p->value_
  //    ("Method:grackle:",g.);
  //  method_grackle_comp_xray                    = 0;
  //  method_grackle_ = p->value_integer
  //    ("Method:grackle:",g.);
  //  method_grackle_temp_xray                    = 0;
  //  method_grackle_ = p->value_integer
  //    ("Method:grackle:",g.);
  //  method_grackle_CaseBRecombination           = 0; // default to case A rates
  //  method_grackle_ = p->value_integer
  //    ("Method:grackle:",g.);
  //  method_grackle_NumberOfDustTemperatureBins  = 250;
  //  method_grackle_ = p->value_integer
  //    ("Method:grackle:",g.);
  //  method_grackle_DustTemperatureStart         = 1.0;
  //  method_grackle_ = p->value_
  //    ("Method:grackle:",g.);
  //  method_grackle_DustTemperatureEnd           = 1500.0;
  //  method_grackle_ = p->value_
  //    ("Method:grackle:",g.);
  //
  //  method_grackle_cloudy_metal.grid_rank        = 0;
  //  method_grackle_ = p->value_integer
  //    ("Method:grackle:",g.);
  //  method_grackle_cloudy_electron_fraction_factor = 9.153959e-3; // Cloudy 07.02   method_grackle_ = p->value_
  //    ("Method:grackle:",g.);
  //abundances
  //  
  //    grackle_chemistry.primordial_chemistry           = MultiSpecies;
  //  method_grackle_ = p->value_integer
  //    ("Method:grackle:",g.);
  //    grackle_chemistry.metal_cooling                  = MetalCooling;
  //  method_grackle_ = p->value_integer
  //    ("Method:grackle:",g.);
  //    grackle_chemistry.h2_on_dust                     = H2FormationOnDust;
  //  method_grackle_ = p->value_integer
  //    ("Method:grackle:",g.);
  //    grackle_chemistry.cmb_temperature_floor          = CloudyCoolingData.CMBTemp  method_grackle_ = p->value_
  //    ("Method:grackle:",g.);
  //eratureFloor;
  //  method_grackle_ = p->value_
  //    ("Method:grackle:",g.);
  //    grackle_chemistry.three_body_rate                = ThreeBodyRate;
  //  method_grackle_ = p->value_
  //    ("Method:grackle:",g.);
  //    grackle_chemistry.cie_cooling                    = CIECooling;
  //  method_grackle_ = p->value_
  //    ("Method:grackle:",g.);
  //    grackle_chemistry.h2_optical_depth_approximation = H2OpticalDepthApproximati  method_grackle_ = p->value_
  //    ("Method:grackle:",g.);
  //on;
  //  method_grackle_ = p->value_
  //    ("Method:grackle:",g.);
  //    grackle_chemistry.photoelectric_heating          = PhotoelectricHeating;
  //  method_grackle_ = p->value_
  //    ("Method:grackle:",g.);
  //    grackle_chemistry.photoelectric_heating_rate     = PhotoelectricHeatingRate;
  //  method_grackle_ = p->value_
  //    ("Method:grackle:",g.);
  //    grackle_chemistry.NumberOfTemperatureBins        = CoolData.NumberOfTemperatureBins;
  //  method_grackle_ = p->value_
  //    ("Method:grackle:",g.);
  //    grackle_chemistry.CaseBRecombination             = RateData.CaseBRecombination;
  //  method_grackle_ = p->value_
  //    ("Method:grackle:",g.);
  //    grackle_chemistry.TemperatureStart               = CoolData.TemperatureStart;
  //  method_grackle_ = p->value_
  //    ("Method:grackle:",g.);
  //    grackle_chemistry.TemperatureEnd                 = CoolData.TemperatureEnd;
  //  method_grackle_ = p->value_
  //    ("Method:grackle:",g.);
  //    grackle_chemistry.NumberOfDustTemperatureBins    = RateData.NumberOfDustTemperatureBins;
  //  method_grackle_ = p->value_
  //    ("Method:grackle:",g.);
  //    grackle_chemistry.DustTemperatureStart           = RateData.DustTemperatureStart;
  //  method_grackle_ = p->value_
  //    ("Method:grackle:",g.);
  //    grackle_chemistry.DustTemperatureEnd             = RateData.DustTemperatureEnd;
  //  method_grackle_ = p->value_
  //    ("Method:grackle:",g.);
  //    grackle_chemistry.HydrogenFractionByMass         = CoolData.HydrogenFractionByMass;
  //  method_grackle_ = p->value_
  //    ("Method:grackle:",g.);
  //    grackle_chemistry.DeuteriumToHydrogenRatio       = CoolData.DeuteriumToHydrogenRatio;
  //  method_grackle_ = p->value_
  //    ("Method:grackle:",g.);
  //    grackle_chemistry.SolarMetalFractionByMass       = CoolData.SolarMetalFractionByMass;
  //  method_grackle_ = p->value_
  //    ("Method:grackle:",g.);

  //    // Initialize units structure.
  //    float a_value, dadt;
  //    a_value = 1.0;
  //    grackle_units.a_units = 1.0;
  //    if (GetUnits(&DensityUnits, &LengthUnits, &TemperatureUnits,
  //                 &TimeUnits, &VelocityUnits, MetaData.Time) == FAIL) {
  //      ENZO_FAIL("Error in GetUnits.\n");
  //    }
  //    if (ComovingCoordinates) {
  //      if (CosmologyComputeExpansionFactor(MetaData.Time, &a_value, 
  //                                          &dadt) == FAIL) {
  //        ENZO_FAIL("Error in CosmologyComputeExpansionFactors.\n");
  //      }
  //      grackle_units.a_units            = 1.0 / (1.0 + InitialRedshift);
  //    }
  //    grackle_units.comoving_coordinates = ComovingCoordinates;
  //  method_grackle_ = p->value_
  //    ("Method:grackle:",g.);
  //    grackle_units.density_units        = DensityUnits;
  //  method_grackle_ = p->value_
  //    ("Method:grackle:",g.);
  //    grackle_units.length_units         = LengthUnits;
  //  method_grackle_ = p->value_
  //    ("Method:grackle:",g.);
  //    grackle_units.time_units           = TimeUnits;
  //  method_grackle_ = p->value_
  //    ("Method:grackle:",g.);
  //    grackle_units.velocity_units       = VelocityUnits;
  //  method_grackle_ = p->value_
  //    ("Method:grackle:",g.);

  TRACE("END   EnzoConfig::read()");
}

//======================================================================

