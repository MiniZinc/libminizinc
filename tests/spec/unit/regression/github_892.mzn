/***
!Test
solvers: [gecode]
expected: !Result
  solution: !Solution
    xb: 3
    x:
      a: 1
      b: 3
      c: 2
***/

% Previously par evaluation of a record merge would only work through eval_par,
% and not through eval_array_lit, leading to the values being concatenated as
% in normal arrays

int: xb :: output = foo();
function int: foo() = x.b; % Par evaluation of the record x is done through eval_array_lit and not eval_par
any: x :: output = (a: 1, c: 2) ++ (b: 3); % If this VarDecl is placed earlier, it gets evaluated with eval_par
