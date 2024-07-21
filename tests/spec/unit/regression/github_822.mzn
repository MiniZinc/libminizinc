/***
!Test
solvers: [gecode]
expected: !Result
  solution: !Solution
    i64_min: '-9223372036854775808'
    i64_max: '9223372036854775807'
***/

string: i64_min :: output = show(-9223372036854775808);
string: i64_max :: output = show(9223372036854775807);
