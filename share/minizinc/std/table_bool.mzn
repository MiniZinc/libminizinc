include "fzn_table_bool.mzn";
include "fzn_table_bool_reif.mzn";

%-----------------------------------------------------------------------------%
% A table constraint: table(x, t) represents the constraint x in t where we
% consider each row in t to be a tuple and t as a set of tuples.
%-----------------------------------------------------------------------------%

predicate table_bool(array [$$E] of var bool: x, array [int, $$E] of bool: t) =
   fzn_table_bool(x, t);

%-----------------------------------------------------------------------------%
