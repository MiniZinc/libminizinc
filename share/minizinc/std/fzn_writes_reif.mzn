predicate fzn_writes_reif(array[int] of var int: I, array[int] of var int: P,
                          array[int] of var int: V, array[int] of var int: O,
                          var bool: b) =
    b <-> (
      forall(j in index_set(P))(O[P[j]] = V[j]) /\
      forall(i in index_set(I))
          (if forall(j in index_set(P))(P[j] != i) then
              O[i] = I[i]
          else true endif)
    );

%-----------------------------------------------------------------------------%
