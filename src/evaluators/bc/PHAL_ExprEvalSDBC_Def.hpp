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
#include "Albany_STKDiscretization.hpp"
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
    typename Traits::EvalData dbc_workset)
{
  auto const          dim = dbc_workset.spatial_dimension_;
  stk::expreval::Eval expr_eval(expression);
  expr_eval.parse();
  expr_eval.bindVariable("t", dbc_workset.current_time);
  auto rcp_disc = dbc_workset.disc;
  auto stk_disc = dynamic_cast<Albany::STKDiscretization*>(rcp_disc.get());
  auto x        = dbc_workset.x;
  auto x_view   = Teuchos::arcp_const_cast<ST>(Albany::getLocalData(x));
  auto const  has_nbi   = stk_disc->hasNodeBoundaryIndicator();
  auto const  ns_id     = this->nodeSetID;
  auto const& ns_nodes  = dbc_workset.nodeSets->find(ns_id)->second;
  auto const& ns_coords = dbc_workset.nodeSetCoords->find(ns_id)->second;

  for (auto ns_node = 0; ns_node < ns_nodes.size(); ns_node++) {
    if (has_nbi == true) {
      auto const& bi_field = stk_disc->getNodeBoundaryIndicator();
      auto const  ns_gids  = dbc_workset.nodeSetGIDs->find(ns_id)->second;
      auto const  gid      = ns_gids[ns_node] + 1;
      auto const  it       = bi_field.find(gid);
      if (it == bi_field.end()) continue;
      auto const nbi = *(it->second);
      if (nbi == 0.0) continue;
    }
    auto const dof = ns_nodes[ns_node][this->offset];
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
    typename Traits::EvalData dbc_workset)
{
  auto rcp_disc = dbc_workset.disc;
  auto stk_disc = dynamic_cast<Albany::STKDiscretization*>(rcp_disc.get());
  auto f        = dbc_workset.f;
  auto f_view   = Albany::getNonconstLocalData(f);
  auto const  has_nbi  = stk_disc->hasNodeBoundaryIndicator();
  auto const  ns_id    = this->nodeSetID;
  auto const& ns_nodes = dbc_workset.nodeSets->find(ns_id)->second;
  for (auto ns_node = 0; ns_node < ns_nodes.size(); ns_node++) {
    if (has_nbi == true) {
      auto const& bi_field = stk_disc->getNodeBoundaryIndicator();
      auto const  ns_gids  = dbc_workset.nodeSetGIDs->find(ns_id)->second;
      auto const  gid      = ns_gids[ns_node] + 1;
      auto const  it       = bi_field.find(gid);
      if (it == bi_field.end()) continue;
      auto const nbi = *(it->second);
      if (nbi == 0.0) continue;
    }
    auto const dof = ns_nodes[ns_node][this->offset];
    f_view[dof]    = 0.0;
    // Record DOFs to avoid setting Schwarz BCs on them.
    dbc_workset.fixed_dofs_.insert(dof);
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
    typename Traits::EvalData dbc_workset)
{
  auto rcp_disc = dbc_workset.disc;
  auto stk_disc = dynamic_cast<Albany::STKDiscretization*>(rcp_disc.get());
  auto J        = dbc_workset.Jac;
  auto range_vs = J->range();
  auto col_vs   = Albany::getColumnSpace(J);
  auto const  has_nbi   = stk_disc->hasNodeBoundaryIndicator();
  auto const  ns_id     = this->nodeSetID;
  auto const& ns_nodes  = dbc_workset.nodeSets->find(ns_id)->second;
  auto const  domain_vs = range_vs;  // we are assuming this!

  row_is_dbc_ = Thyra::createMember(range_vs);
  col_is_dbc_ = Thyra::createMember(col_vs);
  row_is_dbc_->assign(0.0);
  col_is_dbc_->assign(0.0);

  auto const& fixed_dofs      = dbc_workset.fixed_dofs_;
  auto        row_is_dbc_data = Albany::getNonconstLocalData(row_is_dbc_);
  if (dbc_workset.is_schwarz_bc_ == false) {  // regular SDBC
    for (auto ns_node = 0; ns_node < ns_nodes.size(); ++ns_node) {
      if (has_nbi == true) {
        auto const& bi_field = stk_disc->getNodeBoundaryIndicator();
        auto const  ns_gids  = dbc_workset.nodeSetGIDs->find(ns_id)->second;
        auto const  gid      = ns_gids[ns_node] + 1;
        auto const  it       = bi_field.find(gid);
        if (it == bi_field.end()) continue;
        auto const nbi = *(it->second);
        if (nbi == 0.0) continue;
      }
      auto dof             = ns_nodes[ns_node][this->offset];
      row_is_dbc_data[dof] = 1;
    }
  } else {  // special case for Schwarz SDBC
    auto const spatial_dimension = dbc_workset.spatial_dimension_;

    for (auto ns_node = 0; ns_node < ns_nodes.size(); ++ns_node) {
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
    typename Traits::EvalData dbc_workset)
{
  auto       x      = dbc_workset.x;
  auto       f      = dbc_workset.f;
  auto       J      = dbc_workset.Jac;
  auto const fill   = f != Teuchos::null;
  auto       f_view = fill ? Albany::getNonconstLocalData(f) : Teuchos::null;
  auto x_view = fill ? Teuchos::arcp_const_cast<ST>(Albany::getLocalData(x)) :
                       Teuchos::null;

  Teuchos::Array<GO> global_index(1);
  Teuchos::Array<LO> index(1);
  Teuchos::Array<ST> entry(1);
  Teuchos::Array<ST> entries;
  Teuchos::Array<LO> indices;

  auto const& fixed_dofs = dbc_workset.fixed_dofs_;

  this->set_row_and_col_is_dbc(dbc_workset);

  auto     col_is_dbc_data = Albany::getLocalData(col_is_dbc_.getConst());
  auto     range_spmd_vs   = Albany::getSpmdVectorSpace(J->range());
  LO const num_local_rows  = range_spmd_vs->localSubDim();

  for (LO local_row = 0; local_row < num_local_rows; ++local_row) {
    Albany::getLocalRowValues(J, local_row, indices, entries);

    auto row_is_dbc = col_is_dbc_data[local_row] > 0;

    if (row_is_dbc && fill == true) {
      f_view[local_row] = 0.0;
      x_view[local_row] = this->value.val();
    }

    LO const num_row_entries = entries.size();

    for (LO row_entry = 0; row_entry < num_row_entries; ++row_entry) {
      auto local_col         = indices[row_entry];
      auto is_diagonal_entry = local_col == local_row;
      if (is_diagonal_entry) { continue; }

      auto col_is_dbc = col_is_dbc_data[local_col] > 0;
      if (row_is_dbc || col_is_dbc) { entries[row_entry] = 0.0; }
    }
    Albany::setLocalRowValues(J, local_row, indices(), entries());
  }
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
    typename Traits::EvalData /* dbc_workset */)
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
    typename Traits::EvalData dbc_workset)
{
  return;
  Teuchos::RCP<Thyra_MultiVector> fpV = dbc_workset.fpV;

  bool trans    = dbc_workset.transpose_dist_param_deriv;
  int  num_cols = fpV->domain()->dim();

  const std::vector<std::vector<int>>& nsNodes =
      dbc_workset.nodeSets->find(this->nodeSetID)->second;

  if (trans) {
    // For (df/dp)^T*V we zero out corresponding entries in V
    Teuchos::RCP<Thyra_MultiVector>          Vp = dbc_workset.Vp_bc;
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
