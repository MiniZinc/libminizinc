include "fzn_arg_sort_int.mzn";
include "fzn_arg_sort_int_reif.mzn";

predicate arg_sort_int(array[int] of var int:x,
                       array[int] of var int:p) =
   assert(index_set(p) = 1..length(x),
     "arg_sort_int: second argument must have index 1..length(first argument)",
     fzn_arg_sort_int(x, p)
   );

predicate arg_sort_int(array[int] of var int:x,
                       array[int] of var int:p,
                       var bool:b) =
   assert(index_set(p) = 1..length(x),
     "arg_sort_int: second argument must have index 1..length(first argument)",
     fzn_arg_sort_int_reif(x, p, b)
   );
