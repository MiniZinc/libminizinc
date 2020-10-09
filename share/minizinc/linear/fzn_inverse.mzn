%-----------------------------------------------------------------------------%
% Constrains two arrays of int variables to represent inverse functions.
% All the values in each array must be within the index set of the other array.
%
% Linear version.
%-----------------------------------------------------------------------------%

include "fzn_inverse_in_range.mzn";

predicate fzn_inverse(array[int] of var int: f,
                      array[int] of var int: invf) =
    forall(i in index_set(f)) ( f[i] in index_set(invf) ) /\
    forall(j in index_set(invf)) ( invf[j] in index_set(f) ) /\
    fzn_inverse_in_range(f, invf);
