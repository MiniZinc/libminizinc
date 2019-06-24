include "fzn_arg_sort_float.mzn";
include "fzn_arg_sort_float_reif.mzn";

predicate arg_sort_float(array[int] of var float:x,
                         array[int] of var int:p) =
   assert(index_set(p) = 1..length(x),
     "arg_sort_float: second argument must have index 1..length(first argument)",
     fzn_arg_sort_float(x, p)
   );

predicate arg_sort_float_reif(array[int] of var float:x,
                              array[int] of var int:p,
                              var bool: b) =
   assert(index_set(p) = 1..length(x),
     "arg_sort_float: second argument must have index 1..length(first argument)",
     fzn_arg_sort_float_reif(x, p, b)
   );
