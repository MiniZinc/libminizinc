/***
!Test
solvers: [gecode]
expected: !Error
  type: TypeError
  regex: "call to `bar' is ambiguous"
type: compile
***/

% Named arguments must name *enough* parameters to pick a unique name-only
% overload. Here both overloads share the parameter `a` and differ only on a
% defaulted second parameter (`upper` vs `lower`); naming only `a` leaves the
% call ambiguous, so it is a type error.

predicate bar(var int: a, var int: upper = 0) = a >= upper;
predicate bar(var int: a, var int: lower = 0) = a <= lower;

var 0..5: x;
constraint bar(a: x);

solve satisfy;
