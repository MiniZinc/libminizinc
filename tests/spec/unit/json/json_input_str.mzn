/***
!Test
extra_files:
- json_input_str.json
expected: !Result
  solution: !Solution
    s: "abcd"
    ss: ["abc", "def", "ghi"]
***/

string: s ::add_to_output;
array[int] of string: ss ::add_to_output;

