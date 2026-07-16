include "fzn_global_cardinality_closed.mzn";
include "fzn_global_cardinality_closed_reif.mzn";
include "fzn_global_cardinality_closed_opt.mzn";
include "fzn_global_cardinality_closed_set.mzn";

include "fzn_global_cardinality_low_up_closed.mzn";
include "fzn_global_cardinality_low_up_closed_reif.mzn";
include "fzn_global_cardinality_low_up_closed_opt.mzn";
include "fzn_global_cardinality_low_up_closed_set.mzn";

predicate fzn_global_cardinality_closed(
  array [int] of var int: xs,
  array [int] of int: cover,
  array [int] of var int: count,
);
predicate fzn_global_cardinality_closed_opt(
  array [int] of var opt int: xs,
  array [int] of int: cover,
  array [int] of var int: count,
);
predicate fzn_global_cardinality_closed_set(
  array [int] of var set of int: xs,
  array [int] of int: cover,
  array [int] of var int: count,
);
predicate fzn_global_cardinality_low_up_closed(
  array [int] of var int: xs,
  array [int] of int: cover,
  array [int] of int: lower_bound,
  array [int] of int: upper_bound,
);
predicate fzn_global_cardinality_low_up_closed_opt(
  array [int] of var opt int: xs,
  array [int] of int: cover,
  array [int] of int: lower_bound,
  array [int] of int: upper_bound,
);
predicate fzn_global_cardinality_low_up_closed_set(
  array [int] of var set of int: xs,
  array [int] of int: cover,
  array [int] of int: lower_bound,
  array [int] of int: upper_bound,
);

/** @group globals.counting
  Requires that the number of occurrences of \a cover[\p i] in \a xs is \a count[\p i].

  The elements of \a xs must take their values from \a cover.
*/
predicate global_cardinality_closed(
  array [$X] of var $$E: xs,
  array [$Y] of $$E: cover,
  array [$Y] of var int: count,
) =
  assert(
    index_sets_agree(cover, count),
    "global_cardinality_closed: " ++
      "cover has index sets " ++
      show_index_sets(cover) ++
      " and count has index sets " ++
      show_index_sets(count) ++
      ", but they must have identical index sets",
    if length(xs) == 0 then
      forall (c in array1d(count)) (c = 0)
    else
      fzn_global_cardinality_closed(
        enum2int(array1d(xs)),
        enum2int(array1d(cover)),
        array1d(count),
      )
    endif,
  );

/** @group globals.counting
  Requires that the number of occurrences of \a cover[\p i] in \a xs is \a count[\p i].

  The elements of \a xs must take their values from \a cover or be absent.
*/
predicate global_cardinality_closed(
  array [$X] of var opt $$E: xs,
  array [$Y] of $$E: cover,
  array [$Y] of var int: count,
) =
  assert(
    index_sets_agree(cover, count),
    "global_cardinality_closed: " ++ "cover and count must have identical index sets",
    if length(xs) == 0 then
      forall (c in array1d(count)) (c = 0)
    else
      fzn_global_cardinality_closed_opt(
        enum2int(array1d(xs)),
        enum2int(array1d(cover)),
        array1d(count),
      )
    endif,
  );

/** @group globals.counting
  Requires that for all \p i, the value \a cover[\p i] appears at least \a lower_bound[\p i]
  and at most \a upper_bound[\p i] times in the array \a xs.

  The elements of \a xs must take their values from \a cover.
*/
predicate global_cardinality_closed(
  array [$X] of var $$E: xs,
  array [$Y] of $$E: cover,
  array [$Y] of int: lower_bound,
  array [$Y] of int: upper_bound,
) =
  assert(
    index_sets_agree(cover, lower_bound) /\ index_sets_agree(cover, upper_bound),
    "global_cardinality_low_up_closed: " ++
      "cover has index sets " ++
      show_index_sets(cover) ++
      ", lower_bound has index sets " ++
      show_index_sets(lower_bound) ++
      ", and upper_bound has index sets " ++
      show_index_sets(lower_bound) ++
      ", but they must have identical index sets",
    if length(xs) == 0 then
      assert(
        forall (l in array1d(lower_bound)) (l <= 0) /\ forall (u in array1d(upper_bound)) (u >= 0) \/
          length(cover) == 0,
        "global_cardinality_low_up_closed: " ++
          "lower_bound and upper_bound must allow a count of 0 when xs is empty, or also be empty",
        true,
      )
    elseif length(cover) == 0 then
      assert(
        false,
        "global_cardinality_low_up_closed: " ++ "cover must be empty when xs is empty",
        false,
      )
    else
      fzn_global_cardinality_low_up_closed(
        enum2int(array1d(xs)),
        enum2int(array1d(cover)),
        array1d(lower_bound),
        array1d(upper_bound),
      )
    endif,
  );

