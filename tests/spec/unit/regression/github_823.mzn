/***
!Test
solvers: [gecode]
expected: !Result
  solution: !Solution
    dzn_str: "('x + \\': 1)"
    json_str: '{"x + \\": 1}'
***/
any: r = ('x + \': 1);
string: dzn_str :: output = show(r);
string: json_str :: output = showJSON(r);
