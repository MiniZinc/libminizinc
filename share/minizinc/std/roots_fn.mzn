include "roots.mzn";

/** @group globals.set
  Returns \a s such that \a x[\p i] in \a t for all \p i in \a s
*/
function var set of $$X: roots(
	array[$$X] of var $$Y: x, 
	var set of $$Y: t,
) :: promise_total =
	let {
		var set of index_set(x): s ::is_defined_var;
		constraint roots(x,s,t) ::defines_var(s);
	} in s;
