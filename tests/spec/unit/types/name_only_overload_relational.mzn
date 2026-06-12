/***
!Test
solvers: [gecode]
expected: !Result
  status: SATISFIED
  solution: !Solution
    _output_item: "a=103 b=203"
***/

% Regression test for name-only overloads whose call is rewritten into its
% relational (arity+1) form during flattening. A total function whose body
% introduces a defined result variable is posted as a relational constraint:
% the rewrite re-resolves the function by appending the result argument. A
% type-only re-match cannot tell the `upper` and `lower` siblings apart, so both
% calls collapsed onto one body (printing `a=103 b=103`). The relational
% re-match must honour the call's parameter names.

function var int: g(var int: upper) ::promise_total =
  let { var int: r; constraint r = upper + 100; } in r;
function var int: g(var int: lower) ::promise_total =
  let { var int: r; constraint r = lower + 200; } in r;

var 0..5: x;
constraint x = 3;
array [1..2] of var int: ys = [g(upper: x), g(lower: x)];
constraint forall (i in 1..2) (ys[i] >= 0);

output ["a=", show(ys[1]), " b=", show(ys[2])];
