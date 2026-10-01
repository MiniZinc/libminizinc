/***
!Test
type: compile
solvers: [gecode]
expected: !FlatZinc bool_def_ctx.fzn
***/

% Boolean definitions are flattened in the context of their uses, so they are
% half-reified when they are only used positively.

predicate int_le_imp(var int: a, var int: b, var bool: r);

var 0..10: x;
var 0..10: y;
var bool: q;

predicate p(var int: z) = let { var bool: b1 = z > 6; var bool: b2 = z < 3 } in b1 \/ b2;

% Root context: half-reified
constraint let { var bool: b1 = x > 5; var bool: b2 = x < 4 } in b1 \/ b2;
% Inside a predicate called in a positive context: half-reified
constraint p(y) \/ q;
% Negated let: fully reified
constraint not (let { var bool: b1 = x > 7; var bool: b2 = x < 2 } in b1 \/ b2) \/ q;
% b1 used positively and negatively: fully reified, b2 half-reified
constraint let { var bool: b1 = y > 8; var bool: b2 = y < 1 } in (b1 \/ b2) /\ (not b1 \/ q);
% b1 used positively through a later let variable: half-reified
constraint let { var bool: b1 = x > 8; var bool: b3 = b1 \/ q; var bool: b2 = x < 1 } in b3 \/ b2;

% The same holds for top-level variables
var bool: t1 = x > 9 \/ y > 9;
constraint t1 \/ q;
% t2 is used positively and, through t3, negatively: fully reified
var bool: t2 = y < 5;
var bool: t3 = not t2 \/ q;
constraint t3 \/ t2;

% The same holds for the elements of Boolean arrays
array[1..2] of var bool: a1 = [x > 3 + i | i in 1..2];
constraint exists(a1) \/ q;
constraint let { array[1..2] of var bool: a2 = [y < 3 - i | i in 1..2] } in a2[1] \/ a2[2] \/ q;
% a3[2] is used positively and, through xorall, in both polarities: fully reified
array[1..2] of var bool: a3 = [y > 3, y > 4];
constraint a3[1] \/ q;
constraint xorall(a3) \/ q;
