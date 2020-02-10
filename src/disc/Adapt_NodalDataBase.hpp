//
// Albany 3.0: Copyright 2016 National Technology & Engineering Solutions of
// Sandia, LLC (NTESS). This Software is released under the BSD license detailed
// in the file license.txt in the top-level Albany directory.
//

#ifndef ADAPT_NODAL_DATA_BASE_HPP
#define ADAPT_NODAL_DATA_BASE_HPP

#include "Adapt_NodalFieldUtils.hpp"
#include "Albany_AbstractNodeFieldContainer.hpp"
#include "Albany_CommTypes.hpp"
#include "Albany_Macros.hpp"
#include "Albany_ThyraCrsMatrixFactory.hpp"
#include "Albany_ThyraTypes.hpp"
#include "Teuchos_RCP.hpp"

namespace Adapt {

class NodalDataVector;

/*!
 * \brief This is a container class that deals with managing data values at the
 * nodes of a mesh.
 */
class NodalDataBase
{
 public:
  NodalDataBase();

  virtual ~NodalDataBase() = default;

  Teuchos::RCP<Albany::NodeFieldContainer>
  getNodeContainer()
  {
    return nodeContainer;
  }

  void
  updateNodalGraph(
      const Teuchos::RCP<const Albany::ThyraCrsMatrixFactory>& nGraph);

  const Teuchos::RCP<const Albany::ThyraCrsMatrixFactory>&
  getNodalOpFactory() const
  {
    return nodalOpFactory;
  }

  void
  replaceOwnedVectorSpace(const Teuchos::RCP<const Thyra_VectorSpace>& vs);

  void
  replaceOverlapVectorSpace(const Teuchos::RCP<const Thyra_VectorSpace>& vs);

  void
  replaceOwnedVectorSpace(
      const Teuchos::Array<GO>&               local_nodeGIDs,
      const Teuchos::RCP<const Teuchos_Comm>& comm_);

  void
  replaceOverlapVectorSpace(
      const Teuchos::Array<GO>&               overlap_nodeGIDs,
      const Teuchos::RCP<const Teuchos_Comm>& comm_);

  bool
  isNodeDataPresent()
  {
    return Teuchos::nonnull(nodal_data_vector);
  }

  void
  registerVectorState(const std::string& stateName, int ndofs);

  LO
  getVecsize()
  {
    return vectorsize;
  }

  Teuchos::RCP<Adapt::NodalDataVector>
  getNodalDataVector()
  {
    ALBANY_PANIC(
        nodal_data_vector.is_null(),
        std::logic_error,
        "nodal_data_vector has not been allocated.");
    return nodal_data_vector;
  }

  // The following are for use by response functions.
  //   Inherit from Manager to make an object shared by the several response
  // function field managers constructed when there are multiple element
  // blocks. Register the Manager holder.
  class Manager
  {
   public:
    virtual ~Manager() {}
  };
  // Register a manager. Throws if the key is already in use.
  void
  registerManager(const std::string& key, const Teuchos::RCP<Manager>& manager);
  // Check whether a manager has been registered with this key.
  bool
  isManagerRegistered(const std::string& key) const;
  // Get a manager. Throws if there is no manager associated with key.
  const Teuchos::RCP<Manager>&
  getManager(const std::string& key) const;

 private:
  Teuchos::RCP<Albany::NodeFieldContainer>          nodeContainer;
  Teuchos::RCP<const Albany::ThyraCrsMatrixFactory> nodalOpFactory;
  NodeFieldSizeVector                               nodeVectorLayout;
  NodeFieldSizeMap                                  nodeVectorMap;
  LO                                                vectorsize;
  Teuchos::RCP<Adapt::NodalDataVector>              nodal_data_vector;
  bool                                              initialized;

  typedef std::map<std::string, Teuchos::RCP<Manager>> ManagerMap;
  ManagerMap                                           mgr_map;

  void
  initialize();
};

}  // namespace Adapt

#endif  // ADAPT_NODAL_DATA_BASE_HPP
