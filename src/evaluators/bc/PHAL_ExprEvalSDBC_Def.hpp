//
// Albany 3.0: Copyright 2016 National Technology & Engineering Solutions of
// Sandia, LLC (NTESS). This Software is released under the BSD license detailed
// in the file license.txt in the top-level Albany directory.
//

#ifndef PHAL_EXPREVALSDBC_DEF_HPP
#define PHAL_EXPREVALSDBC_DEF_HPP

#include <stk_expreval/Evaluator.hpp>

#include "Albany_CombineAndScatterManager.hpp"
#include "Albany_Macros.hpp"
#include "Albany_ThyraUtils.hpp"
#include "PHAL_ExprEvalSDBC.hpp"
#include "Phalanx_DataLayout.hpp"
#include "Sacado_ParameterRegistration.hpp"

namespace PHAL {

//
// Specialization: Residual
//
template <typename Traits>
ExprEvalSDBC<PHAL::AlbanyTraits::Residual, Traits>::ExprEvalSDBC(
    Teuchos::ParameterList& p)
    : PHAL::DirichletBase<PHAL::AlbanyTraits::Residual, Traits>(p)
{
  expression = p.get<std::string>("Dirichlet Expression");
}

//
//
//
template <typename Traits>
void
ExprEvalSDBC<PHAL::AlbanyTraits::Residual, Traits>::preEvaluate(
    typename Traits::EvalData dirichlet_workset)
{
  auto const          dim = dirichlet_workset.spatial_dimension_;
  stk::expreval::Eval expr_eval(expression);
  expr_eval.parse();
  expr_eval.bindVariable("t", dirichlet_workset.current_time);

  Teuchos::RCP<Thyra_Vector const> x = dirichlet_workset.x;
  Teuchos::ArrayRCP<ST>            x_view =
      Teuchos::arcp_const_cast<ST>(Albany::getLocalData(x));
  // Grab the vector of node GIDs for this Node Set ID
  auto const& ns_nodes =
      dirichlet_workset.nodeSets->find(this->nodeSetID)->second;

  auto const& ns_coords =
      dirichlet_workset.nodeSetCoords->find(this->nodeSetID)->second;

  for (size_t ns_node = 0; ns_node < ns_nodes.size(); ns_node++) {
    int const dof = ns_nodes[ns_node][this->offset];
    if (dim > 0) expr_eval.bindVariable("x", ns_coords[ns_node][0]);
    if (dim > 1) expr_eval.bindVariable("y", ns_coords[ns_node][1]);
    if (dim > 2) expr_eval.bindVariable("z", ns_coords[ns_node][2]);
    x_view[dof] = this->value = expr_eval.evaluate();
  }
}

//
//
//
template <typename Traits>
void
ExprEvalSDBC<PHAL::AlbanyTraits::Residual, Traits>::evaluateFields(
    typename Traits::EvalData dirichlet_workset)
{
  Teuchos::RCP<Thyra_Vector> f      = dirichlet_workset.f;
  Teuchos::ArrayRCP<ST>      f_view = Albany::getNonconstLocalData(f);

  // Grab the vector of node GIDs for this Node Set ID
  std::vector<std::vector<int>> const& ns_nodes =
      dirichlet_workset.nodeSets->find(this->nodeSetID)->second;

  for (size_t ns_node = 0; ns_node < ns_nodes.size(); ns_node++) {
    int const dof = ns_nodes[ns_node][this->offset];
    f_view[dof]   = 0.0;
    // Record DOFs to avoid setting Schwarz BCs on them.
    dirichlet_workset.fixed_dofs_.insert(dof);
  }
}

//
// Specialization: Jacobian
//
template <typename Traits>
ExprEvalSDBC<PHAL::AlbanyTraits::Jacobian, Traits>::ExprEvalSDBC(
    Teuchos::ParameterList& p)
    : PHAL::DirichletBase<PHAL::AlbanyTraits::Jacobian, Traits>(p)
{
}

//
//
//
template <typename Traits>
void
ExprEvalSDBC<PHAL::AlbanyTraits::Jacobian, Traits>::set_row_and_col_is_dbc(
    typename Traits::EvalData dirichlet_workset)
{
  Teuchos::RCP<const Thyra_LinearOp> J = dirichlet_workset.Jac;

  auto  range_vs  = J->range();
  auto  col_vs    = Albany::getColumnSpace(J);
  auto& ns_nodes  = dirichlet_workset.nodeSets->find(this->nodeSetID)->second;
  auto  domain_vs = range_vs;  // we are assuming this!

  row_is_dbc_ = Thyra::createMember(range_vs);
  col_is_dbc_ = Thyra::createMember(col_vs);
  row_is_dbc_->assign(0.0);
  col_is_dbc_->assign(0.0);

  auto const& fixed_dofs      = dirichlet_workset.fixed_dofs_;
  auto        row_is_dbc_data = Albany::getNonconstLocalData(row_is_dbc_);
  if (dirichlet_workset.is_schwarz_bc_ == false) {  // regular SDBC
    for (size_t ns_node = 0; ns_node < ns_nodes.size(); ns_node++) {
      auto dof             = ns_nodes[ns_node][this->offset];
      row_is_dbc_data[dof] = 1;
    }
  } else {  // special case for Schwarz SDBC
    int const spatial_dimension = dirichlet_workset.spatial_dimension_;

    for (size_t ns_node = 0; ns_node < ns_nodes.size(); ns_node++) {
      for (int offset = 0; offset < spatial_dimension; ++offset) {
        auto dof = ns_nodes[ns_node][offset];
        // If this DOF already has a DBC, skip it.
        if (fixed_dofs.find(dof) != fixed_dofs.end()) continue;
        row_is_dbc_data[dof] = 1;
      }
    }
  }
  auto cas_manager = Albany::createCombineAndScatterManager(domain_vs, col_vs);
  cas_manager->scatter(row_is_dbc_, col_is_dbc_, Albany::CombineMode::INSERT);
}

//
//
//
template <typename Traits>
void
ExprEvalSDBC<PHAL::AlbanyTraits::Jacobian, Traits>::evaluateFields(
    typename Traits::EvalData dirichlet_workset)
{
  Teuchos::RCP<const Thyra_Vector> x = dirichlet_workset.x;
  Teuchos::RCP<Thyra_Vector>       f = dirichlet_workset.f;
  Teuchos::RCP<Thyra_LinearOp>     J = dirichlet_workset.Jac;

  bool const fill_residual = f != Teuchos::null;

  auto f_view = fill_residual ? Albany::getNonconstLocalData(f) : Teuchos::null;
  auto x_view = fill_residual ?
                    Teuchos::arcp_const_cast<ST>(Albany::getLocalData(x)) :
                    Teuchos::null;

  Teuchos::Array<GO> global_index(1);
  Teuchos::Array<LO> index(1);
  Teuchos::Array<ST> entry(1);
  Teuchos::Array<ST> entries;
  Teuchos::Array<LO> indices;

  auto const& fixed_dofs = dirichlet_workset.fixed_dofs_;

  this->set_row_and_col_is_dbc(dirichlet_workset);

  auto     col_is_dbc_data = Albany::getLocalData(col_is_dbc_.getConst());
  auto     range_spmd_vs   = Albany::getSpmdVectorSpace(J->range());
  const LO num_local_rows  = range_spmd_vs->localSubDim();

  for (LO local_row = 0; local_row < num_local_rows; ++local_row) {
    Albany::getLocalRowValues(J, local_row, indices, entries);

    auto row_is_dbc = col_is_dbc_data[local_row] > 0;

    if (row_is_dbc && fill_residual == true) {
      f_view[local_row] = 0.0;
      x_view[local_row] = this->value.val();
    }

    const LO num_row_entries = entries.size();

    for (LO row_entry = 0; row_entry < num_row_entries; ++row_entry) {
      auto local_col         = indices[row_entry];
      auto is_diagonal_entry = local_col == local_row;
      if (is_diagonal_entry) { continue; }

      auto col_is_dbc = col_is_dbc_data[local_col] > 0;
      if (row_is_dbc || col_is_dbc) { entries[row_entry] = 0.0; }
    }
    Albany::setLocalRowValues(J, local_row, indices(), entries());
  }
  return;
}

//
// Specialization: Tangent
//
template <typename Traits>
ExprEvalSDBC<PHAL::AlbanyTraits::Tangent, Traits>::ExprEvalSDBC(
    Teuchos::ParameterList& p)
    : PHAL::DirichletBase<PHAL::AlbanyTraits::Tangent, Traits>(p)
{
}

//
//
//

template <typename Traits>
void ExprEvalSDBC<PHAL::AlbanyTraits::Tangent, Traits>::evaluateFields(
    typename Traits::EvalData /* dirichlet_workset */)
{
}

//
// Specialization: DistParamDeriv
//
template <typename Traits>
ExprEvalSDBC<PHAL::AlbanyTraits::DistParamDeriv, Traits>::ExprEvalSDBC(
    Teuchos::ParameterList& p)
    : PHAL::DirichletBase<PHAL::AlbanyTraits::DistParamDeriv, Traits>(p)
{
}

//
//
//
template <typename Traits>
void
ExprEvalSDBC<PHAL::AlbanyTraits::DistParamDeriv, Traits>::evaluateFields(
    typename Traits::EvalData dirichlet_workset)
{
  return;
  Teuchos::RCP<Thyra_MultiVector> fpV = dirichlet_workset.fpV;

  bool trans    = dirichlet_workset.transpose_dist_param_deriv;
  int  num_cols = fpV->domain()->dim();

  const std::vector<std::vector<int>>& nsNodes =
      dirichlet_workset.nodeSets->find(this->nodeSetID)->second;

  if (trans) {
    // For (df/dp)^T*V we zero out corresponding entries in V
    Teuchos::RCP<Thyra_MultiVector>          Vp = dirichlet_workset.Vp_bc;
    Teuchos::ArrayRCP<Teuchos::ArrayRCP<ST>> Vp_nonconst2dView =
        Albany::getNonconstLocalData(Vp);

    for (unsigned int inode = 0; inode < nsNodes.size(); inode++) {
      int lunk = nsNodes[inode][this->offset];

      for (int col = 0; col < num_cols; ++col) {
        Vp_nonconst2dView[col][lunk] = 0.0;
      }
    }
  } else {
    // for (df/dp)*V we zero out corresponding entries in df/dp
    Teuchos::ArrayRCP<Teuchos::ArrayRCP<ST>> fpV_nonconst2dView =
        Albany::getNonconstLocalData(fpV);
    for (unsigned int inode = 0; inode < nsNodes.size(); inode++) {
      int lunk = nsNodes[inode][this->offset];

      for (int col = 0; col < num_cols; ++col) {
        fpV_nonconst2dView[col][lunk] = 0.0;
      }
    }
  }
}

}  // namespace PHAL

#endif  // PHAL_EXPREVALSDBC_DEF_HPP
