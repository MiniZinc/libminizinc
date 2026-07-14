%-----------------------------------------------------------------------------%
% A table constraint: table(xs, tuples) represents the constraint xs in tuples where we
% consider each row in tuples to be a tuple and tuples as a set of tuples.
%-----------------------------------------------------------------------------%

predicate fzn_table_bool_reif(
  array [int] of var bool: xs,
  array [int, int] of bool: tuples,
  var bool: b,
) = abort("Reified table/2 for Booleans is not supported.");
