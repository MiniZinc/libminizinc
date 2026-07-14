predicate fzn_regular_nfa(
  array [int] of var int: xs,
  int: Q,
  set of int: S,
  array [int, int] of set of int: d,
  int: q0,
  set of int: F,
) =
  if length(xs) = 0 then
    q0 in F
  else
    let {
      % If xs has index set m..n-1, then a[m] holds the initial state
      % (q0), and a[i+1] holds the state we're in after  processing
      % xs[i].  If a[n] is in F, then we succeed (ie. accept the string).
      int: m = min(index_set(xs));
      int: n = max(index_set(xs)) + 1;
      array [m..n] of var 1..Q: a;
    } in a[m] = q0 /\ % Set a[0].
      forall (
        i in index_set(xs),
      ) (
        xs[i] in S /\ % Do this in case it's a var.
          a[i + 1] in d[a[i], xs[i]] % Determine a[i+1].
      ) /\
      a[n] in F % Check the final state is in F.
  endif;
