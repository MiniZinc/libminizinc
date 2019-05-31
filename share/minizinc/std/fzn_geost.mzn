/** @group globals.packing
 A global non-overlap constraint for \a k dimensional objects. It enforces that no two objects overlap.
    
@param k:            the number of dimensions
@param rect_size:    the size of each box in \a k dimensios
@param rect_offset:  the offset of each box from the base position in \a k dimensions
 
@param shape:        the set of rectangles defining the \p i-th shape. 

@param x:       the base position of each object. \a x[\p i,\p j] is the position of object \p i in. dimension \p j.
@param kind:      the shape used by each object.
*/
 
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


/** @group globals.packing
 A global non-overlap constraint for \a k dimensional objects. It enforces that no two objects overlap, and that all objects fit within a global \a k dimensional bounding box.
    
@param k:            the number of dimensions
@param rect_size:    the size of each box in \a k dimensios
@param rect_offset:  the offset of each box from the base position in \a k dimensions
 
@param shape:        the set of rectangles defining the \p i-th shape. 

@param x:       the base position of each object. \a x[\p i,\p j] is the position of object \p i in dimension \p j.
@param kind:      the shape used by each object.

@param l:            is an array of lower bounds, \a l[\p i] is the minimum bounding box for all objects in dimension \p i.
@param u:            is an array of upper bounds, \a u[\p i] is the maximum bounding box for all objects in dimension \p i.
*/

predicate fzn_geost_bb(
                      int        : k           ,
    array[int,int] of int        : rect_size   ,
    array[int,int] of int        : rect_offset ,
    array[int    ] of set of int : shape       , 
    array[int,int] of var int    : x           ,
    array[int    ] of var int    : kind        ,
    array[int    ] of var int    : l           ,
    array[int    ] of var int    : u
) =
        % Two useful definitions
        let {
            set of int: DIMS    = 1..k;
            set of int: OBJECTS = index_set(kind); 
        } in
        % Posting the geost constraint
        fzn_geost(k, rect_size, rect_offset, shape, x, kind)
        /\ % Posting the bounding box constraints
        forall(o in OBJECTS)(
            forall(s in dom(kind[o]))(
                (kind[o] = s ->
                    forall(r in shape[s], j in DIMS)(
                        x[o,j] + rect_offset[r,j] >= l[j]
                    /\  x[o,j] + rect_offset[r,j] + rect_size[r,j] <= u[j]
                    )
                )
            )
        );

/** @group globals.packing
 A global non-overlap constraint for \a k dimensional objects. It enforces that no two objects overlap, and that all objects fit within a global \a k dimensional bounding box. In addition, it enforces that the bounding box is the smallest one containing all objects, i.e., each of the \a 2k boundaries is touched by at least by one object.
    
@param k:            the number of dimensions
@param rect_size:    the size of each box in \a k dimensios
@param rect_offset:  the offset of each box from the base position in \a k dimensions
 
@param shape:        the set of rectangles defining the \p i-th shape. Assumption: Each pair of boxes in a shape must not overlap.

@param x:       the base position of each object. \a x[\p i,\p j] is the position of object \p i in dimension \p j.
@param kind:      the shape used by each object.

@param l:            is an array of lower bounds, \a l[\p i] is the minimum bounding box for all objects in dimension \p i.
@param u:            is an array of upper bounds, \a u[\p i] is the maximum bounding box for all objects in dimension \p i.
*/

predicate fzn_geost_smallest_bb(
                      int        : k           ,
    array[int,int] of int        : rect_size   ,
    array[int,int] of int        : rect_offset ,
    array[int    ] of set of int : shape       , 
    array[int,int] of var int    : x           ,
    array[int    ] of var int    : kind        ,
    array[int    ] of var int    : l           ,
    array[int    ] of var int    : u
) = 
    % Two useful definitions
    let {
        set of int: DIMS    = 1..k;
        set of int: OBJECTS = index_set(kind); 
    } in (
        % Posting the geost constraint
        geost_bb(k, rect_size, rect_offset, shape, x, kind, l, u)
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


predicate fzn_geost_nonoverlap_k(
    array[int] of var int : x1, 
    array[int] of int     : w1, 
    array[int] of var int : x2, 
    array[int] of int     : w2
) = 
    assert(
        % Some sanity checks
        index_set( x1 ) = index_set( w1 ) /\ 
        index_set( x1 ) = index_set( x2 ) /\ 
        index_set( x1 ) = index_set( w2 ),
        % Error message
        "geost_nonoverlap_k: index sets of arguments do not match",
        % Non-overlap constraint
        exists(j in index_set(x1))(
            x1[j] + w1[j] <= x2[j] \/ x2[j] + w2[j] <= x1[j]
        )
    );


test fzn_geost_nonoverlap_k(
    array[int] of int: x1, 
    array[int] of int: w1, 
    array[int] of int: x2, 
    array[int] of int: w2
) = 
    assert(
        % Some sanity checks
        index_set( x1 ) = index_set( w1 ) /\ 
        index_set( x1 ) = index_set( x2 ) /\ 
        index_set( x1 ) = index_set( w2 ),
        % Error message
        "geost_nonoverlap_k: index sets of arguments do not match",
        % Non-overlap test
        exists(j in index_set(x1))(
            x1[j] + w1[j] <= x2[j] \/ x2[j] + w2[j] <= x1[j]
        )
    );

