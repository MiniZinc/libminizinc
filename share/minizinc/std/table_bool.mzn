include "fzn_table_bool.mzn";

%-----------------------------------------------------------------------------%
% A table constraint: table(x, t) represents the constraint x in t where we
% consider each row in t to be a tuple and t as a set of tuples.
%-----------------------------------------------------------------------------%

predicate table_bool(array[int] of var bool: x, array[int, int] of bool: t) =
    fzn_table_bool(x, t);

predicate table_bool_reif(array[int] of var bool: x, array[int, int] of bool: t,
      var bool: b) = 
    fzn_table_bool_reif(x, t, b);

%-----------------------------------------------------------------------------%
