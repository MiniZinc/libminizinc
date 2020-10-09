include "fzn_cost_mdd.mzn";   
include "fzn_cost_mdd_reif.mzn";   
   
/** @group globals.extensional
   Requires that \a x defines a path in the cost MDD with total edge weight \a totalcost.

   @param N: the number of nodes, the root node is node 1
   @param level: the level of each node, the root is level 1, T is level \a length(x)+1
   @param E: the number of edges
   @param from: the leaving node (1..\a N)for each edge
   @param label: the set of value of the x variable for each edge
   @param cost: the cost for each edge
   @param to: the entering node for each edge, where 0 = T node
   @param totalcost: the total cost of the path defined by \a x  

*/
predicate cost_mdd(array[int] of var int: x,      % variables constrained by MDD
              int: N,                             % number of nodes    root is node 1
              array[int] of int: level,           % level of each node root is level 1, T is level length(x)+1
              int: E,                             % number of edges
              array[int] of int: from,            % edge leaving node  1..N
              array[int] of set of int: label,    % values of variable on edge
              array[int] of int: cost,            % cost of using edge
              array[int] of int: to,              % edge entering node 0..N where 0 = T node
              var int: totalcost                  % total cost of path
             ) =
        let { set of int: NODE = 1..N;
              set of int: EDGE = 1..E; 
              int: L = length(x);
              array[0..N] of int: levele = array1d(0..N,[L+1]++level); 
          } in
        assert(index_set(level) = NODE,
               "cost_mdd: level argument must be of length N = \(N)") /\
        assert(level[1] = 1, "cost_mdd: level of root (1) must be 1") /\
        forall(n in 2..N)(assert(level[n] != 1, "cost_mdd: level of non root node (\(n)) must not be 1")) /\
        assert(index_set(from) = EDGE,
               "cost_mdd: from argument must be of length E = \(E)") /\
        assert(index_set(to) = EDGE,
               "cost_mdd: to argument must be of length E = \(E)") /\
        assert(index_set(label) = EDGE,
               "cost_mdd: label argument must be of length E = \(E)") /\
        assert(index_set(cost) = EDGE,
               "cost_mdd: cost argument must be of length E = \(E)") /\
        forall(e in EDGE)(assert(from[e] in NODE,
                                 "cost_mdd: from[\(e)] must be in \(NODE)")) /\
        forall(e in EDGE)(assert(to[e] in 0..N,
                                 "cost_mdd: to[\(e)] must be in 0..\(N)")) /\
        forall(e in EDGE)(assert(level[from[e]]+1 = levele[to[e]],
                                 "cost_mdd: mdd level of from[\(e)] = \(level[from[e]])" ++
                                 "must be 1 less than level of to[\(e)] = \(levele[to[e]])")) /\
        forall(e1,e2 in EDGE where e1 < e2 /\ from[e1] = from[e2])
              (assert(label[e1] intersect label[e2] = {},
                      "cost_mdd: Two edges \(e1) and \(e2) leaving node \(from[e1]) with overlapping labels")) /\
        fzn_cost_mdd(x,N,level,E,from,label,cost,to,totalcost);

predicate cost_mdd_reif(array[int] of var int: x,      % variables constrained by MDD
              int: N,                             % number of nodes    root is node 1
              array[int] of int: level,           % level of each node root is level 1, T is level length(x)+1
              int: E,                             % number of edges
              array[int] of int: from,            % edge leaving node  1..N
              array[int] of set of int: label,    % values of variable on edge
              array[int] of int: cost,            % cost of using edge
              array[int] of int: to,              % edge entering node 0..N where 0 = T node
              var int: totalcost,                 % total cost of path
              var bool: b                         % reification variable
             ) = 
        fzn_cost_mdd_reif(x, N, level, E, from, label, cost, to, totalcost, b);

% Example consider an MDD over 3 variables
% 5 nodes and 8 edges
% level 1 root = 1
% level 2      2   3
% level 3      4   5
% level 4        T
% with edges (from,label,cost,to) given by 
%            (1,{1,3},3,2), (1,{2},1,3),  
%            (2,{2},4,4), (2,{3},2,5),
%            (3,{3},3,4), (3,{2},5,5),
%            (4,{1,5},2,0),
%            (5,{2,4,6},4,0)
% this is defined by the call
% cost_mdd([x1,x2,x3],5,[1,2,2,3,3],8,[1,1,2,2,3,3,4,5],
%          [{1,3},{2},{2},{3},{3},{2},{1,5},{2,4,6}],[3,1,4,2,3,5,2,4],[2,3,4,5,4,5,0,0],tc)                                                                