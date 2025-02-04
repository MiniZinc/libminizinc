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
    
@param rect_size:    the size of each box in \a k dimensions
@param rect_offset:  the offset of each box from the base position in \a k dimensions
 
@param shape:        the set of rectangles defining the \p i-th shape. 

@param x:       the base position of each object. \a x[\p i,\p j] is the position of object \p i in. dimension \p j.
@param kind:      the shape used by each object.
*/

predicate geost(
  array [$$R, $$K] of int: rect_size,
  array [$$R, $$K] of int: rect_offset,
  array [$$S] of set of $$R: shape,
  array [$$O, $$K] of var int: x,
  array [$$O] of var $$S: kind,
) =
  let {
    constraint
      assert(
        index_sets_agree(rect_size, rect_offset),
        "geost: rect_size and rect_offset must have the same index sets",
      );
    constraint
      assert(
        index_set_2of2(rect_size) = index_set_2of2(x),
        "geost: rectangles must have same number of dimensions as x",
      );
    constraint
      assert(
        array_union(shape) subset index_set_1of2(rect_size),
        "geost: shapes must be made of valid rectangles",
      );
    constraint
      assert(
        index_set_1of2(x) = index_set(kind),
        "geost: index set of kind must match first index set of x",
      );
    constraint assert(forall (s in shape) (card(s) > 0), "geost: shapes must not be empty");
    int: k = card(index_set_2of2(rect_size));
    set of int: RECT = enum2int(index_set_1of2(rect_size));
    set of int: OBJ = enum2int(index_set(kind));
  } in fzn_geost(
    k,
    array2d(RECT, 1..k, rect_size),
    array2d(RECT, 1..k, rect_offset),
    index2int(enum2int(shape)),
    array2d(OBJ, 1..k, x),
    index2int(enum2int(kind)),
  );

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
  int: k,
  array [$$R, $$K] of int: rect_size,
  array [$$R, $$K] of int: rect_offset,
  array [$$S] of set of $$R: shape,
  array [$$O, $$K] of var int: x,
  array [$$O] of var $$S: kind,
) =
  let {
    constraint
      assert(
        k = card(index_set_2of2(rect_size)),
        "geost: k must match second index set of rect_size",
      );
  } in geost(rect_size, rect_offset, shape, x, kind);

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
  array [$$R, $$K] of int: rect_size,
  array [$$R, $$K] of int: rect_offset,
  array [$$S] of set of $$R: shape,
  array [$$O, $$K] of var int: x,
  array [$$O] of var $$S: kind,
  array [$$K] of var int: l,
  array [$$K] of var int: u,
) =
  let {
    constraint
      assert(
        index_sets_agree(rect_size, rect_offset),
        "geost_bb: rect_size and rect_offset must have the same index sets",
      );
    constraint
      assert(
        index_set_2of2(rect_size) = index_set_2of2(x),
        "geost_bb: rectangles must have same number of dimensions as x",
      );
    constraint
      assert(
        array_union(shape) subset index_set_1of2(rect_size),
        "geost_bb: shapes must be made of valid rectangles",
      );
    constraint
      assert(
        index_set_1of2(x) = index_set(kind),
        "geost_bb: index set of kind must match first index set of x",
      );
    constraint assert(forall (s in shape) (card(s) > 0), "geost_bb: shapes must not be empty");
    constraint assert(index_sets_agree(l, u), "geost_bb: l must have the same index set as u");
    constraint
      assert(
        index_set_2of2(rect_size) = index_set(l),
        "geost_bb: rectangles must have same number of dimensions as bounds",
      );
    int: k = card(index_set_2of2(rect_size));
    set of int: RECT = enum2int(index_set_1of2(rect_size));
    set of int: OBJ = enum2int(index_set(kind));
  } in fzn_geost_bb(
    k,
    array2d(RECT, 1..k, rect_size),
    array2d(RECT, 1..k, rect_offset),
    index2int(enum2int(shape)),
    array2d(OBJ, 1..k, x),
    index2int(enum2int(kind)),
    array1d(l),
    array1d(u),
  );

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
  int: k,
  array [$$R, $$K] of int: rect_size,
  array [$$R, $$K] of int: rect_offset,
  array [$$S] of set of $$R: shape,
  array [$$O, $$K] of var int: x,
  array [$$O] of var $$S: kind,
  array [$$K] of var int: l,
  array [$$K] of var int: u,
) =
  let {
    constraint
      assert(
        k = card(index_set_2of2(rect_size)),
        "geost_bb: k must match second index set of rect_size",
      );
  } in geost_bb(rect_size, rect_offset, shape, x, kind, l, u);

