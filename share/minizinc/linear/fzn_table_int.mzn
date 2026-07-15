%-----------------------------------------------------------------------------%
% A 'table' constraint table(x, T) represents the constraint x in T where we
% consider each row in T to be a tuple and T as a set of tuples.
%
% Linear version.
%
% See also the equality encoding of the 'element' constraint.
%-----------------------------------------------------------------------------%

predicate fzn_table_int(array [int] of var int: xs, array [int, int] of int: tuples) =
  let {
    set of int: it = index_set_1of2(tuples);
    array [it] of var 0..1: lmda;
  } in sum(lmda) = 1 /\
    forall (j in index_set(xs)) (sum (i in it) (tuples[i, j] * lmda[i]) = xs[j]);

%-----------------------------------------------------------------------------%
