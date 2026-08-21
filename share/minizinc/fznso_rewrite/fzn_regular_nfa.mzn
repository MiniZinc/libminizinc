% Rewrites `std`'s spelling into the registry's. Nothing here is a solver's
% concern — the constraint itself lives in `fznso_constraints/`.

% A FlatZinc array is one-dimensional, so the transition matrix is passed flat,
% one state's row after another, and declared `list of` — one-based, as the
% type-checker checks. The two-dimensional form the standard library's
% `regular_nfa` calls forwards to it.
%
% This is the non-deterministic automaton: a transition names a *set* of
% successor states. A propagator written for the deterministic one is wrong
% here, which is why the registry gives the two a name each.

include "fzn_int_regular_nondeterministic.mzn";

predicate fzn_regular_nfa(
  array [int] of var int: xs,
  int: states,
  int: alphabet,
  array [int, int] of set of int: transition,
  int: initial,
  set of int: accepting,
) =
  fzn_int_regular_nondeterministic(
    array1d(xs),
    states,
    alphabet,
    1,
    array1d(transition),
    initial,
    accepting,
  );
