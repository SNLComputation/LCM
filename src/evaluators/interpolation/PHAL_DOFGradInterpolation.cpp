//*****************************************************************
// Albany 3.0: Copyright 2016 National Technology & Engineering Solutions of
// Sandia, LLC (NTESS). This Software is released under the BSD license detailed
// in the file license.txt in the top-level Albany directory
//*****************************************************************

#include "PHAL_DOFGradInterpolation.hpp"

#include "PHAL_AlbanyTraits.hpp"
#include "PHAL_DOFGradInterpolation_Def.hpp"

PHAL_INSTANTIATE_TEMPLATE_CLASS_WITH_ONE_SCALAR_TYPE(
    PHAL::DOFGradInterpolationBase)
PHAL_INSTANTIATE_TEMPLATE_CLASS_WITH_ONE_SCALAR_TYPE(
    PHAL::FastSolutionGradInterpolationBase)
