%-----------------------------------------------------------------------------%
% Constrains 'c' to be greater than or equal to the number of occurrences of
% 'y' in 'x'.
%-----------------------------------------------------------------------------%

predicate count_geq(array[int] of var int: x, var int: y, var int: c) =
    if(is_fixed(y)) 
    then ilogcp_count_geq(x,fix(y),c)
    else c >= sum(i in index_set(x)) ( bool2int(x[i] == y) )
    endif;

%-----------------------------------------------------------------------------%
%-----------------------------------------------------------------------------%
predicate ilogcp_count_geq(array[int] of var int: x, int: y, var int: c);
