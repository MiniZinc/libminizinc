/**  @group globals.packing
  Requires that each item \p i with weight \a w[\p i], be put into \a bin[\p i] such 
  that the sum of the weights of the items in each bin does not exceed the 
  capacity \a c.

  Assumptions:
  - forall \p i, \a w[\p i] >=0
  - \a c >=0
*/
predicate fzn_bin_packing(int: c,
                      array[int] of var int: bin,
                      array[int] of int: w) =
        forall( b in lb_array(bin)..ub_array(bin) ) (
            c >= sum ( i in index_set(bin) ) (
                w[i] * bool2int( bin[i] == b )
            )
        );

%-----------------------------------------------------------------------------%
