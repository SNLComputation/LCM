//*****************************************************************//
//    Albany 3.0:  Copyright 2016 Sandia Corporation               //
//    This Software is released under the BSD license detailed     //
//    in the file "license.txt" in the top-level Albany directory  //
//*****************************************************************//

#include <fstream>
#include "Albany_Utils.hpp"
#include "Phalanx_DataLayout.hpp"
#include "Teuchos_TestForException.hpp"

namespace LCM {

template <typename EvalT, typename Traits>
ACEheatCapacity<EvalT, Traits>::ACEheatCapacity(Teuchos::ParameterList& p)
    : heat_capacity_(
          p.get<std::string>("QP Variable Name"),
          p.get<Teuchos::RCP<PHX::DataLayout>>("QP Scalar Data Layout"))
{
  Teuchos::ParameterList* heat_capacity_list =
    p.get<Teuchos::ParameterList*>("Parameter List");

  Teuchos::RCP<PHX::DataLayout> vector_dl =
    p.get< Teuchos::RCP<PHX::DataLayout>>("QP Vector Data Layout");
  std::vector<PHX::DataLayout::size_type> dims;
  vector_dl->dimensions(dims);
  num_qps_  = dims[1];
  num_dims_ = dims[2];

  Teuchos::RCP<ParamLib> paramLib =
    p.get< Teuchos::RCP<ParamLib>>("Parameter Library", Teuchos::null);

  std::string type = heat_capacity_list->get("ACE Heat Capacity Type", "Constant");
  if (type == "Constant") {
    is_constant_ = true;
    constant_value_ = heat_capacity_list->get<double>("Value");

    // Add heat capacity as a Sacado-ized parameter
    this->registerSacadoParameter("ACE Heat Capacity", paramLib);
  }
  else {
    TEUCHOS_TEST_FOR_EXCEPTION(true, Teuchos::Exceptions::InvalidParameter,
             "Invalid ACE heat capacity type " << type);
  }

  this->addEvaluatedField(heat_capacity_);
  this->setName("ACE Heat Capacity" + PHX::typeAsString<EvalT>());
}

//
template <typename EvalT, typename Traits>
void
ACEheatCapacity<EvalT, Traits>::postRegistrationSetup(
    typename Traits::SetupData d,
    PHX::FieldManager<Traits>& fm)
{
  this->utils.setFieldData(heat_capacity_, fm);
  return;
}

//
template <typename EvalT, typename Traits>
void
ACEheatCapacity<EvalT, Traits>::evaluateFields(typename Traits::EvalData workset)
{
  int num_cells = workset.numCells;

  if (is_constant_ == true) {
    for (int cell = 0; cell < num_cells; ++cell) {
      for (int qp = 0; qp < num_qps_; ++qp) {
        heat_capacity_(cell, qp) = constant_value_;
      }
    }
  }

  return;
}

//
template <typename EvalT, typename Traits>
typename ACEheatCapacity<EvalT, Traits>::ScalarT&
ACEheatCapacity<EvalT, Traits>::getValue(const std::string& n)
{
  if (n == "ACE Heat Capacity") {
    return constant_value_;
  }

  ALBANY_ASSERT(false, "Invalid request for value of ACE Heat Capacity");

  return constant_value_;
}

}  // namespace LCM
