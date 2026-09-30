/***
!Test
extra_files:
- record_missing_opt_field_json.json
solvers: [gecode]
expected: !Result
  status: SATISFIED
  solution: !Solution
    a: [{"x": null, "y": 1}, {"x": 2, "y": 3}, {"x": null, "y": 4}]
    merged: [{"x": null, "y": 1}, {"x": 2, "y": 3}]
    r: {"x": null, "y": 5}
***/

% JSON objects may omit the optional fields of the declared record type, including record types
% that are merged using ++. The missing fields are absent.

array[1..3] of record(opt int: x, int: y): a :: output;
type Y = record(int: y);
type X = record(opt int: x);
array[1..2] of Y ++ X: merged :: output;
Y ++ X: r :: output;
