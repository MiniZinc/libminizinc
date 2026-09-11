% The constraint `float_piecewise_linear_non_continuous`, as the FZnSO registry
% defines it.
%
% This file is a solver's override point: declare
% `float_piecewise_linear_non_continuous` and the body below is dropped in
% favour of the propagator, or ship a file of this name with a decomposition of
% your own.

predicate fzn_float_piecewise_linear_non_continuous(
  var float: x,
  list of float: x_start,
  list of float: x_end,
  list of float: y_start,
  list of float: y_end,
  var float: y,
) =
  let {
    set of int: is = index_set(x_start);
    constraint
      assert(
        is == index_set(y_start) /\
          is == index_set(y_end) /\
          is == index_set(x_end) /\
           0 < card(is),
        "fzn_pwl: index sets unequal or empty",
      );
    array [int, int] of var float: fs = fzn_piecewise_linear_non_continuous_base(x, x_start, x_end);
  } in y == sum (i in is) (y_start[i] * fs[1, i] + (y_end[i] - y_start[i]) * fs[2, i]);

function array [int, int] of var 0.0..1.0: fzn_piecewise_linear_non_continuous_base(
  var float: x,
  array [int] of float: x_start,
  array [int] of float: x_end,
)
  :: promise_total =
  let {
    set of int: is = index_set(x_start);
    array [is] of var 0.0..1.0: s; %% Segment variables
    array [is] of var 0..1: f;
    constraint 1 == sum(f);
    constraint
      forall (i in is) (
        if x_start[i] < x_end[i] then
          f[i] >= s[i]
        elseif x_start[i] == x_end[i] then
          s[i] == 0.0
        else
          abort("piecewise linear discontinuous: interval \(i): x_end < x_start")
        endif
      );
    constraint x == sum (i in is) (x_start[i] * f[i] + (x_end[i] - x_start[i]) * s[i]);
  } in array2d(1..2, is, f ++ s);
