/***
!Test
solvers: [gecode, chuffed]
expected:
- !Result
  solution: !Solution
    pos:
    - [1, 1, 1]
    - [1, 1, 1]
    - [1, 1, 1]
    - [1, 1, 1]
    - [1, 1, 1]
    - [1, 1, 1]
    - [1, 1, 1]
    - [1, 1, 1]
    - [1, 1, 1]
    - [1, 1, 1]
    - [1, 1, 1]
    - [1, 1, 1]
    - [1, 1, 1]
- !Result
  solution: !Solution
    pos:
    - [1, 1, 1]
    - [1, 1, 1]
    - [1, 1, 1]
    - [1, 1, 1]
    - [1, 1, 1]
    - [2, 1, 1]
    - [3, 1, 1]
    - [4, 1, 1]
    - [5, 1, 1]
    - [4, 1, 1]
    - [3, 1, 1]
    - [2, 1, 1]
    - [1, 1, 1]
- !Result
  solution: !Solution
    pos:
    - [6, 6, 6]
    - [6, 6, 6]
    - [6, 6, 6]
    - [6, 6, 6]
    - [6, 6, 6]
    - [6, 6, 6]
    - [6, 6, 6]
    - [6, 6, 6]
    - [6, 6, 6]
    - [6, 6, 6]
    - [6, 6, 6]
    - [6, 6, 6]
    - [6, 6, 6]
***/

% This caused mzn2fzn (r9627 and before) to abort.


% NOTE: we cannot run this on LazyFD because it the redefinition of
% array_var_bool_element below will conflict with the LazyFD backend's
% redefinitions file.  We run this only with the std fd variant since
% we know that does not use any redefinitions.

array[1..6,1..6] of bool: linked;
array[1..13,1..3] of var 1..6: pos;

constraint
  forall(t in 1..3, x in 2..13)(linked[pos[x,t],pos[x-1,t]]);

solve satisfy;

linked = [| true,  true,  false, false, false, false
          | true,  true,  true,  false, false, false
          | false, true,  true,  true,  false, true
          | false, false, true,  true,  true,  false
          | false, false, false, true,  true,  false
          | false, false, true,  false, false, true |];

% From g12/zinc/lib/minizinc/g12_lazyfd/redefinitions.mzn:
%
predicate array_var_bool_element(var int : idx, array[int] of var bool : arr, 
  var bool : v) =
  let { int : N = length(arr),
        array[1..N] of var bool : tmp = [idx = I | I in 1..N]
      } 
  in
      exists(I in 1..N)(tmp[I]) 
  /\  forall(I in 1..N) (tmp[I] -> v = arr[I]);

output ["pos = ", show(pos), ";\n"];
