/** @group globals.extensional
  The sequence of values in array \a x (which must all be in the range 1..\a S)
  is accepted by the DFA of \a Q states with input 1..\a S and transition
  function \a d (which maps (1..\a Q, 1..\a S) -> 0..\a Q)) and initial state \a q0
  (which must be in 1..\a Q) and accepting states \a F (which all must be in
  1..\a Q).  We reserve state 0 to be an always failing state.
*/
predicate fzn_regular(array[int] of var int: x, int: Q, int: S,
                  array[int,int] of int: d, int: q0, set of int: F) =
        let {
            % If x has index set m..n-1, then a[m] holds the initial state
            % (q0), and a[i+1] holds the state we're in after  processing
            % x[i].  If a[n] is in F, then we succeed (ie. accept the string).
              int: m = min(index_set(x)),
              int: n = max(index_set(x)) + 1,
              array[m..n] of var 1..Q: a
            } in
        a[m] = q0 /\                    % Set a[0].
        forall(i in index_set(x)) (
            x[i] in 1..S /\             % Do this in case it's a var.
            a[i+1] = d[a[i], x[i]]      % Determine a[i+1].
        ) /\
        a[n] in F                       % Check the final state is in F.
	;

/** @group globals.extensional
  The sequence of values in array \a x (which must all be in the set \a S)
  is accepted by the DFA of \a Q states with input \a S and transition
  function \a d (which maps (1..\a Q, \a S) -> 0..\a Q)) and initial state \a q0
  (which must be in 1..\a Q) and accepting states \a F (which all must be in
  1..\a Q).  We reserve state 0 to be an always failing state.
*/
predicate fzn_regular(array[int] of var int: x, int: Q, set of int: S,
                  array[int,int] of int: d, int: q0, set of int: F) =
        let {
            % If x has index set m..n-1, then a[m] holds the initial state
            % (q0), and a[i+1] holds the state we're in after  processing
            % x[i].  If a[n] is in F, then we succeed (ie. accept the string).
              int: m = min(index_set(x)),
              int: n = max(index_set(x)) + 1,
              array[m..n] of var 1..Q: a
            } in
        a[m] = q0 /\                    % Set a[0].
        forall(i in index_set(x)) (
            x[i] in S /\                % Do this in case it's a var.
            a[i+1] = d[a[i], x[i]]      % Determine a[i+1].
        ) /\
        a[n] in F                       % Check the final state is in F.
	;
