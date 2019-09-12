%-----------------------------------------------------------------------------%
% A 'table' constraint table(x, T) represents the constraint x in T where we
% consider each row in T to be a tuple and T as a set of tuples.
%
% Linear version.
%
% See also the equality encoding of the 'element' constraint.
%-----------------------------------------------------------------------------%

predicate fzn_table_int(array[int] of var int: x, array[int, int] of int: t) =
    let { set of int: it = index_set_1of2(t),
          array[it] of var 0..1: lambda }
    in
    sum(lambda) = 1
    /\
    forall(j in index_set(x))( sum(i in it)( t[i,j]*lambda[i] ) = x[j] );

%-----------------------------------------------------------------------------%
