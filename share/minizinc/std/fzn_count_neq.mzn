/** @group globals.counting
  Constrains \a c to be not equal to the number of occurrences
  of \a y in \a x. 
*/
predicate fzn_count_neq(array[int] of var int: x, var int: y, var int: c) =
    c != sum(i in index_set(x)) ( bool2int(x[i] == y) );

%-----------------------------------------------------------------------------%
%-----------------------------------------------------------------------------%

