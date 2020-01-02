/***
!Test
expected: !Result
  status: SATISFIED
  solution: !Solution
    killed: 1
    eFailureModes: 1
***/

var int: killed;
constraint killed = bool2int(eFailureModes = 1);

var int: eFailureModes;
constraint eFailureModes = 1;

solve satisfy;