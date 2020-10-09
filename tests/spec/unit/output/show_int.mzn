/***
!Test
expected:
- !Result
  solution: !Solution
    _output_item: "561@\n 561@\n561 @\n1234567@\n"
***/

var int: x;
constraint x > 560;
constraint x < 562;

solve satisfy;
output [
	show_int(0, x), "@\n",
	show_int(4, x), "@\n",
	show_int(-4, x), "@\n",
	show_int(4, 1234567), "@\n"
];
