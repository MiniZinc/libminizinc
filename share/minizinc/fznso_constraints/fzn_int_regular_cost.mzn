% The constraint `int_regular_cost`, as the FZnSO registry defines it.
%
% This file is a solver's override point: declare `int_regular_cost` and the
% body below is dropped in favour of the propagator, or ship a file of this name
% with a decomposition of your own.

% A FlatZinc array is one-dimensional, so the transition matrix and the edge
% weights are passed flat, one state's row after another, and declared `list of`
% — one-based, as the type-checker checks. `cost_regular`'s alphabet is always
% `1..S`, so the offset the registry carries is one.

predicate fzn_int_regular_cost(
  list of var int: xs,
  int: states,
  int: alphabet,
  int: alphabet_offset,
  list of int: transition,
  int: initial,
  set of int: accepting,
  list of int: weight,
  var int: total_cost,
) =
  if length(xs) = 0 then
    total_cost = 0 /\ initial in accepting
  else
    let {
      % `a[i]` is the state reached after processing `xs[1..i - 1]` and `cc[i]`
      % the weight accumulated getting there, so `a[1]` is the initial state and
      % `a[length(xs) + 1]` has to be accepting.
      int: n = length(xs);
      set of int: symbols = alphabet_offset..alphabet_offset + alphabet - 1;
      array [1..states, symbols] of int: dd = array2d(1..states, symbols, transition);
      array [1..states, symbols] of int: ww = array2d(1..states, symbols, weight);
      array [1..n + 1] of var 1..states: a;
      array [1..n + 1] of var int: cc;
    } in a[1] = initial /\
      cc[1] = 0 /\
      forall (i in 1..n) (
        xs[i] in symbols /\ a[i + 1] = dd[a[i], xs[i]] /\ cc[i + 1] = ww[a[i], xs[i]] + cc[i]
      ) /\
      a[n + 1] in accepting /\
      total_cost = cc[n + 1]
  endif;
