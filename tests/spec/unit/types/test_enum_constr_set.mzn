/***
!Test
solvers: [gecode]
expected: !Result
  status: SATISFIED
***/


enum A = {A1,A2,A3};
enum B = {B1,B2,B3} ++ BA(A) ++ {B4,B5,B6};
set of B: bs = BA();
constraint assert(bs == BA(A), "bs != BA(A)");
