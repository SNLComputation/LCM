//*****************************************************************//
//    Albany 2.0:  Copyright 2012 Sandia Corporation               //
//    This Software is released under the BSD license detailed     //
//    in the file "license.txt" in the top-level Albany directory  //
//*****************************************************************//
#ifndef SCHWARZ_OBSERVERIMPL_HPP
#define SCHWARZ_OBSERVERIMPL_HPP

#include "Schwarz_StatelessObserverImpl.hpp"

namespace LCM {

class ObserverImpl : public StatelessObserverImpl {
public:
  explicit ObserverImpl(const Teuchos::RCP<Albany::Application>& app);

  virtual void observeSolutionT(
    double stamp, const Tpetra_Vector& nonOverlappedSolutionT,
    const Teuchos::Ptr<const Tpetra_Vector>& nonOverlappedSolutionDotT);

private:
  ObserverImpl(const ObserverImpl&);
  ObserverImpl& operator=(const ObserverImpl&);
};

} // namespace LCM

#endif // SCHWARZ_OBSERVERIMPL_HPP
