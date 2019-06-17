/** @group globals
  Constrains \a y(\a x) to be a piecewise-linear interpolation of
  the provided point sequence.
  
  The formulation follows "Modeling SOS2 constraints in GLPK", 2008.
*/
predicate fzn_piecewise_linear(var float: x, var float: y,
              array[int] of float: xi, array[int] of float: vi) = 
    let {
        set of int: is = index_set(xi),
        constraint assert(is == index_set(vi) /\ 0<card(is),
                              "fzn_pwl: index sets unequal or empty"),
        set of int: is_1 = is diff { max(is) },
        array[is_1] of var 0.0..1.0: s,          %% Segment variables
        array[is_1] of var 0..1: f,
    } in
      1 == sum(f) /\
      forall(i in is_1)(f[i] >= s[i]) /\
      x == sum(i in is_1)(xi[i] * f[i] + (xi[i+1]-xi[i]) * s[i]) /\
      y == sum(i in is_1)(vi[i] * f[i] + (vi[i+1]-vi[i]) * s[i])
    ;

%-----------------------------------------------------------------------------%
