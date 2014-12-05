% FlatZinc built-in redefinitions for G12/LazyFD.

predicate array_bool_element(var int : idx, array[int] of bool : arr, 
  var bool : v) =
  let { int : N = length(arr),
        array[1..N] of var bool : tmp = [idx = I | I in 1..N]
      } 
  in
      exists(I in 1..N)(tmp[I]) 
  /\  forall(I in 1..N) (tmp[I] -> v = arr[I]);

predicate array_var_bool_element(var int : idx, array[int] of var bool : arr, 
  var bool : v) =
  let { int : N = length(arr),
        array[1..N] of var bool : tmp = [idx = I | I in 1..N]
      } 
  in
      exists(I in 1..N)(tmp[I]) 
  /\  forall(I in 1..N) (tmp[I] -> v = arr[I]);

predicate array_bool_xor(array[int] of var bool: bs) =
    let { int: bs_lower = min(index_set(bs)),
          int: bs_upper = max(index_set(bs)),
          int: n = length(bs)
    } in
        if n == 1 then bs[bs_lower] else
        if n == 2 then bs[bs_lower] xor bs[bs_upper] else
        if n == 3 then bs[bs_lower] = (bs[bs_lower + 1] = bs[bs_upper])
        else
            let { int: cs_lower = bs_lower + 1,
                  int: cs_upper = bs_upper - 1,
                  array [cs_lower..cs_upper] of var bool: cs
            } in
                forall(i in cs_lower..cs_upper-1)(
                    cs[i+1] = bs[i+1] xor cs[i]
                )
                /\ (cs[cs_lower] = bs[bs_lower] xor bs[bs_lower + 1])
                /\ (bs[bs_upper] xor cs[cs_upper])
        endif endif endif;

predicate int_abs(var int: a, var int: b) = 
  b >= 0 /\ b >= a /\ b >= -a /\ (b <= a \/ b <= - a);

predicate int_mod(var int: a, var int: b, var int: c) = 
  let { 
    int : bnd = max([lb(a), ub(a), -lb(a), -ub(a)]),
    var -bnd .. bnd : z
  } in
  ( b >= 0 -> c >= 0 /\ c < b) /\
  ( b <= 0 ->  c <= 0 /\ c > b) /\
    b * z + c = a;
