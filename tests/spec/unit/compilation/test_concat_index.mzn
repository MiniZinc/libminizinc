/***
!Test
solvers: [gecode]
expected: !Result
  solution: !Solution
***/

any: x = concat_index_1([|1,2,3|4,5,6|], [|7,8,9|]);
any: y = concat_index_2([|1,2,3|4,5,6|], [|7|8|]);
any: z1 = concat_index_1([(i,j,k):i*j*k | i,j,k in 1..2], [(1,i,j):42+i+j | i,j in 1..2]);
any: z2 = concat_index_2([(i,j,k):i*j*k | i,j,k in 1..2], [(i,1,j):42+i+j | i,j in 1..2]);
any: z3 = concat_index_3([(i,j,k):i*j*k | i,j,k in 1..2], [(i,j,1):42+i+j | i,j in 1..2]);
any: e1 = concat_index_1([|1,2,3|4,5,6|], array2d(1..0,1..3,[]));

constraint assert(array1d(x) = [1,2,3,4,5,6,7,8,9], "Error");
constraint assert(array1d(y) = [1,2,3,7,4,5,6,8], "Error");
constraint assert(array1d(z1) = [1, 2, 2, 4, 2, 4, 4, 8, 44, 45, 45, 46], "Error");
constraint assert(array1d(z2) = [1, 2, 2, 4, 44, 45, 2, 4, 4, 8, 45, 46], "Error");
constraint assert(array1d(z3) = [1, 2, 44, 2, 4, 45, 2, 4, 45, 4, 8, 46], "Error");
constraint assert(array1d(e1) = [1,2,3,4,5,6], "Error");

output ["OK"];
