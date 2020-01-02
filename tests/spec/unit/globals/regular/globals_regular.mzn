/***
!Test
solvers: [gecode]
expected:
- !Result
  solution: !Solution
    reg_input:
    - 1
    - 2
    - 3
    - 3
    - 3
    - 2
    - 1
    reg_input2:
    - 1
    - 2
    - 3
    - 3
    - 3
    - 3
    - 3
    - 3
    - 3
    - 2
    - 1
***/

include "regular.mzn";

%-----------------------------------------------------------------------------%
% regular
%-----------------------------------------------------------------------------%
% regexp is:  123*21.

int: n_states      = 5;
int: input_max     = 3;
int: initial_state = 1;
set of int: accepting_states = {5};
array[1..5, 1..3] of int: transition_fn =
    [|2, 0, 0   % transitions from state 1:   --1--> state 2
     |0, 3, 0   % transitions from state 2:   --2--> state 3
     |0, 4, 3   % transitions from state 3:   --2--> state 4, --3--> state 3
     |5, 0, 0   % transitions from state 4:   --1--> state 5
     |0, 0, 0|];% transitions from state 5:   (none)

array[-2..4]  of var int: reg_input ::add_to_output =
	array1d(-2..4, [1, 2, _, _, _, 2, 1]);
array[44..54] of var int: reg_input2 ::add_to_output;

constraint regular(reg_input, n_states, input_max, transition_fn,
                   initial_state, accepting_states);

constraint regular(reg_input2, n_states, input_max, transition_fn,
                   initial_state, accepting_states);

solve satisfy;

output [
    "reg_input = array1d(-2..4, ", show(reg_input), ");\n",
    "reg_input2 = array1d(44..54, ", show(reg_input2), ");\n"
];
