/***
!Test
solvers: [gecode]
expected: !Result
- !Solution
  r:
    a: 1
    b: 1
***/

array[int] of record( int: a, opt int: b ): records = [
  ( a: 1, b: 1 ),
];
record( var int: a, var opt int: b ): r;
constraint r in records;
