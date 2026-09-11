% Shadows `std`'s file of the same name, so everything it declares is repeated
% here: the four array extrema go to their registry constraints.
%
% `std` also decomposes `bool_clause_reif` here, for a language version that
% predates the builtin. That is gone: `redefinitions.mzn` routes the builtin to
% `bool_clause_reif` in the registry like any other, and defining it twice is a
% duplicate definition rather than an override.

include "fzn_int_array_maximum.mzn";
include "fzn_int_array_minimum.mzn";
include "fzn_float_array_maximum.mzn";
include "fzn_float_array_minimum.mzn";

% The registry puts the extremum first, as FlatZinc does, so these are renames.

predicate array_int_maximum(var int: m, array [int] of var int: xs) =
  fzn_int_array_maximum(array1d(xs), m);

predicate array_int_minimum(var int: m, array [int] of var int: xs) =
  fzn_int_array_minimum(array1d(xs), m);

predicate array_float_maximum(var float: m, array [int] of var float: xs) =
  fzn_float_array_maximum(array1d(xs), m);

predicate array_float_minimum(var float: m, array [int] of var float: xs) =
  fzn_float_array_minimum(array1d(xs), m);
