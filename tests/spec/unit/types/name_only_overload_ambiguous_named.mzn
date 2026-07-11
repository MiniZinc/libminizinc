/***
!Test
solvers: [gecode]
expected: !Error
  type: TypeError
  regex: "call to `bar' is ambiguous"
type: compile
***/

% A call must name *enough* parameters to pick a unique name-only overload. These two
% declarations are legal: no purely-named call matches both, because one requires `a` and
% the other requires `c`, and neither accepts the other's name. But a call with a
% positional prefix can still leave the choice open -- `bar(x, b: 1)` binds the first
% parameter by position, so only `b` is named, and both overloads accept it at the same
% parameter type. That is a type error rather than a silent pick of whichever sibling is
% registered first.

predicate bar(var int: a, var int: b = 0) = a >= b;
predicate bar(var int: c, var int: b = 0) = c <= b;

var 0..5: x;
constraint bar(x, b: 1);

solve satisfy;
