/***
!Test
solvers: [gecode, chuffed]
expected:
- !Result
  solution: !Solution
    node_used: [3, 4, 6, 9, 11, 13, 15, 8, 9, 10, 11, 12, 13, 14, 15]
    x: [6, 2, 6, 2, 4, 6, 8, 1, 2, 3, 4, 5, 6, 7, 8]
***/

% Regression test for a problem in mzn2fzn 1.2 where log/2
% was not recognised as a built-in operation by the flattening
% engine.

% THe model is from: http://www.hakank.org/minizinc/decision_tree_binary.mzn

% Decision tree in MiniZinc.
% 
% Simple zero sum binary decision trees.
%
% 
% Model created by Hakan Kjellerstrand, hakank@bonetmail.com

% The model only handles complete binary trees of sizes 
% n = (a**2) - 1, for some a. The values are at the lowest level.
%
% Example with n = 7
%    
%        x1         A maximizes
%        /\
%      x2  x3       B minimizes
%     / \   /\          
%    x4 x5 x6 x7   (A)
%
% The value nodes (the last level):
% x4 = 6
% x5 = 1
% x6 = 5
% x7 = 3
%
% First B minimizes the last level (x4..x7):
% x2 = argmin(x5, x4)  -> x2 = x5
% x3 = argmin(x6, x7)  -> x3 = x7
%
% A then maximizes from B's choices as the last step:
% x1 = argmax( x3,  x2)  -> x1 = x3
% 
% 
% The solution is a decision tree represented here as as:
%
%
%  x (the values):
%  3
%  1 3
%  6 1 5 3
% 
%  node_used:
%  3
%  5 7
%  4 5 6 7


%
% minizinc and fz can handle n as large as 4095 (2**12-1) 
% without breaking much sweat. 
% For n = 8191 (2**13-1), it core dumps however.
%


int: n; % must be a (a**2) - 1, for some a, e.g. 7, 15, 31, 63 etc
int: levels = n div 2; % the number of levels

array[1..n] of var int: x;          % the decision variables, the value tree
array[1..n] of var 1..n: node_used; % the nodes indices


%
% tree_levels contains the levels in the tree, e.g.
% [1, 2,2, 3,3,3,3, 4,4,4,4,4,4,4,4, ....]
% for odd levels: A is trying to maximize his/her gain,
% for even levels B is trying to minimize A's gains 
%
array[1..n] of 0..n: tree_levels = [1+floor(log(2.0,int2float(i))) | i in 1..n ];

% solve :: int_search(x, "first_fail", "indomain", "complete") satisfy;
solve satisfy;

%
% argmax: maximize A's values
%
predicate argmax(array[int] of var int: x, int: i1, int: i2, array[int] of var int: node_used, int: node_used_ix) =
    (x[i1] >= x[i2] -> node_used[node_used_ix] = i1)
    /\
    (x[i1]  < x[i2] -> node_used[node_used_ix] = i2)     
;

% argmin: minimize A's values
predicate argmin(array[int] of var int: x, int: i1, int: i2, array[int] of var int: node_used, int: node_used_ix) =
    (-x[i1] >= -x[i2] -> node_used[node_used_ix] = i1)
    /\
    (-x[i1]  < -x[i2] -> node_used[node_used_ix] = i2)
;

predicate cp1d(array[int] of var int: x, array[int] of var int: y) =
  assert(index_set(x) = index_set(y),
           "cp1d: x and y have different sizes",
    forall(i in index_set(x)) ( x[i] = y[i] ) )
; 


constraint

  % the first 1..n div 2 in x is to be decided by the model,
  % the rest is the node values.

  cp1d(x, [_, _,_, _,_, _,_, 1,2, 3,4, 5,6, 7,8])  % n = 15

   /\ % the last n div 2 positions (the node "row") in node_used is static
   forall(i in levels+1..n) ( 
       node_used[i] = i 
       % more general: makes the node value 1.. . 
       % Comment it if another tree should be used.
       % /\ x[i] = -(n - i - levels) + 1
   )
   /\
   % the first n div 2 positions and values are dynamic, 
   % the rest are static.
   forall(i in 1..levels) (
     x[i] = x[node_used[i]]
   )
   /\ % Should we maximize or minimize?
      % It depends on the level.
   forall(i in 1..levels) (
     if tree_levels[i] mod 2 = 1 then
       argmax(x, 2*i, 2*i+1, node_used, i) 
     else 
       argmin(x, 2*i, 2*i+1, node_used, i) 
     endif
   )

;

% A "nice tree" (OK, it's not so nice. :-)
output 
["x (the values):\n" , show(x[1])]
++
[
  if tree_levels[i] > tree_levels[i-1] then "\n" else " " endif ++
     show(x[i]) 
  | i in 2..n
] ++
["\n\nnode_used:\n", show(node_used[1])]
++
[
  if tree_levels[i] > tree_levels[i-1] then "\n" else " " endif ++
    show(node_used[i]) 
  | i in 2..n
] ++ ["\n"];

n = 15;
