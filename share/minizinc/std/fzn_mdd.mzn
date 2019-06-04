/** @group globals.extensional
  Requires that \a x defines a path from root to true node T through the MDD defined by

   \a N is the number of nodes, the root node is node 1
   \a level is the level of each node, the root is level 1, T is level \a length(x)+1
   \a E is the number of edges
   \a from is the leaving node (1..\a N)for each edge
   \a label is the set of values of the \a x variable for each edge
   \a to is the entering node for each edge, where 0 = T node
*/
predicate fzn_mdd(array[int] of var int: x,    % variables constrained by MDD
              int: N,                      % number of nodes    root is node 1
              array[int] of int: level,    % level of each node root is level 1, T is level length(x)+1
              int: E,                      % number of edges
              array[int] of int: from,     % edge leaving node  1..N
              array[int] of set of int: label,    % value of variable 
              array[int] of int: to        % edge entering node 0..N where 0 = T node
             ) =
        let { set of int: NODE = 1..N;
              set of int: EDGE = 1..E; 
              int: L = length(x);
              array[0..N] of var bool: bn; 
              array[EDGE] of var bool: be; 
              set of int: D = dom_array(x); } in
        bn[0] /\ % true node is true
        bn[1] /\ % root must hold
        % T1 each node except the root enforces an outgoing edge
        forall(n in NODE)(bn[n] -> exists(e in EDGE where from[e] = n)(be[e])) /\
        % T23 each edge enforces its endpoints
        forall(e in EDGE)((be[e] -> bn[to[e]]) /\ (be[e] -> bn[to[e]])) /\
        % T4 each edge enforces its label
        forall(e in EDGE)(be[e] -> x[level[from[e]]] in label[e]) /\
        % P1 each node enforces its outgoing edges
        forall(e in EDGE)(bn[from[e]] /\ x[level[from[e]]] in label[e] -> be[e]) /\
        % P2 each node except the root enforces an incoming edge
        exists(e in EDGE where to[e] = 0)(be[e]) /\
        forall(n in 2..N)(bn[n] -> exists(e in EDGE where to[e] = n)(be[e])) /\
        % P3 each label has a support
        forall(i in 1..L, d in D)
              (x[i] = d -> exists(e in EDGE where level[from[e]] = i /\ d in label[e])(be[e]));

/** @group globals.extensional
  Requires that \a b holds iff \a x defines a path from root to true node T through the MDD defined by

   \a N is the number of nodes, the root node is node 1
   \a level is the level of each node, the root is level 1, T is level \a length(x)+1
   \a E is the number of edges
   \a from is the leaving node (1..\a N)for each edge
   \a label is the set of values of the \a x variable for each edge
   \a to is the entering node for each edge, where 0 = T node
   \a b is whether the path exists
*/
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
        bn[0] /\ % true node is true
        (b <-> bn[1]) /\ % root gives truth value 
        % T1 each node except the root enforces an outgoing edge
        forall(n in NODE)(bn[n] -> exists(e in EDGE where from[e] = n)(be[e])) /\
        % T23 each edge enforces its endpoints
        forall(e in EDGE)((be[e] -> bn[to[e]]) /\ (be[e] -> bn[to[e]])) /\
        % T4 each edge enforces its label
        forall(e in EDGE)(be[e] -> x[level[from[e]]] in label[e]) /\
        % P1 each node enforces its outgoing edges
        forall(e in EDGE)(bn[from[e]] /\ x[level[from[e]]] in label[e] -> be[e]) /\
        % P2 each node except the root enforces an incoming edge
        exists(e in EDGE where to[e] = 0)(be[e]) /\
        forall(n in 2..N)(bn[n] -> exists(e in EDGE where to[e] = n)(be[e])) /\
        % P3 each label has a support
        forall(i in 1..L, d in D)
              (x[i] = d -> exists(e in EDGE where level[from[e]] = i /\ d in label[e])(be[e]));
                             
% Example consider an MDD over 3 variables
% 5 nodes and 12 edges
% level 1 root = 1
% level 2      2   3
% level 3      4   5
% level 4        T
% with edges (from,label,to) given by 
%            (1,1,2), (1,2,3), (1,3,2)  
%            (2,2,4), (2,3,5)
%            (3,3,4), (3,2,5)
%            (4,1,0), (4,5,0) 
%            (5,2,0), (5,4,0), (5,6,0)
% this is defined by the call
% mdd([x1,x2,x3],5,[1,2,2,3,3],12,[1,1,1,2,2,3,3,4,4,5,5,5],[1,3,2,2,3,3,2,1,5,2,4,6],[2,2,3,4,5,4,5,0,0,0,0,0])                                                                
