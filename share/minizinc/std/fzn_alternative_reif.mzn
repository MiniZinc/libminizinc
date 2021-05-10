include "span.mzn";

predicate fzn_alternative_reif(var opt int: s0, var int: d0,
                      array[int] of var opt int: s,
                      array[int] of var int: d, var bool: b) =
          b <-> ( sum(i in index_set(s))(occurs(s[i])) = occurs(s0) /\
                  span(s0,d0,s,d) );
