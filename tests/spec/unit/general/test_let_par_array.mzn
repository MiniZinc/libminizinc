/***
!Test
expected: !Result
  status: SATISFIED
  solution: !Solution
    _output_item: Ok
***/

int: x =
	let {
		array [1..3] of int: a = [1, 2, 3]
	}
	in a[1];
solve satisfy;
output ["Ok"];
