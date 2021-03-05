/***
!Test
expected:
- !Result
  solution: !Solution
    bs: [false, true, false, true]
- !Result
  solution: !Solution
    bs: [true, false, true, false]
***/

function array[int] of var int: func() =
  let {
    array[1..2] of var 1..2: x;
  } in x;

array[1..2] of var bool: as;
array[1..4] of var bool: bs ::add_to_output = [ a \/ x=1 | a in as, x in func() ];

constraint bs[1] != bs[2];
constraint bs[2] != bs[3];
constraint bs[3] != bs[4];
