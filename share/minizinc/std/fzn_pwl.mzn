/** @group globals
  Constrains \a y(\a x) to be a piecewise-linear interpolation of
  the provided point sequence.
*/
predicate fzn_pwl(var float: x, var float: y,
              array[int] of float: xi, array[int] of float: vi) = 
    let {
        set of int: is = index_set(xi),
        constraint assert(is == index_set(vi) /\ 0<card(is),
                              "fzn_pwl: index sets unequal or empty"),
        set of int: is_1 = is diff { max(is) },
        array[is] of var 0.0..1.0: lambda,
        array[is_1] of var 0..1: f,
    } in
      forall(i in is_1)(f[i]>=lambda[i] /\ f[i]>=lambda[i+1]) /\
      1 == sum(f) /\
      x == sum(i in is)(xi[i] * lambda[i]) /\
      y == sum(i in is)(vi[i] * lambda[i])
    ;

%-----------------------------------------------------------------------------%
