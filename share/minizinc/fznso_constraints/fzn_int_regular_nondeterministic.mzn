% The constraint `int_regular_nondeterministic`, as the FZnSO registry defines
% it.
%
% This file is a solver's override point: declare `int_regular_nondeterministic`
% and the body below is dropped in favour of the propagator, or ship a file of
% this name with a decomposition of your own.

% A FlatZinc array is one-dimensional, so the transition matrix is passed flat,
% one state's row after another, and declared `list of` — one-based, as the
% type-checker checks. The two-dimensional form the standard library's
% `regular_nfa` calls forwards to it.
%
% This is the non-deterministic automaton: a transition names a *set* of
% successor states. A propagator written for the deterministic one is wrong
% here, which is why the registry gives the two a name each.

predicate fzn_int_regular_nondeterministic(
  list of var int: xs,
  int: states,
  int: alphabet,
  int: alphabet_offset,
  list of set of int: transition,
  int: initial,
  set of int: accepting,
) =
  if length(xs) = 0 then
    initial in accepting
  else
    let {
      % `a[i]` is the state reached after processing `xs[1..i - 1]`, so `a[1]`
      % is the initial state and `a[length(xs) + 1]` has to be accepting.
      int: n = length(xs);
      set of int: symbols = alphabet_offset..alphabet_offset + alphabet - 1;
      array [1..states, symbols] of set of int: dd = array2d(1..states, symbols, transition);
      array [1..n + 1] of var 1..states: a;
    } in a[1] = initial /\
      forall (i in 1..n) (xs[i] in symbols /\ a[i + 1] in dd[a[i], xs[i]]) /\
      a[n + 1] in accepting
  endif;
