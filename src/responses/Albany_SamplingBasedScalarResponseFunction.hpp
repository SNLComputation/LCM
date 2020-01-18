//
// Albany 3.0: Copyright 2016 National Technology & Engineering Solutions of
// Sandia, LLC (NTESS). This Software is released under the BSD license detailed
// in the file license.txt in the top-level Albany directory.
//

#ifndef ALBANY_SAMPLING_BASED_SCALAR_RESPONSE_FUNCTION_HPP
#define ALBANY_SAMPLING_BASED_SCALAR_RESPONSE_FUNCTION_HPP

#include "Albany_ScalarResponseFunction.hpp"

namespace Albany {

/*!
 * \brief Response function implementation for SG and MP functions using
 * a sampling-based scheme
 */
class SamplingBasedScalarResponseFunction : public ScalarResponseFunction
{
 public:
  //! Default constructor
  SamplingBasedScalarResponseFunction(
      const Teuchos::RCP<const Teuchos_Comm>& commT)
      : ScalarResponseFunction(commT)
  {
    // Nothing to be done here
  }

  //! Destructor
  virtual ~SamplingBasedScalarResponseFunction() = default;
};

}  // namespace Albany

#endif  // ALBANY_SAMPLING_BASED_SCALAR_RESPONSE_FUNCTION_HPP
