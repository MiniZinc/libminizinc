/***
!Test
expected:
- !Result
  solution: !Solution
    nchange: 2
    partitions:
    - !!set {1, 3}
    - !!set {4}
    - !!set {2, 6}
    variables: [6, 6, 2, 1, 3, 3, 1, 6, 2, 2, 2]
***/

% Regression test for the bug descibed in var_self_assign_bug.mzn.
% (This is the model from the original bug report.)

% Global constraint in MiniZinc.
% 
% From Global Constraint Catalogue
% http://www.emn.fr/x-info/sdemasse/gccat/Cchange_parition.html
% """
% NCHANGE is the number of times that the following constraint holds: 
% X and Y do not belong to the same partition of the collection PARTITIONS. 
% X and Y correspond to consecutive variables of the collection VARIABLES.
% 
% Example
%  (
%     2,​<
%     var-6,
%     var-6,
%     var-2,
%     var-1,
%     var-3,
%     var-3,
%     var-1,
%     var-6,
%     var-2,
%     var-2,
%     var-2
%     >,
%     <
%     p-<1, 3>,
%     p-<4>,​
%     p-<2,6>
%     >
%     )
%
% In the example we have the following two changes:
% * One change between values 2 and 1 (since 2 and 1 respectively belong 
%   to the third and the first partition),
% * One change between values 1 and 6 (since 1 and 6 respectively belong 
%   to the first and the third partition).
%
% Consequently the change_partition constraint holds since its first 
% argument NCHANGE is assigned to 2.
% """

% 
% Model created by Hakan Kjellerstrand, hakank@bonetmail.com
% See also my MiniZinc page: http://www.hakank.org/minizinc

include "globals.mzn"; 

int: n = 11;
int: m = 3;
set of int: S = {1,2,3,4,6};
array[1..n] of var S: variables;
array[1..3] of var set of S: partitions;
var int: nchange;

output [
    "nchange = ", show(nchange), ";\n",
    "partitions = ", show(partitions), ";\n",
    "variables = ", show(variables), ";\n"
];

% a variant of the partition_set from globals.mzn where universe is
% a var set of int (instead of par set of int)
predicate partition_set2(array[int] of var set of int: S,
                        var set of int: universe) =
    all_disjoint(S) /\ universe == array_union(i in index_set(S)) ( S[i] )
;


%
% change_partition(NCHANGE, VARIABLES, PARTITIONS)
%
predicate change_partition(var int: nchange, array[int] of var int: variables, array[int] of var set of int: partitions) =
  let {
     int: lbv = min(index_set(variables)),
     int: ubv = max(index_set(variables)),
     int: lbp = min(index_set(partitions)),
     int: ubp = max(index_set(partitions))
  }
  in
  % number of patition changes
  nchange = n - sum(v in lbv+1..ubv, p in lbp..ubp) (
     bool2int(
       variables[v-1] in partitions[p]  /\ variables[v] in partitions[p]
     )
   ) - 1
  /\
  partition_set2(partitions, array_union(i in lbp..ubp) (partitions[i]))

  /\ % for generating partitions: exclude empty partitions
  forall(i in lbp..ubp) (
     card(partitions[i]) > 0
  )
;

predicate cp1d(array[int] of int: x, array[int] of var int: y) =
  assert(index_set(x) = index_set(y),
           "cp1d: x and y have different sizes",
    forall(i in index_set(x)) (
        x[i] = y[i]
    )
  )
; 

predicate cp1d(array[int] of set of int: x, array[int] of var set of int: y) =
  assert(index_set(x) = index_set(y),
           "cp1d: x and y have different sizes",
    forall(i in index_set(x)) (
        x[i] = y[i]
    )
  )
; 


solve satisfy;

constraint
  cp1d([6,6,2,1,3,3,1,6,2,2,2], variables)
  /\
  cp1d([
    {1,3},
    {4},
    {2,6}], partitions)
  /\
  change_partition(nchange, variables, partitions)
  /\
  nchange = 2
;
