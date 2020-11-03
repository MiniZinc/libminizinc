% This file contains redefinitions of standard builtins for version 2.5.2
% that can be overridden by solvers.

predicate array_var_int_element2d_nonshifted(var int: idx1, var int: idx2, array[int,int] of var int: x, var int: c) =
  let {
    int: dim = card(index_set_2of2(x));
    int: min_flat = min(index_set_1of2(x))*dim+min(index_set_2of2(x))-1;
  } in array_var_int_element_nonshifted((idx1*dim+idx2-min_flat)::domain, array1d(x), c);

predicate array_var_bool_element2d_nonshifted(var int: idx1, var int: idx2, array[int,int] of var bool: x, var bool: c) =
  let {
    int: dim = card(index_set_2of2(x));
    int: min_flat = min(index_set_1of2(x))*dim+min(index_set_2of2(x))-1;
  } in array_var_bool_element_nonshifted((idx1*dim+idx2-min_flat)::domain, array1d(x), c);

predicate array_var_float_element2d_nonshifted(var int: idx1, var int: idx2, array[int,int] of var float: x, var float: c) =
  let {
    int: dim = card(index_set_2of2(x));
    int: min_flat = min(index_set_1of2(x))*dim+min(index_set_2of2(x))-1;
  } in array_var_float_element_nonshifted((idx1*dim+idx2-min_flat)::domain, array1d(x), c);

predicate array_var_set_element2d_nonshifted(var int: idx1, var int: idx2, array[int,int] of var set of int: x, var set of int: c) =
  let {
    int: dim = card(index_set_2of2(x));
    int: min_flat = min(index_set_1of2(x))*dim+min(index_set_2of2(x))-1;
  } in array_var_set_element_nonshifted((idx1*dim+idx2-min_flat)::domain, array1d(x), c);

