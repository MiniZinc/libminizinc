/***
!Test
expected:
- !Result
  solution: !Solution
    nvar: 2
    partitions:
    - !!set {1, 3}
    - !!set {4}
    - !!set {2, 6}
    variables: [2, 3, 7, 1, 6, 0]
***/

% Another regression test for the problem described in var_self_assign_bug.mzn.
% (This model is from http://www.hakank.org/minizinc/cardinality_atmost_partition.mzn.)

% From Global Constraint Catalogue
% http://www.emn.fr/x-info/sdemasse/gccat/Ccardinality_atmost_partition.html
% """
% ATMOST is the maximum number of time that values of a same partition of 
% PARTITIONS are taken by the variables of the collection VARIABLES.
% 
% Example
%  (
%   2, <2, 3, 7, 1, 6, 0>,
%  <
%    p-<1, 3>,
%    p-<4>,
%    p-<2, 6>
%  >
% )
% In this example, two variables of the collection 
% VARIABLES = <2, 3, 7, 1, 6, 0> 
% are assigned to values of the first partition, no variable is assigned to 
% a value of the second partition, and finally two variables are assigned to 
% values of the last partition. As a consequence, the 
% cardinality_atmost_partition constraint holds since its first argument 
% ATMOST is assigned to the maximum number of occurrences 2.

%
% """
% 
% Model created by Hakan Kjellerstrand, hakank@bonetmail.com
% See also my MiniZinc page: http://www.hakank.org/minizinc

include "globals.mzn"; 


int: n = 6;
int: m = 3;
array[1..n] of var 0..10: variables;
array[1..m] of var set of 1..7: partitions;
var 0..9: nvar;

output [
    "nvar = ", show(nvar), ";\n",
    "partitions = ", show(partitions), ";\n",
    "variables = ", show(variables), ";\n"
];

solve satisfy;

% a variant of the partition_set from globals.mzn where universe is
% a var set of int (instead of par set of int)
predicate partition_set2(array[int] of var set of int: S,
                        var set of int: universe) =
    all_disjoint(S) /\ universe == array_union(i in index_set(S)) ( S[i] )
;


%
% cardinality_atmost_exclude_value
% 
% as cardinality_atmost but we exclude the "rest partition" index (m+1)
% (as a set for generality)
% (cf. cardinality_atmost.mzn)
%
predicate cardinality_atmost_exclude_value(var int: nvar, array[int] of var int: variables, array[int] of var int: values, var set of int: exclude) =
   forall(i in index_set(values)) (
     sum(j in index_set(variables)) (bool2int(not(values[i] in exclude) /\  values[i] = variables[j])) <= nvar
   )
;

% 
% cardinality_atmost_partition
% 
% ("Unassigned" values are put in the m+1'th partition, the "rest partition,
% and we don't care how many that are assigned to the rest partition.)
% 
predicate cardinality_atmost_partition(var int: nvar, array[int] of var int: variables, array[int] of var set of int: partitions) =
   let {
       int: lbv = min(index_set(variables)),
       int: ubv = max(index_set(variables)),
       int: lbp = min(index_set(partitions)),
       int: ubp = max(index_set(partitions)),
       array[lbv..ubv] of var lbp..ubp+1: selected_partition
   }   
   in

   % the partitions must be partitioned
   partition_set2(partitions, array_union(i in lbp..ubp) (partitions[i]))

   /\ % assign a partition index to each value
   forall(i in lbv..ubv) (
      forall(j in lbp..ubp) (
        selected_partition[i] = j  <-> variables[i] in partitions[j]
      )
   )
   /\ % assure that we have atmost nvar occurrences of each partition index 
      % (except the "rest partition")
   cardinality_atmost_exclude_value(nvar, selected_partition, selected_partition, {m+1})
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


constraint

  cp1d([2, 3, 7, 1, 6, 0], variables)
  /\
  cp1d([
      {1,3},
      {4},
      {2,6}], partitions)
  /\
  nvar = 2
  /\
  cardinality_atmost_partition(nvar, variables, partitions)
;

