/***
!Test
solvers: [gecode]
expected: !Result
  solution: !Solution
    b: false
    c: false
***/

% Previously we could not create array dummy values so these gave an internal error

array[int] of array [int] of int: t = [[1]];
bool: b :: output = let {
  array [int] of int: x = t[0];
} in true;

array[int] of tuple(array [int] of int): u = [([1],)];
bool: c :: output = let {
  array [int] of int: x = u[0].1;
} in true;
