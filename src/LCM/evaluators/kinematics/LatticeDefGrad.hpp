//
// Albany 3.0: Copyright 2016 National Technology & Engineering Solutions of
// Sandia, LLC (NTESS). This Software is released under the BSD license detailed
// in the file license.txt in the top-level Albany directory
//

#ifndef LATTICEDEFGRAD_HPP
#define LATTICEDEFGRAD_HPP

#include "Albany_Types.hpp"
#include "Intrepid2_CellTools.hpp"
#include "Intrepid2_Cubature.hpp"
#include "Phalanx_Evaluator_Derived.hpp"
#include "Phalanx_Evaluator_WithBaseImpl.hpp"
#include "Phalanx_MDField.hpp"
#include "Phalanx_config.hpp"

namespace LCM {
/** \brief Lattice Deformation Gradient

    This evaluator computes the hydrogen induced multiplicative decomposition
    of deformation gradient


*/

template <typename EvalT, typename Traits>
class LatticeDefGrad : public PHX::EvaluatorWithBaseImpl<Traits>,
                       public PHX::EvaluatorDerived<EvalT, Traits>
{
 public:
  LatticeDefGrad(const Teuchos::ParameterList& p);

  void
  postRegistrationSetup(
      typename Traits::SetupData d,
      PHX::FieldManager<Traits>& vm);

  void
  evaluateFields(typename Traits::EvalData d);

 private:
  using ScalarT     = typename EvalT::ScalarT;
  using MeshScalarT = typename EvalT::MeshScalarT;

  // Input:
  PHX::MDField<const ScalarT, Cell, QuadPoint, Dim, Dim> defgrad;
  PHX::MDField<const ScalarT, Cell, QuadPoint>           J;
  PHX::MDField<const ScalarT, Cell, QuadPoint> VH;  // partial molar volume
  PHX::MDField<const ScalarT, Cell, QuadPoint> VM;  // molar volume of Fe
  PHX::MDField<const ScalarT, Cell, QuadPoint>
      CtotalRef;  // stress free concentration
  PHX::MDField<const ScalarT, Cell, QuadPoint>
                                                   Ctotal;  // current total concentration
  PHX::MDField<const MeshScalarT, Cell, QuadPoint> weights;

  // Output:
  PHX::MDField<ScalarT, Cell, QuadPoint, Dim, Dim> latticeDefGrad;
  PHX::MDField<ScalarT, Cell, QuadPoint>           JH;

  unsigned int numQPs;
  unsigned int numDims;
  unsigned int worksetSize;

  //! flag to compute the weighted average of J
  bool weightedAverage;

  //! stabilization parameter for the weighted average
  ScalarT alpha;
};

}  // namespace LCM
#endif
