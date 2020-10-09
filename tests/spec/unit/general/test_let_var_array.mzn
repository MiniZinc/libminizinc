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

var int: x :: add_to_output =
	let {
		array [1..3] of var 1..10: a = [1, _, 3]
	}
	in a[2];
solve satisfy;
