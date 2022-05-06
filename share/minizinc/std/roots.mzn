include "fzn_roots.mzn";
include "fzn_roots_reif.mzn";

/** @group globals.set
  Requires that \a x[\p i] in \a t for all \p i in \a s
*/
predicate roots(
	array[$$X] of var $$Y: x,
	var set of $$X: s,
	var set of $$Y: t
) = assert(
		ub(s) subset index_set(x),
		"roots: upper bound of 's' must be a subset of the index set of 'x'",
	)
	/\ fzn_roots(x,s,t);
