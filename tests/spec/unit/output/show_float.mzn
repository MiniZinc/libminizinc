/***
!Test
expected:
- !Result
  solution: !Solution
    _output_item: "  123.46@\n123.46  @\n123     @\n"
***/

float: f = 123.45679123;
solve satisfy;
output [
	show_float(8, 2, f), "@\n",
	show_float(-8, 2, f), "@\n",
	show_float(-8, 0, f), "@\n",
];
