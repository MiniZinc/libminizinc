%-----------------------------------------------------------------------------%
% Constrains the array of objects 'x' to be all different.
%-----------------------------------------------------------------------------%

include "alldifferent_except.mzn";
include "alldifferent_except_0.mzn";

predicate fzn_all_different_int_opt(array[int] of var opt int: x) =
    if had_zero(x) then
        let {
            int: dummy = lb_array(x) - 1;
            array [int] of var int: xx = [x[i] default dummy | i in index_set(x)];
        } in alldifferent_except(xx, {dummy})
    else
        let {
            array [int] of var int: xx = [x[i] default 0 | i in index_set(x)];
        } in alldifferent_except_0(xx)
    endif;