/** @group globals.packing
 A global non-overlap constraint for \a k dimensional objects. It enforces that no two objects overlap, and that all objects fit within a global \a k dimensional bounding box. In addition, it enforces that the bounding box is the smallest one containing all objects, i.e., each of the \a 2k boundaries is touched by at least by one object.
    
@param rect_size:    the size of each box in \a k dimensions
@param rect_offset:  the offset of each box from the base position in \a k dimensions
 
@param shape:        the set of rectangles defining the \p i-th shape. 

@param x:       the base position of each object. \a x[\p i,\p j] is the position of object \p i in dimension \p j.
@param kind:      the shape used by each object.

@param l:            is an array of lower bounds, \a l[\p i] is the minimum bounding box for all objects in dimension \p i.
@param u:            is an array of upper bounds, \a u[\p i] is the maximum bounding box for all objects in dimension \p i.
*/

predicate geost_smallest_bb(
  array [$$R, $$K] of int: rect_size,
  array [$$R, $$K] of int: rect_offset,
  array [$$S] of set of $$R: shape,
  array [$$O, $$K] of var int: x,
  array [$$O] of var $$S: kind,
  array [$$K] of var int: l,
  array [$$K] of var int: u,
) =
  let {
    constraint
      assert(
        index_sets_agree(rect_size, rect_offset),
        "geost_smallest_bb: rect_size and rect_offset must have the same index sets",
      );
    constraint
      assert(
        index_set_2of2(rect_size) = index_set_2of2(x),
        "geost_smallest_bb: rectangles must have same number of dimensions as x",
      );
    constraint
      assert(
        array_union(shape) subset index_set_1of2(rect_size),
        "geost_smallest_bb: shapes must be made of valid rectangles",
      );
    constraint
      assert(
        index_set_1of2(x) = index_set(kind),
        "geost_smallest_bb: index set of kind must match first index set of x",
      );
    constraint
      assert(forall (s in shape) (card(s) > 0), "geost_smallest_bb: shapes must not be empty");
    constraint
      assert(index_sets_agree(l, u), "geost_smallest_bb: l must have the same index set as u");
    constraint
      assert(
        index_set_2of2(rect_size) = index_set(l),
        "geost_smallest_bb: rectangles must have same number of dimensions as bounds",
      );
    int: k = card(index_set_2of2(rect_size));
    set of int: RECT = enum2int(index_set_1of2(rect_size));
    set of int: OBJ = enum2int(index_set(kind));
  } in fzn_geost_smallest_bb(
    k,
    array2d(RECT, 1..k, rect_size),
    array2d(RECT, 1..k, rect_offset),
    index2int(enum2int(shape)),
    array2d(OBJ, 1..k, x),
    index2int(enum2int(kind)),
    array1d(l),
    array1d(u),
  );

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
  int: k,
  array [$$R, $$K] of int: rect_size,
  array [$$R, $$K] of int: rect_offset,
  array [$$S] of set of $$R: shape,
  array [$$O, $$K] of var int: x,
  array [$$O] of var $$S: kind,
  array [$$K] of var int: l,
  array [$$K] of var int: u,
) =
  let {
    constraint
      assert(
        k = card(index_set_2of2(rect_size)),
        "geost_smallest_bb: k must match second index set of rect_size",
      );
  } in geost_smallest_bb(rect_size, rect_offset, shape, x, kind, l, u);

/** @group globals.packing
 A non-overlap constraint for two \a k dimensional objects. It enforces that
 there is at least one dimension where the objects occupy a different space.
    
@param x1: coordinate of each dimension for the first object
@param w1: width of each dimension for the first object
@param x2: coordinate of each dimension for the second object
@param w2: width of each dimension for the second object
*/
predicate geost_nonoverlap_k(
  array [$$K] of var int: x1,
  array [$$K] of int: w1,
  array [$$K] of var int: x2,
  array [$$K] of int: w2,
) =
  let {
    constraint
      assert(
        index_sets_agree(x1, w1) /\ index_sets_agree(x1, x2) /\ index_sets_agree(x1, w2),
        "geost_nonoverlap_k: index sets of arguments do not match",
      );
  } in fzn_geost_nonoverlap_k(index2int(x1), index2int(w1), index2int(x2), index2int(w2));

test geost_nonoverlap_k(
  array [$$K] of int: x1,
  array [$$K] of int: w1,
  array [$$K] of int: x2,
  array [$$K] of int: w2,
) =
  let {
    constraint
      assert(
        index_sets_agree(x1, w1) /\ index_sets_agree(x1, x2) /\ index_sets_agree(x1, w2),
        "geost_nonoverlap_k: index sets of arguments do not match",
      );
  } in exists (j in index_set(x1)) (x1[j] + w1[j] <= x2[j] \/ x2[j] + w2[j] <= x1[j]);
