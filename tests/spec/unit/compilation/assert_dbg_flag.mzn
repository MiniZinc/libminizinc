/***
!Test
solvers: [gecode]
options:
  debug: true
expected: !Error
  type: MiniZincError
***/

constraint assert_dbg(false, "This fails with the --debug flag");
