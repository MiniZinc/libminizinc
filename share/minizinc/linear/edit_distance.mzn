% Longest common subsequence distance (insertion cost = deletion cost = 1, substitution cost = 2)
predicate edit_distance(int: max_char,
                        array[int] of var int: S1,
                        array[int] of var int: S2,
                        var int: ED) =
    let { int: u1 = length(S1);
          var 0..u1: p1;
          var 0..u1: p2;
          var 0..u1: kept_sum;
        } in
    ED = p1 + p2 - 2*kept_sum
    /\
    edit_distance_kept(max_char, S1, S2, p1, p2, kept_sum);
    
% Fixed insertion/deletion/substitution costs
predicate edit_distance(int: max_char,
                        int: C_ins,
                        int: C_del,
                        int: C_sub,
                        array[int] of var int: S1,
                        array[int] of var int: S2,
                        var int: ED) =
    let { int: u1 = length(S1);
          var 0..u1: p1;
          var 0..u1: p2;
          var 0..1: p1_greater;
          var 0..u1: kept_sum;
          var 0..u1: insertions;
          var 0..u1: deletions;
          var 0..u1: substitutions;
        } in
    p1 + (1-p1_greater)*u1 >= p2
    /\
    p2 + p1_greater*u1 >= p1
    /\
    substitutions >= p1 - kept_sum - p1_greater * u1
    /\
    substitutions >= p2 - kept_sum - (1-p1_greater) * u1
    /\
    insertions >= p2-p1
    /\
    deletions >= p1-p2
    /\
    ED = C_ins * insertions + C_sub * substitutions + C_del * deletions
    /\
    edit_distance_kept(max_char, S1, S2, p1, p2, kept_sum);

% Decomposition from the paper "Constraint-Based Scheduling for Paint Shops in the Automotive Supply Industry", Winter and Musliu, ACM Transactions on Intelligent Systems and Technology, 2020.
predicate edit_distance_kept(int: max_char,
                        array[int] of var int: S1,
                        array[int] of var int: S2,
                        var int: p1,
                        var int: p2,
                        var int: kept_sum) =
    let { int: u1 = length(S1);
          int: u2 = length(S2);
          array[1..u1] of var 0..u1: kept_r1_from_position;
          array[1..u1] of var 0..u1: kept_r2_from_position;
          array[1..u1,0..u1] of var 0..1: kept_r2_from_position_i;
          array[1..u1,0..u1] of var 0..1: kept_r1_from_position_i;
          array[1..u1] of var 1..max_char: z;
          array [1..u1] of var 0..1: kept;
          array [1..u1] of var 0..1: used1;
          array [1..u1] of var 0..1: used2;
          array [1..u1] of var 0..max_char: kept_char;
        } in
    % check that strings are correctly aligned
    u1 = u2
    /\
    % see which positions are used (i.e. positions which are not 0)
    forall(i in 1..u1)(
      S1[i] <= max_char*used1[i]
      /\
      used1[i] < 1 + S1[i]
      /\
      S2[i] <= max_char*used2[i]
      /\
      used2[i] < 1 + S2[i]
    )
    % determine lengths and substitutions
    /\
    p1 = sum(i in 1..u1)(used1[i])
    /\
    p2 = sum(i in 1..u2)(used2[i])
    /\
    kept_sum = sum(i in 1..u1)(kept[i])
    /\
    forall(k in 2..u1)(
      kept_r1_from_position[k] + (1-kept[k])*u1 > kept_r1_from_position[k-1]
      /\
      kept_r2_from_position[k] + (1-kept[k])*u1 > kept_r2_from_position[k-1]
    )
    /\
    forall(i in 1..u1-1)(
      kept[i+1] <= kept[i]
      /\
      used1[i+1] <= used1[i]
      /\
      used2[i+1] <= used2[i]
    )
    /\
    forall(k in 1..u1)(
      kept[k]*u1 >= kept_r1_from_position[k]
      /\
      kept[k]*u1 >= kept_r2_from_position[k]
      /\
      kept[k] <= kept_r1_from_position[k]
      /\
      kept[k] <= kept_r2_from_position[k]
    )
    /\
    forall(k in 1..u1)(
      kept_r2_from_position[k] - (1-kept[k])*u1 <= p1
      /\
      kept_r1_from_position[k] - (1-kept[k])*u2 <= p2
    )
    /\
    forall(i in 1..u1)(
      sum(j in 0..u1)(kept_r2_from_position_i[i,j]) == 1
      /\
      sum(j in 0..u1)(kept_r2_from_position_i[i,j]*j) == kept_r2_from_position[i]
      /\
      sum(j in 0..u1)(kept_r1_from_position_i[i,j]) == 1
      /\
      sum(j in 0..u1)(kept_r1_from_position_i[i,j]*j) == kept_r1_from_position[i]
    )
    /\
    forall(i in 1..u1)(
      z[i] <= 1 + (1-kept_r2_from_position_i[i,0])*max_char
    )
    /\
    forall(i in 1..u1, j in 1..u1)(
      z[i] <= S1[j] + (1-kept_r2_from_position_i[i,j])*max_char
      /\
      z[i] >= S1[j] - (1-kept_r2_from_position_i[i,j])*max_char
    )
    /\
    forall(i in 1..u1, j in 1..u1)(
      z[i] <= S2[j] + (1-kept_r1_from_position_i[i,j])*max_char
      /\
      z[i] >= S2[j] - (1-kept_r1_from_position_i[i,j])*max_char
    );
    
