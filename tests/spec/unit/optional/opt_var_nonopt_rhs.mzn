/***
!Test
solvers: [gecode]
options:
  all_solutions: true
expected: !Result
  solution: !SolutionSet
  - !Solution
    x: 2
    y: 2
    s: !Range 1..3
***/

% An optional variable whose right hand side flattens to a non-optional value (here because the
% if-then-else conditions are par, or because a function with an optional return type returns a
% non-optional value) used to be disconnected from its right hand side
function var opt int: f(var int: z) = z;

var 0..5: x :: add_to_output;
var 0..5: y :: add_to_output;
var set of 1..3: s :: add_to_output;

var opt int: oi = if true then x + 1 else <> endif;
var opt bool: ob = if false then <> else y = 2 endif;
var opt set of 1..3: os = if true then s else <> endif;
var opt int: ofn = f(x);

constraint absent(oi) \/ deopt(oi) = 3;
constraint absent(ob) \/ deopt(ob);
constraint absent(os) \/ card(deopt(os)) = 3;
constraint absent(ofn) \/ deopt(ofn) = 2;
