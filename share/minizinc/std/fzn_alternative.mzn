include "span.mzn";

/** @group globals.scheduling
Alternative constraint for optional tasks. Task (\a s0,\a d0) spans the
optional tasks (\a s[\p i],\a d[\p i]) in the array arguments
and at most one can occur
*/
predicate fzn_alternative(var opt int: s0, var int: d0,
                      array[int] of var opt int: s,
                      array[int] of var int: d) =
          sum(i in index_set(s))(bool2int(occurs(s[i]))) <= 1 /\
          span(s0,d0,s,d);
