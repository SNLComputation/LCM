//
// Albany 3.0: Copyright 2016 National Technology & Engineering Solutions of
// Sandia, LLC (NTESS). This Software is released under the BSD license detailed
// in the file license.txt in the top-level Albany directory.
//

#ifndef PHAL_GATHER_EIGEN_VECTORS_HPP
#define PHAL_GATHER_EIGEN_VECTORS_HPP

#include "Albany_Layouts.hpp"
#include "PHAL_Dimension.hpp"
#include "Phalanx_Evaluator_Derived.hpp"
#include "Phalanx_Evaluator_WithBaseImpl.hpp"
#include "Phalanx_MDField.hpp"
#include "Phalanx_config.hpp"
#include "Teuchos_ParameterList.hpp"

namespace PHAL {

template <typename EvalT, typename Traits>
class GatherEigenvectors : public PHX::EvaluatorWithBaseImpl<Traits>,
                           public PHX::EvaluatorDerived<EvalT, Traits>
{
 public:
  GatherEigenvectors(
      const Teuchos::ParameterList&        p,
      const Teuchos::RCP<Albany::Layouts>& dl);

  void
  postRegistrationSetup(
      typename Traits::SetupData d,
      PHX::FieldManager<Traits>& vm);

  void
  evaluateFields(typename Traits::EvalData d);

 protected:
  typedef typename EvalT::ScalarT                ScalarT;
  std::vector<PHX::MDField<ScalarT, Cell, Node>> eigenvector_Re;
  std::vector<PHX::MDField<ScalarT, Cell, Node>> eigenvector_Im;
  std::size_t                                    numNodes;
  std::size_t                                    nEigenvectors;
};

// **************************************************************
}  // namespace PHAL

#endif  // PHAL_GATHER_EIGEN_VECTORS_HPP
