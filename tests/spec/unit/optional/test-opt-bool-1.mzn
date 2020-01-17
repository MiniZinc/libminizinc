/***
!Test
solvers: [gecode, chuffed, cbc]
expected: !Result
  solution: !Solution
    conjunction: [true, false, true, false, false, false, true, false, true]
    disjunction: [false, false, true, false, false, true, true, true, true]
    negation: [null, true, false]
  status: SATISFIED
***/

% conjunction of var opt bool
array [1..9] of bool: conjunction :: add_to_output = [x /\ y | x, y in [<>, false, true]];

% disjunction of var opt bool
array [1..9] of bool: disjunction :: add_to_output = [x \/ y | x, y in [<>, false, true]];

% not of var opt bool
array [1..3] of opt bool: negation:: add_to_output = [not x | x in [<>, false, true]];

solve satisfy;
