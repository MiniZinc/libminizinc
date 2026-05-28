/***
!Test
solvers: [gecode]
expected: !Result
  status: SATISFIED
  solution: !Solution
    a: [2, 2]
    b: [2, 2]
    fr: [2, 2]
***/

% Overload ranking compares the parameters a call actually supplies, not arity.
% Each call below supplies one array of non-optional ints; both overloads accept
% it, but the two-parameter one is more specific there and its `except`
% parameter defaults. Ranking by arity first would let the more general
% `opt` overload win and silently run the wrong body.
%
% Both resolution paths are pinned: `p(a)` goes through matchFn, `p(xs: b)`
% through matchFnNamed. The par `f` overloads pin down which decl the
% *typechecker* chose - a par call is constant-folded, so a later re-resolution
% cannot mask a wrong choice there.

predicate p(array [int] of var opt int: xs) = forall (i in index_set(xs)) (xs[i] = 1);

predicate p(array [int] of var int: xs, set of int: except = {}) =
  forall (i in index_set(xs)) (xs[i] = 2);

function int: f(array [int] of opt int: xs) = 1;

function int: f(array [int] of int: xs, set of int: except = {}) = 2;

array [1..2] of var 1..2: a;
array [1..2] of var 1..2: b;
constraint p(a);
constraint p(xs: b);

array [1..2] of int: c = [5, 6];
array [1..2] of var 1..2: fr;
constraint fr[1] = f(c);
constraint fr[2] = f(xs: c);

solve satisfy;
