//*****************************************************************
// Albany 3.0: Copyright 2016 National Technology & Engineering Solutions of
// Sandia, LLC (NTESS). This Software is released under the BSD license detailed
// in the file license.txt in the top-level Albany directory
//*****************************************************************

#include <iostream>

#include "Albany_Dakota.hpp"
#include "Albany_SolverFactory.hpp"
#include "Teuchos_GlobalMPISession.hpp"
#include "Teuchos_StackedTimer.hpp"
#include "Teuchos_StandardCatchMacros.hpp"
#include "Teuchos_TimeMonitor.hpp"
#include "Teuchos_VerboseObject.hpp"

int
main(int argc, char* argv[])
{
  int                       status  = 0;  // 0 = pass, failures are incremented
  bool                      success = true;
  Teuchos::GlobalMPISession mpiSession(&argc, &argv);
  Kokkos::initialize(argc, argv);
  Teuchos::RCP<Teuchos::FancyOStream> out(
      Teuchos::VerboseObjectBase::getDefaultOStream());

  Albany::PrintHeader(*out);

  const auto stackedTimer =
      Teuchos::rcp(new Teuchos::StackedTimer("Albany Total Time"));
  Teuchos::TimeMonitor::setStackedTimer(stackedTimer);

  try {
    status += Albany_Dakota(argc, argv);

    // Regression comparisons fopr Dakota runs only valid on Proc 0.
    if (mpiSession.getRank() > 0)
      status = 0;
    else
      *out << "\nNumber of Failed Comparisons: " << status << std::endl;
  }
  TEUCHOS_STANDARD_CATCH_STATEMENTS(true, std::cerr, success);
  if (!success) status += 10000;

  stackedTimer->stop("Albany Total Time");
  Teuchos::StackedTimer::OutputOptions options;
  options.output_fraction = true;
  options.output_minmax   = true;
  stackedTimer->report(
      std::cout, Teuchos::DefaultComm<int>::getComm(), options);

  Kokkos::finalize_all();
  return status;
}
