/** @group globals Constrains \a m to be the minimum of the values in \a x.

  Assumptions: |\a x| > 0.
*/
predicate minimum(var float: m, array[int] of var float: x) =
    array_float_minimum(m, x);

/** @group globals Constrains \a m to be the minimum of the values in \a x.

  Assumptions: |\a x| > 0.
*/
predicate minimum(var int: m, array[int] of var int: x) =
    array_int_minimum(m, x);

    % XXX: currently doesn't work:  'uy' is used as a type but that isn't
    %      allowed because it's not a global variable.
%predicate minimum(var set of int: m, array[int] of var set of int: x) =
%    let { int: l = min(index_set(x)),
%          int: u = max(index_set(x)),
%          set of int: uy = ub(x),
%          array[l..u] of var set of uy: y } in
%    y[l] = x[l] /\
%    m = y[u] /\
%    forall (i in l+1 .. u) ( y[i] == min(x[i],y[i-1]) );

