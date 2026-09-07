include "fzn_edit_distance_exact.mzn";

predicate fzn_edit_distance(int: max_char,
                        array[int] of var int: S1,
                        array[int] of var int: S2,
                        var int: ED) =
    let {
          int: len = length(S1);
          var 0..len: p1;
          var 0..len: p2;
          var 0..len: kept_sum;
          int: l1 = min(index_set(S1));
          int: l2 = min(index_set(S2));
          int: u1 = max(index_set(S1));
          int: u2 = max(index_set(S2));
          int: na1 = l1-1;
          int: na2 = l2-1;
          array[1..len] of var na2..u2: kept_r1_from_position;
          array[1..len] of var na1..u1: kept_r2_from_position;
        } in
       ED = p1 + p2 - 2 * kept_sum                
       /\
       forall(i in 1..len where kept_r1_from_position[i] != na2) (
         S1[kept_r2_from_position[i]] = S2[kept_r1_from_position[i]]
       )
       /\    
       fzn_edit_distance_kept(max_char, S1, S2, p1, p2, kept_r1_from_position, kept_r2_from_position, kept_sum);

predicate fzn_edit_distance(int: max_char,
                        int: C_ins,
                        int: C_del,
                        int: C_sub,
                        array[int] of var int: S1,
                        array[int] of var int: S2,
                        var int: ED) =
    let { int: len = length(S1);
          var 0..len: p1;
          var 0..len: p2;
          var 0..len: kept_sum;
          var 0..len: insertions;
          var 0..len: deletions;
          var 0..len: substitutions;
          int: l1 = min(index_set(S1));
          int: l2 = min(index_set(S2));
          int: u1 = max(index_set(S1));
          int: u2 = max(index_set(S2));
          int: na1 = l1-1;
          int: na2 = l2-1;
          array[1..len] of var na2..u2: kept_r1_from_position;
          array[1..len] of var na1..u1: kept_r2_from_position;
        } in
    substitutions = count(i in 1..len)(kept_r1_from_position[i] != na2 /\ S1[kept_r2_from_position[i]] != S2[kept_r1_from_position[i]])
    /\
    insertions = p2 - kept_sum
    /\
    deletions = p1 - kept_sum
    /\
    ED = C_ins * insertions + C_sub * substitutions + C_del * deletions
    /\                
    fzn_edit_distance_kept(max_char, S1, S2, p1, p2, kept_r1_from_position, kept_r2_from_position, kept_sum);
    
% Decomposition from the paper "Constraint-Based Scheduling for Paint Shops in the Automotive Supply Industry", Winter and Musliu, ACM Transactions on Intelligent Systems and Technology, 2020.
predicate fzn_edit_distance_kept(int: max_char,
                        array[int] of var int: S1,
                        array[int] of var int: S2,
                        var int: p1,
                        var int: p2,
                        array[int] of var int: kept_r1_from_position,
                        array[int] of var int: kept_r2_from_position,
                        var int: kept_sum) =
    let { int: l1 = min(index_set(S1));
          int: l2 = min(index_set(S2));
          int: u1 = max(index_set(S1));
          int: u2 = max(index_set(S2));
          int: len = length(S1);
          int: na1 = l1-1;
          int: na2 = l2-1;
          var 0..len: insertions;
          var 0..len: deletions;
          var 0..len: substitutions;
        } in
    p1 = sum(i in l1..u1 where S1[i] != 0)(1)
    /\
    p2 = sum(i in l2..u2 where S2[i] != 0)(1)
    /\
    kept_sum = sum(i in 1..len where kept_r1_from_position[i] != na2)(1)
    /\
    forall(i in l1..u1-1)(
      S1[i] == 0 -> S1[i+1] == 0
    )
    /\
    forall(i in l2..u2-1)(
      S2[i] == 0 -> S2[i+1] == 0
    )
    /\
    forall(i in 2..len where kept_r1_from_position[i] != na2) (
      kept_r1_from_position[i] > kept_r1_from_position[i-1]
    )
    /\
    forall(i in 2..len where kept_r2_from_position[i] != na1) (
      kept_r2_from_position[i] > kept_r2_from_position[i-1]
    )
    /\
    forall(i in 1..len) (
      kept_r1_from_position[i] > na2 <-> kept_r2_from_position[i] > na1
    )
    /\
    forall(i in 1..len-1) (
      (kept_r1_from_position[i] = na2) -> (kept_r1_from_position[i+1] = na2)
    )
    /\
    forall(i in 1..len-1) (
      (kept_r2_from_position[i] = na1) -> (kept_r2_from_position[i+1] = na1)
    )
    /\
    forall(i in 1..len where kept_r2_from_position[i] != na1) (
      kept_r2_from_position[i] <= l1-1 + p1
    )
    /\
    forall(i in 1..len where kept_r1_from_position[i] != na2) (
      kept_r1_from_position[i] <= l2-1 + p2
    );

predicate fzn_edit_distance(int: max_char,
                        array[int] of int: W_ins,
                        array[int] of int: W_del,
                        array[int, int] of int: W_sub,
                        array[int] of var int: S1,
                        array[int] of var int: S2,
                        var int: ED) =
    fzn_edit_distance_exact(max_char, W_ins, W_del, W_sub, S1, S2, ED);