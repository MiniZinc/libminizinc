/***
--- !Test
solvers: [gecode]
expected: !Result
  solution: !Solution
    a: !!set {1, 3, 5}
    b: !!set {1, 3, 5}
    c: true
    d: true
***/

% The par overloads used to only accept one-dimensional arrays, so these calls
% resolved to the var overloads and returned a var set
set of int: a :: add_to_output = array2set([|5, 1 | 3, 1|]);
set of int: b :: add_to_output = array2set([|5, <> | 3, 1|]);
bool: c :: add_to_output = array2set([|true, true | true, true|]) = {true};
bool: d :: add_to_output = array2set([|1.5, 2.5 | 2.5, 1.5|]) = {1.5, 2.5};
