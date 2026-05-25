/***
!Test
extra_files:
- alias_record_json_input.json
solvers: [gecode]
expected: !Result
  solution: !Solution
  ports: [["LCORE"], ["MCORE"]]
***/

enum Unit;

type Port = record(Unit: x);
array[int] of Port: ports ::output;
