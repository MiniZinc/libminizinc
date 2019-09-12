%-----------------------------------------------------------------------------%
% Constrains two arrays of int variables to represent inverse functions.
% All the values in each array must be within the index set of the other array.
%
% Linear version.
%-----------------------------------------------------------------------------%

% include "domain_encodings.mzn";

% predicate inverse(array[int] of var int: f, array[int] of var int: g) =
%     let {
%       array[int,int] of var 0..1: map_f = eq_encode(f);
%       array[int,int] of var 0..1: map_g = eq_encode(g);
%     } in forall (i in index_set(f), j in index_set(g)) (map_f[i,j] = map_g[j,i]);

%-----------------------------------------------------------------------------%


%% Now copying the std version, seems a little faster on vrp-s2-v2-c7_vrp-v2-c7_det_ADAPT_1_INVERSE.mzn

predicate fzn_inverse(array[int] of var int: f,
                      array[int] of var int: invf) =
    forall(i in index_set(f)) ( f[i] in index_set(invf) ) /\
    forall(j in index_set(invf)) ( invf[j] in index_set(f) ) /\
    forall(i in index_set(f), j in index_set(invf)) (
        ((j == f[i]) <-> (i == invf[j]))
    );
