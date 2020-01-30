include "fzn_geost.mzn";
include "fzn_geost_reif.mzn";
include "fzn_geost_bb.mzn";
include "fzn_geost_bb_reif.mzn";
include "fzn_geost_smallest_bb.mzn";
include "fzn_geost_smallest_bb_reif.mzn";
include "fzn_geost_nonoverlap_k.mzn";
include "fzn_geost_nonoverlap_k_reif.mzn";

/** @group globals.packing
 A global non-overlap constraint for \a k dimensional objects. It enforces that no two objects overlap.
    
@param k:            the number of dimensions
@param rect_size:    the size of each box in \a k dimensions
@param rect_offset:  the offset of each box from the base position in \a k dimensions
 
@param shape:        the set of rectangles defining the \p i-th shape. 

@param x:       the base position of each object. \a x[\p i,\p j] is the position of object \p i in. dimension \p j.
@param kind:      the shape used by each object.
*/
 
predicate geost(
                      int        : k           ,
    array[int,int] of int        : rect_size   ,
    array[int,int] of int        : rect_offset ,
    array[int    ] of set of int : shape       , 
    array[int,int] of var int    : x           ,
    array[int    ] of var int    : kind
) =
    assert(
        % Some sanity checks
        index_set_1of2( rect_size   ) = index_set_1of2(rect_offset) /\
        index_set_2of2( rect_size   ) = 1..k                        /\
        index_set_2of2( rect_offset ) = 1..k                        /\
        index_set(      shape       ) = 1..length(shape)            /\
        index_set_1of2( x           ) = index_set(kind)             /\
        index_set_2of2( x           ) = 1..k                        /\
        forall(i in index_set(shape))(
            shape[i] subset index_set_1of2(rect_size)
        ),
        % Error message
        "geost: index sets of arguments are incorrect",
    assert(
        % More sanity checks
        forall(i in index_set(shape))(card(shape[i]) > 0),
        % Error message
        "geost: sets in shape must be non-empty",

        fzn_geost(k, rect_size, rect_offset, shape, x, kind)
    )); % End assert statements


/** @group globals.packing
 A global non-overlap constraint for \a k dimensional objects. It enforces that no two objects overlap, and that all objects fit within a global \a k dimensional bounding box.
    
@param k:            the number of dimensions
@param rect_size:    the size of each box in \a k dimensions
@param rect_offset:  the offset of each box from the base position in \a k dimensions
 
@param shape:        the set of rectangles defining the \p i-th shape. 

@param x:       the base position of each object. \a x[\p i,\p j] is the position of object \p i in dimension \p j.
@param kind:      the shape used by each object.

@param l:            is an array of lower bounds, \a l[\p i] is the minimum bounding box for all objects in dimension \p i.
@param u:            is an array of upper bounds, \a u[\p i] is the maximum bounding box for all objects in dimension \p i.
*/

predicate geost_bb(
                      int        : k           ,
    array[int,int] of int        : rect_size   ,
    array[int,int] of int        : rect_offset ,
    array[int    ] of set of int : shape       , 
    array[int,int] of var int    : x           ,
    array[int    ] of var int    : kind        ,
    array[int    ] of var int    : l           ,
    array[int    ] of var int    : u
) =
    assert(
        % Some sanity checks
        index_set_1of2( rect_size   ) = index_set_1of2(rect_offset) /\
        index_set_2of2( rect_size   ) = 1..k                        /\
        index_set_2of2( rect_offset ) = 1..k                        /\
        index_set(      shape       ) = 1..length(shape)            /\
        index_set_1of2( x           ) = index_set(kind)             /\
        index_set_2of2( x           ) = 1..k                        /\
        forall(i in index_set(shape))(
            shape[i] subset index_set_1of2(rect_size)
        ),
        % Error message
        "geost_bb: index sets of arguments are incorrect",
    assert(
        % More sanity checks
        forall(i in index_set(shape))(card(shape[i]) > 0),
        % Error message
        "geost_bb: sets in shape must be non-empty",

    assert(
        % Sanity check
        index_set(l) = 1..k /\ index_set(u) = 1..k,
        % Error message
        "geost_bb: index set of bounds arrays is not 1.." ++ show(k),
        % Posting the geost constraint
        fzn_geost_bb(k, rect_size, rect_offset, shape, x, kind, l, u)
    )));

/** @group globals.packing
 A global non-overlap constraint for \a k dimensional objects. It enforces that no two objects overlap, and that all objects fit within a global \a k dimensional bounding box. In addition, it enforces that the bounding box is the smallest one containing all objects, i.e., each of the \a 2k boundaries is touched by at least by one object.
    
@param k:            the number of dimensions
@param rect_size:    the size of each box in \a k dimensions
@param rect_offset:  the offset of each box from the base position in \a k dimensions
 
@param shape:        the set of rectangles defining the \p i-th shape. 

@param x:       the base position of each object. \a x[\p i,\p j] is the position of object \p i in dimension \p j.
@param kind:      the shape used by each object.

@param l:            is an array of lower bounds, \a l[\p i] is the minimum bounding box for all objects in dimension \p i.
@param u:            is an array of upper bounds, \a u[\p i] is the maximum bounding box for all objects in dimension \p i.
*/

predicate geost_smallest_bb(
                      int        : k           ,
    array[int,int] of int        : rect_size   ,
    array[int,int] of int        : rect_offset ,
    array[int    ] of set of int : shape       , 
    array[int,int] of var int    : x           ,
    array[int    ] of var int    : kind        ,
    array[int    ] of var int    : l           ,
    array[int    ] of var int    : u
) = 
    assert(
        % Some sanity checks
        index_set_1of2( rect_size   ) = index_set_1of2(rect_offset) /\
        index_set_2of2( rect_size   ) = 1..k                        /\
        index_set_2of2( rect_offset ) = 1..k                        /\
        index_set(      shape       ) = 1..length(shape)            /\
        index_set_1of2( x           ) = index_set(kind)             /\
        index_set_2of2( x           ) = 1..k                        /\
        forall(i in index_set(shape))(
            shape[i] subset index_set_1of2(rect_size)
        ),
        % Error message
        "geost_bb: index sets of arguments are incorrect",
    assert(
        % More sanity checks
        forall(i in index_set(shape))(card(shape[i]) > 0),
        % Error message
        "geost_bb: sets in shape must be non-empty",

        % A few useful definitions
        let {
            set of int: DIMS    = 1..k;
            set of int: SHAPES  = 1..length(shape);
            set of int: OBJECTS = index_set(kind);
        } in
    % Two useful definitions
    let {
        set of int: DIMS    = 1..k;
        set of int: OBJECTS = index_set(kind); 
    } in (
    assert(
        % Sanity check
        index_set(l) = 1..k /\ index_set(u) = 1..k,
        % Error message
        "geost_bb: index set of bounds arrays is not 1.." ++ show(k),
        % Posting the geost constraint
        fzn_geost_smallest_bb(k, rect_size, rect_offset, shape, x, kind, l, u)
    ))));

/** @group globals.packing
 A global non-overlap constraint for \a 2 dimensional objects. It enforces that no two objects overlap
 and zero-length objects do not appear in the middle of other objects.
    
@param x1: first coordinate of each object
@param w2: width in first dimension for each object
@param x2: second coordinate of each object
@param w2: width in second dimension for each object
*/
predicate geost_nonoverlap_k(
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
        fzn_geost_nonoverlap_k(x1,w1,x2,w2)
    );

test geost_nonoverlap_k(
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

