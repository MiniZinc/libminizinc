predicate fzn_mdd_reif(array[int] of var int: x,    % variables constrained by MDD
              int: N,                      % number of nodes    root is node 1
              array[int] of int: level,    % level of each node root is level 1, T is level length(x)+1
              int: E,                      % number of edges
              array[int] of int: from,     % edge leaving node  1..N
              array[int] of set of int: label,    % value of variable 
              array[int] of int: to,       % edge entering node 0..N where 0 = T node
              var bool: b                  % reification value
              ) =
        let { set of int: NODE = 1..N;
              set of int: EDGE = 1..E; 
              int: L = length(x);
              array[0..N] of var bool: bn; 
              array[EDGE] of var bool: be; 
              set of int: D = dom_array(x); } in
        (b <-> bn[0]) /\ % true node is result
        bn[1] /\ % root is always true
        % T1 each node except the true enforces an outgoing edge
        forall(n in NODE)(bn[n] -> (exists(e in EDGE where from[e] = n)(be[e]) \/ b = 0)) /\
        % T23 each edge enforces its endpoints
        forall(e in EDGE)((be[e] -> bn[from[e]]) /\ (be[e] -> bn[to[e]])) /\
        % T4 each edge enforces its label
        forall(e in EDGE)(be[e] -> x[level[from[e]]] in label[e]) /\
        % P1 each node enforces its outgoing edges
        forall(e in EDGE)(bn[from[e]] /\ x[level[from[e]]] in label[e] -> be[e]) /\
        % P2 each node except the root enforces an incoming edge
        (bn[0] -> exists(e in EDGE where to[e] = 0)(be[e])) /\
        forall(n in 2..N)(bn[n] -> exists(e in EDGE where to[e] = n)(be[e])) /\
        % P3 each label has a support, either an edge or to give false
        forall(i in 1..L, d in D)
              (x[i] = d -> forall(n in NODE where level[n] = i,  
                                  e in EDGE where from[e] = n /\ d in label[e])
                                 (bn[n] -> be[e]) /\
                           forall(n in NODE where level[n] = i /\ 
                                                  not exists(e in EDGE where from[e] = n)
                                                            (d in label[e]))
                                 (bn[n] -> b = 0)) /\
        % P4 at most one node at every level is true (redundant)
        forall(i in 1..L)
              (sum(n in NODE where level[n] = i)(bn[n]) <= 1);
