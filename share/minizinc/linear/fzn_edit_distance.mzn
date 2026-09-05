% Longest common subsequence distance (insertion cost = deletion cost = 1, substitution cost = 2)
predicate fzn_edit_distance(int: max_char,
                        array[int] of var int: S1,
                        array[int] of var int: S2,
                        var int: ED) =
    let { int: len = length(S1);
          var 0..len: p1;
          var 0..len: p2;
          var 0..len: kept_sum;
          array[1..len] of var 0..len: kept_r1_from_position;
          array[1..len] of var 0..len: kept_r2_from_position;
        } in
    ED = p1 + p2 - 2 * kept_sum
    /\
    fzn_edit_distance_kept(max_char, array1d(1..len, S1), array1d(1..len, S2), p1, p2, kept_sum);
    
% Fixed insertion/deletion/substitution costs
predicate fzn_edit_distance(int: max_char,
                        int: C_ins,
                        int: C_del,
                        int: C_sub,
                        array[int] of var int: S1,
                        array[int] of var int: S2,
                        var int: ED) =
    let { int: len = length(S1);
          var 0..len: insertions;
          var 0..len: deletions;
          var 0..len: substitutions;
        } in
    ED = C_ins * insertions + C_sub * substitutions + C_del * deletions
    /\
    fzn_edit_distance_kept_general(max_char, array1d(1..len, S1), array1d(1..len, S2), insertions, deletions, substitutions);

% Linearized decomposition from the paper "Constraint-Based Scheduling for Paint Shops in the Automotive Supply Industry", Winter and Musliu, ACM Transactions on Intelligent Systems and Technology, 2020.
predicate fzn_edit_distance_kept(int: max_char,
                        array[int] of var int: S1,
                        array[int] of var int: S2,
                        var int: p1,
                        var int: p2,
                        var int: kept_sum) =
    let { int: len = length(S1);
          array[1..len] of var 0..len: kept_r1_from_position;
          array[1..len] of var 0..len: kept_r2_from_position;
          array[1..len,0..len] of var 0..1: kept_r2_from_position_i;
          array[1..len,0..len] of var 0..1: kept_r1_from_position_i;
          array[1..len] of var 1..max_char: z;
          array[1..len] of var 0..1: kept;
          array[1..len] of var 0..1: used1;
          array[1..len] of var 0..1: used2;
          array[1..len] of var 0..max_char: kept_char;
        } in
    % see which positions are used (i.e. positions which are not 0)
    forall(i in 1..len)(
      S1[i] <= max_char*used1[i]
      /\
      used1[i] < 1 + S1[i]
      /\
      S2[i] <= max_char*used2[i]
      /\
      used2[i] < 1 + S2[i]
    )
    /\
    p1 = sum(i in 1..len)(used1[i])
    /\
    p2 = sum(i in 1..len)(used2[i])
    /\
    kept_sum = sum(i in 1..len)(kept[i])
    /\
    forall(k in 2..len)(
      kept_r1_from_position[k] + (1-kept[k])*(len+1) > kept_r1_from_position[k-1]
      /\
      kept_r2_from_position[k] + (1-kept[k])*(len+1) > kept_r2_from_position[k-1]
    )
    /\
    forall(i in 1..len-1)(
      kept[i+1] <= kept[i]
      /\
      used1[i+1] <= used1[i]
      /\
      used2[i+1] <= used2[i]
    )
    /\
    forall(k in 1..len)(
      kept[k]*len >= kept_r1_from_position[k]
      /\
      kept[k]*len >= kept_r2_from_position[k]
      /\
      kept[k] <= kept_r1_from_position[k]
      /\
      kept[k] <= kept_r2_from_position[k]
    )
    /\
    forall(k in 1..len)(
      kept_r2_from_position[k] - (1-kept[k])*len <= p1
      /\
      kept_r1_from_position[k] - (1-kept[k])*len <= p2
    )
    /\
    forall(i in 1..len)(
      sum(j in 0..len)(kept_r2_from_position_i[i,j]) == 1
      /\
      sum(j in 0..len)(kept_r2_from_position_i[i,j]*j) == kept_r2_from_position[i]
      /\
      sum(j in 0..len)(kept_r1_from_position_i[i,j]) == 1
      /\
      sum(j in 0..len)(kept_r1_from_position_i[i,j]*j) == kept_r1_from_position[i]
    )
    /\
    forall(i in 1..len)(
      z[i] <= 1 + (1-kept_r2_from_position_i[i,0])*max_char
    )
    /\
    forall(i in 1..len, j in 1..len)(
      z[i] - (1-kept[i])*max_char <= S1[j] + (1-kept_r2_from_position_i[i,j])*max_char
      /\
      z[i] + (1-kept[i])*max_char >= S1[j] - (1-kept_r2_from_position_i[i,j])*max_char
    )
    /\
    forall(i in 1..len, j in 1..len)(
      z[i] - (1-kept[i])*max_char <= S2[j] + (1-kept_r1_from_position_i[i,j])*max_char
      /\
      z[i] + (1-kept[i])*max_char >= S2[j] - (1-kept_r1_from_position_i[i,j])*max_char
    );    
  
