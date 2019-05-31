/** @group globals.extensional
  The sequence of values in array \a x (which must all be in the range 1..\a S)
  is accepted by the DFA of \a Q states with input 1..\a S and transition
  function \a d (which maps (1..\a Q, 1..\a S) -> 0..\a Q)) and initial state \a q0
  (which must be in 1..\a Q) and accepting states \a F (which all must be in
  1..\a Q).  We reserve state 0 to be an always failing state. Each edge has an associated cost \a c,
  and \a C is the sum of costs taken on the accepting path for \a x.
*/
predicate cost_regular(array[int] of var int: x, int: Q, int: S,
                       array[int,int] of int: d, int: q0, set of int: F,
                       array[int,int] of int: c, var int: C) =
    assert(Q > 0,
        "cost_regular: 'Q' must be greater than zero",

    assert(S > 0,
        "cost_regular: 'S' must be greater than zero",

    assert(index_set_1of2(d) = 1..Q /\ index_set_2of2(d) == 1..S,
        "cost_regular: the transition function 'd' must be [1..Q,1..S]",

    assert(index_set_1of2(c) = 1..Q /\ index_set_2of2(c) == 1..S,
        "cost_regular: the transition cost function 'c' must be [1..Q,1..S]",

    assert(forall([d[i, j] in 0..Q | i in 1..Q, j in 1..S]),
        "cost_regular: transition function 'd' points to states outside 0..Q",

        % Nb: we need the parentheses around the expression otherwise the
        % parser thinks it's a generator call!
    assert((q0 in 1..Q),
        "cost_regular: start state 'q0' not in 1..Q",

    assert(F subset 1..Q,
        "cost_regular: final states in 'F' contain states outside 1..Q",

	fzn_cost_regular(x,Q,S,d,q0,F,c,C)

     )))))));

include "fzn_cost_regular.mzn";
