/** @group globals.sort
Returns the permutation \a p which causes \a x to be in sorted order hence
\a x[\a p[\p i]] <= \a x[\a p[\p i+1]].

The permutation is the stable sort hence \a x[\a p[\p i]] = \a x[\a p[\p i+1]] \(\rightarrow\) \a p[\p i] < \a p[\p i+1].
*/
function array[int] of var int: arg_sort(array[int] of var int:x) :: promise_total =
         if length(x) = 0 then [] 
         else
            let { int: l = min(index_set(x));
                  int: u = max(index_set(x));
                  array[1..u-l+1] of var l..u: p;
		  constraint analyse_alldifferent(p);
                  constraint fzn_arg_sort_int(x,p); }
            in  p
         endif;


/** @group globals.sort
Returns the permutation \a p which causes \a x to be in sorted order hence
\a x[\a p[\p i]] <= \a x[\a p[\p i+1]].

The permutation is the stable sort hence \a x[\a p[\p i]] = \a x[\a p[\p i+1]] \(\rightarrow\) \a p[\p i] < \a p[\p i+1].
*/
function array[int] of var int: arg_sort(array[int] of var float:x) :: promise_total =
         if length(x) = 0 then [] 
         else
            let { int: l = min(index_set(x));
                  int: u = max(index_set(x));
                  array[1..u-l+1] of var l..u: p;
		  constraint analyse_alldifferent(p);
                  constraint fzn_arg_sort_float(x,p); }
            in  p
         endif;

/** @group globals.sort
Constrains \a p to be the permutation which causes \a x to be in sorted order hence
\a x[\a p[\p i]] <= \a x[\a p[\p i+1]].

The permutation is the stable sort hence \a x[\a p[\p i]] = \a x[\a p[\p i+1]] \(\rightarrow\) \a p[\p i] < \a p[\p i+1].
*/
predicate arg_sort(array[int] of var int:x,
                   array[int] of var int:p) = fzn_arg_sort_int(x,p);

/** @group globals.sort
Constrains \a p to be the permutation which causes \a x to be in sorted order hence
\a x[\a p[\p i]] <= \a x[\a p[\p i+1]].

The permutation is the stable sort hence \a x[\a p[\p i]] = \a x[\a p[\p i+1]] \(\rightarrow\) \a p[\p i] < \a p[\p i+1].
*/
predicate arg_sort(array[int] of var float:x,
                   array[int] of var int:p) = fzn_arg_sort_float(x,p);


include "fzn_arg_sort_int.mzn";
include "fzn_arg_sort_float.mzn";
include "analyse_alldifferent.mzn";
         


