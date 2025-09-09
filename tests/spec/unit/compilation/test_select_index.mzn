/***
!Test
solvers: [gecode]
expected: !Result
  solution: !Solution
***/

any: x1::output = select_from_index_1([|1,2,3|4,5,6|], {1});
any: x2::output = select_from_index_1([|1,2,3|4,5,6|], {2});
any: x3::output = select_from_index_2([|1,2,3|4,5,6|], {1});
any: x4::output = select_from_index_2([|1,2,3|4,5,6|], {2});
any: x5::output = select_from_index_2(array2d(1..2,3..5,[|1,2,3|4,5,6|]), {});
any: x6::output = select_from_index_2(array2d(1..2,3..5,[|1,2,3|4,5,6|]), {3,5});

constraint assert(array1d(x1) = [1,2,3], "Error");
constraint assert(array1d(x2) = [4,5,6], "Error");
constraint assert(array1d(x3) = [1,4], "Error");
constraint assert(array1d(x4) = [2,5], "Error");
constraint assert(array1d(x5) = [], "Error");
constraint assert(array1d(x6) = [1,3,4,6], "Error");

output ["OK"];
