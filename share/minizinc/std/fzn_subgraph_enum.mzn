predicate fzn_subgraph(array[int] of $$N: from, array[int] of $$N: to,
                       array[$$N] of var bool: ns, array[int] of var bool: es) =
    forall(e in index_set(from)) (
        (es[e] -> ns[from[e]]) /\ (es[e] -> ns[to[e]])
    );

%-----------------------------------------------------------------------------%
