// Albany 3.0: Copyright 2016 National Technology & Engineering Solutions of
// Sandia, LLC (NTESS). This Software is released under the BSD license detailed
// in the file license.txt in the top-level Albany directory.

#include <fstream>

#include "Albany_Macros.hpp"
#include "Albany_Utils.hpp"
#include "Phalanx_DataLayout.hpp"
#include "Sacado_ParameterRegistration.hpp"

namespace LCM {

template <typename EvalT, typename Traits>
ACEThermalConductivity<EvalT, Traits>::ACEThermalConductivity(Teuchos::ParameterList& p)
    : thermal_conductivity_(p.get<std::string>("QP Variable Name"),
          p.get<Teuchos::RCP<PHX::DataLayout>>("QP Scalar Data Layout"))
{
  Teuchos::ParameterList* cond_list =
      p.get<Teuchos::ParameterList*>("Parameter List");

  Teuchos::RCP<Teuchos::ParameterList const> reflist =
      this->getValidThermalCondParameters();

  // Check the parameters contained in the input file. Do not check the defaults
  // set programmatically
  cond_list->validateParameters(
      *reflist,
      0,
      Teuchos::VALIDATE_USED_ENABLED,
      Teuchos::VALIDATE_DEFAULTS_DISABLED);

  Teuchos::RCP<PHX::DataLayout> vector_dl =
      p.get<Teuchos::RCP<PHX::DataLayout>>("QP Vector Data Layout");
  coord_vec_ = decltype(coord_vec_)(p.get<std::string>("QP Coordinate Vector Name"), vector_dl);
  std::vector<PHX::DataLayout::size_type> dims;
  vector_dl->dimensions(dims);
  num_qps_  = dims[1];
  num_dims_ = dims[2];

  // We have a multiple material problem and need to map element blocks to
  // material data
 
  Teuchos::ArrayRCP<std::string> eb_names 
      = p.get<Teuchos::ArrayRCP<std::string>>("Element Block Names", {});

  if (p.isType<Teuchos::RCP<Albany::MaterialDatabase>>("MaterialDB")) {
    material_db_ = p.get<Teuchos::RCP<Albany::MaterialDatabase>>("MaterialDB");
  } 
  else {
    ALBANY_ABORT("\nError! Must specify a material database for thermal conductivity.\n"); 
  }

  this->addDependentField(coord_vec_);
  this->addEvaluatedField(thermal_conductivity_);
  this->setName("ACE Thermal Conductivity");
}


// **********************************************************************
template <typename EvalT, typename Traits>
void
ACEThermalConductivity<EvalT, Traits>::postRegistrationSetup(
    typename Traits::SetupData d,
    PHX::FieldManager<Traits>& fm)
{
  this->utils.setFieldData(thermal_conductivity_, fm);
  this->utils.setFieldData(coord_vec_, fm);
}

// **********************************************************************
template <typename EvalT, typename Traits>
void
ACEThermalConductivity<EvalT, Traits>::evaluateFields(
    typename Traits::EvalData workset)
{
  std::string eb_name = workset.EBName; 
  Teuchos::ParameterList& sublist 
	  = material_db_->getElementBlockSublist(eb_name, "ACE Thermal Conductivity"); 
  ScalarT value_eb  = sublist.get("ACE Thermal Conductivity Value", 1.0);
  for (std::size_t cell = 0; cell < workset.numCells; ++cell) {
    for (std::size_t qp = 0; qp < num_qps_; ++qp) {
      thermal_conductivity_(cell, qp) = value_eb; 
    }
  }
}

// **********************************************************************
template <typename EvalT, typename Traits>
typename ACEThermalConductivity<EvalT, Traits>::ScalarT&
ACEThermalConductivity<EvalT, Traits>::getValue(std::string const& n)
{
  ALBANY_ABORT("\nError! Logic error in getting parameter " << n
      << " in ACE Thermal Conductivity::getValue()!\n");
  return constant_value_;
}

// **********************************************************************
template <typename EvalT, typename Traits>
Teuchos::RCP<Teuchos::ParameterList const>
ACEThermalConductivity<EvalT, Traits>::getValidThermalCondParameters() const
{
  Teuchos::RCP<Teuchos::ParameterList> valid_pl =
      rcp(new Teuchos::ParameterList("Valid ACE Thermal Conductivity Params"));

  valid_pl->set<double>("ACE Thermal Conductivity Value", 1.0,
      "Constant thermal conductivity value across the entire domain");

  return valid_pl;
}

// **********************************************************************
// **********************************************************************
}  // namespace PHAL
