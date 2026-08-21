% The constraint `set_of_int_value_precede`, as the FZnSO registry defines it.
%
% This file is a solver's override point: declare `set_of_int_value_precede` and
% the body below is dropped in favour of the propagator, or ship a file of this
% name with a decomposition of your own.

predicate fzn_set_of_int_value_precede(int: s, int: t, list of var set of int: xs) =
  if length(xs) = 0 then
    true
  else
    let {
      int: imin = min(index_set(xs));
      int: imax = max(index_set(xs));
      array [imin..imax + 1] of var bool: seen;
    } in (
      forall (i in imin..imax) (
        let {
          var bool: xis = (s in xs[i] /\ not (t in xs[i]));
        } in (xis -> (seen[i + 1] == true)) /\
          ((not xis) -> (seen[i] == seen[i + 1])) /\
          ((not seen[i]) -> (s in xs[i] \/ not (t in xs[i])))
      ) /\
        seen[imin] == false
    )
  endif;
