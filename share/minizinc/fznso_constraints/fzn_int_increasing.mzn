% The constraint `int_increasing`, as the FZnSO registry defines it.
%
% This file is a solver's override point: declare `int_increasing` and the body
% below is dropped in favour of the propagator, or ship a file of this name with
% a decomposition of your own.

predicate fzn_int_increasing(list of var int: xs) =
  if length(xs) > 1 then
    forall (i in index_set(xs) diff {min(index_set(xs))}) (xs[i - 1] <= xs[i])
  endif;
