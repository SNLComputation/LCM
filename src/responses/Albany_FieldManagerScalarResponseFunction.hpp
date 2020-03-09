//
// Albany 3.0: Copyright 2016 National Technology & Engineering Solutions of
// Sandia, LLC (NTESS). This Software is released under the BSD license detailed
// in the file license.txt in the top-level Albany directory.
//

#ifndef ALBANY_FIELD_MANAGER_SCALAR_RESPONSE_FUNCTION_HPP
#define ALBANY_FIELD_MANAGER_SCALAR_RESPONSE_FUNCTION_HPP

#include "Albany_ScalarResponseFunction.hpp"
#include "Albany_StateInfoStruct.hpp"  // contains MeshSpecsStuct
#include "PHAL_AlbanyTraits.hpp"
#include "Phalanx_FieldManager.hpp"

namespace Albany {

class AbstractProblem;
class Application;
class StateManager;
struct MeshSpecsStruct;

/*!
 * \brief Reponse function representing the average of the solution values
 */
class FieldManagerScalarResponseFunction : public ScalarResponseFunction
{
 public:
  //! Constructor
  FieldManagerScalarResponseFunction(
      const Teuchos::RCP<Application>&     application,
      const Teuchos::RCP<AbstractProblem>& problem,
      const Teuchos::RCP<MeshSpecsStruct>& ms,
      const Teuchos::RCP<StateManager>&    stateMgr,
      Teuchos::ParameterList&              responseParams);

  //! Destructor
  ~FieldManagerScalarResponseFunction() = default;

  //! Get the number of responses
  unsigned int
  numResponses() const
  {
    return num_responses;
  }

  //! Perform post registration setup
  void
  postRegSetup();

  //! Evaluate responses
  void
  evaluateResponse(
      const double                            current_time,
      Teuchos::RCP<Thyra_Vector const> const& x,
      Teuchos::RCP<Thyra_Vector const> const& xdot,
      Teuchos::RCP<Thyra_Vector const> const& xdotdot,
      const Teuchos::Array<ParamVec>&         p,
      const Teuchos::RCP<Thyra_Vector>&       g);

  void
  evaluateGradient(
      const double                            current_time,
      Teuchos::RCP<Thyra_Vector const> const& x,
      Teuchos::RCP<Thyra_Vector const> const& xdot,
      Teuchos::RCP<Thyra_Vector const> const& xdotdot,
      const Teuchos::Array<ParamVec>&         p,
      ParamVec*                               deriv_p,
      const Teuchos::RCP<Thyra_Vector>&       g,
      const Teuchos::RCP<Thyra_MultiVector>&  dg_dx,
      const Teuchos::RCP<Thyra_MultiVector>&  dg_dxdot,
      const Teuchos::RCP<Thyra_MultiVector>&  dg_dxdotdot,
      const Teuchos::RCP<Thyra_MultiVector>&  dg_dp);

 protected:
  //! Constructor for derived classes
  /*!
   * Derived classes must call setup after using this constructor.
   */
  FieldManagerScalarResponseFunction(
      const Teuchos::RCP<Application>&     application,
      const Teuchos::RCP<AbstractProblem>& problem,
      const Teuchos::RCP<MeshSpecsStruct>& ms,
      const Teuchos::RCP<StateManager>&    stateMgr);

  //! Setup method for derived classes
  void
  setup(Teuchos::ParameterList& responseParams);

  // Do not hide base class setup method
  using ScalarResponseFunction::setup;

 protected:
  //! Application class
  Teuchos::RCP<Application> application;

  //! Problem class
  Teuchos::RCP<AbstractProblem> problem;

  //! Mesh specs
  Teuchos::RCP<MeshSpecsStruct> meshSpecs;

  //! State manager
  Teuchos::RCP<StateManager> stateMgr;

  //! Field manager for Responses
  Teuchos::RCP<PHX::FieldManager<PHAL::AlbanyTraits>> rfm;

  //! Number of responses we compute
  unsigned int num_responses;

  //! Visualize response graph
  int vis_response_graph;

  //! Response name for visualization file
  std::string vis_response_name;

 private:
  template <typename EvalT>
  void
  postRegDerivImpl();

  template <typename EvalT>
  void
  postRegImpl();

  template <typename EvalT>
  void
  postReg();

  template <typename EvalT>
  void
  writePhalanxGraph(const std::string& evalName);

  template <typename EvalT>
  void
  evaluate(PHAL::Workset& workset);

  //! Restrict the field manager to an element block, as is done for fm and
  //! sfm in Application.
  int element_block_index;

  bool performedPostRegSetup;
};

}  // namespace Albany

#endif  // ALBANY_FIELD_MANAGER_SCALAR_RESPONSE_FUNCTION_HPP
