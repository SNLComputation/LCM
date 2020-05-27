// Albany 3.0: Copyright 2016 National Technology & Engineering Solutions of
// Sandia, LLC (NTESS). This Software is released under the BSD license detailed
// in the file license.txt in the top-level Albany directory.
#if !defined(DislocationDensity_hpp)
#define DislocationDensity_hpp

namespace LCM {

namespace DislocationDensity {

int const NUM_SLIP_D1 = 2;

int const NUM_SLIP_D2 = 4;

int const NUM_SLIP_D3 = 24;

constexpr int
get_num_slip(int const num_dims)
{
  return num_dims == 3 ? NUM_SLIP_D3 : num_dims == 2 ? NUM_SLIP_D2 : NUM_SLIP_D1;
}

}  // namespace DislocationDensity
}  // namespace LCM

#endif  //! defined(DislocationDensity_hpp)
