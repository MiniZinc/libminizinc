predicate fzn_geost(
                      int        : k           ,
    array[int,int] of int        : rect_size   ,
    array[int,int] of int        : rect_offset ,
    array[int    ] of set of int : shape       , 
    array[int,int] of var int    : x           ,
    array[int    ] of var int    : kind
) =
        % A few useful definitions
        let { 
            set of int: DIMS    = 1..k;
            set of int: SHAPES  = 1..length(shape);
            set of int: OBJECTS = index_set(kind); 
        } in
        forall(o1, o2 in OBJECTS where o1 < o2)(
            forall(s1 in dom(kind[o1]), s2 in dom(kind[o2]))(
                (kind[o1] = s1 /\ kind[o2] = s2 ->  
                    forall(r1 in shape[s1], r2 in shape[s2])(
                        fzn_geost_nonoverlap_k(
                            [ x[o1,j] + rect_offset[r1,j] | j in DIMS ],   
                            [ rect_size[r1,j]             | j in DIMS ], 
                            [ x[o2,j] + rect_offset[r2,j] | j in DIMS ],   
                            [ rect_size[r2,j]             | j in DIMS ]
                        )
                    )
                )
            )
        );

