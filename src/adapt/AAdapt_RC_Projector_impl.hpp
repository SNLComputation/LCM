//
// Albany 3.0: Copyright 2016 National Technology & Engineering Solutions of
// Sandia, LLC (NTESS). This Software is released under the BSD license detailed
// in the file license.txt in the top-level Albany directory
//

#ifndef AADAPT_RC_PROJECTOR_IMPL_HPP
#define AADAPT_RC_PROJECTOR_IMPL_HPP

#include "Albany_ThyraTypes.hpp"

namespace AAdapt {
namespace rc {

/*! \brief Implement details related to projection for rc::Manager.
 *
 *  For efficient rebuilding, separate out the solver-related code. Including
 *  Ifpack2 and Belos slows build time of a file, and I want
 *  AAdapt_RC_Manager.cpp to build quickly.
 */

//! Solve A x = b using preconditioner P. Construct P if it is null on input.
Teuchos::RCP<Thyra_MultiVector>
solve(
    const Teuchos::RCP<const Thyra_LinearOp>&    A,
    Teuchos::RCP<Thyra_LinearOp>&                P,
    const Teuchos::RCP<const Thyra_MultiVector>& b,
    Teuchos::ParameterList&                      belos_pl);

}  // namespace rc
}  // namespace AAdapt

#endif  // AADAPT_RC_PROJECTOR_IMPL_HPP
