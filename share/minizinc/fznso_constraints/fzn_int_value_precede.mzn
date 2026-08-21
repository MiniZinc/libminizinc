% The constraint `int_value_precede`, as the FZnSO registry defines it.
%
% This file is a solver's override point: declare `int_value_precede` and the
% body below is dropped in favour of the propagator, or ship a file of this name
% with a decomposition of your own.

predicate fzn_int_value_precede(int: s, int: t, list of var int: xs) =
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
          var bool: xis = (xs[i] == s);
        } in (xis -> (seen[i + 1] == true)) /\
          ((not xis) -> (seen[i] == seen[i + 1])) /\
          ((not seen[i]) -> (xs[i] != t))
      ) /\
        seen[imin] == false
    )
  endif;
