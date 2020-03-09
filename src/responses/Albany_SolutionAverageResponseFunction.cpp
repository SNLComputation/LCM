//
// Albany 3.0: Copyright 2016 National Technology & Engineering Solutions of
// Sandia, LLC (NTESS). This Software is released under the BSD license detailed
// in the file license.txt in the top-level Albany directory.
//

#include "Albany_SolutionAverageResponseFunction.hpp"

#include "Albany_ThyraUtils.hpp"

namespace Albany {

SolutionAverageResponseFunction::SolutionAverageResponseFunction(
    const Teuchos::RCP<const Teuchos_Comm>& comm)
    : ScalarResponseFunction(comm)
{
  // Nothing to be done here
}

void
SolutionAverageResponseFunction::evaluateResponse(
    double const /*current_time*/,
    Teuchos::RCP<Thyra_Vector const> const& x,
    Teuchos::RCP<Thyra_Vector const> const& /*xdot*/,
    Teuchos::RCP<Thyra_Vector const> const& /*xdotdot*/,
    const Teuchos::Array<ParamVec>& /*p*/,
    const Teuchos::RCP<Thyra_Vector>& g)
{
  evaluateResponseImpl(*x, *g);
}

void
SolutionAverageResponseFunction::evaluateGradient(
    double const /* current_time */,
    Teuchos::RCP<Thyra_Vector const> const& x,
    Teuchos::RCP<Thyra_Vector const> const& /*xdot*/,
    Teuchos::RCP<Thyra_Vector const> const& /*xdotdot*/,
    const Teuchos::Array<ParamVec>& /* p */,
    ParamVec* /* deriv_p */,
    const Teuchos::RCP<Thyra_Vector>&      g,
    const Teuchos::RCP<Thyra_MultiVector>& dg_dx,
    const Teuchos::RCP<Thyra_MultiVector>& dg_dxdot,
    const Teuchos::RCP<Thyra_MultiVector>& dg_dxdotdot,
    const Teuchos::RCP<Thyra_MultiVector>& dg_dp)
{
  // Evaluate response g
  if (!g.is_null()) { evaluateResponseImpl(*x, *g); }

  // Evaluate dg/dx
  if (!dg_dx.is_null()) { dg_dx->assign(1.0 / x->space()->dim()); }

  // Evaluate dg/dxdot
  if (!dg_dxdot.is_null()) { dg_dxdot->assign(0.0); }

  // Evaluate dg/dxdotdot
  if (!dg_dxdotdot.is_null()) { dg_dxdotdot->assign(0.0); }

  // Evaluate dg/dp
  if (!dg_dp.is_null()) { dg_dp->assign(0.0); }
}

void
SolutionAverageResponseFunction::evaluateResponseImpl(
    Thyra_Vector const& x,
    Thyra_Vector&       g)
{
  if (one.is_null() || !sameAs(one->range(), x.range())) {
    one = Thyra::createMember(x.space());
    one->assign(1.0);
  }
  const ST mean = one->dot(x) / x.space()->dim();
  g.assign(mean);
}

}  // namespace Albany