% Linearized decomposition based on the paper "Constraint-Based Scheduling for Paint Shops in the Automotive Supply Industry", Winter and Musliu, ACM Transactions on Intelligent Systems and Technology, 2020.
predicate fzn_edit_distance_kept_general(int: max_char,
                        array[int] of var int: S1,
                        array[int] of var int: S2,
                        var int: insertions,
                        var int: deletions,
                        var int: substitutions) =
    let { int: len = length(S1);
          var 0..len: p1;
          var 0..len: p2;
          var 0..1: p1_greater;
          var 0..len: kept_sum;
          array[1..len] of var 0..len: kept_r1_from_position;
          array[1..len] of var 0..len: kept_r2_from_position;
          array[1..len,0..len] of var 0..1: kept_r2_from_position_i;
          array[1..len,0..len] of var 0..1: kept_r1_from_position_i;
          array[1..len] of var 1..max_char: z;
          array[1..len] of var 0..1: kept;
          array[1..len] of var 0..1: kept_equal;
          array[1..len] of var 0..1: used1;
          array[1..len] of var 0..1: used2;
          array[1..len] of var 0..max_char: kept_char;
        } in
    substitutions = sum(i in 1..len)(1-kept_equal[i])
    /\
    insertions >= p2 - kept_sum
    /\
    deletions >= p1 - kept_sum
    /\
    forall(i in 1..len)(
      S1[i] <= max_char*used1[i]
      /\
      used1[i] < 1 + S1[i]
      /\
      S2[i] <= max_char*used2[i]
      /\
      used2[i] < 1 + S2[i]
    )
    /\
    p1 = sum(i in 1..len)(used1[i])
    /\
    p2 = sum(i in 1..len)(used2[i])
    /\
    kept_sum = sum(i in 1..len)(kept[i])
    /\
    forall(k in 2..len)(
      kept_r1_from_position[k] + (1-kept[k])*(len+1) > kept_r1_from_position[k-1]
      /\
      kept_r2_from_position[k] + (1-kept[k])*(len+1) > kept_r2_from_position[k-1]
    )
    /\
    forall(i in 1..len-1)(
      kept[i+1] <= kept[i]
      /\
      used1[i+1] <= used1[i]
      /\
      used2[i+1] <= used2[i]
    )
    /\
    forall(k in 1..len)(
      kept[k]*len >= kept_r1_from_position[k]
      /\
      kept[k]*len >= kept_r2_from_position[k]
      /\
      kept[k] <= kept_r1_from_position[k]
      /\
      kept[k] <= kept_r2_from_position[k]
      /\
      kept[k] >= 1-kept_equal[k]
    )
    /\
    forall(k in 1..len)(
      kept_r2_from_position[k] - (1-kept[k])*len <= p1
      /\
      kept_r1_from_position[k] - (1-kept[k])*len <= p2
    )
    /\
    forall(i in 1..len)(
      sum(j in 0..len)(kept_r2_from_position_i[i,j]) == 1
      /\
      sum(j in 0..len)(kept_r2_from_position_i[i,j]*j) == kept_r2_from_position[i]
      /\
      sum(j in 0..len)(kept_r1_from_position_i[i,j]) == 1
      /\
      sum(j in 0..len)(kept_r1_from_position_i[i,j]*j) == kept_r1_from_position[i]
    )
    /\
    forall(i in 1..len)(
      z[i] <= 1 + (1-kept_r2_from_position_i[i,0])*max_char
    )
    /\
    forall(i in 1..len, j in 1..len)(
      z[i] - (1-kept_equal[i])*max_char <= S1[j] + (1-kept_r2_from_position_i[i,j])*max_char
      /\
      z[i] + (1-kept_equal[i])*max_char >= S1[j] - (1-kept_r2_from_position_i[i,j])*max_char
    )
    /\
    forall(i in 1..len, j in 1..len)(
      z[i] - (1-kept_equal[i])*max_char <= S2[j] + (1-kept_r1_from_position_i[i,j])*max_char
      /\
      z[i] + (1-kept_equal[i])*max_char >= S2[j] - (1-kept_r1_from_position_i[i,j])*max_char
    );    
        
