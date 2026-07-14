%-----------------------------------------------------------------------------%
% Constrains the array of objects 'xs' to be all different.
%-----------------------------------------------------------------------------%

include "alldifferent_except.mzn";
include "alldifferent_except_0.mzn";

predicate fzn_all_different_int_opt(array [int] of var opt int: xs) =
  if had_zero(xs) then
    let {
      int: dummy = lb_array(xs) - 1;
      array [int] of var int: xx = [xs[i] default dummy | i in index_set(xs)];
    } in alldifferent_except(xx, {dummy})
  else
    let {
      array [int] of var int: xx = [xs[i] default 0 | i in index_set(xs)];
    } in alldifferent_except_0(xx)
  endif;
