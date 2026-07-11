/***
!Test
solvers: [gecode]
expected: !Result
  status: SATISFIED
  solution: !Solution {}
***/

% A parameter whose name starts with "_" can only be passed positionally, so no call can
% supply these declarations' arguments by name and the permutation is not ambiguous. The
% positional calls pick the overload by argument type, as they always have.

predicate foo(float: _a, int: _b) = _b <= _a;
predicate foo(int: _b, float: _a) = _b + _a <= 10.0;

constraint foo(2.0, 1);
constraint foo(1, 2.0);

solve satisfy;
