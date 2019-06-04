/**  @group globals.packing
  Requires that each item \p i with weight \a w[\p i], be put into \a bin[\p i] such 
  that the sum of the weights of the items in each bin \p b does not exceed the 
  capacity \a c[\p b].

  Assumptions:
  - forall \p i, \a w[\p i] >=0
  - forall \p b, \a c[\p b] >=0
*/
predicate fzn_bin_packing_capa(array[int] of int: c,
                           array[int] of var int: bin,
                           array[int] of int: w) =
      forall( i in index_set(bin) ) (
            min(index_set(c)) <= bin[i] /\ bin[i] <= max(index_set(c))
      )
    /\
      forall( b in index_set(c) ) (
            c[b] >= sum ( i in index_set(bin) ) (
                w[i] * bool2int( bin[i] = b )
            )
      );

%-----------------------------------------------------------------------------%
