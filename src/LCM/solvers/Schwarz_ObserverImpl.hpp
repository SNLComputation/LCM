// Albany 3.0: Copyright 2016 National Technology & Engineering Solutions of
// Sandia, LLC (NTESS). This Software is released under the BSD license detailed
// in the file license.txt in the top-level Albany directory.
#if !defined(LCM_Schwarz_ObserverImpl_hpp)
#define LCM_Schwarz_ObserverImpl_hpp

#include "Schwarz_StatelessObserverImpl.hpp"

namespace LCM {

class ObserverImpl : public StatelessObserverImpl
{
 public:
  explicit ObserverImpl(Teuchos::ArrayRCP<Teuchos::RCP<Albany::Application>>& apps);

  virtual void
  observeSolution(
      double                                           stamp,
      Teuchos::Array<Teuchos::RCP<Thyra_Vector const>> non_overlapped_solution,
      Teuchos::Array<Teuchos::RCP<Thyra_Vector const>> non_overlapped_solution_dot);

  virtual ~ObserverImpl();

  ObserverImpl(ObserverImpl const&) = delete;
  ObserverImpl&
  operator=(ObserverImpl const&) = delete;
};

}  // namespace LCM

#endif  // LCM_Schwarz_ObserverImpl_hpp
