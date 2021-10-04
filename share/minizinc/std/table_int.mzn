include "fzn_table_int.mzn";
include "fzn_table_int_reif.mzn";

%-----------------------------------------------------------------------------%
% A table constraint table(x, t) represents the constraint x in t where we
% consider each row in t to be a tuple and t as a set of tuples.
%-----------------------------------------------------------------------------%

predicate table_int(
	array[$$E] of var int: x,
	array[int, $$E] of int: t
) = fzn_table_int(x, t);

%-----------------------------------------------------------------------------%
