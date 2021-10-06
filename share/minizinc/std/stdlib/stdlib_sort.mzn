/***
  @groupdef stdlib.sort Array sorting operations
*/

/** @group stdlib.sort Return array \a x sorted by the values in \a y in non-decreasing order 

The sort is stable, i.e. if \a y[\p i] = \a y[\p j] with \p i < \p j, then
\a x[\p i] will appear in the output before \a x[\p j].
*/
function array[$$E] of any $T: sort_by(array[$$E] of any $T: x, array[$$E] of int: y);

/** @group stdlib.sort Return array \a x sorted by the values in \a y in non-decreasing order 

The sort is stable, i.e. if \a y[\p i] = \a y[\p j] with \p i < \p j, then
\a x[\p i] will appear in the output before \a x[\p j].
*/
function array[$$E] of any $T: sort_by(array[$$E] of any $T: x, array[$$E] of float: y);

/** @group stdlib.sort Return values from array \a x sorted in non-decreasing order */
function array[$$E] of int: sort(array[$$E] of int: x);
/** @group stdlib.sort Return values from array \a x sorted in non-decreasing order */
function array[$$E] of float: sort(array[$$E] of float: x);
/** @group stdlib.sort Return values from array \a x sorted in non-decreasing order */
function array[$$E] of bool: sort(array[$$E] of bool: x);

/** @group stdlib.sort
Returns the permutation \a p which causes \a x to be in sorted order hence
\a x[\a p[\p i]] <= \a x[\a p[\p i+1]].

The permutation is the stable sort hence \a x[\a p[\p i]] = \a x[\a p[\p i+1]] \(\rightarrow\) \a p[\p i] < \a p[\p i+1].
*/
function array[int] of $$E: arg_sort(array[$$E] of int:x) =
  sort_by(array1d(index_set(x), [i | i in index_set(x)]), x);

/** @group stdlib.sort
Returns the permutation \a p which causes \a x to be in sorted order hence
\a x[\a p[\p i]] <= \a x[\a p[\p i+1]].

The permutation is the stable sort hence \a x[\a p[\p i]] = \a x[\a p[\p i+1]] \(\rightarrow\) \a p[\p i] < \a p[\p i+1].
*/
function array[int] of $$E: arg_sort(array[$$E] of float:x) =
  sort_by(array1d(index_set(x), [i | i in index_set(x)]), x);

