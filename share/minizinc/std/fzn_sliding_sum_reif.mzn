predicate fzn_sliding_sum_reif(int: low, int: up, int: seq, 
                               array[int] of var int: vs, var bool: b) =
    /* CS decomposition: see S. Brand, N. Narodytska, C-G. Quimper, P.J. Stuckey, and T. Walsh. Encodings of the sequence constraint. In C. Bessiere, editor, Proceedings of the 13th International Conference on Principles and Practice of Constraint Programming, volume 4741 of LNCS, pages 210–224. Springer-Verlag, 2007.
    */
    let { int: lx = min(index_set(vs));
          int: ux = max(index_set(vs));
          array[lx-1..ux] of var int: S;
        } in
        S[lx-1] = 0 /\     
        forall (i in lx .. ux) (  
            S[i] = vs[i] + S[i-1]
        ) /\
        b <-> forall (i in lx-1 .. ux - seq) (
                      S[i] <= S[i+seq] - low /\
                      S[i+seq] <= S[i] + up
                     );

