//*****************************************************************
// Albany 3.0: Copyright 2016 National Technology & Engineering Solutions of
// Sandia, LLC (NTESS). This Software is released under the BSD license detailed
// in the file license.txt in the top-level Albany directory
//*****************************************************************
#ifndef ALBANY_OBSERVER_IMPL_HPP
#define ALBANY_OBSERVER_IMPL_HPP

#include <string>

#include "Albany_StatelessObserverImpl.hpp"

namespace Albany {

class ObserverImpl : public StatelessObserverImpl
{
 public:
  explicit ObserverImpl(const Teuchos::RCP<Application>& app);

  void
  observeSolution(
      double                                  stamp,
      const Thyra_Vector&                     nonOverlappedSolution,
      const Teuchos::Ptr<const Thyra_Vector>& nonOverlappedSolutionDot,
      const Teuchos::Ptr<const Thyra_Vector>& nonOverlappedSolutionDotDot)
      override;

  void
  observeSolution(double stamp, const Thyra_MultiVector& nonOverlappedSolution)
      override;

  void
  parameterChanged(const std::string& param);
};

}  // namespace Albany

#endif  // ALBANY_OBSERVER_IMPL_HPP
