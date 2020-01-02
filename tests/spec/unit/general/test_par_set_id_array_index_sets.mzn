/***
!Test
expected: !Result
  status: SATISFIED
  solution: !Solution
    _output_item: Ok
***/

predicate p(array [int] of var int: xs) =
	let {
		set of int: ix = index_set(xs),
		array [ix] of var bool: ys
	} in
		ys[1];

solve satisfy;

output ["Ok"];
