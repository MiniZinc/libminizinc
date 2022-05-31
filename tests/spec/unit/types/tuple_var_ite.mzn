/***
!Test
solvers: [gecode]
expected: !Result
  status: SATISFIED
  solution: !Solution
    tups: [[12, false], [10, true], [11, false]]
***/

array[1..3, 1..2] of var bool: sel;
array[1..3] of var tuple(10..12, bool): tups;

var tuple(10..12, bool): some_tuple;

function var tuple(10..12, bool): ret_tuple() =
  let {
    var 10..12: x;
    var bool: y;
  } in (x, y);

function var tuple(10..12, bool): repeat_ite(array[1..2] of var bool: b) =
  if b[1] then
    some_tuple % tuple identifier
  elseif b[2] then
    ret_tuple() % function call
  else
    (12, false) % tuple literal
  endif;

% Actual test
constraint forall(i in 1..3) (
  tups[i] = repeat_ite(sel[i, ..])
);

% Set values for var choices in repeat_ite
constraint some_tuple.2 <-> (some_tuple.1 mod 2 = 0);
constraint some_tuple.1 mod 2 = 0;
constraint some_tuple.1 < 12;
constraint ret_tuple().2 <-> (ret_tuple().1 mod 2 = 0);
constraint ret_tuple().1 mod 2 = 1;

% Ensure different selections
constraint not sel[1,1] /\ not sel[1,2];
constraint sel[1,1] xor sel[2,1];
constraint sel[2,1] xor sel[2,2];
constraint sel[2,1] xor sel[3,1];
constraint sel[3,1] xor sel[3,2];
