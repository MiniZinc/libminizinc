predicate fzn_minimum_arg_int(array[int] of var int: x, var int: z) =
      % general case: min could be 0 or 1
      let { int: l = min(index_set(x)) ;
            int: u = max(index_set(x)) ;
            int: n = u-l+1;
            array[int] of var int: xs = array1d(l..u,[ n*x[j]+j | j in l..u ]);
            var int: Mx = min(xs) ;
      } in
      forall (j in l..u) ( (z != j) = (Mx < xs[j]) );

%%% only the new decomposition from argmax paper CP2020 submission 
