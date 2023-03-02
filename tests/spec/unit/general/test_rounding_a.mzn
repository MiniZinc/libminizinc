/***
!Test
check_against: []
expected:
- !Result
  solution:
  - !Solution
    _output_item: -2.00, -2, -2, -2 OK
    x: -2.0
  - !Solution
    _output_item: -1.75, -1, -2, -2 OK
    x: -1.75
  - !Solution
    _output_item: -1.50, -1, -2, -2 OK
    x: -1.5
  - !Solution
    _output_item: -1.25, -1, -2, -1 OK
    x: -1.25
  - !Solution
    _output_item: -1.00, -1, -1, -1 OK
    x: -1.0
  - !Solution
    _output_item: -0.25, 0, -1, 0 OK
    x: -0.25
  - !Solution
    _output_item: -0.00, 0, 0, 0 OK
    x: -0.0
  - !Solution
    _output_item: 0.00, 0, 0, 0 OK
    x: 0.0
  - !Solution
    _output_item: 0.25, 1, 0, 0 OK
    x: 0.25
  - !Solution
    _output_item: 0.50, 1, 0, 1 OK
    x: 0.5
  - !Solution
    _output_item: 0.75, 1, 0, 1 OK
    x: 0.75
  - !Solution
    _output_item: 1.00, 1, 1, 1 OK
    x: 1.0
  - !Solution
    _output_item: 1.25, 2, 1, 1 OK
    x: 1.25
  - !Solution
    _output_item: 1.50, 2, 1, 2 OK
    x: 1.5
  - !Solution
    _output_item: 1.75, 2, 1, 2 OK
    x: 1.75
  - !Solution
    _output_item: 2.00, 2, 2, 2 OK
    x: 2.0
  status: ALL_SOLUTIONS
extra_files: []
markers: []
name: ''
options:
  all_solutions: true
solvers:
- gecode
type: solve

***/

var -2.0..2.0: x;
var int: c = ceil(x);
var int: f = floor(x);
var int: r = round(x);

constraint exists(i in [-2.0, -1.75, -1.5, -1.25, -1.0, -0.25, 0.0, 0.25, 0.5, 0.75, 1.0, 1.25, 1.5, 1.75, 2.0])(x=i);

output [ "\(2, 2, x), \(c), \(f), \(r) " ++ let {
    constraint assert(fix(c)=ceil(fix(x)), "incorrect ceil");
    constraint assert(fix(f)=floor(fix(x)), "incorrect floor");
    constraint assert(fix(r)=round(fix(x)), "incorrect round"); } in "OK" ];
