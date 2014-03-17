%-----------------------------------------------------------------------------%
% 'all_different' constrains an array of objects to be all different.
%
% Linear version: equality encoding; see e.g. [Refalo, CP 2000]
%
% For a given d in dom(x), at most one i with x_i = d can exist.
%-----------------------------------------------------------------------------%

include "domain_encodings.mzn";

predicate all_different_int(array[int] of var int: x) =
    let {
        set of int: ix = index_set(x),
        set of int: domx = dom_array(x),
        set of int: dx = min(domx)..max(domx),
        array[ix, dx] of var 0..1: x_eq_d
    } in (
    	equality_encoding(x, x_eq_d) /\
        forall(d in dx)( sum(i in ix)( x_eq_d[i,d] ) <= 1 )
    );

%-----------------------------------------------------------------------------%
