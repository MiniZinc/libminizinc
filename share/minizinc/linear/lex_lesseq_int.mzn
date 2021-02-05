include "fzn_lex_lesseq_int.mzn";
include "fzn_lex_lesseq_int_reif.mzn";

%-----------------------------------------------------------------------------%
% Requires that the array 'x' is lexicographically less than or equal to
% array 'y'.  Compares them from first to last element, regardless of indices
%-----------------------------------------------------------------------------%

%% SCIP constraint handler orbisack
predicate fzn_lex_lesseq__orbisack(
    array[int] of var int: vec1, array[int] of var int: vec2, bool: isModelCons);

predicate lex_lesseq_int(array[int] of var int: x,
                         array[int] of var int: y) =
    if MZN__Orbisack /\
            dom_array(x) subset 0..1 /\ dom_array(y) subset 0..1 then
        fzn_lex_lesseq__orbisack(x, y, true) %% SCIP 7.0.2 crashes: not mzn_in_symmetry_breaking_constraint())
    else
        fzn_lex_lesseq_int(x, y)
    endif;

predicate lex_leq_int(array[int] of var int: x,
                      array[int] of var int: y) =
    lex_lesseq_int(x, y);

%-----------------------------------------------------------------------------%
