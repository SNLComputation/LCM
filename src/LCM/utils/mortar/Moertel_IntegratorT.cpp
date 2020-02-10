//
// Albany 3.0: Copyright 2016 National Technology & Engineering Solutions of
// Sandia, LLC (NTESS). This Software is released under the BSD license detailed
// in the file license.txt in the top-level Albany directory.
//

#include "Moertel_ExplicitTemplateInstantiation.hpp"

#if defined(HAVE_MOERTEL_EXPLICIT_INSTANTIATION)
#include "Moertel_IntegratorT.hpp"
#include "Moertel_IntegratorT_Def.hpp"

namespace MoertelT {

MOERTEL_INSTANTIATE_TEMPLATE_CLASS(IntegratorT)

}  // namespace MoertelT

#endif
