% the sum of booleans x = s
predicate bool_sum_eq(array[int] of var bool:x, int:s) =
     let { int: c = length(x),
           array[1..c] of var bool: y = [x[i] | i in index_set(x)] 
     } in
     rec_bool_sum_eq(y, 1, s);

predicate rec_bool_sum_eq(array[int] of var bool:x, int: f, int:s) =
     let { int: c = length(x) } in
     if s < 0 then false 
     elseif s == 0 then
          forall(i in f..c)(x[i] == false)
     elseif s < c - f + 1 then
          (x[f] == true /\ rec_bool_sum_eq(x,f+1,s-1)) \/
          (x[f] == false /\ rec_bool_sum_eq(x,f+1,s))
     elseif s == c - f + 1 then
          forall(i in f..c)(x[i] == true)
     else false endif;
