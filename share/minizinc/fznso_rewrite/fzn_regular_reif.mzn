% Rewrites `std`'s spelling into the registry's. Nothing here is a solver's
% concern — the constraint itself lives in `fznso_constraints/`.

% The reified form, under the registry's name. See `fzn_regular.mzn`.
%
% `std` branches on `fzn_regular_is_decomposed` and has a direct decomposition
% for the case where the solver has no propagator. The overlay never sets that
% flag — a solver may implement the plain form — so only the augmented-automaton
% construction is kept, and it is correct either way: it posts an ordinary
% `int_regular`, which a solver takes natively or decomposes as it would any
% other.
%
% The construction appends a marker symbol encoding `b`. States `1..states` are
% the original; `ACC` is the only accepting state and `DEAD` a live but
% non-accepting sink, so that a string rejected by a missing transition can
% still reach `ACC` with `b = false`.

include "fzn_int_regular_reif.mzn";

predicate fzn_regular_reif(
  array [int] of var int: xs,
  int: states,
  int: alphabet,
  array [int, int] of int: transition,
  int: initial,
  set of int: accepting,
  var bool: b,
) =
  fzn_int_regular_reif(
    array1d(xs),
    states,
    alphabet,
    1,
    array1d(transition),
    initial,
    accepting,
    b,
  );
