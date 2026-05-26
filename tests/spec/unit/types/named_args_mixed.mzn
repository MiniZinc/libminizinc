/***
!Test
solvers: [gecode]
type: compile
expected: !FlatZinc named_args_mixed.fzn
***/

% Mixed positional and named arguments. Positional first, then named.
% `bar(7, b: 3)` must bind 7 to the first parameter and 3 to `b`, yielding
% FlatZinc `bar(7, 3, 5)` (the third positional default-less arg `c: 5` is
% supplied by name).

predicate bar(int: a, int: b, int: c);
constraint bar(7, b: 3, c: 5);

solve satisfy;
