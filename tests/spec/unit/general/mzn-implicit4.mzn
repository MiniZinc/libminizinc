/***
!Test
expected: !Result
  status: SATISFIED
  solution: !Solution
    _output_item: Ok
***/

constraint let {
        array[int] of int: x = [1, 2, 3, 4]
    } in (
	length(x) = 4
    );

solve satisfy;

output ["Ok"];
