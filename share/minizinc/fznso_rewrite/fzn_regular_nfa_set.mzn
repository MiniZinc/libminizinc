% Rewrites `std`'s spelling into the registry's. Nothing here is a solver's
% concern — the constraint itself lives in `fznso_constraints/`.

% Shadows `std`'s file of the same name, exactly as `fzn_regular_set.mzn` does
% for the deterministic automaton: a contiguous alphabet goes to
% `fzn_int_regular_nondeterministic` with the number its symbols start from, so
% `xs` keeps the caller's own alphabet rather than being rebased onto one.

include "fzn_int_regular_nondeterministic.mzn";
include "fzn_regular_nfa.mzn";

predicate fzn_regular_nfa(
  array [int] of var int: xs,
  int: Q,
  set of int: S,
  array [int, int] of set of int: d,
  int: q0,
  set of int: F,
) =
  if S = min(S)..max(S) then
    fzn_int_regular_nondeterministic(array1d(xs), Q, card(S), min(S), array1d(d), q0, F)
  else
    % A sparse alphabet has no offset to travel, so it keeps `std`'s form.
    let {
      int: m = min(index_set(xs));
      int: n = max(index_set(xs)) + 1;
      array [m..n] of var 1..Q: a;
    } in a[m] = q0 /\
      forall (i in index_set(xs)) (xs[i] in S /\ a[i + 1] in d[a[i], xs[i]]) /\
      a[n] in F
  endif;
