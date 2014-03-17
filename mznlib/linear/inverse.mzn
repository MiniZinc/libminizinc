%-----------------------------------------------------------------------------%
% Constrains two arrays of int variables to represent inverse functions.
% All the values in each array must be within the index set of the other array.
%
% Linear version.
%-----------------------------------------------------------------------------%

include "domain_encodings.mzn";

predicate inverse(array[int] of var int: f, array[int] of var int: g) =
    let { set of int: ixf = index_set(f),
          set of int: ixg = index_set(g),
          array[ixf, ixg] of var 0..1: map }
    in
    equality_encoding(f, map)
    /\
    equality_encoding(g, array2d(ixg, ixf, [ map[i,j] | j in ixg, i in ixf ]));

%-----------------------------------------------------------------------------%
