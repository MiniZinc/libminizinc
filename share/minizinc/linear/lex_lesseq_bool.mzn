include "fzn_lex_lesseq_bool.mzn";
include "fzn_lex_lesseq_bool_reif.mzn";

%-----------------------------------------------------------------------------%
% Requires that the array 'x' is lexicographically less than or equal to
% array 'y'.  Compares them from first to last element, regardless of indices
%-----------------------------------------------------------------------------%

%% SCIP constraint handler orbisack
predicate fzn_lex_lesseq__orbisack(
    array[int] of var int: vec1, array[int] of var int: vec2, bool: isModelCons);

predicate lex_lesseq_bool(array[int] of var bool: x,
                          array[int] of var bool: y) =
    if MZN__Orbisack then
        fzn_lex_lesseq__orbisack(x, y, true)  %% SCIP 7.0.2 crashes: not mzn_in_symmetry_breaking_constraint())
    else
        fzn_lex_lesseq_bool(x, y)
    endif;

predicate lex_leq_bool(array[int] of var bool: x,
                       array[int] of var bool: y) =
    lex_lesseq_bool(x, y);

%-----------------------------------------------------------------------------%
