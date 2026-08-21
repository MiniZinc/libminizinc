% Rewrites `std`'s spelling into the registry's. Nothing here is a solver's
% concern — the constraint itself lives in `fznso_constraints/`.

% Shadows `std`'s file of the same name. `std` reaches here whenever the alphabet
% does not start at 1 and decomposes; a contiguous alphabet instead goes to
% `fzn_int_regular` with the number its symbols start from, so `xs` keeps the
% caller's own alphabet. Without that offset the constraint could not be handed
% over at all.
%
% As in `fzn_regular.mzn`, `fzn_regular_set_is_decomposed` stays unassigned, so
% that `fzn_regular_set_reif` builds an automaton over a native propagator rather
% than decomposing.

include "fzn_int_regular.mzn";
include "fzn_regular.mzn";

predicate fzn_regular_set(
  array [int] of var int: xs,
  int: Q,
  set of int: S,
  array [int, int] of int: d,
  int: q0,
  set of int: F,
) =
  if S = min(S)..max(S) then
    fzn_int_regular(array1d(xs), Q, card(S), min(S), array1d(d), q0, F)
  else
    % A sparse alphabet has no offset to travel, so it keeps `std`'s form.
    let {
      int: m = min(index_set(xs));
      int: n = max(index_set(xs)) + 1;
      array [m..n] of var 1..Q: a;
    } in a[m] = q0 /\
      forall (i in index_set(xs)) (xs[i] in S /\ a[i + 1] = d[a[i], xs[i]]) /\
      a[n] in F
  endif;

predicate fzn_regular(
  array [int] of var int: xs,
  int: Q,
  set of int: S,
  array [int, int] of int: d,
  int: q0,
  set of int: F,
) = fzn_regular_set(xs, Q, S, d, q0, F);
