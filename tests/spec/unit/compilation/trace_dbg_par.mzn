/***
--- !Test
solvers: [gecode]
expected: !Result
  status: SATISFIED
--- !Test
solvers: [gecode]
options:
  debug: true
expected: !Result
  status: UNSATISFIABLE
***/
constraint trace_dbg(let { constraint false; } in "foo\n");
