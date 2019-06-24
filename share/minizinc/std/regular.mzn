include "fzn_regular.mzn";
include "fzn_regular_set.mzn";

/** @group globals.extensional
  The sequence of values in array \a x (which must all be in the range 1..\a S)
  is accepted by the DFA of \a Q states with input 1..\a S and transition
  function \a d (which maps (1..\a Q, 1..\a S) -> 0..\a Q)) and initial state \a q0
  (which must be in 1..\a Q) and accepting states \a F (which all must be in
  1..\a Q).  We reserve state 0 to be an always failing state.
*/
predicate regular(array[int] of var int: x, int: Q, int: S,
                  array[int,int] of int: d, int: q0, set of int: F) =
    assert(Q > 0,
        "regular: 'Q' must be greater than zero",

    assert(S > 0,
        "regular: 'S' must be greater than zero",

    assert(index_set_1of2(d) = 1..Q /\ index_set_2of2(d) == 1..S,
        "regular: the transition function 'd' must be [1..Q,1..S]",

    assert(forall([d[i, j] in 0..Q | i in 1..Q, j in 1..S]),
        "regular: transition function 'd' points to states outside 0..Q",

        % Nb: we need the parentheses around the expression otherwise the
        % parser thinks it's a generator call!
    assert((q0 in 1..Q),
        "regular: start state 'q0' not in 1..Q",
    assert(F subset 1..Q,
        "regular: final states in 'F' contain states outside 1..Q",

        fzn_regular(x,Q,S,d,q0,F)

     ))))));

/** @group globals.extensional
  The sequence of values in array \a x (which must all be in the set \a S)
  is accepted by the DFA of \a Q states with input \a S and transition
  function \a d (which maps (1..\a Q, \a S) -> 0..\a Q)) and initial state \a q0
  (which must be in 1..\a Q) and accepting states \a F (which all must be in
  1..\a Q).  We reserve state 0 to be an always failing state.
*/
predicate regular(array[int] of var int: x, int: Q, set of int: S,
                  array[int,int] of int: d, int: q0, set of int: F) =
    assert(Q > 0,
        "regular: 'Q' must be greater than zero",

    assert(card(S) > 0,
        "regular: 'S' must be non empty",

    assert(index_set_1of2(d) = 1..Q /\ index_set_2of2(d) == S,
        "regular: the transition function 'd' must be [1..Q,S]",

    assert(forall([d[i, j] in 0..Q | i in 1..Q, j in S]),
        "regular: transition function 'd' points to states outside 0..Q",

        % Nb: we need the parentheses around the expression otherwise the
        % parser thinks it's a generator call!
    assert((q0 in 1..Q),
        "regular: start state 'q0' not in 1..Q",
    assert(F subset 1..Q,
        "regular: final states in 'F' contain states outside 1..Q",

        fzn_regular(x,Q,S,d,q0,F)

     ))))));
