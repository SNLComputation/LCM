// Albany 3.0: Copyright 2016 National Technology & Engineering Solutions of
// Sandia, LLC (NTESS). This Software is released under the BSD license detailed
// in the file license.txt in the top-level Albany directory.

#if !defined(HeliumODEs_hpp)
#define HeliumODEs_hpp

#include "Albany_Layouts.hpp"
#include "PHAL_AlbanyTraits.hpp"
#include "Phalanx_Evaluator_Derived.hpp"
#include "Phalanx_Evaluator_WithBaseImpl.hpp"
#include "Phalanx_MDField.hpp"
#include "Phalanx_config.hpp"

namespace LCM {
/// \brief
///
/// This evaluator integrates coupled ODEs for the
///   1. He concentration
///   2. Total bubble density
///   3. Bubble volume fraction
/// We employ implicit integration (backward Euler)
///
template <typename EvalT, typename Traits>
class HeliumODEs : public PHX::EvaluatorWithBaseImpl<Traits>, public PHX::EvaluatorDerived<EvalT, Traits>
{
 public:
  ///
  /// Constructor
  ///
  HeliumODEs(Teuchos::ParameterList& p, const Teuchos::RCP<Albany::Layouts>& dl);

  ///
  /// Phalanx method to allocate space
  ///
  void
  postRegistrationSetup(typename Traits::SetupData d, PHX::FieldManager<Traits>& vm);

  ///
  /// Implementation of physics
  ///
  void
  evaluateFields(typename Traits::EvalData d);

 private:
  using ScalarT     = typename EvalT::ScalarT;
  using MeshScalarT = typename EvalT::MeshScalarT;

  ///
  /// Input: total_concentration - addition of lattice and trapped
  ///        concentration
  ///
  PHX::MDField<ScalarT const, Cell, QuadPoint> total_concentration_;

  ///
  /// Input: time step
  ///
  PHX::MDField<ScalarT const, Dummy> delta_time_;

  ///
  /// Input: temperature dependent diffusion coefficient
  ///
  PHX::MDField<ScalarT const, Cell, QuadPoint> diffusion_coefficient_;

  ///
  /// Input: spatial dimension and number of integration points
  ///
  std::size_t num_dims_;
  std::size_t num_pts_;

  ///
  /// Output
  /// (1) Helium concentration
  /// (2) Total bubble density
  /// (3) Bubble volume fracture
  ///
  PHX::MDField<ScalarT, Cell, QuadPoint> he_concentration_;
  PHX::MDField<ScalarT, Cell, QuadPoint> total_bubble_density_;
  PHX::MDField<ScalarT, Cell, QuadPoint> bubble_volume_fraction_;

  ///
  /// Constants
  ///
  RealType avogadros_num_, omega_, t_decay_constant_, he_radius_, eta_;

  ///
  /// Scalar names for obtaining state old
  ///

  std::string total_concentration_name_;
  std::string he_concentration_name_;
  std::string total_bubble_density_name_;
  std::string bubble_volume_fraction_name_;
};
}  // namespace LCM

#endif
