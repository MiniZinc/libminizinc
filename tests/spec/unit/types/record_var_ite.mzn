/***
!Test
solvers: [gecode]
expected: !Result
  status: SATISFIED
  solution: !Solution
    tups: [{"x": 12, "y": false}, {"x": 10, "y": true}, {"x": 11, "y": false}]
***/

array[1..3, 1..2] of var bool: sel;
array[1..3] of var record(10..12: x, bool: y): tups;

var record(10..12: x, bool: y): some_rec;
var record(10..12: x, bool: y): other_rec;

function var record(10..12: x, bool: y): repeat_ite(array[1..2] of var bool: b) =
  if b[1] then
    some_rec
  elseif b[2] then
    other_rec
  else
    (x: 12, y: false) % tuple literal
  endif;

% Actual test
constraint forall(i in 1..3) (
  tups[i] = repeat_ite(sel[i, ..])
);

% Set values for var choices in repeat_ite
constraint some_rec.y <-> (some_rec.x mod 2 = 0);
constraint some_rec.x mod 2 = 0;
constraint some_rec.x < 12;
constraint other_rec.y <-> (other_rec.x mod 2 = 0);
constraint other_rec.x mod 2 = 1;

% Ensure different selections
constraint not sel[1,1] /\ not sel[1,2];
constraint sel[1,1] xor sel[2,1];
constraint sel[2,1] xor sel[2,2];
constraint sel[2,1] xor sel[3,1];
constraint sel[3,1] xor sel[3,2];
