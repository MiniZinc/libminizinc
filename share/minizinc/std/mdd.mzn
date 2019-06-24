include "fzn_mdd.mzn";
include "fzn_mdd_reif.mzn";

/** @group globals.extensional
  Requires that \a x defines a path from root to true node T through the MDD defined by

   \a N is the number of nodes, the root node is node 1
   \a level is the level of each node, the root is level 1, T is level \a length(x)+1
   \a E is the number of edges
   \a from is the leaving node (1..\a N)for each edge
   \a label is the set of values of the \a x variable for each edge
   \a to is the entering node for each edge, where 0 = T node
*/
predicate mdd(array[int] of var int: x,    % variables constrained by MDD
              int: N,                      % number of nodes    root is node 1
              array[int] of int: level,    % level of each node root is level 1, T is level length(x)+1
              int: E,                      % number of edges
              array[int] of int: from,     % edge leaving node  1..N
              array[int] of set of int: label,    % possible values of variable 
              array[int] of int: to        % edge entering node 0..N where 0 = T node
             ) =
        let { set of int: NODE = 1..N;
              set of int: EDGE = 1..E; 
              int: L = length(x);
              array[0..N] of int: levele = array1d(0..N,[L+1]++level); } in
        assert(index_set(level) = NODE,
               "mdd: third argument must be of length N = \(N)") /\
        assert(index_set(from) = EDGE,
               "mdd: 5th argument must be of length E = \(E)") /\
        assert(index_set(to) = EDGE,
               "mdd: 7th argument must be of length E = \(E)") /\
        forall(e in EDGE)(assert(from[e] in NODE,
                                 "mdd: from[\(e)] must be in \(NODE)")) /\
        forall(e in EDGE)(assert(to[e] in 0..N,
                                 "mdd: to[\(e)] must be in 0..\(N)")) /\
        forall(e in EDGE)(assert(level[from[e]]+1 = levele[to[e]],
                                 "mdd level of from[\(e)] = \(level[from[e]])" ++
                                 "must be 1 less than level of to[\(e)] = \(levele[to[e]])")) /\
	fzn_mdd(x, N, level, E, from, label, to);
                             
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
