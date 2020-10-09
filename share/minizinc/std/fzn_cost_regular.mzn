predicate fzn_cost_regular(array[int] of var int: x, int: Q, int: S,
                           array[int,int] of int: d, int: q0, set of int: F,
                           array[int,int] of int: c, var int: C) =
        let {
            % If x has index set m..n-1, then a[m] holds the initial state
            % (q0), and a[i+1] holds the state we're in after  processing
            % x[i].  If a[n] is in F, then we succeed (ie. accept the string).
              int: m = min(index_set(x));
              int: n = max(index_set(x)) + 1;
              array[m..n] of var 1..Q: a;
            % cc[i+1] holds the accumulated cost of edges taken process up to x[i]
              array[m..n] of var int: cc;
            } in
        a[m] = q0 /\                        % Set a[0].
        cc[m] = 0 /\                        % initially zero cost
        forall(i in index_set(x)) (
            x[i] in 1..S /\                 % Do this in case it's a var.
            a[i+1] = d[a[i], x[i]] /\       % Determine a[i+1].
            cc[i+1] = c[a[i],x[i]] + cc[i]  % Calculate new cost sum
        ) /\
        a[n] in F /\                        % Check the final state is in F.
        C = cc[n]                           % return final cost
	;
