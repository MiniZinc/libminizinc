/***
!Test
expected: !Result
  status: SATISFIED
  solution: !Solution
    x: 42
***/

int: x :: add_to_output =
	let {
		int: a = 42
	}
	in a;
solve satisfy;