% Based on the MIP formulation proposed in the paper:
% "Finding median and center strings for a probability distribution on a set of strings under Levenshtein distance based on integer linear programming", Hayashida and Koyano, BIOSTEC 2016.
predicate edit_distance(int: max_char,
                        array[int] of int: W_ins,
                        array[int] of int: W_del,
                        array[int, int] of int: W_sub,
                        array[int] of var int: S1,
                        array[int] of var int: S2,
                        var int: ED) =
    let { int: u1 = length(S1);
          int: u2 = length(S2);
          
          array[int] of int: W_ins_0 = array1d(0..length(W_ins), [0] ++ W_ins);
          int: max_ic = max(W_ins);
          array[int] of int: W_del_0 = array1d(0..length(W_del), [0] ++ W_del);
          int: max_dc = max(W_del);
          array[int, int] of int: W_sub_0 = array2d(0..max_char, 0..max_char,
                                                   [ if i == 0 then W_ins_0[j] else if j == 0 then W_del_0[i] else W_sub[i,j] endif endif | i in 0..max_char, j in 0..max_char]);
          int: max_sc = max(W_sub_0);

          array [0..u1, 0..u2] of var 0..1: x;
          array [0..u1, 0..u2] of var 0..max_dc: x_c;
          array [0..u1, 0..u2] of var 0..1: y;
          array [0..u1, 0..u2] of var 0..max_ic: y_c;
          array [0..u1, 0..u2] of var 0..1: z;

          array [1..u1, 1..u2] of var 0..1: g;
          array [1..u1, 1..u2] of var 0..1: h;
          array [1..u1, 1..u2] of var 0..max_sc: h_c;


          array [1..u1] of var 0..1: used1;
          array [1..u2] of var 0..1: used2;
        } in
    % see which positions are used (i.e. positions which are not 0)
    forall(i in 1..u1)(
      S1[i] <= max_char*used1[i]
      /\
      used1[i] < 1 + S1[i]
    )
    /\
    forall(i in 1..u2)(
      S2[i] <= max_char*used2[i]
      /\
      used2[i] < 1 + S2[i]
    )
    /\
    % check that strings are correctly aligned
    u1 = u2
    /\
    forall(i in 1..u1-1)(
      used1[i+1] <= used1[i]
    )
    /\
    forall(i in 1..u2-1)(
      used2[i+1] <= used2[i]
    )
    /\
    % a1
    1 = x[1,0] + y[1,0] + z[1,1]
    /\
    % a2
    forall(i in 1..u1-1)(
      x[i,0] = x[i+1,0] + y[i,1] + z[i+1,1]
    )
    /\
    % a3
    x[u1,0] = y[u1,1]
    /\
    % a4
    forall(j in 1..u2-1)(
      y[0,j] = x[1,j] + y[0,j+1] + z[1,j+1]
    )
    /\
    % a5
    y[0,u2] = x[1,u2]
    /\
    % a6
    forall(i in 1..u1-1, j in 1..u2-1)(
      x[i,j] + y[i,j] + z[i,j] = x[i+1,j] + y[i,j+1] + z[i+1,j+1]
    )
    /\
    % a7
    forall(j in 1..u2-1)(
      x[u1,j] + y[u1,j] + z[u1,j] = y[u1,j+1]
    )
    /\
    % a8
    forall(i in 1..u1-1)(
      x[i,u2] + y[i,u2] + z[i,u2] = x[i+1,u2]
    )
    /\
    % a9
    x[u1,u2] + y[u1,u2] + z[u1,u2] = 1
    /\
    % special 0 length string constraint, we need to force a variable in this case
    y[0,u2] * u2 >= u2 * (1-u1)
    /\
    % b
    forall(j in 1..u2)(
      y[u1,j] >= (1/u2) * (j-u2)
    )
    /\
    % c1
    forall(i in 1..u1, j in 1..u2)(
      S1[i] - S2[j] <= max_char * g[i,j]
    )
    /\
    % c2
    forall(i in 1..u1, j in 1..u2)(
       S2[j] - S1[i] <= max_char * g[i,j]
    )
    /\
    % d2
    forall(i in 1..u1, j in 1..u2)(
      h[i,j] >= z[i,j] + g[i,j] - 1
    )
    /\
    forall(i in 1..u1, j in 1..u2)(
      h[i,j] <= 1/2 * (z[i,j] + g[i,j])
    )
    /\
    % if the empty character zero is used, we forbid substitution
    forall(i in 1..u1, j in 1..u1)(
      S1[i] >= h[i,j]
      /\
      S2[j] >= h[i,j]
    )
    /\
    % linearize deletion costs
    forall(i in 1..u1, j in 0..u2)(
      x_c[i,j] >= W_del_0[S1[i]] - (1-x[i,j])*max_dc
    )
    /\
    % linearize insertion costs
    forall(i in 0..u1, j in 1..u2)(
      y_c[i,j] >= W_ins_0[S2[j]] - (1-y[i,j])*max_ic
    )
    /\
    % linearize substitution costs
    forall(i in 1..u1, j in 1..u2)(
      h_c[i,j] >= W_sub_0[S1[i],S2[j]] - (1-h[i,j])*max_sc
    )
    /\
    ED = sum(i in 1..u1)(x_c[i,0])
         + sum(j in 1..u2)(y_c[0,j])
         + sum(i in 1..u1, j in 1..u2)(x_c[i,j] + y_c[i,j] + h_c[i,j]);