% Based on the MIP formulation proposed in the paper:
% "Finding median and center strings for a probability distribution on a set of strings under Levenshtein distance based on integer linear programming", Hayashida and Koyano, BIOSTEC 2016.
predicate fzn_edit_distance(int: max_char,
                        array[int] of int: W_ins,
                        array[int] of int: W_del,
                        array[int, int] of int: W_sub,
                        array[int] of var int: S1,
                        array[int] of var int: S2,
                        var int: ED) =
    let { int: len = length(S1);
          
          array[int] of var int: O1 = array1d(1..len, S1);
          array[int] of var int: O2 = array1d(1..len, S2);
          array[int] of int: W_ins_0 = array1d(0..length(W_ins), [0] ++ W_ins);
          int: max_ic = max(W_ins);
          array[int] of int: W_del_0 = array1d(0..length(W_del), [0] ++ W_del);
          int: max_dc = max(W_del);
          array[int, int] of int: W_sub_0 = array2d(0..max_char, 0..max_char,
                                                   [ if i == 0 then W_ins_0[j] else if j == 0 then W_del_0[i] else W_sub[i,j] endif endif | i in 0..max_char, j in 0..max_char]);
          int: max_sc = max(W_sub_0);

          array[0..len, 0..len] of var 0..1: x;
          array[0..len, 0..len] of var 0..max_dc: x_c;
          array[0..len, 0..len] of var 0..1: y;
          array[0..len, 0..len] of var 0..max_ic: y_c;
          array[0..len, 0..len] of var 0..1: z;

          array[1..len, 1..len] of var 0..1: g;
          array[1..len, 1..len] of var 0..1: h;
          array[1..len, 1..len] of var 0..max_sc: h_c;


          array[1..len] of var 0..1: used1;
          array[1..len] of var 0..1: used2;
        } in
    % see which positions are used (i.e. positions which are not 0)
    forall(i in 1..len)(
      O1[i] <= max_char*used1[i]
      /\
      used1[i] < 1 + O1[i]
    )
    /\
    forall(i in 1..len)(
      O2[i] <= max_char*used2[i]
      /\
      used2[i] < 1 + O2[i]
    )
    /\
    % check that strings are correctly aligned
    len = len
    /\
    forall(i in 1..len-1)(
      used1[i+1] <= used1[i]
    )
    /\
    forall(i in 1..len-1)(
      used2[i+1] <= used2[i]
    )
    /\
    % a1
    1 = x[1,0] + y[1,0] + z[1,1]
    /\
    % a2
    forall(i in 1..len-1)(
      x[i,0] = x[i+1,0] + y[i,1] + z[i+1,1]
    )
    /\
    % a3
    x[len,0] = y[len,1]
    /\
    % a4
    forall(j in 1..len-1)(
      y[0,j] = x[1,j] + y[0,j+1] + z[1,j+1]
    )
    /\
    % a5
    y[0,len] = x[1,len]
    /\
    % a6
    forall(i in 1..len-1, j in 1..len-1)(
      x[i,j] + y[i,j] + z[i,j] = x[i+1,j] + y[i,j+1] + z[i+1,j+1]
    )
    /\
    % a7
    forall(j in 1..len-1)(
      x[len,j] + y[len,j] + z[len,j] = y[len,j+1]
    )
    /\
    % a8
    forall(i in 1..len-1)(
      x[i,len] + y[i,len] + z[i,len] = x[i+1,len]
    )
    /\
    % a9
    x[len,len] + y[len,len] + z[len,len] = 1
    /\
    % special 0 length string constraint, we need to force a variable in this case
    y[0,len] * len >= len * (1-len)
    /\
    % b
    forall(j in 1..len)(
      y[len,j] >= (1/len) * (j-len)
    )
    /\
    % c1
    forall(i in 1..len, j in 1..len)(
      O1[i] - O2[j] <= max_char * g[i,j]
    )
    /\
    % c2
    forall(i in 1..len, j in 1..len)(
       O2[j] - O1[i] <= max_char * g[i,j]
    )
    /\
    % d2
    forall(i in 1..len, j in 1..len)(
      h[i,j] >= z[i,j] + g[i,j] - 1
    )
    /\
    forall(i in 1..len, j in 1..len)(
      h[i,j] <= 1/2 * (z[i,j] + g[i,j])
    )
    /\
    % if the empty character zero is used, we forbid substitution
    forall(i in 1..len, j in 1..len)(
      O1[i] >= h[i,j]
      /\
      O2[j] >= h[i,j]
    )
    /\
    % linearize deletion costs
    forall(i in 1..len, j in 0..len)(
      x_c[i,j] >= W_del_0[O1[i]] - (1-x[i,j])*max_dc
    )
    /\
    % linearize insertion costs
    forall(i in 0..len, j in 1..len)(
      y_c[i,j] >= W_ins_0[O2[j]] - (1-y[i,j])*max_ic
    )
    /\
    % linearize substitution costs
    forall(i in 1..len, j in 1..len)(
      h_c[i,j] >= W_sub_0[O1[i],O2[j]] - (1-h[i,j])*max_sc
    )
    /\
    ED = sum(i in 1..len)(x_c[i,0])
         + sum(j in 1..len)(y_c[0,j])
         + sum(i in 1..len, j in 1..len)(x_c[i,j] + y_c[i,j] + h_c[i,j]);