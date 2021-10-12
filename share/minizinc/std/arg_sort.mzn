include "fzn_arg_sort_int.mzn";
include "fzn_arg_sort_float.mzn";
include "analyse_all_different.mzn";

/** @group globals.sort
Constrains \a p to be the permutation which causes \a x to be in sorted order hence
\a x[\a p[\p i]] <= \a x[\a p[\p i+1]].

The permutation is the stable sort hence \a x[\a p[\p i]] = \a x[\a p[\p i+1]] \(\rightarrow\) \a p[\p i] < \a p[\p i+1].
*/
predicate arg_sort(
	array[$$E] of var int:x,
	array[int] of var $$E:p,
) = fzn_arg_sort_int(x, p);

/** @group globals.sort
Constrains \a p to be the permutation which causes \a x to be in sorted order hence
\a x[\a p[\p i]] <= \a x[\a p[\p i+1]].

The permutation is the stable sort hence \a x[\a p[\p i]] = \a x[\a p[\p i+1]] \(\rightarrow\) \a p[\p i] < \a p[\p i+1].
*/
predicate arg_sort(
	array[$$E] of var float:x,
	array[int] of var $$E: p,
) = fzn_arg_sort_float(x, p);

/** @group globals.sort
Returns the permutation \a p which causes \a x to be in sorted order hence
\a x[\a p[\p i]] <= \a x[\a p[\p i+1]].

The permutation is the stable sort hence \a x[\a p[\p i]] = \a x[\a p[\p i+1]] \(\rightarrow\) \a p[\p i] < \a p[\p i+1].
*/
function array[int] of var $$E: arg_sort(array[$$E] of var int:x) :: promise_total =
	if length(x) = 0 then
		[] 
	else
		let {
			array[1..length(x)] of var index_set(x): p;
			constraint analyse_all_different(p);
			constraint fzn_arg_sort_int(x, p);
		} in p
	endif;


/** @group globals.sort
Returns the permutation \a p which causes \a x to be in sorted order hence
\a x[\a p[\p i]] <= \a x[\a p[\p i+1]].

The permutation is the stable sort hence \a x[\a p[\p i]] = \a x[\a p[\p i+1]] \(\rightarrow\) \a p[\p i] < \a p[\p i+1].
*/
function array[int] of var $$E: arg_sort(array[$$E] of var float: x) :: promise_total =
	if length(x) = 0 then
		[] 
	else
		let {
			array[1..length(x)] of var index_set(x): p;
			constraint analyse_all_different(p);
			constraint fzn_arg_sort_float(x, p);
		} in p
	endif;
