/***
!Test
solvers: [gecode, cp-sat]
options:
  all_solutions: true
expected: !Result
  solution: !SolutionSet
  - !Solution
    x: null
    y: null
    un: !!set {}
    sd: !!set {}
    it: null
    df: null
    cd: null
    ai: null
    au: !!set {}
  - !Solution
    x: null
    y: !!set {2}
    un: !!set {2}
    sd: !!set {2}
    it: !!set {2}
    df: null
    cd: null
    ai: !!set {2}
    au: !!set {2}
  - !Solution
    x: null
    y: !!set {}
    un: !!set {}
    sd: !!set {}
    it: !!set {}
    df: null
    cd: null
    ai: !!set {}
    au: !!set {}
  - !Solution
    x: !!set {1}
    y: null
    un: !!set {1}
    sd: !!set {1}
    it: !!set {1}
    df: !!set {1}
    cd: 1
    ai: !!set {1}
    au: !!set {1}
  - !Solution
    x: !!set {1, 2}
    y: null
    un: !!set {1, 2}
    sd: !!set {1, 2}
    it: !!set {1, 2}
    df: !!set {1, 2}
    cd: 2
    ai: !!set {1, 2}
    au: !!set {1, 2}
  - !Solution
    x: !!set {1}
    y: !!set {2}
    un: !!set {1, 2}
    sd: !!set {1, 2}
    it: !!set {}
    df: !!set {1}
    cd: 1
    ai: !!set {}
    au: !!set {1, 2}
  - !Solution
    x: !!set {1}
    y: !!set {}
    un: !!set {1}
    sd: !!set {1}
    it: !!set {}
    df: !!set {1}
    cd: 1
    ai: !!set {}
    au: !!set {1}
  - !Solution
    x: !!set {1, 2}
    y: !!set {2}
    un: !!set {1, 2}
    sd: !!set {1}
    it: !!set {2}
    df: !!set {1}
    cd: 2
    ai: !!set {2}
    au: !!set {1, 2}
  - !Solution
    x: !!set {1, 2}
    y: !!set {}
    un: !!set {1, 2}
    sd: !!set {1, 2}
    it: !!set {}
    df: !!set {1, 2}
    cd: 2
    ai: !!set {}
    au: !!set {1, 2}
  status: ALL_SOLUTIONS
***/

% Operations on var opt sets ignore absent operands
% (union/symdiff of two absent sets is {}, intersect/diff with an absent lhs is absent)
var opt set of 1..2: x;
var opt set of {2}: y;
var set of int: un :: output = x union y;
var set of int: sd :: output = x symdiff y;
var opt set of int: it :: output = x intersect y;
var opt set of int: df :: output = x diff y;
var opt int: cd :: output = card(x);
var opt set of int: ai :: output = array_intersect([x, y]);
var set of int: au :: output = array_union([x, y]);
constraint x != {2} /\ x != {};
