# The vendored FZnSO headers

`fznso.hpp` and `fznso_types.h` are copied verbatim from an FZnSO release —
`cpp/fznso.hpp` and `c/fznso_types.h` in that checkout. `fznso.hpp` includes
`fznso_types.h` by a relative path, so the two have to stay side by side.

They are header-only and describe a protocol, not a library: nothing is linked
against them, and a solver is a shared object loaded at run time. Vendoring them
is what lets the FZnSO solver interface be built unconditionally rather than
depending on a checkout being present.

`FZNSO_ABI_VERSION` in `fznso_types.h` is checked against every library loaded,
so updating these files is what decides which solvers MiniZinc will accept.
Re-vendor both together, and do not edit them here.
