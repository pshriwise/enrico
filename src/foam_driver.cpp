#include "enrico/foam_driver.h"
#include "enrico/enricoFoamLibrary.H"

#include "enrico/error.h"
#include "gsl/gsl"
#include "iapws/iapws.h"

#include "xtensor/xadapt.hpp"

#include <climits>
#include <string>
#include <unistd.h>
#include <vector>

namespace enrico {

FoamDriver::FoamDriver(MPI_Comm comm, pugi::xml_node node)
  : HeatFluidsDriver(comm,node)
{
  //! Need to determine what this portion of the nekDriver does and how it needs
  //! to be applied for an OpenFOAM driver and setting up MPI
  if (active()) {

    foam_init(comm);
  //! Determining fluid mask may need to be done here, as there need be a link between the
  //! local_elem variable IDs and which material it is (i.e. nelt=SUM of elements in each region
  //! on a local process

  //! nelgt_ = foam_get_nelgt();
  //! nelt_ = foam_get_nelt();

    init_displs();
  }
  MPI_Barrier(MPI_COMM_WORLD);
}

std::vector<double> FoamDriver::temperature_local() const
{
  // Each Foam proc finds the temperatures of its local elements
  std::vector<double> local_elem_temperatures(nelt_);
  for (int32_t i = 0; i < nelt_; ++i) {
    local_elem_temperatures[i] = this->temperature_at(i + 1);
  }

  return local_elem_temperatures;
}

std::vector<int> FoamDriver::fluid_mask_local() const
{
  std::vector<int> local_fluid_mask(nelt_);
  for (int32_t i = 0; i < nelt_; ++i) {
    local_fluid_mask[i] = this->in_fluid_at(i + 1);
  }
  return local_fluid_mask;
}

std::vector<double> FoamDriver::density_local() const
{
  std::vector<double> local_densities(nelt_);

  for (int32_t i = 0; i < nelt_; ++i) {
    if (this->in_fluid_at(i + 1) == 1) {
      auto T = this->temperature_at(i + 1);
      // nu1 returns specific volume in [m^3/kg]
      local_densities[i] = 1.0e-3 / iapws::nu1(pressure_bc_, T);
    } else {
      local_densities[i] = 0.0;
    }
  }

  return local_densities;
}

void FoamDriver::solve_step()
{
  //! add calls to the foam module for running x timesteps
}


int FoamDriver::n_local_elem() const
{
//!  foam_get_n_local_elements()
}

std::size_t FoamDriver::n_global_elem() const
{
//!  foam_get_n_global_elements()
}

Position FoamDriver::centroid_at(int32_t local_elem) const
{
  double x, y, z;
//  err_chk(foam_get_local_elem_centroid(local_elem, &x, &y, &z),
//          "Could not find centroid of local element " + std::to_string(local_elem));
  return {x, y, z};
}

std::vector<Position> FoamDriver::centroid_local() const
{
  int n_local = this->n_local_elem();
  std::vector<Position> local_element_centroids(n_local);
  for (int32_t i = 0; i < n_local; ++i) {
    local_element_centroids[i] = this->centroid_at(i + 1);
  }
  return local_element_centroids;
}

double FoamDriver::volume_at(int32_t local_elem) const
{
  double volume;
//  err_chk(foam_get_local_elem_volume(local_elem, &volume),
///          "Could not find volume of local element " + std::to_string(local_elem));
  return volume;
}

std::vector<double> FoamDriver::volume_local() const
{
  int n_local = this->n_local_elem();
  std::vector<double> local_elem_volumes(n_local);
  for (int32_t i = 0; i < n_local; ++i) {
    local_elem_volumes[i] = this->volume_at(i + 1);
  }
  return local_elem_volumes;
}

double FoamDriver::temperature_at(int32_t local_elem) const
{
  double temperature;
//  err_chk(foam_get_local_elem_temperature(local_elem, &temperature),
//          "Could not find temperature of local element " + std::to_string(local_elem));
  return temperature;
}

int FoamDriver::in_fluid_at(int32_t local_elem) const
{
  //! This can potentially be (May need to be) wrapped into the init portion that gets
  //! number of elements locally and globally (which also need to be different than Nek)
//  return foam_local_elem_is_in_fluid(local_elem);
}

int FoamDriver::set_heat_source_at(int32_t local_elem, double heat)
{
  Expects(local_elem >= 1 && local_elem <= nelt_);
//  return foam_set_heat_source(local_elem, heat);
}

FoamDriver::~FoamDriver()
{
  //! if (active())
  //!   C2F_nek_end();
  //! MPI_Barrier(MPI_COMM_WORLD);
}

} // namespace enrico