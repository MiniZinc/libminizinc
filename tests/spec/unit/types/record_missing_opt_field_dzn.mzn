/***
!Test
extra_files:
- record_missing_opt_field_dzn.dzn
solvers: [gecode]
expected: !Result
  status: SATISFIED
  solution: !Solution
    r: {"x": null, "y": 4}
    a: [{"x": null, "y": 1}, {"x": 2, "y": 3}, {"x": null, "y": 4}]
    a2d: [[{"x": null, "y": 1}, {"x": null, "y": 2}], [{"x": 3, "y": 4}, {"x": null, "y": 5}]]
    a2d_call: [[{"x": null, "y": 1}, {"x": 2, "y": 3}]]
    indexed: [{"x": null, "y": 1}, {"x": 2, "y": 3}]
    alias: [{"x": null, "y": 1}, {"x": 2, "y": 3}]
    alias_array: [{"x": null, "y": 1}, {"x": 2, "y": 3}]
    merged: [{"x": null, "y": 1}, {"x": 2, "y": 3}]
    opt_alias: [{"x": null, "y": 1}, {"x": 2, "y": 3}]
    nested: [{"c": 2, "r": {"a": null, "b": 1}}, {"c": 3, "r": {"a": 4, "b": 5}}]
    tup: [[{"x": null, "y": 1}, 2], [{"x": 3, "y": 4}, 5]]
    enum_field: [{"c": null, "y": 1}, {"c": "A", "y": 2}]
    opt_structs: [{"t": null, "u": null, "y": 1}, {"t": [1, true], "u": {"a": 2}, "y": 3}]
    v: 8
***/

% Record literals in data files may omit the optional fields of the declared record type.
% The missing fields are absent.

record(opt int: x, int: y): r :: output;
array[1..3] of record(opt int: x, int: y): a :: output;
array[1..2, 1..2] of record(opt int: x, int: y): a2d :: output;
array[1..1, 1..2] of record(opt int: x, int: y): a2d_call :: output;
array[1..2] of record(opt int: x, int: y): indexed :: output;

type R = record(opt int: x, int: y);
array[1..2] of R: alias :: output;
type Rs = array[1..2] of record(opt int: x, int: y);
Rs: alias_array :: output;
type Y = record(int: y);
type X = record(opt int: x);
array[1..2] of Y ++ X: merged :: output;
type OptInt = opt int;
array[1..2] of record(OptInt: x, int: y): opt_alias :: output;

array[1..2] of record(record(opt int: a, int: b): r, int: c): nested :: output;
array[1..2] of tuple(record(opt int: x, int: y), int): tup :: output;
enum C = {A, B};
array[1..2] of record(opt C: c, int: y): enum_field :: output;
array[1..2] of record(opt tuple(int, bool): t, opt record(int: a): u, int: y): opt_structs :: output;

% Records with var fields
array[1..2] of record(opt int: x, var int: y): vars;
var 1..10: v :: output;
constraint v = vars[1].y + vars[2].y + (vars[1].x default 0) + (vars[2].x default 10);
