/***
!Test
solvers: [gecode, cp-sat, chuffed]
options:
  all_solutions: true
expected: !Result
  solution: !SolutionSet
  - !Solution
    x: null
    y: null
    e: null
    in_: true
    sub: true
    sup: true
    le: true
  - !Solution
    x: !!set {1}
    y: null
    e: null
    in_: true
    sub: true
    sup: true
    le: true
  - !Solution
    x: !!set {}
    y: null
    e: null
    in_: true
    sub: true
    sup: true
    le: true
  - !Solution
    x: null
    y: null
    e: 1
    in_: true
    sub: true
    sup: true
    le: true
  - !Solution
    x: !!set {1}
    y: null
    e: 1
    in_: true
    sub: true
    sup: true
    le: true
  - !Solution
    x: !!set {}
    y: null
    e: 1
    in_: true
    sub: true
    sup: true
    le: true
  - !Solution
    x: null
    y: !!set {1}
    e: null
    in_: true
    sub: true
    sup: true
    le: true
  - !Solution
    x: null
    y: !!set {}
    e: null
    in_: true
    sub: true
    sup: true
    le: true
  - !Solution
    x: !!set {1}
    y: !!set {}
    e: null
    in_: true
    sub: false
    sup: true
    le: false
  - !Solution
    x: !!set {1}
    y: !!set {1}
    e: null
    in_: true
    sub: true
    sup: true
    le: true
  - !Solution
    x: !!set {}
    y: !!set {1}
    e: null
    in_: true
    sub: true
    sup: false
    le: true
  - !Solution
    x: !!set {}
    y: !!set {}
    e: null
    in_: true
    sub: true
    sup: true
    le: true
  - !Solution
    x: null
    y: !!set {}
    e: 1
    in_: false
    sub: true
    sup: true
    le: true
  - !Solution
    x: null
    y: !!set {1}
    e: 1
    in_: true
    sub: true
    sup: true
    le: true
  - !Solution
    x: !!set {1}
    y: !!set {}
    e: 1
    in_: false
    sub: false
    sup: true
    le: false
  - !Solution
    x: !!set {}
    y: !!set {}
    e: 1
    in_: false
    sub: true
    sup: true
    le: true
  - !Solution
    x: !!set {1}
    y: !!set {1}
    e: 1
    in_: true
    sub: true
    sup: true
    le: true
  - !Solution
    x: !!set {}
    y: !!set {1}
    e: 1
    in_: true
    sub: true
    sup: false
    le: true
  status: ALL_SOLUTIONS
***/

% Relations on var opt sets are true if an operand is absent (e in s is {e} subset s)
var opt set of 1..1: x;
var opt set of 1..1: y;
var opt 1..1: e;
var bool: in_ :: output = e in y;
var bool: sub :: output = x subset y;
var bool: sup :: output = x superset y;
var bool: le :: output = x <= y;
