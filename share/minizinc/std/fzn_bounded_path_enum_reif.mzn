include "path.mzn";

predicate fzn_bounded_path_reif(array[int] of $$N: from, array[int] of $$N: to, array[int] of int: w,
                        var $$N: s, var $$N: t, array[$$N] of var bool: ns, array[int] of var bool: es, var int: K, var bool: b) =
   b <-> ( path(from,to,s,t,ns,es) /\
           K = sum(e in index_set(es))(es[e]*w[e]) );
