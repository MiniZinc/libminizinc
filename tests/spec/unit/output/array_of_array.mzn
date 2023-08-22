/***
!Test
solvers: [gecode]
options:
  all_solutions: true
expected: !Result
  status: ALL_SOLUTIONS
  solution: !SolutionSet
  - !Solution
    x: [[[1]], [[2,2]]]
    y: [[[1],1], [[2,2],2]]
    z: [[-1,1], [0,2]]
***/

array[_] of int: len = [1,2];
array[_] of tuple(array[_] of var int): x ::output = [ ([let {var int: i} in i | _ in 1..l],) | l in len];
constraint forall (i in index_set(len), j in 1..len[i])( x[i].1[j] = len[i] );
array[_] of tuple(array[_] of var int, int): y ::output = [ ([let {var int: i} in i | _ in 1..l],l) | l in len];
constraint forall (i in index_set(len), j in 1..len[i])( y[i].1[j] = len[i] );
array[_] of tuple(int, int): z ::output = [ (l-2,l) | l in len];
