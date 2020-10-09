include "subgraph.mzn";

predicate fzn_dreachable(int: N, int: E, array[int] of int: from, array[int] of int: to,
                        var int: r, array[int] of var bool: ns, array[int] of var bool: es) =
      let {
          set of int: NODE = 1..N;
          array[NODE] of var 0..N-1: dist; /* distance from root */
          array[NODE] of var 0..N: parent; /* parent */
      } in
          ns[r] /\ % the root must be chosen
          dist[r] = 0 /\ % root is at distance 0
          forall(n in NODE) % nonselected nodes have parent 0
                (not ns[n] -> parent[n] = 0) /\
          forall(n in NODE) % nonselected nodes have distance 0
                (not ns[n] -> dist[n] = 0) /\
          forall(n in NODE) % each in node except root must have a parent
                (ns[n] -> (n = r \/ parent[n] > 0)) /\
          forall(n in NODE) % each in node with a parent must be in and also its parent
                (parent[n] > 0 -> (ns[n] /\ ns[parent[n]])) /\
          forall(n in NODE) % each except with a parent is one more than its parent
                (parent[n] > 0 -> dist[n] = dist[parent[n]] + 1) /\
          forall(n in NODE) % each node with a parent must have that edge in
                (parent[n] > 0 -> exists(e in 1..E)(es[e] /\ from[e] = parent[n] /\ to[e] = n)) /\
          subgraph(N,E,from,to,ns,es);
   
%-----------------------------------------------------------------------------%
