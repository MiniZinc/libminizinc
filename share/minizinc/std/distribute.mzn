/** @group globals.counting
  Requires that \a card[\p i] is the number of occurences of \a value[\p i] in
  \a base. The values in \a value need not be distinct.
*/
predicate distribute(array[int] of var int: card,
                     array[int] of var int: value,
                     array[int] of var int: base) =
    assert(index_set(card) == index_set(value),
        "distribute: card and value arrays must have identical index sets",
	fzn_distribute(card, value, base)
    );

/** @group globals.counting
  Returns an array of the number of occurences of \a value[\p i] in
  \a base. The values in \a value need not be distinct.
*/
function array[int] of var int: distribute(array[int] of var int: value,
                                           array[int] of var int: base) :: promise_total =
  let { array[index_set(value)] of var 0..length(base): card;
        constraint fzn_distribute(card, value, base); } 
  in card;

include "fzn_distribute.mzn";
