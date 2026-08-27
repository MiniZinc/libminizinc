include "reachable_multiroot.mzn";

predicate fzn_cluster(int: N, int: E, array[int] of int: from, array[int] of int: to,
                      array[int,int] of var bool: r, array[int] of var bool: ns, array[int] of var bool: es) =
    let {
        array[1..N] of var 1..N: NodeRoot;
        array[1..N] of var bool: NodeRootnessMask;
        array[1..N] of bool: ALL_GRAPH_NODES = [ true | n in 1..N ];
    } in
        symmetry_breaking_constraint(forall (j,i in 1..N where i > j)(
            r[i,j] == r[j,i]
        )) /\
        forall (c in 1..N)(
            r[c,c] == true
        ) /\
        forall (j,i in 1..N where i > j)(
            r[j,i] -> (ns[j] /\ ns[i])
        ) /\
        reachable_multiroot(N,E,from,to,NodeRootnessMask,ALL_GRAPH_NODES,es) /\
        forall (n in 1..N)(
            NodeRootnessMask[n] == (NodeRoot[n] == n)
        ) /\
        forall (e in 1..E)(
            es[e] -> (NodeRoot[from[e]] == NodeRoot[to[e]])
        ) /\
        forall (j,i in 1..N where i > j)(
            r[j,i] == (NodeRoot[i] == NodeRoot[j])
        );

%-----------------------------------------------------------------------------%
