include "fzn_table_int.mzn";

%-----------------------------------------------------------------------------%
% A table constraint table(x, t) represents the constraint x in t where we
% consider each row in t to be a tuple and t as a set of tuples.
%-----------------------------------------------------------------------------%

predicate table_int(array[int] of var int: x, array[int, int] of int: t) =
    fzn_table_int(x, t);

predicate table_int_reif(array[int] of var int: x, array[int, int] of int: t,
                         var bool: b) =
    fzn_table_int_reif(x, t, c);

%-----------------------------------------------------------------------------%
