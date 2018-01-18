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

%          my_trace("  regular: index_set(x)=" ++ show(index_set(x))
%            ++ ",  dom_array(x)=" ++ show(dom_array(x))
%            ++ ",  dom_array(a)=" ++ show(1..Q)
%            ++ "\n") /\
        let {
            % If x has index set m..n-1, then a[m] holds the initial state
            % (q0), and a[i+1] holds the state we're in after  processing
            % x[i].  If a[n] is in F, then we succeed (ie. accept the string).
              int: m = min(index_set(x)),
              int: n = max(index_set(x)) + 1,
              array[m..n] of var 1..Q: a,
              constraint a[m] = q0 /\                    % Set a[0].
                         a[n] in F,                      % Check the final state is in F.
              constraint
                forall(i in index_set(x)) (
                    x[i] in 1..S             % Do this in case it's a var.
                      /\  %% trying to eliminate non-reachable states:
                    let {
                      set of int: va_R = { d[va, vx] | va in dom(a[i]), vx in dom(x[i]) } diff { 0 }     %% Bug in MZN 2.0.4
                    } in
                        a[i+1] in va_R
                )
            } in
      let {
        constraint
        forall(i in [n-i | i in 1..length(x)]) (
          a[i] in { va | va in dom(a[i])
            where exists(vx in dom(x[i]))(d[va, vx] in dom(a[i+1])) }
          /\
          x[i] in { vx | vx in dom(x[i])
            where exists(va in dom(a[i]))(d[va, vx] in dom(a[i+1])) }
        )
      } in
        forall(i in index_set(x)) (
            let {
              set of int: va_R = { d[va, vx] | va in dom(a[i]), vx in dom(x[i]) } diff { 0 }     %% Bug in MZN 2.0.4
            } in
%              my_trace("   S" ++ show(i)
%                ++ ": dom(a[i])=" ++ show(dom(a[i]))
%                ++ ", va_R="++show(va_R)
%                ++ ", index_set_2of2(eq_a) diff va_R=" ++ show(index_set_2of2(eq_a) diff va_R)
%                ++ ", dom(a[i+1])=" ++ show(dom(a[i+1]))
%              ) /\
              a[i+1] in va_R
              %/\ a[i+1] in min(va_R)..max(va_R)
        )
%         /\ my_trace("  regular -- domains after prop: index_set(x)=" ++ show(index_set(x))
%            ++ ",  dom_array(x)=" ++ show(dom_array(x))
%            ++ ",  dom_array(a)=" ++ show(dom_array(a))
%            ++ "\n")
%        /\ my_trace("\n")
        /\
        let {
              array[int, int] of var int: eq_a=eq_encode(a),
              array[int, int] of var int: eq_x=eq_encode(x),
            } in
        forall(i in index_set(x)) (
   %         a[i+1] = d[a[i], x[i]]      % Determine a[i+1].
        if card(dom(a[i]))*card(dom(x[i])) > nMZN__UnarySizeMax_1step_regular then
       %% Implication  decomposition:
          forall(va in dom(a[i]), vx in dom(x[i]))(
            if d[va, vx] in dom(a[i+1]) then
              eq_a[i+1, d[va, vx]] >= eq_a[i, va] + eq_x[i, vx] - 1    %% The only-if part of conj
            else
              1 >= eq_a[i, va] + eq_x[i, vx]
            endif
          )
        else
       %% Network-flow decomposition: 
       %% {regularIP07} M.-C. C{\^o}t{\'e}, B.~Gendron, and L.-M. Rousseau.
                %% \newblock Modeling the regular constraint with integer programming.
          let {
       %       array[int, int] of set of int: VX_a12 =   %% set of x for given a1 that produce a2
       %         array2d(1..S, 1..Q, [ { vx | vx in 1..S where d[va1, vx]==va2 } | va1 in dom(a[i]), va2 in dom(a[i+1]) ]);
              array[int, int] of var int: ppAX = eq_encode(a[i], x[i]);
          } in
            forall (va2 in dom(a[i+1])) (
              eq_a[i+1, va2] = sum(va1 in dom(a[i]),  vx in dom(x[i]) where d[va1, vx]==va2)
                                (ppAX[va1, vx])
            )
            /\
            forall(va1 in dom(a[i]), vx in dom(x[i]))(
              if not (d[va1, vx] in dom(a[i+1])) then
                ppAX[va1, vx] == 0
              else
                true
              endif
            )
        endif
        )

    ))))));
