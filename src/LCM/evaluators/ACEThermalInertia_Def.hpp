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
ACEThermalInertia<EvalT, Traits>::ACEThermalInertia(Teuchos::ParameterList& p)
    : thermal_inertia_(p.get<std::string>("QP Variable Name"),
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
  std::vector<PHX::DataLayout::size_type> dims;
  vector_dl->dimensions(dims);
  num_qps_  = dims[1];
  num_dims_ = dims[2];

  std::string eb_name = p.get<std::string>("Element Block Name", "Missing");

  type_ = cond_list->get("ACEThermalInertia Type", "Constant");
  if (type_ == "Constant") {
    ScalarT value = cond_list->get("Value", 1.0);
    init_constant(value, p);

  }

  else if (type_ == "Block Dependent") {
    // We have a multiple material problem and need to map element blocks to
    // material data

    if (p.isType<Teuchos::RCP<Albany::MaterialDatabase>>("MaterialDB")) {
      material_db_ = p.get<Teuchos::RCP<Albany::MaterialDatabase>>("MaterialDB");
    } else {
      ALBANY_ABORT(
          std::endl
          << "Error! Must specify a material database if using block "
             "dependent "
          << "thermal inertia" << std::endl);
    }

    // Get the sublist for thermal inertia for the element block in the mat
    // DB (the material in the elem block eb_name.

    Teuchos::ParameterList& subList =
        material_db_->getElementBlockSublist(eb_name, "ACE Thermal Inertia");

    std::string typ = subList.get("ACE Thermal Inertia Type", "Constant");

    if (typ == "Constant") {
      ScalarT value = subList.get("Value", 1.0);
      std::cout << "IKT eb_name, thermal inertia value = " << eb_name << ", " << value << "\n"; 
      init_constant(value, p);
    }
  }  // Block dependent

  else {
    ALBANY_ABORT("ACEThermalInertia: Invalid thermal inertia type " << type_ << "!  Valid options are currently 'Constant' and 'Block Depdendent'.");
  }

  this->addEvaluatedField(thermal_inertia_);
  this->setName("ACE Thermal Inertia");
}

template <typename EvalT, typename Traits>
void
ACEThermalInertia<EvalT, Traits>::init_constant(
    ScalarT                 value,
    Teuchos::ParameterList& p)
{
  is_constant = true;

  constant_value_ = value;

  // Add thermal inertia as a Sacado-ized parameter
  Teuchos::RCP<ParamLib> param_lib =
      p.get<Teuchos::RCP<ParamLib>>("Parameter Library", Teuchos::null);

  this->registerSacadoParameter("ACE Thermal Inertia", param_lib);

}  // init_constant

// **********************************************************************
template <typename EvalT, typename Traits>
void
ACEThermalInertia<EvalT, Traits>::postRegistrationSetup(
    typename Traits::SetupData d,
    PHX::FieldManager<Traits>& fm)
{
  this->utils.setFieldData(thermal_inertia_, fm);
  if (!is_constant) this->utils.setFieldData(coord_vec_, fm);
}

// **********************************************************************
template <typename EvalT, typename Traits>
void
ACEThermalInertia<EvalT, Traits>::evaluateFields(
    typename Traits::EvalData workset)
{
  if (is_constant) {
    for (std::size_t cell = 0; cell < workset.numCells; ++cell) {
      for (std::size_t qp = 0; qp < num_qps_; ++qp) {
        thermal_inertia_(cell, qp) = constant_value_;
      }
    }
  }
}

// **********************************************************************
template <typename EvalT, typename Traits>
typename ACEThermalInertia<EvalT, Traits>::ScalarT&
ACEThermalInertia<EvalT, Traits>::getValue(std::string const& n)
{
  if (is_constant) { return constant_value_; }
  ALBANY_ABORT(
      std::endl
      << "Error! Logic error in getting parameter " << n
      << " in ACEThermalInertia::getValue()" << std::endl);
  return constant_value_;
}

// **********************************************************************
template <typename EvalT, typename Traits>
Teuchos::RCP<Teuchos::ParameterList const>
ACEThermalInertia<EvalT, Traits>::getValidThermalCondParameters() const
{
  Teuchos::RCP<Teuchos::ParameterList> valid_pl =
      rcp(new Teuchos::ParameterList("Valid ACE Thermal Inertia Params"));
  ;

  valid_pl->set<std::string>("ACE Thermal Inertia Type", "Constant",
      "Constant thermal inertia across the entire domain");
  valid_pl->set<double>("Value", 1.0, "Constant thermal inertia value");

  return valid_pl;
}

// **********************************************************************
// **********************************************************************
}  // namespace PHAL
