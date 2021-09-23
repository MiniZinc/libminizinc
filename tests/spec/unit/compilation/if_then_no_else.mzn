/***
!Test
solvers: [gecode]
expected: !Result
  solution: !Solution
    b: true
    s: ""
    x: []
    y: []
***/

bool: b ::add_to_output = if false then false endif;
string: s ::add_to_output = if false then "failed" endif;
array[int] of int: x ::add_to_output = if false then [1] endif;
array[int,int] of int: y ::add_to_output = if false then [|1,2|3,4|] endif;

% Tests the annotation version
constraint b ::if false then expression_name("failed") endif;
