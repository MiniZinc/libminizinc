% `set_of_int_array_element_nd` for a solver with no set variables.
%
% The row-major index is arithmetic on the indices, which has nothing to do with
% sets; what is left is the one-dimensional element this directory already
% decomposes. `fznso_constraints/` aborts on this one and shadows nothing, so
% without this file a model indexing a two-dimensional array of sets fails on a
% solver that has none.

include "fzn_set_of_int_array_element.mzn";

predicate fzn_set_of_int_array_element_nd(
  list of var set of int: xs,
  list of int: offsets,
  list of int: sizes,
  list of var int: indices,
  var set of int: value,
) =
  let {
    int: d = length(indices);
    array [1..d] of int: stride = [product (e in i + 1..d) (sizes[e]) | i in 1..d];
    var 1..length(xs): flat =
      1 + sum (i in 1..d) (stride[i] * (indices[i] - offsets[i]));
  } in fzn_set_of_int_array_element(xs, 1, flat, value);
