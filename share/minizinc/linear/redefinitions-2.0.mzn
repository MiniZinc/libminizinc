predicate bool_clause_reif(array[int] of var bool: p, array[int] of var bool: n, var bool: c) =
    c = ( sum(i in index_set(p))( bool2int(p[i]) )
          - sum(i in index_set(n))( bool2int(n[i]) )
          + length(n)
            >= 1
        );

    
predicate array_float_minimum(var float: m, array[int] of var float: x) =
  if false then
    array_float_minimum__IND(m, x)     %% transfer to Concert because of prepro
  else
    array_float_minimum_I( m, [ x[i] | i in index_set(x)])
  endif;
predicate array_float_maximum(var float: m, array[int] of var float: x) =
  if false then
    array_float_maximum__IND(m, x)
  else
    array_float_minimum_I(-m, [-x[i] | i in index_set(x)])
  endif;
predicate array_int_minimum(var int: m, array[int] of var int: x) =
  if false then
    array_int_minimum__IND(m, x)
  else
    array_float_minimum_I( int2float(m), [ int2float(x[i]) | i in index_set(x)])
  endif;
predicate array_int_maximum(var int: m, array[int] of var int: x) =
  if false then
    array_int_maximum__IND(m, x)
  else
    array_float_minimum_I(-int2float(m), [-int2float(x[i]) | i in index_set(x)])
  endif;

% predicate array_int_maximum__OLD_SYMMETRIES(var int: m, array[int] of var int: x) =
%     let { int: l = min(index_set(x)),
%           int: u = max(index_set(x)),
%           int: ly = lb_array(x),
%           int: uy = ub_array(x),
%           array[l..u] of var ly..uy: y } in
%     y[l] = x[l] /\
%     m = y[u] /\
%     forall (i in l+1 .. u) ( y[i] == max(x[i],y[i-1]) );

