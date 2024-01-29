/***
!Test
solvers: [gecode]
expected: !Result
  solution: !Solution
***/
set of int: S = 0..9;

array[S] of var opt S: x_inv;
array[S] of var set of int: x_roff =
    array1d(S,
    [ let { var opt S: s = x_inv[l]; } % trigger (1): opt
      in if occurs(s)
         then {deopt(s)}
         else {}
         endif
    | l in S ]);
array[S] of var set of int: x_occupancy =
    array1d(S,
    [ { r  
      | r in x_roff[l]
        where r <= l } % trigger (2): where
    | l in S ]);
