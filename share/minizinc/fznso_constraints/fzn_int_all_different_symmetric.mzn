% The constraint `int_all_different_symmetric`, as the FZnSO registry defines
% it.
%
% This file is a solver's override point: declare `int_all_different_symmetric`
% and the body below is dropped in favour of the propagator, or ship a file of
% this name with a decomposition of your own.

% The values in `xs` index `xs` itself, so the numbering they index from travels
% next to the array as a plain `int` — a FlatZinc array is one-based and cannot
% carry one.

include "all_different.mzn";
include "inverse.mzn";

predicate fzn_int_all_different_symmetric(list of var int: xs, int: offset) =
  let {
    array [offset..offset + length(xs) - 1] of var int: x =
      array1d(offset..offset + length(xs) - 1, xs);
  } in all_different(x) /\ inverse(x, x);
