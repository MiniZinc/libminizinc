% La suma de booleanos x = s
predicate bool_sum_eq(array[int] of var bool:x, int:s) =
     let { int: c = length(x) } in
     if s < 0 then false
     elseif s == 0 then
          forall(i in 1..c)(x[i] == false)
     elseif s < c then
          let {
                % cp = número de bits necesarios para representar 0..c
                int: cp = floor(log2(int2float(c))),
                % z es la suma de x en binario
                array[0..cp] of var bool:z  } in
          binary_sum(x, z) /\
          % z == s
          forall(i in 0..cp)(z[i] == ((s div pow(2,i)) mod 2 == 1))
     elseif s == c then
          forall(i in 1..c)(x[i] == true)
     else false endif;

include "binarysum.mzn";
