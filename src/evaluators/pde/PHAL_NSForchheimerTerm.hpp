// Albany 3.0: Copyright 2016 National Technology & Engineering Solutions of
// Sandia, LLC (NTESS). This Software is released under the BSD license detailed
// in the file license.txt in the top-level Albany directory.

#ifndef PHAL_NSFORCHHEIMERTERM_HPP
#define PHAL_NSFORCHHEIMERTERM_HPP

#include "PHAL_Dimension.hpp"
#include "Phalanx_Evaluator_Derived.hpp"
#include "Phalanx_Evaluator_WithBaseImpl.hpp"
#include "Phalanx_MDField.hpp"
#include "Phalanx_config.hpp"

/** \brief Finite Element Interpolation Evaluator

    This evaluator interpolates nodal DOF values to quad points.

*/
namespace PHAL {

template <typename EvalT, typename Traits>
class NSForchheimerTerm : public PHX::EvaluatorWithBaseImpl<Traits>,
                          public PHX::EvaluatorDerived<EvalT, Traits>
{
 public:
  typedef typename EvalT::ScalarT ScalarT;

  NSForchheimerTerm(Teuchos::ParameterList const& p);

  void
  postRegistrationSetup(
      typename Traits::SetupData d,
      PHX::FieldManager<Traits>& vm);

  void
  evaluateFields(typename Traits::EvalData d);

 private:
  typedef typename EvalT::MeshScalarT MeshScalarT;

  // Input:
  PHX::MDField<ScalarT const, Cell, QuadPoint, Dim> V;
  PHX::MDField<ScalarT const, Cell, QuadPoint>      rho;
  PHX::MDField<ScalarT const, Cell, QuadPoint>      phi;
  PHX::MDField<ScalarT const, Cell, QuadPoint>      K;
  PHX::MDField<ScalarT const, Cell, QuadPoint>      F;

  // Output:
  PHX::MDField<ScalarT, Cell, QuadPoint, Dim> ForchTerm;

  unsigned int numQPs, numDims, numNodes, numCells;
  bool         enableTransient;
  bool         haveHeat;

  Kokkos::DynRankView<ScalarT, PHX::Device> normV;
};
}  // namespace PHAL

#endif
