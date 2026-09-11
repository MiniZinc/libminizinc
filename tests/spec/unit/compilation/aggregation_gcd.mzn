/***
--- !Test
type: compile
solvers: [gecode]
expected: !FlatZinc aggregation_gcd.fzn
***/

% Expect divided coefficients and bounds, with the non-divisible disequality
% removed as true, because integer relations are normalised by their GCD.

array [1..2] of var 0..10: a;

constraint 2 * a[1] + 4 * a[2] <= 7;
constraint 3 * a[1] + 6 * a[2] != 8;
constraint 6 * a[1] + 9 * a[2] = 12;
