/** @group globals.counting
  Constrains \a c to be greater than or equal to the number of occurrences
  of \a y in \a x. 
*/
predicate fzn_count_geq(array[int] of var int: x, var int: y, var int: c) =
    c >= sum(i in index_set(x)) ( bool2int(x[i] == y) );

%-----------------------------------------------------------------------------%
%-----------------------------------------------------------------------------%
