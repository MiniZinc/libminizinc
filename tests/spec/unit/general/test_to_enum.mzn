/***
!Test
solvers: [gecode]
expected: !Result
  solution: !Solution
    par_int: b
    var_int: c
    array_of_int: [a, b, c]
    array_of_var_int: [a, b, c]
    array_of_set_of_int: [!!set {a}, !!set {a, b}, !!set {a, b, c}]
    array_of_var_set_of_int: [!!set {a}, !!set {a, b}, !!set {a, b, c}]
    set_of_int: !!set {a, b, c}
    var_set_of_int: !!set {a, b, c}
***/

enum X = {a, b, c};

var 1..3: i;
array [1..3] of var 1..3: j;
var set of 1..3: k;
array [1..3] of var set of 1..3: l;

constraint j[1] < j[2] /\ j[2] < j[3];
constraint card(k) = 3;
constraint forall (x in 1..3) (card(l[x]) = x);
constraint 1 in l[1] /\ not (3 in l[2]);

X: par_int :: add_to_output = to_enum(X, 2);
var X: var_int :: add_to_output = to_enum(X, i);
array [1..3] of X: array_of_int :: add_to_output = to_enum(X, [1, 2, 3]);
array [1..3] of var X: array_of_var_int :: add_to_output = to_enum(X, j);
set of X: set_of_int :: add_to_output = to_enum(X, {1, 2, 3});
var set of X: var_set_of_int :: add_to_output = to_enum(X, k);
array [1..3] of set of X: array_of_set_of_int :: add_to_output = to_enum(X, [{1}, {1, 2}, {1, 2, 3}]);
array [1..3] of var set of X: array_of_var_set_of_int :: add_to_output = to_enum(X, l);

solve maximize i;