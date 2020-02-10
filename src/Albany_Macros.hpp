//
// Albany 3.0: Copyright 2016 National Technology & Engineering Solutions of
// Sandia, LLC (NTESS). This Software is released under the BSD license detailed
// in the file license.txt in the top-level Albany directory.
//

#ifndef ALBANY_DEBUG_HPP
#define ALBANY_DEBUG_HPP

// Get Albany configuration macros
#include "Albany_config.h"

// Checks if the previous Kokkos::Cuda kernel has failed
#define cudaCheckError()

// NVTX Range creates a colored range which can be viewed on the nvvp timeline
// (from Parallel Forall blog)
#define PUSH_RANGE(name, cid)
#define POP_RANGE

#include <iostream>  // For std::cerr
#define ALBANY_ASSERT_IMPL(cond, msg, ...)                                     \
  do {                                                                         \
    if (!(cond)) {                                                             \
      std::ostringstream omsg;                                                 \
      omsg << #cond " failed at ";                                             \
      omsg << __FILE__ << " +" << __LINE__ << '\n';                            \
      omsg << msg << '\n';                                                     \
      std::cerr << #cond " failed at " << __FILE__ << " +" << __LINE__ << "\n" \
                << msg << '\n';                                                \
      abort();                                                                 \
    }                                                                          \
  } while (0)

#define ALBANY_ASSERT(...) ALBANY_ASSERT_IMPL(__VA_ARGS__, "")

#if defined(NDEBUG)
#define ALBANY_EXPECT(...)
#else
#define ALBANY_EXPECT(...) ALBANY_ASSERT(__VA_ARGS__)
#endif

#define ALBANY_ALWAYS_ASSERT(cond) ALBANY_ASSERT(cond)
#define ALBANY_ALWAYS_ASSERT_VERBOSE(cond, msg) ALBANY_ASSERT(cond, msg)
#define ALBANY_DEBUG_ASSERT(cond) ALBANY_EXPECT(cond)
#define ALBANY_DEBUG_ASSERT_VERBOSE(cond, msg) ALBANY_EXPECT(cond, msg)

#endif  // ALBANY_DEBUG_HPP
