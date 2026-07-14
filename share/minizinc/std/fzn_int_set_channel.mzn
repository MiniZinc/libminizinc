predicate fzn_int_set_channel(array [int] of var int: ints, array [int] of var set of int: sets) =
  forall (i in index_set(ints)) (ints[i] in index_set(sets)) /\
    forall (j in index_set(sets)) (sets[j] subset index_set(ints)) /\
    forall (i in index_set(ints), j in index_set(sets)) (ints[i] = j <-> i in sets[j]);

%-----------------------------------------------------------------------------%
