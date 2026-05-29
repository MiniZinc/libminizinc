/***
  @groupdef stdlib.sort Array sorting operations
*/

/** @group stdlib.sort Return array \a x sorted by the values in \a y in non-decreasing order 

The sort is stable, i.e. if \a y[\p i] = \a y[\p j] with \p i < \p j, then
\a x[\p i] will appear in the output before \a x[\p j].
*/
function array [$$E] of any $T: sort_by(array [$$E] of any $T: x, array [$$E] of int: y);

/** @group stdlib.sort Return array \a x sorted by the values in \a y in non-decreasing order 

The sort is stable, i.e. if \a y[\p i] = \a y[\p j] with \p i < \p j, then
\a x[\p i] will appear in the output before \a x[\p j].
*/
function array [$$E] of any $T: sort_by(array [$$E] of any $T: x, array [$$E] of float: y);

/** @group stdlib.sort Return values from array \a xs sorted in non-decreasing order */
function array [$$E] of $$F: sort(array [$$E] of $$F: xs);
/** @group stdlib.sort Return values from array \a xs sorted in non-decreasing order */
function array [$$E] of float: sort(array [$$E] of float: xs);
/** @group stdlib.sort Return values from array \a xs sorted in non-decreasing order */
function array [$$E] of bool: sort(array [$$E] of bool: xs);

/** @group stdlib.sort
Returns the permutation \a permutation which causes \a xs to be in sorted order hence
\a xs[\a permutation[\p i]] <= \a xs[\a permutation[\p i+1]].

The permutation is the stable sort hence \a xs[\a permutation[\p i]] = \a xs[\a permutation[\p i+1]] \(\rightarrow\) \a permutation[\p i] < \a permutation[\p i+1].
*/
function array [int] of $$E: arg_sort(array [$$E] of $$T: xs) =
  sort_by(set2array(index_set(xs)), enum2int(array1d(xs)));

/** @group stdlib.sort
Returns the permutation \a permutation which causes \a xs to be in sorted order hence
\a xs[\a permutation[\p i]] <= \a xs[\a permutation[\p i+1]].

The permutation is the stable sort hence \a xs[\a permutation[\p i]] = \a xs[\a permutation[\p i+1]] \(\rightarrow\) \a permutation[\p i] < \a permutation[\p i+1].
*/
function array [int] of $$E: arg_sort(array [$$E] of float: xs) =
  sort_by(set2array(index_set(xs)), array1d(xs));
