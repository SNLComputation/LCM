//
// Albany 3.0: Copyright 2016 National Technology & Engineering Solutions of
// Sandia, LLC (NTESS). This Software is released under the BSD license detailed
// in the file license.txt in the top-level Albany directory.
//

#ifndef AADAPT_INITIAL_CONDITION_HPP
#define AADAPT_INITIAL_CONDITION_HPP

#include <string>

#include "Albany_DataTypes.hpp"
#include "Albany_DiscretizationUtils.hpp"
#include "Teuchos_ParameterList.hpp"

namespace AAdapt {

void
InitialConditions(
    const Teuchos::RCP<Thyra_Vector>&     solnT,
    const Albany::Conn&                   wsElNodeEqID,
    const Teuchos::ArrayRCP<std::string>& wsEBNames,
    const Teuchos::ArrayRCP<Teuchos::ArrayRCP<Teuchos::ArrayRCP<double*>>>
                            coords,
    const int               neq,
    const int               numDim,
    Teuchos::ParameterList& icParams,
    const bool              gasRestartSolution = false);

}  // namespace AAdapt

#endif  // AADAPT_INITIAL_CONDITION_HPP
