predicate fzn_subgraph(int: N, int: E, array[int] of int: from, array[int] of int: to,
                       array[int] of var bool: ns, array[int] of var bool: es) =
    forall(e in 1..E) (
        (es[e] -> ns[from[e]]) /\ (es[e] -> ns[to[e]])
    );

%-----------------------------------------------------------------------------%
