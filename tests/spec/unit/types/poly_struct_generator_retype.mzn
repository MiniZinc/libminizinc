/***
!Test
solvers: [gecode]
options:
  all_solutions: true
expected: !Result
  solution: !SolutionSet
  - !Solution
    a: 2
***/

% When retyping a specialised copy of a polymorphic function, a generator over an array of
% structs kept the struct fields of the polymorphic type (`any $T'), so `x_i.2' was not typed
% with the concrete type and the arrayNd call could not be resolved (crash).
function array [int] of any $T: g(array [int] of tuple(int, any $T): x) =
  arrayNd([x_i.1 | x_i in x], [x_i.2 | x_i in x]);
function array [int] of any $T: r(array [int] of record(int: k, any $T: v): x) =
  arrayNd([x_i.k | x_i in x], [x_i.v | x_i in x]);
function array [$$E, $$F] of any $T: mia(array [int] of tuple(tuple($$E, $$F), any $T): x) =
  arrayNd([x_i.1.1 | x_i in x], [x_i.1.2 | x_i in x], [x_i.2 | x_i in x]);
var 1..3: a :: add_to_output;
array [int] of var int: ys = g([(i, a + i) | i in 1..2]);
array [int] of var int: zs = r([(k: i, v: a + i) | i in 1..2]);
array [int, int] of tuple(var bool, var int): ts =
  mia([((j, i), (j = 1, a + i)) | j in 1..2, i in 1..2]);
constraint ys[1] = 3;
constraint zs[2] = 4;
constraint ts[2, 1].2 = 3 /\ not ts[2, 1].1;
