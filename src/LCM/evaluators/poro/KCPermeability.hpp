//
// Albany 3.0: Copyright 2016 National Technology & Engineering Solutions of
// Sandia, LLC (NTESS). This Software is released under the BSD license detailed
// in the file license.txt in the top-level Albany directory.
//

#ifndef KC_PERMEABILITY_HPP
#define KC_PERMEABILITY_HPP

#include "Albany_Types.hpp"
#include "Albany_config.h"
#include "Phalanx_Evaluator_Derived.hpp"
#include "Phalanx_Evaluator_WithBaseImpl.hpp"
#include "Phalanx_MDField.hpp"
#include "Phalanx_config.hpp"
#include "Sacado_ParameterAccessor.hpp"
#include "Teuchos_Array.hpp"
#include "Teuchos_ParameterList.hpp"

namespace LCM {
/**
 * \brief Evaluates permeability based on porosity
 *  through the Kozeny-Carman relation, either as
 *  a constant or a truncated KL expansion.The permeability
 *  is divided by viscosity.
 */

template <typename EvalT, typename Traits>
class KCPermeability : public PHX::EvaluatorWithBaseImpl<Traits>,
                       public PHX::EvaluatorDerived<EvalT, Traits>,
                       public Sacado::ParameterAccessor<EvalT, SPL_Traits>
{
 public:
  using ScalarT     = typename EvalT::ScalarT;
  using MeshScalarT = typename EvalT::MeshScalarT;

  KCPermeability(Teuchos::ParameterList& p);

  void
  postRegistrationSetup(
      typename Traits::SetupData d,
      PHX::FieldManager<Traits>& vm);

  void
  evaluateFields(typename Traits::EvalData d);

  ScalarT&
  getValue(std::string const& n);

 private:
  int                                                   numQPs;
  int                                                   numDims;
  PHX::MDField<const MeshScalarT, Cell, QuadPoint, Dim> coordVec;
  PHX::MDField<ScalarT, Cell, QuadPoint>                kcPermeability;

  //! Is Kozeny-Carman Permeability constant, or random field
  bool is_constant;

  //! Constant value
  ScalarT constant_value;

  //! Optional dependence on porosity

  PHX::MDField<const ScalarT, Cell, QuadPoint> porosity;
  bool                                         isPoroElastic;

  //! Values of the random variables
  Teuchos::Array<ScalarT> rv;
};
}  // namespace LCM

#endif
