//! \file foam_driver.h
//! Driver to initalize and run OpenFOAM in stages
//! Developed to run a modified chtMultiRegionFoam
#ifndef ENRICO_FOAM_DRIVER_H
#define ENRICO_FOAM_DRIVER_H

#include "enrico/geom.h"
#include "enrico/heat_fluids_driver.h"
#include "mpi.h"
#include "pugixml.hpp"
#include "xtensor/xtensor.hpp"

#include <string>
#include <vector>

namespace enrico {

//! driver to initialize and run OpenFOAM in stages
class FoamDriver : public HeatFluidsDriver {
public:
  //! Initializes OpenFOAM with the given MPI communicator
  //!
  //!
  //! \param comm The MPI communicator used to initialize OpenFOAM
  explicit FoamDriver(MPI_Comm comm, pugi::xml_node xml_root);


  void foam_init(MPI_Comm comm);


  ~FoamDriver();



  //! runs all timesteps for a heat/fluid solve in OpenFOAM
  //!
  void solve_step() final;



  //! whether the calling rank has access to the full thermal-hydraulic solution field.
  //! Only OpenFOAM's master rank has access to the global data; data on other ranks is empty
  bool has_coupling_data() const final {return comm_.rank==0; }

  //! Get the coordinate of a local element's centroid
  //!
  //! The coordinate is dimensionless. Its units depend on the unit system thst was
  //! to setp the OpenFOAM problem. The user must handle any necessary conversions.
  //!
  //! \param local_elem The local index of the desired element
  //! \return The dimensionless coordinate of the element's centroid
  Position centroid_at(int32_t local_elem) const;

  //! Get the volume of a local element
  //!
  //! The volume is dimensionless.  Its units depend on the unit system used that was used
  //! to setup the Nek problem. The user must handle any necessary conversions.
  //!
  //! \param local_elem The local index of the desired element
  //! \return The dimensionless Volume of the element
  double volume_at(int32_t local_elem) const;

  //! Get the volume-averaged temperature of a local element
  //!
  //! The returned temperature is dimensionless.  Its units depend on the unit system that
  //! was used to setup the Nek5000 problem. The user must handle any necessary
  //! conversions.
  //!
  //! \param local_elem A local element ID
  //! \return The volume-averaged temperature of the element
  double temperature_at(int32_t local_elem) const;

  //! Return true if a local element is in the fluid region
  //! \param local_elem A local element ID
  //! \return 1 if the local element is in fluid; 0 otherwise
  int in_fluid_at(int32_t local_elem) const;

  //! Set the heat source for a given local element
  //!
  //! The units of heat must match on the unit system that was used to setup the OpenFOAM
  //! problem (presumably !!Check Units!!). The caller must handle any necessary conversions.
  //!
  //! \param local_elem A local element ID
  //! \param heat A heat source term
  //! \return Error code
  int set_heat_source_at(int32_t local_elem, double heat) override;

  //! Get the number of local mesh elements
  //! \return Number of local mesh elements
  int n_local_elem() const;

  //! Get the number of global mesh elements
  //! \return Number of global mesh elements
  std::size_t n_global_elem() const;

private:
  //! Get temperautre of local mesh elements
  //! \return Temperature of local mesh elements in [K]
  std::vector<double> temperature_local() const override;

  //! Get density of local mesh elements
  //! \return Density of local mesh elements in [g/cm^3]
  std::vector<double> density_local() const override;

  //! States whether each local region is in fluid
  //! \return For each local region, 1 if region is in fluid and 0 otherwise
  std::vector<int> fluid_mask_local() const override;

  //! Get centroids of local mesh elements
  //! \return Centroids of local mesh elements
  std::vector<Position> centroid_local() const override;

  //! Get volumes on local mesh elements
  //! \return Volumes on local mesh elements
  std::vector<double> volume_local() const override;

  int32_t nelgt_;  //!< number of local mesh elements
  int32_t nelt_;  //!< number of local mesh elements
};

} // namespace enrico

#endif // ENRICO_FOAM_DRIVER_H