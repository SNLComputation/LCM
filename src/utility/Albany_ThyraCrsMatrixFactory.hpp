#ifndef ALBANY_THYRA_CRS_MATRIX_FACTORY_HPP
#define ALBANY_THYRA_CRS_MATRIX_FACTORY_HPP

#include <set>

#include "Albany_ThyraTypes.hpp"
#include "Albany_TpetraThyraUtils.hpp"
#include "Teuchos_RCP.hpp"

namespace Albany {

/*
 * A class to setup a crs graph and then build an empty operator
 *
 * Thyra does not have the concept of 'Graph', since it is designed to abstract
 * at a mathematical level, in a world of vector spaces, operators, and vectors.
 * The concept of graph has to do with the particular implementation of an
 * operator, namely its storage. From the computational point of view it is an
 * important object, hence we need to have access to its functionalities. Since
 * we can't get it in Thyra, and we don't want to expose particular
 * implementations, we implementa a very light-weight
 * structure, that can create and setup a graph, and, upon request, create a
 * linear operator associated with that graph. The implementation details of the
 * graph are hidden, as is the concrete linear algebra package underneath.
 */

struct ThyraCrsMatrixFactory
{
  // Create an empty graph, that needs to be filled later
  ThyraCrsMatrixFactory(
      Teuchos::RCP<Thyra_VectorSpace const> const domain_vs,
      Teuchos::RCP<Thyra_VectorSpace const> const range_vs,
      int const                                   nonzeros_per_row = -1);  // currently not used

  // Create a graph from an overlapped one
  ThyraCrsMatrixFactory(
      Teuchos::RCP<Thyra_VectorSpace const> const     domain_vs,
      Teuchos::RCP<Thyra_VectorSpace const> const     range_vs,
      const Teuchos::RCP<const ThyraCrsMatrixFactory> overlap_src);

  // Inserts global indices in a temporary local graph.
  // Indices that are not owned by callig processor are ignored
  // The actual graph is created when FillComplete is called
  void
  insertGlobalIndices(const GO row, const Teuchos::ArrayView<const GO>& indices);

  // Creates the CrsGraph,
  // inserting indices from the temporary local graph,
  // and calls fillComplete.
  void
  fillComplete();

  Teuchos::RCP<Thyra_VectorSpace const>
  getDomainVectorSpace() const
  {
    return m_domain_vs;
  }
  Teuchos::RCP<Thyra_VectorSpace const>
  getRangeVectorSpace() const
  {
    return m_range_vs;
  }

  bool
  is_filled() const
  {
    return m_filled;
  }

  Teuchos::RCP<Thyra_LinearOp>
  createOp() const;

 private:
  // Struct hiding the concrete implementation. This is an implementation
  // detail of this class, so it's private and its implementation is not in the
  // header.
  struct Impl;
  Teuchos::RCP<Impl> m_graph;

  Teuchos::RCP<Thyra_VectorSpace const> m_domain_vs;
  Teuchos::RCP<Thyra_VectorSpace const> m_range_vs;

  std::vector<std::set<Tpetra_GO>> t_local_graph;
  Teuchos::RCP<const Tpetra_Map>   t_range;

  bool m_filled;
};

}  // namespace Albany

#endif  // ALBANY_THYRA_CRS_MATRIX_FACTORY_HPP
