%-----------------------------------------------------------------------------%
% A table constraint: table(x, t) represents the constraint x in t where we
% consider each row in t to be a tuple and t as a set of tuples.
%-----------------------------------------------------------------------------%

predicate fzn_table_bool_reif(array[int] of var bool: x, array[int, int] of bool: t,
      var bool: b) = 
    abort("Reified table/2 for Booleans is not supported.");
