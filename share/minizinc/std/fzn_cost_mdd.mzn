
predicate fzn_cost_mdd(array[int] of var int: x,      % variables constrained by MDD
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
              array[1..L] of int: maxlevelcost = 
                  [ max(e in EDGE where level[from[e]] = l)(cost[e]) | l in 1..L];
              array[1..L] of int: minlevelcost = 
                  [ min([0] ++ [ cost[e] | e in EDGE where level[from[e]] = l /\ cost[e] < 0])| l in 1..L] ;
              int: maxcost = sum(maxlevelcost);
              set of int: COST = sum(minlevelcost)..L*(maxcost+1);
              array[0..N] of var bool: bn; 
              array[EDGE] of var bool: be; 
              array[0..N] of var COST: ln; % distance from T
              array[0..N] of var COST: un; % distance from root 
        } in        
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
        forall(i in 1..L, d in dom(x[i]))
              (x[i] = d -> exists(e in EDGE where level[from[e]] = i /\ d in label[e])(be[e])) /\
        ln[0] = 0 /\ un[1] = 0 /\
        forall(n in NODE)
              (ln[n] = min(e in EDGE where from[e] = n)(ln[to[e]] + cost[e] + (not be[e])*(maxcost+1 - cost[e]))) /\
        forall(n in 2..N)
              (un[n] = min(e in EDGE where to[e] = n)(un[from[e]] + cost[e] + (not be[e])*(maxcost+1 - cost[e]))) /\
        forall(e in EDGE)(be[e] -> un[from[e]] + cost[e] + ln[to[e]] <= maxcost) /\
        totalcost = ln[1];

predicate fzn_cost_mdd_reif(array[int] of var int: x,      % variables constrained by MDD
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
    abort("Reified cost_mdd/9 is not supported.");
                             
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