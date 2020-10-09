include "seq_precede_chain.mzn";

predicate fzn_value_precede_chain_int(array[int] of int: T, array[int] of var int: X) = 
    if min(index_set(T)) = 1 /\ forall (i in index_set(T))(T[i] = i)
       /\ max(T) = ub_array(X)
    then
        seq_precede_chain(X)
    else
        let {
            int: l = lb_array(X);
            int: u = ub_array(X);
            array[1.. u -l +1] of int : p
                = [sum([i | i in index_set(T) where T[i] = j]) | j in l..u];
            array [int] of var 0..length(T): Y
                = array1d(index_set(X),[p[X[i]-l+1] | i in index_set(X)]);
        } in seq_precede_chain(Y)
    endif;