/** @group globals.counting
  Requires that for all \p i, the value \a cover[\p i] appears at least \a lower_bound[\p i]
  and at most \a upper_bound[\p i] times in the array \a xs.

  The elements of \a xs must take their values from \a cover or be absent.
*/
predicate global_cardinality_closed(
  array [$X] of var opt $$E: xs,
  array [$Y] of $$E: cover,
  array [$Y] of int: lower_bound,
  array [$Y] of int: upper_bound,
) =
  assert(
    index_sets_agree(cover, lower_bound) /\ index_sets_agree(cover, upper_bound),
    "global_cardinality_low_up_closed: " ++
      "cover has index sets " ++
      show_index_sets(cover) ++
      ", lower_bound has index sets " ++
      show_index_sets(lower_bound) ++
      ", and upper_bound has index sets " ++
      show_index_sets(lower_bound) ++
      ", but they must have identical index sets",
    if length(xs) == 0 then
      assert(
        forall (l in array1d(lower_bound)) (l <= 0) /\ forall (u in array1d(upper_bound)) (u >= 0) \/
          length(cover) == 0,
        "global_cardinality_low_up_closed: " ++
          "lower_bound and upper_bound must allow a count of 0 when xs is empty, or also be empty",
        true,
      )
    elseif length(cover) == 0 then
      assert(
        false,
        "global_cardinality_low_up_closed: " ++ "cover must be empty when xs is empty",
        false,
      )
    else
      fzn_global_cardinality_low_up_closed_opt(
        enum2int(array1d(xs)),
        enum2int(array1d(cover)),
        array1d(lower_bound),
        array1d(upper_bound),
      )
    endif,
  );

/** @group globals.counting
  Requires that the number of occurrences of \a cover[\p i] in \a xs is \a count[\p i].

  The elements of \a xs must take their values from \a cover or be absent.
*/
predicate global_cardinality_closed(
  array [$X] of var set of $$E: xs,
  array [$Y] of $$E: cover,
  array [$Y] of var int: count,
) =
  assert(
    index_sets_agree(cover, count),
    "global_cardinality_closed: " ++
      "cover has index sets " ++
      show_index_sets(cover) ++
      " and count has index sets " ++
      show_index_sets(count) ++
      ", but they must have identical index sets",
    if length(xs) == 0 then
      forall (c in array1d(count)) (c = 0)
    else
      fzn_global_cardinality_closed_set(
        enum2int(array1d(xs)),
        enum2int(array1d(cover)),
        array1d(count),
      )
    endif,
  );

/** @group globals.counting
  Requires that for all \p i, the value \a cover[\p i] appears at least \a lower_bound[\p i]
  and at most \a upper_bound[\p i] times in the array \a xs.

  The elements of \a xs must take their values from \a cover or be absent.
*/
predicate global_cardinality_closed(
  array [$X] of var set of $$E: xs,
  array [$Y] of $$E: cover,
  array [$Y] of int: lower_bound,
  array [$Y] of int: upper_bound,
) =
  assert(
    index_sets_agree(cover, lower_bound) /\ index_sets_agree(cover, upper_bound),
    "global_cardinality_low_up_closed: " ++
      "cover has index sets " ++
      show_index_sets(cover) ++
      ", lower_bound has index sets " ++
      show_index_sets(lower_bound) ++
      ", and upper_bound has index sets " ++
      show_index_sets(lower_bound) ++
      ", but they must have identical index sets",
    if length(xs) == 0 then
      assert(
        forall (l in array1d(lower_bound)) (l <= 0) /\ forall (u in array1d(upper_bound)) (u >= 0) \/
          length(cover) == 0,
        "global_cardinality_low_up_closed: " ++
          "lower_bound and upper_bound must allow a count of 0 when xs is empty, or also be empty",
        true,
      )
    elseif length(cover) == 0 then
      assert(
        false,
        "global_cardinality_low_up_closed: " ++ "cover must be empty when xs is empty",
        false,
      )
    else
      fzn_global_cardinality_low_up_closed_set(
        enum2int(array1d(xs)),
        enum2int(array1d(cover)),
        array1d(lower_bound),
        array1d(upper_bound),
      )
    endif,
  );
