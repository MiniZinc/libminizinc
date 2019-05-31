/** @group globals.packing
  Constrains \p k-dimensional boxes to be non-overlapping. For each box \p i
  and dimension \p j, \a box_posn[\p i, \p j] is the base position of the box
  in dimension \p j, and \a box_size[\p i, \p j] is the size in that dimension.
  Boxes whose size is 0 in at least one dimension can be packed anywhere.
*/
predicate diffn_nonstrict_k(array[int,int] of var int: box_posn,
                            array[int,int] of var int: box_size) =
    let { set of int: DIMS= index_set_2of2(box_posn) } in
    assert(index_set_2of2(box_size) = DIMS /\
           index_set_1of2(box_posn) = index_set_1of2(box_size),
           "diffn: index sets of arguments are incorrect",
    fzn_diffn_nonstrict_k(box_posn, box_size)
    );


include "fzn_diffn_nonstrict_k.mzn";
