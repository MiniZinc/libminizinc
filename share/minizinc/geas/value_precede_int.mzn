predicate value_precede_int(int: s, int: t, array[int] of var int: x) =
    let {
      var index_set(x): pos_s;
      constraint forall (i in index_set(x)) (
        (pos_s <= i) -> (x[i] = s) \/ (pos_s < i)
      );
    } in forall(i in index_set(x)) (
      (x[i] = t) -> (pos_s < i)
    );
