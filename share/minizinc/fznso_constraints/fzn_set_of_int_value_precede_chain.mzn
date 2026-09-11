% The constraint `set_of_int_value_precede_chain`, as the FZnSO registry defines
% it.
%
% This file is a solver's override point: declare
% `set_of_int_value_precede_chain` and the body below is dropped in favour of
% the propagator, or ship a file of this name with a decomposition of your own.

include "fzn_set_of_int_value_precede.mzn";

predicate fzn_set_of_int_value_precede_chain(list of int: values, list of var set of int: xs) =
  forall (i in min(index_set(values)) + 1..max(index_set(values))) (
    fzn_set_of_int_value_precede(values[i - 1], values[i], xs)
  );
