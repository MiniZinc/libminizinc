/***
!Test
solvers: [gecode]
expected: !Result
  solution: !Solution
    s: [2, 2]
***/

% The conditions of these if-then-else expressions are par but contain variables (the argument
% of f), so the if-then-else must not be evaluated directly. The common type of the branches lost
% the cv flag of the condition (the first branch is joined with bot, and `<>' with a tuple).
array[1..2] of var 1..3: s;
function int: f(array [int] of var int: x) = length(x);
array[int] of var int: t = [let { constraint s[i] > 1 } in s[i] | i in 1..2];
constraint if f(t) = 2 then true else abort("x") endif;
constraint occurs(if f(t) = 3 then <> else (1, 2) endif);
