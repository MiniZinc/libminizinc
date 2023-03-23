/***
!Test
solvers: [gecode]
expected: !Error
  regex: .* Declared index set of `x\.1' is \[1\.\.1\], but is assigned to array with index set \[2\.\.2\].*
***/

tuple(array [1..1] of var 1..2): x = ([2: 1],);
