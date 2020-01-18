//*****************************************************************
// Albany 3.0: Copyright 2016 National Technology & Engineering Solutions of
// Sandia, LLC (NTESS). This Software is released under the BSD license detailed
// in the file license.txt in the top-level Albany directory
//*****************************************************************

#if !defined(PHAL_TimeDepSDBC_hpp)
#define PHAL_TimeDepSDBC_hpp

#include <vector>

#include "PHAL_SDirichlet.hpp"

namespace PHAL {

///
/// Time-dependent, strongly-enforced Dirichlet BC evauator
///
template <typename EvalT, typename Traits>
class TimeDepSDBC_Base : public PHAL::SDirichlet<EvalT, Traits>
{
 private:
  using ScalarT = typename EvalT::ScalarT;

 public:
  TimeDepSDBC_Base(Teuchos::ParameterList& p);

  ScalarT
  computeVal(RealType time);

 protected:
  int                   offset_{0};
  std::vector<RealType> times_;
  std::vector<RealType> values_;
};

template <typename EvalT, typename Traits>
class TimeDepSDBC : public TimeDepSDBC_Base<EvalT, Traits>
{
 public:
  TimeDepSDBC(Teuchos::ParameterList& p);

  void
  preEvaluate(typename Traits::EvalData d);

  void
  evaluateFields(typename Traits::EvalData d);

 private:
  using ScalarT = typename EvalT::ScalarT;
};

}  // namespace PHAL

#endif  // PHAL_TimeDepSDBC_hpp
