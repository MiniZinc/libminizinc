/***
!Test
solvers: [gecode]
expected: !Result
  solution: !Solution
    in_: [true, true, true, true, true, true, false, true, false, true, true, false, false, true, true]
    sub: [true, true, true, true, true, true, true, true, true, true, true, false, true, false, true, true, false, false, true, true, true, false, false, false, true]
    sup: [true, true, true, true, true, true, true, false, false, false, true, true, true, false, false, true, true, false, true, false, true, true, true, true, true]
    lt: [true, true, true, true, true, true, false, true, true, true, true, false, false, true, true, true, false, false, false, false, true, false, false, true, false]
    le: [true, true, true, true, true, true, true, true, true, true, true, false, true, true, true, true, false, false, true, false, true, false, false, true, true]
    un: [!!set {}, !!set {}, !!set {1.5}, !!set {2.5}, !!set {1.5, 2.5}, !!set {}, !!set {}, !!set {1.5}, !!set {2.5}, !!set {1.5, 2.5}, !!set {1.5}, !!set {1.5}, !!set {1.5}, !!set {1.5, 2.5}, !!set {1.5, 2.5}, !!set {2.5}, !!set {2.5}, !!set {1.5, 2.5}, !!set {2.5}, !!set {1.5, 2.5}, !!set {1.5, 2.5}, !!set {1.5, 2.5}, !!set {1.5, 2.5}, !!set {1.5, 2.5}, !!set {1.5, 2.5}]
    sd: [!!set {}, !!set {}, !!set {1.5}, !!set {2.5}, !!set {1.5, 2.5}, !!set {}, !!set {}, !!set {1.5}, !!set {2.5}, !!set {1.5, 2.5}, !!set {1.5}, !!set {1.5}, !!set {}, !!set {1.5, 2.5}, !!set {2.5}, !!set {2.5}, !!set {2.5}, !!set {1.5, 2.5}, !!set {}, !!set {1.5}, !!set {1.5, 2.5}, !!set {1.5, 2.5}, !!set {2.5}, !!set {1.5}, !!set {}]
    it: [null, !!set {}, !!set {1.5}, !!set {2.5}, !!set {1.5, 2.5}, !!set {}, !!set {}, !!set {}, !!set {}, !!set {}, !!set {1.5}, !!set {}, !!set {1.5}, !!set {}, !!set {1.5}, !!set {2.5}, !!set {}, !!set {}, !!set {2.5}, !!set {2.5}, !!set {1.5, 2.5}, !!set {}, !!set {1.5}, !!set {2.5}, !!set {1.5, 2.5}]
    df: [null, null, null, null, null, !!set {}, !!set {}, !!set {}, !!set {}, !!set {}, !!set {1.5}, !!set {1.5}, !!set {}, !!set {1.5}, !!set {}, !!set {2.5}, !!set {2.5}, !!set {2.5}, !!set {}, !!set {}, !!set {1.5, 2.5}, !!set {1.5, 2.5}, !!set {2.5}, !!set {1.5}, !!set {}]
    cd: [null, 0, 1, 1, 2]
    ai: [null, !!set {}, !!set {1.5}, !!set {2.5}, !!set {1.5, 2.5}, !!set {}, !!set {}, !!set {}, !!set {}, !!set {}, !!set {1.5}, !!set {}, !!set {1.5}, !!set {}, !!set {1.5}, !!set {2.5}, !!set {}, !!set {}, !!set {2.5}, !!set {2.5}, !!set {1.5, 2.5}, !!set {}, !!set {1.5}, !!set {2.5}, !!set {1.5, 2.5}]
    au: [!!set {}, !!set {}, !!set {1.5}, !!set {2.5}, !!set {1.5, 2.5}, !!set {}, !!set {}, !!set {1.5}, !!set {2.5}, !!set {1.5, 2.5}, !!set {1.5}, !!set {1.5}, !!set {1.5}, !!set {1.5, 2.5}, !!set {1.5, 2.5}, !!set {2.5}, !!set {2.5}, !!set {1.5, 2.5}, !!set {2.5}, !!set {1.5, 2.5}, !!set {1.5, 2.5}, !!set {1.5, 2.5}, !!set {1.5, 2.5}, !!set {1.5, 2.5}, !!set {1.5, 2.5}]
    ai_empty: null
***/

% Par opt set operations work on float sets as well
array [1..5] of opt set of float: v = [<>, {}, {1.5}, {2.5}, {1.5, 2.5}];
array [1..3] of opt float: oe = [<>, 1.5, 2.5];
set of int: I = 1..5;
array [int] of bool: in_ :: output = [oe[k] in v[j] | k in 1..3, j in I];
array [int] of bool: sub :: output = [v[i] subset v[j] | i, j in I];
array [int] of bool: sup :: output = [v[i] superset v[j] | i, j in I];
array [int] of bool: lt :: output = [v[i] < v[j] | i, j in I];
array [int] of bool: le :: output = [v[i] <= v[j] | i, j in I];
array [int] of set of float: un :: output = [v[i] union v[j] | i, j in I];
array [int] of set of float: sd :: output = [v[i] symdiff v[j] | i, j in I];
array [int] of opt set of float: it :: output = [v[i] intersect v[j] | i, j in I];
array [int] of opt set of float: df :: output = [v[i] diff v[j] | i, j in I];
array [int] of opt int: cd :: output = [card(v[i]) | i in I];
array [int] of opt set of float: ai :: output = [array_intersect([v[i], v[j]]) | i, j in I];
array [int] of set of float: au :: output = [array_union([v[i], v[j]]) | i, j in I];
array [1..0] of opt set of float: emp = [];
opt set of float: ai_empty :: output = array_intersect(emp);
