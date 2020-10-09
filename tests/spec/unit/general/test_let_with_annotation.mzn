/***
!Test
expected:
- !Result
  status: SATISFIED
  solution: !Solution
    x: 1
- !Result
  status: SATISFIED
  solution: !Solution
    x: 10
***/

var 1..10: x :: add_to_output;

constraint x =
	let
	{ var int: a::domain }
	in
	a;

solve satisfy;
