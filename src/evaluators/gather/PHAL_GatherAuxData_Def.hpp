//*****************************************************************//
//    Albany 3.0:  Copyright 2016 Sandia Corporation               //
//    This Software is released under the BSD license detailed     //
//    in the file "license.txt" in the top-level Albany directory  //
//*****************************************************************//

#include <string>
#include <vector>

#include "Phalanx_DataLayout.hpp"
#include "Teuchos_TestForException.hpp"

namespace PHAL {

template <typename EvalT, typename Traits>
GatherAuxData<EvalT, Traits>::GatherAuxData(
    const Teuchos::ParameterList&        p,
    const Teuchos::RCP<Albany::Layouts>& dl)
{
  std::string field_name = p.get<std::string>("Field Name");
  auxDataIndex           = p.get<int>("Aux Data Vector Index");

  PHX::MDField<ScalarT, Cell, Node> f(field_name, dl->node_scalar);
  vector_data = f;
  this->addEvaluatedField(vector_data);

  char buf[200];
  sprintf(
      buf, "Gather Aux Data %d to %s", (int)auxDataIndex, field_name.c_str());
  this->setName(buf);
}

// **********************************************************************
template <typename EvalT, typename Traits>
void
GatherAuxData<EvalT, Traits>::postRegistrationSetup(
    typename Traits::SetupData d,
    PHX::FieldManager<Traits>& fm)
{
  this->utils.setFieldData(vector_data, fm);
  numNodes = vector_data.extent(1);
}

// **********************************************************************

template <typename EvalT, typename Traits>
void
GatherAuxData<EvalT, Traits>::evaluateFields(typename Traits::EvalData workset)
{
  auto nodeID = workset.wsElNodeEqID;
    Teuchos::RCP<const Tpetra_Vector> vT =
        (workset.auxDataPtrT)->getVector(auxDataIndex);
    Teuchos::ArrayRCP<ST const> vT_constView = vT->get1dView();
    for (std::size_t cell = 0; cell < workset.numCells; ++cell) {
      for (std::size_t node = 0; node < this->numNodes; ++node) {
        int offsetIntoVec = nodeID(cell, node, 0);  // neq==1 hardwired
        this->vector_data(cell, node) = vT_constView[offsetIntoVec];
      }
    }
}

// **********************************************************************

}  // namespace PHAL
