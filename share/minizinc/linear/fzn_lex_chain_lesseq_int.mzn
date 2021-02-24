include "lex_lesseq.mzn";
include "fzn_lex_chain_lesseq_orbitope.mzn";

predicate fzn_lex_chain_lesseq_int(array[int, int] of var int: a) =
    if MZN__Orbitope /\ dom_array(a) subset 0..1 then
        fzn_lex_chain_lesseq__orbitope( a,
            card(index_set_1of2(a)), 0, true,
            not mzn_in_symmetry_breaking_constraint()
        )
    else
        fzn_lex_chain_lesseq_int__CP(a)
    endif;


predicate fzn_lex_chain_lesseq_int__CP(array[int, int] of var int: a) =
    let {
        int: lb2 = min(index_set_2of2(a)),
        int: ub2 = max(index_set_2of2(a))
    } in (
        forall(j in lb2 + 1 .. ub2) (
            lex_lesseq(col(a, j-1), col(a, j))
        )
    );
