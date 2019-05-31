%-----------------------------------------------------------------------------%
% Requires that the set 'x' is strictly lexicographically less than set 'y'.
% Compares them from first to last element
%-----------------------------------------------------------------------------%

predicate fzn_set_less(var set of int: x,
                       var set of int: y) =
    let { set of int: U = ub(x) union ub(y);
	  int: l = min(U);
          int: u = max(U);
          array[U] of var bool: xb = [ i in x | i in U];
          var l-1..u: xmax = max(x union {l-1});
          array[U] of var bool: yb = [ i in y | i in U];
          var l-1..u: ymax = max(y union {l-1});
          array[U] of var bool: b; 
    } in
    b[l] /\
    forall(i in l..u-1)
          (b[i] = let { var 1..4: cmp = 2*xb[i] + yb[i] + 1 } in
                  [ b[i+1], xmax < i, ymax > i, b[i+1] ][cmp] )
    /\
    b[u] = (not xb[u] /\ yb[u]);

predicate fzn_set_less_reif(var set of int: x,
                            var set of int: y,
                            var bool: c) =

    let { set of int: U = ub(x) union ub(y);
	  int: l = min(U);
          int: u = max(U);
          array[U] of var bool: xb = [ i in x | i in U];
          var l-1..u: xmax = max(x union {l-1});
          array[U] of var bool: yb = [ i in y | i in U];
          var l-1..u: ymax = max(y union {l-1});
          array[U] of var bool: b; 
    } in
    (c <-> b[l]) /\
    forall(i in l..u-1)
          (b[i] = let { var 1..4: cmp = 2*xb[i] + yb[i] + 1 } in
                  [ b[i+1], xmax < i, ymax > i, b[i+1] ][cmp] )
    /\
    b[u] = (not xb[u] /\ yb[u]);

%-----------------------------------------------------------------------------%
