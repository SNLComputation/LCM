// Albany 3.0: Copyright 2016 National Technology & Engineering Solutions of
// Sandia, LLC (NTESS). This Software is released under the BSD license detailed
// in the file license.txt in the top-level Albany directory.

#ifndef ACETHERMALCONDUCTIVITY_HPP
#define ACETHERMALCONDUCTIVITY_HPP

#include "Albany_MaterialDatabase.hpp"
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
 * \brief Evaluates thermal conductivity.

This class may be used in two ways.

1. The simplest is to use a constant thermal conductivity across the entire
domain (one element block, one material), say with a value of 5.0. In this case,
one would declare at the "Problem" level, that a constant thermal conductivity
was being used, and its value was 5.0:

<ParameterList name="Problem">
   ...
    <ParameterList name="ACE Thermal Conductivity">
       <Parameter name="ACE Thermal Conductivity Type" type="string"
value="Constant"/> <Parameter name="Value" type="double" value="5.0"/>
    </ParameterList>
</ParameterList>

2. The other extreme is to have a multiple element block problem, say 3, with
each element block corresponding to a material. Each element block has its own
field manager, and different evaluators are used in each element block. 

 */

template <typename EvalT, typename Traits>
class ACEThermalConductivity : public PHX::EvaluatorWithBaseImpl<Traits>,
                            public PHX::EvaluatorDerived<EvalT, Traits>,
                            public Sacado::ParameterAccessor<EvalT, SPL_Traits>
{
 public:
  typedef typename EvalT::ScalarT     ScalarT;
  typedef typename EvalT::MeshScalarT MeshScalarT;

  ACEThermalConductivity(Teuchos::ParameterList& p);

  void
  postRegistrationSetup(
      typename Traits::SetupData d,
      PHX::FieldManager<Traits>& vm);

  void
  evaluateFields(typename Traits::EvalData d);

  ScalarT&
  getValue(std::string const& n);

 private:
  //! Validate the name strings under "ACE Thermal Conductivity" section in xml input
  //! file,
  Teuchos::RCP<Teuchos::ParameterList const>
  getValidThermalCondParameters() const;

  std::size_t                                           num_qps_;
  std::size_t                                           num_dims_;
  PHX::MDField<const MeshScalarT, Cell, QuadPoint, Dim> coord_vec_;
  PHX::MDField<ScalarT, Cell, QuadPoint>                thermal_conductivity_;
  PHX::MDField<ScalarT, Cell, QuadPoint>                thermal_inertia_;

  //! Constant value
  ScalarT constant_value_{0.0};

  //! Material database - holds thermal conductivity among other quantities
  Teuchos::RCP<Albany::MaterialDatabase> material_db_;

};
}  // namespace LCM

#endif
