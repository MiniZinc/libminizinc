%-----------------------------------------------------------------------------%
% Constrains two arrays of int variables to represent inverse functions.
% All the values in each array must be within the index set of the other array.
%
% Linear version.
%-----------------------------------------------------------------------------%

include "domain_encodings.mzn";

predicate inverse(array[int] of var int: f, array[int] of var int: g) =
    let {
      array[int,int] of var 0..1: map_f = eq_encode(f);
      array[int,int] of var 0..1: map_g = eq_encode(g);
    } in forall (i in index_set(f), j in index_set(g)) (map_f[i,j] = map_g[j,i]);

%-----------------------------------------------------------------------------%
