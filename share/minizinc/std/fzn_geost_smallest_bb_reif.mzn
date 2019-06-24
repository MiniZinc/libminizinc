include "fzn_geost_bb.mzn";
include "fzn_geost_bb_reif.mzn";

predicate fzn_geost_smallest_bb_reif(
                      int        : k           ,
    array[int,int] of int        : rect_size   ,
    array[int,int] of int        : rect_offset ,
    array[int    ] of set of int : shape       , 
    array[int,int] of var int    : x           ,
    array[int    ] of var int    : kind        ,
    array[int    ] of var int    : l           ,
    array[int    ] of var int    : u           ,
    var bool: b
) = 
    % Two useful definitions
    let {
        set of int: DIMS    = 1..k;
        set of int: OBJECTS = index_set(kind); 
    } in b <-> (
        % Posting the geost constraint
        fzn_geost_bb(k, rect_size, rect_offset, shape, x, kind, l, u)
        /\ % Posting the smallest bounding box constraints
        forall(j in DIMS)(
            % Lower boundary
            exists(o in OBJECTS, s in dom(kind[o]))(
                kind[o] = s
            /\  exists(r in shape[s])(
                    x[o,j] + rect_offset[r,j] == l[j]
                )
            )
        /\  % Upper boundary
            exists(o in OBJECTS, s in dom(kind[o]))(
                kind[o] = s
            /\  exists(r in shape[s])(
                    x[o,j] + rect_offset[r,j] + rect_size[r,j] == u[j]
                )
            )
        )
    );
