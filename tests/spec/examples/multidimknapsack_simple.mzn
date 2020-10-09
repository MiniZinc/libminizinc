/***
!Test
expected:
- !Result
  solution: !Solution
    objective: 17
    x: [0, 1, 0, 1, 1]
***/

%------------------------------------------------------------------------------%
% The classical 0/1 multidimensional knapsack problem.
%
% There is a knapsack with m different capacity constraints and n items with
% profits and weights.  The goal is to maximise the total profit of the items
% in the knapsack while not violating any of the capacity constraints.
%------------------------------------------------------------------------------%

int: n;
int: m;

array[1..n] of int: Profits;
array[1..n,1..m] of int: Weights;
array[1..m] of int: Capacities;

array[1..n] of var 0..1: x;

constraint
    forall(i in 1..m) (
        sum([Weights[j,i] * x[j] | j in 1..n])  <=  Capacities[i]
    );

solve maximize
    sum([x[j] * Profits[j] | j in 1..n]);

output [ "multidimknapsack_simple " ] ++
    [ show(x[i]) ++ if i = n then "\n" else " " endif | i in 1..n ];

%------------------------------------------------------------------------------%
% data

n = 5;
m = 3;

Profits    = [5,6,3,7,4];

Capacities = [5,10,15];

Weights    = [| 2, 3, 2
              | 1, 4, 4
              | 1, 2, 5
              | 3, 2, 3
              | 1, 3, 5 |];

