% The constraint `int_subcircuit_reif`, as the FZnSO registry defines it.
%
% This file is a solver's override point: declare `int_subcircuit_reif` and the
% body below is dropped in favour of the propagator, or ship a file of this name
% with a decomposition of your own.

include "alldifferent.mzn";

predicate fzn_int_subcircuit_reif(list of var int: xs, int: offset, var bool: b) =
  let {
    array [offset..offset + length(xs) - 1] of var int: x =
      array1d(offset..offset + length(xs) - 1, xs);
  } in (
    abort("Reified subcircuit/1 is not supported.")
  );
