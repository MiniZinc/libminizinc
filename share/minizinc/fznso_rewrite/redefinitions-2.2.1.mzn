% Shadows `std`'s file of the same name. The registry has one `int_pow`, whose
% exponent a solver may narrow to a constant — which is exactly what this
% builtin is, so it needs no identifier of its own.

include "fzn_int_pow.mzn";

predicate int_pow_fixed(var int: x, int: y, var int: z) = fzn_int_pow(x, y, z);
