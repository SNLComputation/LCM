//
// Albany 3.0: Copyright 2016 National Technology & Engineering Solutions of
// Sandia, LLC (NTESS). This Software is released under the BSD license detailed
// in the file license.txt in the top-level Albany directory
//

#ifndef ALBANY_TPETRA_THYRA_TYPES_HPP
#define ALBANY_TPETRA_THYRA_TYPES_HPP

// Get all Albany configuration macros
#include "Albany_config.h"

// Get the scalar/ordinal types, as well as Tpetra ordinals/node
#include "Albany_ScalarOrdinalTypes.hpp"
#include "Albany_TpetraTypes.hpp"

// Get the converter and the Thyra::TpetraXYZ types
#include "Thyra_TpetraLinearOp.hpp"
#include "Thyra_TpetraMultiVector.hpp"
#include "Thyra_TpetraThyraWrappers.hpp"
#include "Thyra_TpetraVector.hpp"

typedef Thyra::
    TpetraOperatorVectorExtraction<ST, Tpetra_LO, Tpetra_GO, KokkosNode>
        ConverterT;
typedef Thyra::TpetraLinearOp<ST, Tpetra_LO, Tpetra_GO, KokkosNode>
    Thyra_TpetraLinearOp;
typedef Thyra::TpetraMultiVector<ST, Tpetra_LO, Tpetra_GO, KokkosNode>
    Thyra_TpetraMultiVector;
typedef Thyra::TpetraVector<ST, Tpetra_LO, Tpetra_GO, KokkosNode>
    Thyra_TpetraVector;
typedef Thyra::TpetraVectorSpace<ST, Tpetra_LO, Tpetra_GO, KokkosNode>
    Thyra_TpetraVectorSpace;

#endif  // ALBANY_TPETRA_THYRA_TYPES_HPP
