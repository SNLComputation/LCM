//
// Albany 3.0: Copyright 2016 National Technology & Engineering Solutions of
// Sandia, LLC (NTESS). This Software is released under the BSD license detailed
// in the file license.txt in the top-level Albany directory.
//

#include "Albany_KLResponseFunction.hpp"

#include "Teuchos_Array.hpp"
#include "Teuchos_VerboseObject.hpp"

namespace Albany {

KLResponseFunction::KLResponseFunction(
    const Teuchos::RCP<Albany::AbstractResponseFunction>& response_,
    Teuchos::ParameterList&                               responseParams)
    : response(response_),
      responseParams(responseParams),
      out(Teuchos::VerboseObjectBase::getDefaultOStream())
{
  num_kl = responseParams.get("Number of KL Terms", 5);
}

void
KLResponseFunction::evaluateResponse(
    const double                            current_time,
    Teuchos::RCP<Thyra_Vector const> const& x,
    Teuchos::RCP<Thyra_Vector const> const& xdot,
    Teuchos::RCP<Thyra_Vector const> const& xdotdot,
    const Teuchos::Array<ParamVec>&         p,
    const Teuchos::RCP<Thyra_Vector>&       g)
{
  response->evaluateResponse(current_time, x, xdot, xdotdot, p, g);
}

void
KLResponseFunction::evaluateTangent(
    const double                                 alpha,
    const double                                 beta,
    const double                                 omega,
    const double                                 current_time,
    bool                                         sum_derivs,
    Teuchos::RCP<Thyra_Vector const> const&      x,
    Teuchos::RCP<Thyra_Vector const> const&      xdot,
    Teuchos::RCP<Thyra_Vector const> const&      xdotdot,
    const Teuchos::Array<ParamVec>&              p,
    ParamVec*                                    deriv_p,
    const Teuchos::RCP<const Thyra_MultiVector>& Vx,
    const Teuchos::RCP<const Thyra_MultiVector>& Vxdot,
    const Teuchos::RCP<const Thyra_MultiVector>& Vxdotdot,
    const Teuchos::RCP<const Thyra_MultiVector>& Vp,
    const Teuchos::RCP<Thyra_Vector>&            g,
    const Teuchos::RCP<Thyra_MultiVector>&       gx,
    const Teuchos::RCP<Thyra_MultiVector>&       gp)
{
  response->evaluateTangent(
      alpha,
      beta,
      omega,
      current_time,
      sum_derivs,
      x,
      xdot,
      xdotdot,
      p,
      deriv_p,
      Vx,
      Vxdot,
      Vxdotdot,
      Vp,
      g,
      gx,
      gp);
}

//! Evaluate distributed parameter derivative dg/dp
void
KLResponseFunction::evaluateDistParamDeriv(
    const double                            current_time,
    Teuchos::RCP<Thyra_Vector const> const& x,
    Teuchos::RCP<Thyra_Vector const> const& xdot,
    Teuchos::RCP<Thyra_Vector const> const& xdotdot,
    const Teuchos::Array<ParamVec>&         param_array,
    const std::string&                      dist_param_name,
    const Teuchos::RCP<Thyra_MultiVector>&  dg_dp)
{
  response->evaluateDistParamDeriv(
      current_time, x, xdot, xdotdot, param_array, dist_param_name, dg_dp);
}

void
KLResponseFunction::evaluateDerivative(
    const double                                     current_time,
    Teuchos::RCP<Thyra_Vector const> const&          x,
    Teuchos::RCP<Thyra_Vector const> const&          xdot,
    Teuchos::RCP<Thyra_Vector const> const&          xdotdot,
    const Teuchos::Array<ParamVec>&                  p,
    ParamVec*                                        deriv_p,
    const Teuchos::RCP<Thyra_Vector>&                g,
    const Thyra::ModelEvaluatorBase::Derivative<ST>& dg_dx,
    const Thyra::ModelEvaluatorBase::Derivative<ST>& dg_dxdot,
    const Thyra::ModelEvaluatorBase::Derivative<ST>& dg_dxdotdot,
    const Thyra::ModelEvaluatorBase::Derivative<ST>& dg_dp)
{
  response->evaluateDerivative(
      current_time,
      x,
      xdot,
      xdotdot,
      p,
      deriv_p,
      g,
      dg_dx,
      dg_dxdot,
      dg_dxdotdot,
      dg_dp);
}

}  // namespace Albany
