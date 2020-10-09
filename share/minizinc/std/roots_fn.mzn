include "roots.mzn";

/** @group globals
  Returns \a s such that \a x[\p i] in \a t for all \p i in \a s
*/
function var set of int: roots(array[int] of var int: x, 
                               var set of int: t) :: promise_total =
  let { var set of index_set(x): s ::is_defined_var;
        constraint roots(x,s,t) ::defines_var(s); } 
  in s;
