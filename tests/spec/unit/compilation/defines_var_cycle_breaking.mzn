/***
!Test
type: compile
solvers: [gecode]
expected: !FlatZinc defines_var_cycle_breaking.fzn
***/

var int: w :: no_output;
var int: x :: no_output;
var int: y :: no_output;
var int: z :: no_output;

constraint w = x + y;
constraint x = y + z;
constraint y = z + w;
constraint z = w + x;
