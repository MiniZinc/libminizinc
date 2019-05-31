predicate fzn_seq_precede_chain_int(array[int] of var int: X) =
 let { int : l = lb_array (X) ; % least possible value
       int : u = ub_array (X) ; % greatest possible value
       int : f = min ( index_set (X ));
       array [ index_set (X) ] of var l .. u: H; } in
 H[f] <= 1 /\ H[f] = max (X[f], 0) /\
 forall ( i in index_set ( X) diff {f} )
        (H[i] <= H[i-1] + 1 /\ H[i] = max (X[i], H[i-1]) );
