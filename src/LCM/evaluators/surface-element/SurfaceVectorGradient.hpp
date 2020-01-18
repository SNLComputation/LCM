//*****************************************************************
// Albany 3.0: Copyright 2016 National Technology & Engineering Solutions of
// Sandia, LLC (NTESS). This Software is released under the BSD license detailed
// in the file license.txt in the top-level Albany directory
//*****************************************************************

#ifndef SURFACEVECTORGRADIENT_HPP
#define SURFACEVECTORGRADIENT_HPP

#include "Albany_Layouts.hpp"
#include "Albany_Types.hpp"
#include "Intrepid2_CellTools.hpp"
#include "Intrepid2_Cubature.hpp"
#include "Phalanx_Evaluator_Derived.hpp"
#include "Phalanx_Evaluator_WithBaseImpl.hpp"
#include "Phalanx_MDField.hpp"
#include "Phalanx_config.hpp"

namespace LCM {
/** \brief

    Construct a deformation gradient on a surface

**/

template <typename EvalT, typename Traits>
class SurfaceVectorGradient : public PHX::EvaluatorWithBaseImpl<Traits>,
                              public PHX::EvaluatorDerived<EvalT, Traits>
{
 public:
  SurfaceVectorGradient(
      Teuchos::ParameterList&              p,
      const Teuchos::RCP<Albany::Layouts>& dl);

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
  //! Length scale parameter for localization zone
  ScalarT thickness;
  //! Numerical integration rule
  Teuchos::RCP<Intrepid2::Cubature<PHX::Device>> cubature;
  //! Vector to take the jump of
  PHX::MDField<const MeshScalarT, Cell, Vertex, Dim> vector;
  PHX::MDField<const ScalarT, Cell, QuadPoint, Dim>  jump;

  PHX::MDField<const ScalarT, Cell, QuadPoint, Dim, Dim>     currentBasis;
  PHX::MDField<const MeshScalarT, Cell, QuadPoint, Dim, Dim> refDualBasis;
  PHX::MDField<const MeshScalarT, Cell, QuadPoint, Dim>      refNormal;
  PHX::MDField<const MeshScalarT, Cell, QuadPoint>           weights;

  // Output:
  PHX::MDField<ScalarT, Cell, QuadPoint, Dim, Dim> defGrad;
  PHX::MDField<ScalarT, Cell, QuadPoint>           J;

  unsigned int worksetSize;
  unsigned int numNodes;
  unsigned int numQPs;
  unsigned int numDims;
  unsigned int numPlaneNodes;
  unsigned int numPlaneDims;

  //! flag to compute the weighted average of J
  bool weightedAverage;

  //! stabilization parameter for the weighted average
  ScalarT alpha;
};
}  // namespace LCM

#endif
