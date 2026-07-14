/*
 * Function fzn_piecewise_linear_base creates
 * auxiliary variables for the interval representation of xi
 * and constrains them to correspond to the value of x.
 * The auxiliaries are reusable if we have several pwls
 * on the same set of breakpoints and x.
 */
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

predicate fzn_piecewise_linear(
  var float: x,
  var float: y,
  array [int] of float: x_knots,
  array [int] of float: y_knots,
) =
  let {
    set of int: is = index_set(x_knots);
    constraint assert(
      is == index_set(y_knots) /\ 0 < card(is), "fzn_pwl: index sets unequal or empty"
    );
    set of int: is_1 = is diff {max(is)};
    array [int, int] of var float: fs = fzn_piecewise_linear_base(x, x_knots);
  } in y == sum (i in is_1) (y_knots[i] * fs[1, i] + (y_knots[i + 1] - y_knots[i]) * fs[2, i]);

%-----------------------------------------------------------------------------%
