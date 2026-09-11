% The constraint `int_value_precede_chain`, as the FZnSO registry defines it.
%
% This file is a solver's override point: declare `int_value_precede_chain` and
% the body below is dropped in favour of the propagator, or ship a file of this
% name with a decomposition of your own.

include "seq_precede_chain.mzn";

predicate fzn_int_value_precede_chain(list of int: values, list of var int: xs) =
  if length(values) = 0 then
    true
  elseif min(index_set(values)) = 1 /\
    forall (i in index_set(values)) (values[i] = i) /\
     max(values) = ub_array(xs) then
    seq_precede_chain(xs)
  else
    let {
      int: offset = min(index_set(values)) - 1;
      int: l = lb_array(xs);
      int: u = ub_array(xs);
      array [1..u - l + 1] of int: p = [
        sum([i - offset | i in index_set(values) where values[i] = j]) |
          j in l..u,
      ];
      array [int] of var 0..length(values): Y =
        array1d(index_set(xs), [p[xs[i] - l + 1] | i in index_set(xs)]);
    } in seq_precede_chain(Y)
  endif;
