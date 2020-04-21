// Albany 3.0: Copyright 2016 National Technology & Engineering Solutions of
// Sandia, LLC (NTESS). This Software is released under the BSD license detailed
// in the file license.txt in the top-level Albany directory.

#ifndef TIMETRACBC_HPP
#define TIMETRACBC_HPP

#include "PHAL_Neumann.hpp"
#include "Teuchos_TwoDArray.hpp"

namespace LCM {

/** \brief Time dependent Neumann boundary condition evaluator

*/

template <typename EvalT, typename Traits>
class TimeTracBC_Base : public PHAL::Neumann<EvalT, Traits>
{
 public:
  using ScalarT     = typename EvalT::ScalarT;
  using MeshScalarT = typename EvalT::MeshScalarT;

  TimeTracBC_Base(Teuchos::ParameterList& p);

  void
  computeVal(RealType time);
  void
  computeCoordVal(RealType time);

 protected:
  std::vector<RealType>        timeValues;
  Teuchos::TwoDArray<RealType> BCValues;
};

template <typename EvalT, typename Traits>
class TimeTracBC : public TimeTracBC_Base<EvalT, Traits>
{
 public:
  TimeTracBC(Teuchos::ParameterList& p);
  void
  evaluateFields(typename Traits::EvalData d);

 private:
  using ScalarT = typename EvalT::ScalarT;
};

}  // namespace LCM

#endif
