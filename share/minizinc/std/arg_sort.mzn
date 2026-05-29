include "fzn_arg_sort_int.mzn";
include "fzn_arg_sort_float.mzn";
include "analyse_all_different.mzn";

/** @group globals.sort
Constrains \a permutation to be the permutation which causes \a xs to be in sorted order hence
\a xs[\a permutation[\p i]] <= \a xs[\a permutation[\p i+1]].

The permutation is the stable sort hence \a xs[\a permutation[\p i]] = \a xs[\a permutation[\p i+1]] \(\rightarrow\) \a permutation[\p i] < \a permutation[\p i+1].
*/
predicate arg_sort(array [$$E] of var $$T: xs, array [int] of var $$E: permutation) =
  fzn_arg_sort_int(index2int(enum2int(xs)), enum2int(permutation));

/** @group globals.sort
Constrains \a permutation to be the permutation which causes \a xs to be in sorted order hence
\a xs[\a permutation[\p i]] <= \a xs[\a permutation[\p i+1]].

The permutation is the stable sort hence \a xs[\a permutation[\p i]] = \a xs[\a permutation[\p i+1]] \(\rightarrow\) \a permutation[\p i] < \a permutation[\p i+1].
*/
predicate arg_sort(array [$$E] of var float: xs, array [int] of var $$E: permutation) =
  fzn_arg_sort_float(index2int(xs), enum2int(permutation));

/** @group globals.sort
Returns the permutation \a permutation which causes \a xs to be in sorted order hence
\a xs[\a permutation[\p i]] <= \a xs[\a permutation[\p i+1]].

The permutation is the stable sort hence \a xs[\a permutation[\p i]] = \a xs[\a permutation[\p i+1]] \(\rightarrow\) \a permutation[\p i] < \a permutation[\p i+1].
*/
function array [int] of var $$E: arg_sort(array [$$E] of var $$T: xs) =
  to_enum_internal(enum_of(index_set(xs)), arg_sort_int(index2int(enum2int(xs))));
function array [int] of var int: arg_sort_int(array [int] of var int: x) :: promise_total =
  if length(x) = 0 then
    []
  else
    let {
      array [1..length(x)] of var index_set(x): p;
      constraint analyse_all_different(p);
      constraint fzn_arg_sort_int(x, p);
    } in p
  endif;

/** @group globals.sort
Returns the permutation \a permutation which causes \a xs to be in sorted order hence
\a xs[\a permutation[\p i]] <= \a xs[\a permutation[\p i+1]].

The permutation is the stable sort hence \a xs[\a permutation[\p i]] = \a xs[\a permutation[\p i+1]] \(\rightarrow\) \a permutation[\p i] < \a permutation[\p i+1].
*/
function array [int] of var $$E: arg_sort(array [$$E] of var float: xs) =
  to_enum_internal(enum_of(index_set(xs)), arg_sort_float(index2int(xs)));
function array [int] of var int: arg_sort_float(array [int] of var float: x) :: promise_total =
  if length(x) = 0 then
    []
  else
    let {
      array [1..length(x)] of var index_set(x): p;
      constraint analyse_all_different(p);
      constraint fzn_arg_sort_float(x, p);
    } in p
  endif;
