/***
!Test
extra_files:
- alias_nested_record_json_input.json
solvers: [gecode]
expected: !Result
  solution: !Solution
  r: [[7]]
***/

type Inner = record(int: a);
type Outer = record(Inner: i);
Outer: r ::output;
