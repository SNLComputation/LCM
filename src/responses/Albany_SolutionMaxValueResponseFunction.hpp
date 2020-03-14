//
// Albany 3.0: Copyright 2016 National Technology & Engineering Solutions of
// Sandia, LLC (NTESS). This Software is released under the BSD license detailed
// in the file license.txt in the top-level Albany directory.
//

#ifndef ALBANY_SOLUTION_MAX_VALUE_RESPONSE_FUNCTION_HPP
#define ALBANY_SOLUTION_MAX_VALUE_RESPONSE_FUNCTION_HPP

#include "Albany_SamplingBasedScalarResponseFunction.hpp"

namespace Albany {

/*!
 * \brief Reponse function representing the max of the solution values
 */
class SolutionMaxValueResponseFunction
    : public SamplingBasedScalarResponseFunction
{
 public:
  //! Default constructor
  SolutionMaxValueResponseFunction(
      const Teuchos::RCP<Teuchos_Comm const>& comm,
      int                                     neq                 = 1,
      int                                     eq                  = 0,
      bool                                    interleavedOrdering = true);

  //! Destructor
  ~SolutionMaxValueResponseFunction() = default;

  //! Get the number of responses
  unsigned int
  numResponses() const
  {
    return 1;
  }

  //! Evaluate responses
  void
  evaluateResponse(
      double const                            current_time,
      Teuchos::RCP<Thyra_Vector const> const& x,
      Teuchos::RCP<Thyra_Vector const> const& xdot,
      Teuchos::RCP<Thyra_Vector const> const& xdotdot,
      const Teuchos::Array<ParamVec>&         p,
      Teuchos::RCP<Thyra_Vector> const&       g);

  //! Evaluate gradient = dg/dx, dg/dxdot, dg/dp
  void
  evaluateGradient(
      double const                            current_time,
      Teuchos::RCP<Thyra_Vector const> const& x,
      Teuchos::RCP<Thyra_Vector const> const& xdot,
      Teuchos::RCP<Thyra_Vector const> const& xdotdot,
      const Teuchos::Array<ParamVec>&         p,
      ParamVec*                               deriv_p,
      Teuchos::RCP<Thyra_Vector> const&       g,
      const Teuchos::RCP<Thyra_MultiVector>&  dg_dx,
      const Teuchos::RCP<Thyra_MultiVector>&  dg_dxdot,
      const Teuchos::RCP<Thyra_MultiVector>&  dg_dxdotdot,
      const Teuchos::RCP<Thyra_MultiVector>&  dg_dp);

 protected:
  //! Number of equations per node
  int neq;

  //! Equation we want to get the max value from
  int eq;

  Teuchos::RCP<Teuchos_Comm const> comm_;

  //! Flag for interleaved verus blocked unknown ordering
  bool interleavedOrdering;

  //! Compute max value
  void
  computeMaxValue(Teuchos::RCP<Thyra_Vector const> const& x, ST& val);
};

}  // namespace Albany

#endif  // ALBANY_SOLUTION_MAX_VALUE_RESPONSE_FUNCTION_HPP
