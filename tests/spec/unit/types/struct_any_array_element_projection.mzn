/***
!Test
solvers: [gecode]
expected: !Result
  solution: !Solution
    c: 3
    w: 5
***/

% Regression test for the same eval_par abort, reproduced through a polymorphic
% `any $T` function that takes a whole array of records mixing a par field
% (`bas`) and a var field (`by`) and returns one element. This is the shape the
% object compiler hits via a user-defined `'[]'` over `B_objects[i].bas`
% (evaluating the function argument forces eval_par over every record element,
% including the unflattened var field).

function any $T: get(array[int] of any $T: x, int: i) = x[i];

array[1..2] of record(set of int: bas, var 1..5: by): objs =
  [ (bas: {1, 2}, by: let { var 1..5: y } in y),
    (bas: {1, 2, 3}, by: let { var 1..5: y } in y) ];

constraint card(get(objs, 2).bas) = 3;   % par projection of the par field
constraint get(objs, 2).by = 5;          % var projection of the var field

solve satisfy;

any: c :: output = card(get(objs, 2).bas);
any: w :: output = get(objs, 2).by;
