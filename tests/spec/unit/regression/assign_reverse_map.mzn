/***
!Test
solvers: [gecode]
expected:
- !Result
  solution: !Solution
    x: 2
***/

% Test a simple equality constraint after creating a encoding with a reverse mapper.
% The correct behaviour is that this needs to be posted as a constraints.
% Previously an optimisation would simply change the domain, which is incorrect once the encoding has been created.
var 1..2: x;
array[int] of var bool: x_enc = unary_encode(x);
constraint x = 2;

% Simple unary SAT encoding for integers with two options
function array[int] of var bool: unary_encode(var int: x) ::promise_total =
  let {
    array[lb(x)..ub(x)] of var bool: x_enc::expression_name("unary_encoding");
    constraint ub(x) - lb(x) = 1;
    constraint bool_not(x_enc[lb(x)], x_enc[ub(x)]);
    constraint (x = reverse_unary(x_enc))::is_reverse_map;
  } in x_enc;

predicate int_eq(var int: x, int: y) =
  let {
    array[int] of var bool: x_enc = unary_encode(x);
  } in x_enc[y];

% Reverse mappings (unary to integer)
function var int: reverse_unary(array[int] of var bool: x_enc);
function int: reverse_unary(array[int] of bool: x_enc) = arg_max(x_enc);
