/***
!Test
check_against: []
expected:
- !Result
  solution: !Solution
    b: true
    x:
    - false
    - false
    - false
  status: SATISFIED
extra_files: []
markers: []
name: ''
options:
  all_solutions: false
solvers:
- gecode
type: solve

***/

var bool: b;
array[1..3] of var bool: x;
constraint if b then true else x = [true, true, true] endif;
 
% constraint
% let {
% set of int: X = 1..10;
% array [X] of var bool: ys;
% } in
% if global then
%   true
% else
%   % forall (x in X) (ys[x]) % works
%   ys = [true | x in X] % seg fault
% endif;