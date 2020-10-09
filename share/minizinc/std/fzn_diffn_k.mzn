predicate fzn_diffn_k(array[int,int] of var int: box_posn,
                  array[int,int] of var int: box_size) =
     let { set of int: DIMS= index_set_2of2(box_posn) } in
     forall(b1, b2 in index_set_1of2(box_posn) where b1 < b2)
           (fzn_diffn_nonoverlap_k([ box_posn[b1,j] | j in DIMS ],   
                               [ box_size[b1,j] | j in DIMS ], 
                               [ box_posn[b2,j] | j in DIMS ],   
                               [ box_size[b2,j] | j in DIMS ] ) 
    	   );

predicate fzn_diffn_nonoverlap_k(array[int] of var int: x1, 
                             array[int] of var int: w1, 
                             array[int] of var int: x2, 
                             array[int] of var int: w2) = 
    exists(j in index_set(x1))
        (x1[j] + w1[j] <= x2[j] \/ x2[j] + w2[j] <= x1[j]);
