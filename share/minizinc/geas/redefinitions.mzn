%% New annotations
annotation assume(array[int] of var bool: b);
annotation int_priority(array[int] of var int: xs,
  array[int] of ann: br, ann: sel);
annotation bool_priority(array[int] of var bool: xs,
  array[int] of ann: br, ann: sel);

%% Half Reifications
predicate int_lin_eq_imp(array [int] of int: as, array [int] of var int: bs,
                         int: c, var bool: r);
predicate int_lin_ne_imp(array [int] of int: as, array [int] of var int: bs,
                         int: c, var bool: r);
predicate int_lin_le_imp(array [int] of int: as, array [int] of var int: bs,
                         int: c, var bool: r);

%% Special cases for binary-ish x, y.
predicate int_lin_eq_reif(
  array [int] of int: cs, array [int] of var int: xs, int: k, var bool: r) =
  let { var bool: a; var bool: b } in
  (r <-> (a /\ b)) /\ int_lin_le_reif(cs, xs, k, a)
  /\ int_lin_le_reif([-c | c in cs], xs, -k, b);

predicate bool_xor(var bool: x, var bool: y, var bool: r) =
     bool_clause([x,y],[r])
  /\ bool_clause([],[x,y,r])
  /\ bool_clause([x, r], [y])
  /\ bool_clause([y, r], [x]);

predicate set_in_reif(var int: x, set of int: s, var bool: r) =
  if card(s) = max(s) - min(s) + 1 then
    r <-> (x >= min(s) /\ x <= max(s)) 
  else
    bool_clause([x = k | k in s], [r]) /\ forall (k in s) (x = k -> r)
  endif;

predicate int_pow(var int: x, var int: y, var int: z) =
  let {
    array [dom(x), dom(y)] of int: A = array2d(
      dom(x),
      dom(y),
      [ pow(a, b) | a in dom(x), b in dom(y) ]
    );
  } in z = A[x, y];

predicate int_pow(var int: x, int: y, var int: z) =
  if y = 0 then z = 1
  elseif y = 1 then z = x
  else
    let { var int: zp ::is_defined_var ;
          constraint int_pow(x, y div 2, zp) :: defines_var(zp); } in
    if y mod 2 = 0 then
      z = zp * zp
    else
      z = x * zp * zp
    endif
  endif;
