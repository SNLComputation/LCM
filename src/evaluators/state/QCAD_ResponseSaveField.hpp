//
// Albany 3.0: Copyright 2016 National Technology & Engineering Solutions of
// Sandia, LLC (NTESS). This Software is released under the BSD license detailed
// in the file license.txt in the top-level Albany directory
//

#ifndef QCAD_RESPONSE_SAVE_FIELD_HPP
#define QCAD_RESPONSE_SAVE_FIELD_HPP

#include "Albany_ProblemUtils.hpp"
#include "Albany_StateManager.hpp"
#include "PHAL_Dimension.hpp"
#include "Phalanx_DataLayout.hpp"
#include "Phalanx_Evaluator_Derived.hpp"
#include "Phalanx_Evaluator_WithBaseImpl.hpp"
#include "Phalanx_MDField.hpp"
#include "Teuchos_ParameterList.hpp"

namespace QCAD {

class ResponseSaveFieldManager;

/**
 * \brief Response Description
 */
template <typename EvalT, typename Traits>
class ResponseSaveField : public PHX::EvaluatorWithBaseImpl<Traits>,
                          public PHX::EvaluatorDerived<EvalT, Traits>
{
 public:
  typedef typename EvalT::ScalarT     ScalarT;
  typedef typename EvalT::MeshScalarT MeshScalarT;

  ResponseSaveField(
      Teuchos::ParameterList&              p,
      const Teuchos::RCP<Albany::Layouts>& dl);

  void
  postRegistrationSetup(
      typename Traits::SetupData d,
      PHX::FieldManager<Traits>& vm);

  void
  preEvaluate(typename Traits::PreEvalData workset);

  void
  evaluateFields(typename Traits::EvalData d);

  void
  postEvaluate(typename Traits::PostEvalData workset);

  Teuchos::RCP<const PHX::FieldTag>
  getEvaluatedFieldTag() const
  {
    return response_field_tag;
  }

  Teuchos::RCP<const PHX::FieldTag>
  getResponseFieldTag() const
  {
    return response_field_tag;
  }

 private:
  Teuchos::RCP<const Teuchos::ParameterList>
  getValidResponseParameters() const;

  std::string fieldName;
  std::string stateName;

  std::size_t numNodes;
  std::size_t numQPs;
  std::size_t numDims;

  PHX::MDField<const MeshScalarT, Cell, QuadPoint> weights;
  PHX::MDField<const ScalarT>                      field;

  bool outputToExodus;
  bool outputCellAverage;
  bool memoryHolderOnly;
  bool isVectorField;

  std::string vectorOp;
  std::string fieldIndices;

  Albany::StateManager* pStateMgr;

  Teuchos::RCP<PHX::Tag<ScalarT>>        response_field_tag;
  Teuchos::RCP<ResponseSaveFieldManager> mgr_;
};

}  // namespace QCAD

#endif  // QCAD_RESPONSE_SAVE_FIELD_HPP
