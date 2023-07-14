/***
!Test
solvers: [gecode]
options:
  debug: true
expected: !Error
  type: AssertionError
***/

constraint assert_dbg(false, "This fails with the --debug flag");
