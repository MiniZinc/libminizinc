/** @group globals.counting
  Requires that \a card[\p i] is the number of occurences of \a value[\p i] in
  \a base. The values in \a value need not be distinct.
*/
predicate fzn_distribute(array[int] of var int: card,
                     array[int] of var int: value,
                     array[int] of var int: base) =
        forall (i in index_set(card)) (
            card[i] == sum(j in index_set(base)) (
                            bool2int(value[i] = base[j])
                       )
        );

