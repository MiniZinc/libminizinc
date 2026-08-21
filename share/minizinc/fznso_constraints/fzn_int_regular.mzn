% The constraint `int_regular`, as the FZnSO registry defines it.
%
% This file is a solver's override point: declare `int_regular` and the body
% below is dropped in favour of the propagator, or ship a file of this name with
% a decomposition of your own.

% A FlatZinc array is one-dimensional, so the transition matrix is passed flat,
% one state's row after another, and declared `list of` — one-based, as the
% type-checker checks. The two-dimensional form the standard library's `regular`
% calls forwards to it.
%
% Unlike `std`, this does not set `fzn_regular_is_decomposed`: a solver may
% implement the plain form, and `fzn_regular_reif` then builds a reified
% automaton over it rather than decomposing.

predicate fzn_int_regular(
  list of var int: xs,
  int: states,
  int: alphabet,
  int: alphabet_offset,
  list of int: transition,
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
      array [1..states, symbols] of int: dd = array2d(1..states, symbols, transition);
      array [1..n + 1] of var 1..states: a;
    } in a[1] = initial /\
      forall (i in 1..n) (xs[i] in symbols /\ a[i + 1] = dd[a[i], xs[i]]) /\
      a[n + 1] in accepting
  endif;
