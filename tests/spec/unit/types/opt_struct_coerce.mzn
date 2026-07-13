/***
!Test
solvers: [gecode]
expected: !Result
  status: SATISFIED
  solution: !Solution
    _output_item: |-
      present|absent|present
      ok
***/

% A present struct is a subtype of the corresponding optional struct, so both a plain struct and
% `<>' can be passed where an `opt' struct is expected.
function string: f(opt tuple(int, int): t) = if occurs(t) then "present" else "absent" endif;

% An overload on the present type is preferred over the optional one for a present argument.
function string: g(tuple(int, int): t) = "present";
function string: g(opt tuple(int, int): t) = "absent";

% `opt' does not distribute into the fields: `record(opt int: a)' is a *different* type from
% `opt record(int: a)', and neither is a subtype of the other.
function string: h(record(opt int: a): r) = "ok";
record(opt int: a): fieldOpt = (a: <>);

output [
  f((1, 2)), "|", f(<>), "|", g((1, 2)), "\n",
  h(fieldOpt),
];
