include "fzn_regular.mzn";

predicate fzn_regular_reif(
  array [int] of var int: xs,
  int: Q,
  int: S,
  array [int, int] of int: d,
  int: q0,
  set of int: F,
  var bool: b,
) =
  if occurs(fzn_regular_is_decomposed) /\ deopt(fzn_regular_is_decomposed) then
    % Direct decomposition. Track the DFA state with an explicit dead state 0 (which
    % is absorbing and not accepting), so a rejected string yields b = false instead
    % of making the constraint unsatisfiable.
    if length(xs) = 0 then
      b = (q0 in F)
    else
      let {
        int: m = min(index_set(xs));
        int: n = max(index_set(xs)) + 1;
        array [0..Q, 1..S] of int: dd =
          array2d(0..Q, 1..S, [if r = 0 then 0 else d[r, c] endif | r in 0..Q, c in 1..S]);
        array [m..n] of var 0..Q: a;
      } in
        a[m] = q0 /\
        forall (i in index_set(xs)) (
          a[i + 1] = if xs[i] in 1..S then dd[a[i], xs[i]] else 0 endif
        ) /\
        b = (a[n] in F)
    endif
  else
    % Augmented-DFA construction. Append a marker symbol encoding b and post the
    % (non-reified) regular constraint, reusing a native propagator if present.
    %   States 1..Q: original; ACC = Q+1 (only accepting state); DEAD = Q+2 (a live,
    %   non-accepting sink that original dead transitions are redirected to, so that
    %   strings rejected by a missing transition can still reach ACC with b = false).
    %   Symbols 1..S: original; S+1 marks b = false, S+2 marks b = true.
    let {
      int: ACC = Q + 1;
      int: DEAD = Q + 2;
      int: QQ = Q + 2;
      int: SS = S + 2;
      array [1..QQ, 1..SS] of int: dd =
        array2d(1..QQ, 1..SS, [
          if r <= Q then
            if c <= S then (if d[r, c] = 0 then DEAD else d[r, c] endif)
            elseif c = S + 1 then (if r in F then 0 else ACC endif)
            else (if r in F then ACC else 0 endif)
            endif
          elseif r = DEAD then
            (if c <= S then DEAD elseif c = S + 1 then ACC else 0 endif)
          else
            0
          endif
        | r in 1..QQ, c in 1..SS]);
      array [int] of var int: y = xs ++ [S + 1 + bool2int(b)];
    } in
      fzn_regular(y, QQ, SS, dd, q0, {ACC})
  endif;
