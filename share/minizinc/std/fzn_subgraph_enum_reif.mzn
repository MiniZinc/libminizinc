predicate fzn_subgraph_reif(array[int] of $$N: from, array[int] of $$N: to,
                            array[$$N] of var bool: ns, array[int] of var bool: es,
                            var bool: b) =
    b <-> forall(e in index_set(from)) (
        (es[e] -> ns[from[e]]) /\ (es[e] -> ns[to[e]])
    );

%-----------------------------------------------------------------------------%
