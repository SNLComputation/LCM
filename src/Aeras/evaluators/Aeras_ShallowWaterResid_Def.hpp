//*****************************************************************//
//    Albany 2.0:  Copyright 2012 Sandia Corporation               //
//    This Software is released under the BSD license detailed     //
//    in the file "license.txt" in the top-level Albany directory  //
//*****************************************************************//

#include "Teuchos_TestForException.hpp"
#include "Teuchos_VerboseObject.hpp"
#include "Teuchos_RCP.hpp"
#include "Phalanx_DataLayout.hpp"
#include "Sacado_ParameterRegistration.hpp"

#include "Intrepid_FunctionSpaceTools.hpp"

namespace Aeras {

//**********************************************************************
template<typename EvalT, typename Traits>
ShallowWaterResid<EvalT, Traits>::
ShallowWaterResid(const Teuchos::ParameterList& p,
              const Teuchos::RCP<Albany::Layouts>& dl) :
  wBF      (p.get<std::string> ("Weighted BF Name"), dl->node_qp_scalar),
  wGradBF  (p.get<std::string> ("Weighted Gradient BF Name"),dl->node_qp_gradient),
  U        (p.get<std::string> ("QP Variable Name"), dl->qp_vector),
  Ugrad    (p.get<std::string> ("Gradient QP Variable Name"), dl->qp_vecgradient),
  UDot     (p.get<std::string> ("QP Time Derivative Variable Name"), dl->qp_vector),
  surfHeight  (p.get<std::string> ("Aeras Surface Height QP Variable Name"), dl->qp_scalar),
  jacobian_inv  (p.get<std::string>  ("Jacobian Inv Name"), dl->qp_tensor ),
  jacobian_det  (p.get<std::string>  ("Jacobian Det Name"), dl->qp_scalar ),
  weighted_measure (p.get<std::string>  ("Weights Name"),   dl->qp_scalar ),
  jacobian  (p.get<std::string>  ("Jacobian Name"), dl->qp_tensor ),
  Residual (p.get<std::string> ("Residual Name"), dl->node_vector),
  intrepidBasis (p.get<Teuchos::RCP<Intrepid::Basis<RealType, Intrepid::FieldContainer<RealType> > > > ("Intrepid Basis") ),
  cubature      (p.get<Teuchos::RCP <Intrepid::Cubature<RealType> > >("Cubature")),
  spatialDim(p.get<std::size_t>("spatialDim"))

{

  Teuchos::ParameterList* shallowWaterList = p.get<Teuchos::ParameterList*>("Shallow Water Problem");
  // AGS: ToDo Add list validator!
  gravity = shallowWaterList->get<double>("Gravity", 1.0); //Default: Re=1
  usePrescribedVelocity = shallowWaterList->get<bool>("Use Prescribed Velocity", false); //Default: false

  this->addDependentField(U);
  this->addDependentField(Ugrad);
  this->addDependentField(UDot);
  this->addDependentField(wBF);
  this->addDependentField(wGradBF);
  this->addDependentField(surfHeight);


  if(3 == spatialDim ) {
    this->addDependentField(weighted_measure);
    this->addDependentField(jacobian);
    this->addDependentField(jacobian_inv);
    this->addDependentField(jacobian_det);
}
  this->addEvaluatedField(Residual);

  std::vector<PHX::DataLayout::size_type> dims;
    wGradBF.fieldTag().dataLayout().dimensions(dims);
    numNodes = dims[1];
    numQPs   = dims[2];
    numDims  = dims[3];

  refWeights        .resize               (numQPs);
  val_at_cub_points .resize     (numNodes, numQPs);
  grad_at_cub_points.resize     (numNodes, numQPs, 2);
  refPoints         .resize               (numQPs, 2);

  cubature->getCubature(refPoints, refWeights);
  intrepidBasis->getValues(val_at_cub_points,  refPoints, Intrepid::OPERATOR_VALUE);
  intrepidBasis->getValues(grad_at_cub_points, refPoints, Intrepid::OPERATOR_GRAD);

   this->setName("Aeras::ShallowWaterResid"+PHX::TypeString<EvalT>::value);


  U.fieldTag().dataLayout().dimensions(dims);
  vecDim  = dims[2];

  std::vector<PHX::DataLayout::size_type> gradDims;
  wGradBF.fieldTag().dataLayout().dimensions(gradDims);
  std::cout << "wGradBF has numNodes = " << gradDims[1] << " numQuadPts = " << gradDims[2] << " numSpatialComponents = "
      << gradDims[3] << std::endl;


  gradDims.clear();
  Ugrad.fieldTag().dataLayout().dimensions(gradDims);

  std::cout << "Ugrad has numQuadPts = " << gradDims[1] << " numVecDim = "
      << gradDims[2] << " numDim = " << gradDims[3] << std::endl;

//  std::cout << " vecDim = " << vecDim << std::endl;
//  std::cout << " numDims = " << numDims << std::endl;
//  std::cout << " numQPs = " << numQPs << std::endl;
//  std::cout << " numNodes = " << numNodes << std::endl;

  // Register Reynolds number as Sacado-ized Parameter
  Teuchos::RCP<ParamLib> paramLib = p.get<Teuchos::RCP<ParamLib> >("Parameter Library");
  new Sacado::ParameterRegistration<EvalT, SPL_Traits>("Gravity", this, paramLib);
}

//**********************************************************************
template<typename EvalT, typename Traits>
void ShallowWaterResid<EvalT, Traits>::
postRegistrationSetup(typename Traits::SetupData d,
                      PHX::FieldManager<Traits>& fm)
{
  this->utils.setFieldData(U,fm);
  this->utils.setFieldData(Ugrad,fm);
  this->utils.setFieldData(UDot,fm);
  this->utils.setFieldData(wBF,fm);
  this->utils.setFieldData(wGradBF,fm);
  this->utils.setFieldData(surfHeight,fm);
  if(3 == spatialDim ) {
    this->utils.setFieldData(weighted_measure, fm);
    this->utils.setFieldData(jacobian, fm);
    this->utils.setFieldData(jacobian_inv, fm);
    this->utils.setFieldData(jacobian_det, fm);
  }
  this->utils.setFieldData(Residual,fm);
}

//**********************************************************************
template<typename EvalT, typename Traits>
void ShallowWaterResid<EvalT, Traits>::
evaluateFields(typename Traits::EvalData workset)
{

//  double dt = 1;

//  std::cout << "current_time = " << workset.current_time <<
//      " previous time = " << workset.previous_time << " dt = " << dt << std::endl;

  for (std::size_t i=0; i < Residual.size(); ++i) Residual(i)=0.0;

 // ScalarT cfl = -1;

  Intrepid::FieldContainer<ScalarT>  div_hU(numQPs);
  Intrepid::FieldContainer<MeshScalarT>  meshSource(numQPs, 2);

  const int qpToNodeMap[9] = {0, 4, 1,
                              7, 8, 5,
                              3, 6, 2 };
  const int nodeToQPMap[9] = {0, 2, 8,
                              6, 1, 5,
                              7, 3, 4 };

  for (std::size_t cell=0; cell < workset.numCells; ++cell) {

    div_hU.initialize();
    meshSource.initialize();

//    for (std::size_t qp=0; qp < numQPs; ++qp) {
//      for (std::size_t node=0; node < numNodes; ++node) {
//
//        int i = nodeToQPMap[node];
////        MeshScalarT jinv00 = jacobian_inv(cell, i, 0,0);
////        MeshScalarT jinv01 = jacobian_inv(cell, i, 0,1);
////        MeshScalarT jinv10 = jacobian_inv(cell, i, 1,0);
////        MeshScalarT jinv11 = jacobian_inv(cell, i, 1,1);
//        MeshScalarT j00 = jacobian(cell, i, 0,0);
//        MeshScalarT j01 = jacobian(cell, i, 0,1);
//        MeshScalarT j10 = jacobian(cell, i, 1,0);
//        MeshScalarT j11 = jacobian(cell, i, 1,1);
//
//        const MeshScalarT rootg = jacobian_det(i);
//
//        MeshScalarT jinv00 = j11/rootg;
//        MeshScalarT jinv01 = -j01/rootg;
//        MeshScalarT jinv10 = -j10/rootg;
//        MeshScalarT jinv11 = j00/rootg;
//
//        ScalarT depth = U(cell,i,0);
//        ScalarT ulambda = U(cell, i,1);
//        ScalarT utheta  = U(cell, i,2);
//
//
//        const ScalarT vcontra1 = depth*(jinv00*ulambda + jinv01*utheta);
//        const ScalarT vcontra2 = depth*(jinv10*ulambda + jinv11*utheta);
//
//        div_hU(qp) += rootg*( vcontra1 )*grad_at_cub_points(node, qp, 0) +
//            rootg*( vcontra2)*grad_at_cub_points(node, qp, 1);
//
//        meshSource(qp, 0) += (jinv00*grad_at_cub_points(node, qp, 0) + jinv10*grad_at_cub_points(node, qp, 1) )*rootg;
//        meshSource(qp, 1) += (jinv01*grad_at_cub_points(node, qp, 0) + jinv11*grad_at_cub_points(node, qp, 1) )*rootg;
//      }
//
//      div_hU(qp) /= jacobian_det(cell,qp);
//    }

    // Depth Equation (Eq# 0)

    for (std::size_t qp=0; qp < numQPs; ++qp) {

      ScalarT depth = U(cell,qp,0);
      ScalarT ulambda = U(cell, qp,1);
      ScalarT utheta  = U(cell, qp,2);

      for (std::size_t node=0; node < numNodes; ++node) {


//        Residual(cell,node,0) += UDot(cell,qp,0)*wBF(cell,node,qp)
//               + div_hU(qp)*wBF(cell, node, qp) ;

        Residual(cell,node,0) += UDot(cell,qp,0)*wBF(cell,node,qp)
               - (U(cell,qp,0)-surfHeight(cell,qp))*( U(cell,qp,1)*wGradBF(cell,node,qp,0)
                   + U(cell,qp,2)*wGradBF(cell,node,qp,1) );
      }
    }

//    MeshScalarT cellArea = 0;
//    for (std::size_t qp=0; qp < numQPs; ++qp) {
//      cellArea += jacobian_det(cell, qp);
//    }
//
//    const MeshScalarT length = std::sqrt(cellArea);
//
//    for (std::size_t qp=0; qp < numQPs; ++qp) {
//
//      ScalarT waveSpeed = std::sqrt(gravity*(std::abs(U(cell, qp, 0) - surfHeight(cell,qp) ) ));
//      ScalarT speed = std::sqrt( U(cell, qp, 1)*U(cell, qp, 1) +
//          U(cell, qp, 2)*U(cell, qp, 2));
//      cfl = std::max((speed + waveSpeed)*dt/length, cfl);
//    }

  }

//  std::cout << "cfl = " << cfl << std::endl;

  // Velocity Equations (Eq# 1,2) -- u_dot = 0 only for now.
  if (usePrescribedVelocity) {
    for (std::size_t cell=0; cell < workset.numCells; ++cell) {
      for (std::size_t qp=0; qp < numQPs; ++qp) {
        for (std::size_t node=0; node < numNodes; ++node) {
                  Residual(cell,node,1) += UDot(cell,qp,1)*wBF(cell,node,qp);
                  Residual(cell,node,2) += UDot(cell,qp,2)*wBF(cell,node,qp);
        }
      }
    }
  }
  else { // Solve for velocity
    // Velocity Equations (Eq# 1,2)
    for (std::size_t cell=0; cell < workset.numCells; ++cell) {
      for (std::size_t qp=0; qp < numQPs; ++qp) {
        for (std::size_t node=0; node < numNodes; ++node) {
                    Residual(cell,node,1) += ( UDot(cell,qp,1)
                                               + U(cell,qp,1)*Ugrad(cell,qp,1,0) + U(cell,qp,2)*Ugrad(cell,qp,1,1)
                                               + gravity*Ugrad(cell,qp,0,0)
                                              )*wBF(cell,node,qp);
                    Residual(cell,node,2) += ( UDot(cell,qp,2)
                                               + U(cell,qp,1)*Ugrad(cell,qp,2,0) + U(cell,qp,2)*Ugrad(cell,qp,2,1)
                                               + gravity*Ugrad(cell,qp,0,1)
                                              )*wBF(cell,node,qp);
        }
      }
    }
  }
}

//**********************************************************************
// Provide Access to Parameter for sensitivity/optimization/UQ
template<typename EvalT,typename Traits>
typename ShallowWaterResid<EvalT,Traits>::ScalarT&
ShallowWaterResid<EvalT,Traits>::getValue(const std::string &n)
{
  return gravity;
}
//**********************************************************************
}
