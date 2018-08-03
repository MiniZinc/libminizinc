% 布尔型变量x的总和 = s
predicate bool_sum_eq(array[int] of var bool:x, int:s) =
     let { int: c = length(x) } in
     if s < 0 then false 
     elseif s == 0 then
          forall(i in 1..c)(x[i] == false)
     elseif s < c then
          let { % cp = number of bits required for representing 0..c
                int: cp = floor(log2(int2float(c))),
                % z is sum of x in binary 
                array[0..cp] of var bool:z  } in
          binary_sum(x, z) /\ 
          % z == s
          forall(i in 0..cp)(z[i] == ((s div pow(2,i)) mod 2 == 1)) 
     elseif s == c then
          forall(i in 1..c)(x[i] == true)
     else false endif;

include "binarysum.mzn";
