% The constraint `float_piecewise_linear`, as the FZnSO registry defines it.
%
% This file is a solver's override point: declare `float_piecewise_linear` and
% the body below is dropped in favour of the propagator, or ship a file of this
% name with a decomposition of your own.

predicate fzn_float_piecewise_linear(
  var float: x,
  list of float: x_knot,
  list of float: y_knot,
  var float: y,
) =
  let {
    set of int: is = index_set(x_knot);
    constraint assert(
      is == index_set(y_knot) /\ 0 < card(is), "fzn_pwl: index sets unequal or empty"
    );
    set of int: is_1 = is diff {max(is)};
    array [int, int] of var float: fs = fzn_piecewise_linear_base(x, x_knot);
  } in y == sum (i in is_1) (y_knot[i] * fs[1, i] + (y_knot[i + 1] - y_knot[i]) * fs[2, i]);

function array [int, int] of var 0.0..1.0: fzn_piecewise_linear_base(
  var float: x,
  array [int] of float: xi,
)
  :: promise_total =
  let {
    set of int: is = index_set(xi);
    set of int: is_1 = is diff {max(is)};
    array [is_1] of var 0.0..1.0: s; %% Segment variables
    array [is_1] of var 0..1: f;
    constraint 1 == sum(f);
    constraint forall (i in is_1) (f[i] >= s[i]);
    constraint x == sum (i in is_1) (xi[i] * f[i] + (xi[i + 1] - xi[i]) * s[i]);
  } in array2d(1..2, is_1, f ++ s);
