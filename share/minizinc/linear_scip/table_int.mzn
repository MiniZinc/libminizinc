%-----------------------------------------------------------------------------%
% A 'table' constraint table(x, T) represents the constraint x in T where we
% consider each row in T to be a tuple and T as a set of tuples.
%
% Linear version.
%
% See also the equality encoding of the 'element' constraint.
%-----------------------------------------------------------------------------%

predicate table_int(array[int] of var int: x, array[int, int] of int: t) =
    assert(index_set_2of2(t) = index_set(x),
        "The second dimension of the table must equal the number of " ++
        "variables in the first argument",
    let { set of int: it = index_set_1of2(t),
          array[it] of var 0..1: lambda }
    in
    sum(lambda) = 1
    /\
    forall(j in index_set(x))( sum(i in it)( t[i,j]*lambda[i] ) = x[j] ) );

%-----------------------------------------------------------------------------%
