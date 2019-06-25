%-----------------------------------------------------------------------------%
% 'all_different' constrains an array of objects to be all different.
%
% Linear version: equality encoding; see e.g. [Refalo, CP 2000]
%
% For a given d in dom(x), at most one i with x_i = d can exist.
%-----------------------------------------------------------------------------%

include "domain_encodings.mzn";

predicate all_different_int(array[int] of var int: x) =
    if length(x)<=1 then true
    else
    let {
        array[int,int] of var 0..1: x_eq_d = eq_encode(x)
    } in (
%        my_trace("  all_different_int: x[" ++ show(index_set(x)) ++ "]\n") /\
        forall(d in index_set_2of2(x_eq_d))( sum(i in index_set_1of2(x_eq_d))( x_eq_d[i,d] ) <= 1 )
    )
    endif;


%-----------------------------------------------------------------------------%
