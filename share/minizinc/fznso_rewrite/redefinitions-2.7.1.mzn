% Shadows `std`'s file of the same name. All three are registry constraints.

include "fzn_float_ceil.mzn";
include "fzn_float_floor.mzn";
include "fzn_float_round.mzn";

predicate float_ceil(var float: x, var int: y) = fzn_float_ceil(x, y);

predicate float_floor(var float: x, var int: y) = fzn_float_floor(x, y);

predicate float_round(var float: x, var int: y) = fzn_float_round(x, y);
