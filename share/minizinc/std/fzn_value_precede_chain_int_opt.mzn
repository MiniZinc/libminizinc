include "value_precede_chain.mzn";

predicate fzn_value_precede_chain_int_opt(array[int] of int: T, array[int] of var opt int: X) = 
    let {
        int: def = if had_zero(X) then max(T) + 1 else 0 endif;
        array [int] of var int: x = array1d(index_set(X), [xi default def | xi in X]);
    } in value_precede_chain(T, x);
