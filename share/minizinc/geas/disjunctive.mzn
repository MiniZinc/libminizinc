%predicate geas_disjunctive_var(array[int] of var int: s,
%  array[int] of var int: d);
predicate geas_disjunctive(array[int] of var int: s,
  array[int] of int: d);

%predicate disjunctive(array[int] of var int: s, array[int] of var int: d) =
%  geas_disjunctive_var(s, d);
predicate disjunctive(array[int] of var int: s, array[int] of int: d) =
  geas_disjunctive(s, d);
