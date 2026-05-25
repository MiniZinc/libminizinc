/***
!Test
extra_files:
- alias_set_json_input.json
solvers: [gecode]
expected: !Result
  solution: !Solution
  us: !!set {LCORE, MCORE}
***/

enum Unit;
type Units = set of Unit;
Units: us ::output;
