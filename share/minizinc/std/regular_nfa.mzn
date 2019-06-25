include "fzn_regular_nfa.mzn";
include "fzn_regular_nfa_reif.mzn";
include "fzn_regular_nfa_set.mzn";
include "fzn_regular_nfa_set_reif.mzn";

/** @group globals.extensional
  The sequence of values in array \a x (which must all be in the range 1..\a S)
  is accepted by the NFA of \a Q states with input 1..\a S and transition
  function \a d (which maps (1..\a Q, 1..\a S) -> set of 1..\a Q)) and initial state \a q0
  (which must be in 1..\a Q) and accepting states \a F (which all must be in
  1..\a Q).
*/
predicate regular_nfa(array[int] of var int: x, int: Q, int: S,
                      array[int,int] of set of int: d, int: q0, set of int: F) =
     assert(Q > 0,
        "regular_nfa: 'Q' must be greater than zero",

    assert(S > 0,
        "regular_nfa: 'S' must be greater than zero",

    assert(index_set_1of2(d) = 1..Q /\ index_set_2of2(d) == 1..S,
        "regular_nfa: the transition function 'd' must be [1..Q,1..S]",

    assert(forall([d[i, j] subset 1..Q | i in 1..Q, j in 1..S]),
        "regular_nfa: transition function 'd' points to states outside 1..Q",

        % Nb: we need the parentheses around the expression otherwise the
        % parser thinks it's a generator call!
    assert((q0 in 1..Q),
        "regular: start state 'q0' not in 1..Q",

    assert(F subset 1..Q,
        "regular: final states in 'F' contain states outside 1..Q",

        fzn_regular_nfa(x,Q,S,d,q0,F)

     ))))));
     
/** @group globals.extensional
  The sequence of values in array \a x (which must all be in the range \a S)
  is accepted by the NFA of \a Q states with input \a S and transition
  function \a d (which maps (1..\a Q, \a S) -> set of 1..\a Q)) and initial state \a q0
  (which must be in 1..\a Q) and accepting states \a F (which all must be in
  1..\a Q).
*/
predicate regular_nfa(array[int] of var int: x, int: Q, set of int: S,
                      array[int,int] of set of int: d, int: q0, set of int: F) =
     assert(Q > 0,
        "regular_nfa: 'Q' must be greater than zero",

    assert(card(S) > 0,
        "regular_nfa: 'S' must be non empty",

    assert(S = min(S)..max(S),
        "regular_nfa: 'S' must be a range",

    assert(index_set_1of2(d) = 1..Q /\ index_set_2of2(d) == S,
        "regular_nfa: the transition function 'd' must be [1..Q,S]",

    assert(forall([d[i, j] subset 1..Q | i in 1..Q, j in S]),
        "regular_nfa: transition function 'd' points to states outside 1..Q",

        % Nb: we need the parentheses around the expression otherwise the
        % parser thinks it's a generator call!
    assert((q0 in 1..Q),
        "regular: start state 'q0' not in 1..Q",

    assert(F subset 1..Q,
        "regular: final states in 'F' contain states outside 1..Q",

        fzn_regular_nfa(x,Q,S,d,q0,F)

     )))))));
