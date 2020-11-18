/** @group globals
  Constrains \a ED to be greater than or equal to the minimum edit distance between two strings \a S1 and \a S2.
  Any character insertion or deletion has a cost of 1, whereas any character substitution has a cost of 2.
  All characters need to be numbered from 1 to \a max_char.
  The integer arrays \a S1 and \a S2 need to have the same length, but may contain zeros at the end to indicate an "empty" character.
  Example: Let \a max_char = 2, \a S1 = [1,1,0,0,0], \a S2 = [1,2,1,0,0], then \a ED = 1, as the minimum edit distance is 1 between the strings "11" and "121" (a single '2' character needs to be inserted).
  
@param max_char: The maximum character that can occur in a string (all characters should be numbered from 1..max_char, 0 represents an empty character)
@param S1: Integer array that represents the first string, the index set has to be 1..length(\a S1 )
@param S2: Integer array that represents the second string, the index set has to be 1..length(\a S2 )
@param ED: Variable that is constrained to be >= than the minimum edit distance between \a S1 to \a S2
*/
predicate edit_distance(int: max_char,
                        array[int] of var int: S1,
                        array[int] of var int: S2,
                        var int: ED) =
    let {
          int: len1 = length(S1);
          int: len2 = length(S2);
          var 0..len1: p1;
          var 0..len2: p2;
          var 0..len1: kept_sum;
        } in
       ED = p1 + p2 - 2*kept_sum                
       /\                
       edit_distance_kept(max_char, S1, S2, p1, p2, kept_sum);

/** @group globals
  Constrains \a ED to be greater than or equal to the minimum edit distance between two strings \a S1 and \a S2.
  This constraint functions in the same way as the constraint \a edit_distance(max_char, S1, S2, ED), however it additionally specifies custom character insertion, deletion and substitution costs.
  All costs should be >= 1 and the following inequality must hold: C_sub <= C_ins + C_del
  
@param max_char: The maximum character that can occur in a string (all characters should be numbered from 1..max_char, 0 represents an empty character)
@param C_ins: Costs for a single character insertion
@param C_del: Costs for a single character deletion
@param C_sub: Costs for a single character substitution
@param S1: Integer array that represents the first string, the index set has to be 1..length(\a S1 )
@param S2: Integer array that represents the second string, the index set has to be 1..length(\a S2 )
@param ED: Variable that is constrained to be >= than the minimum edit distance between \a S1 to \a S2
*/
predicate edit_distance(int: max_char,
                        int: C_ins,
                        int: C_del,
                        int: C_sub,
                        array[int] of var int: S1,
                        array[int] of var int: S2,
                        var int: ED) =
    let { int: len1 = length(S1);
          int: len2 = length(S2);
          var 0..len1: p1;
          var 0..len2: p2;
          var 0..len1: kept_sum;
          var 0..len2: insertions;
          var 0..len1: deletions;
          var 0..max(len1,len2): substitutions;
        } in
    substitutions = min(p1,p2) - kept_sum
    /\
    insertions = max(p2-p1, 0)
    /\
    deletions = max(p1-p2, 0)
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
    let { int: l1 = min(index_set(S1));
          int: l2 = min(index_set(S2));
          int: u1 = max(index_set(S1));
          int: u2 = max(index_set(S2));
          int: len1 = length(S1);
          int: len2 = length(S2);
          var 0..len2: insertions;
          var 0..len1: deletions;
          var 0..max(len1,len2): substitutions;
          array[l1..u1] of var 0..len2: kept_r1_from_position;
          array[l2..u2] of var 0..len1: kept_r2_from_position;
        } in
    p1 = sum(i in l1..u1 where S1[i] != 0)(1)
    /\
    p2 = sum(i in l2..u2 where S2[i] != 0)(1)
    /\
    kept_sum = sum(i in l1..u1 where kept_r1_from_position[i] != 0)(1)
    /\
    forall(i in l1..u1-1)(
      S1[i] == 0 -> S1[i+1] == 0
    )
    /\
    forall(i in l2..u2-1)(
      S2[i] == 0 -> S2[i+1] == 0
    )
    /\
    forall(i in l1+1..u1 where kept_r1_from_position[i] != 0) (
      kept_r1_from_position[i] > kept_r1_from_position[i-1]
    )
    /\
    forall(i in l2+1..u2 where kept_r2_from_position[i] != 0) (
      kept_r2_from_position[i] > kept_r2_from_position[i-1]
    )
    /\
    forall(i in l1..u1) (
      kept_r1_from_position[i] > 0 <-> kept_r2_from_position[i] > 0
    )
    /\
    forall(j in l2..u2-1) (
      (kept_r1_from_position[j] = 0) -> (kept_r1_from_position[j+1] = 0)
    )
    /\
    forall(j in l1..u1-1) (
      (kept_r2_from_position[j] = 0) -> (kept_r2_from_position[j+1] = 0)
    )
    /\
    forall(i in l2..u2 where kept_r2_from_position[i] != 0) (
      kept_r2_from_position[i] <= p1
    )
    /\
    forall(i in l1..u1 where kept_r1_from_position[i] != 0) (
      kept_r1_from_position[i] <= p2
    )
    /\
    forall(i in l1..u1 where kept_r1_from_position[i] != 0) (
      S1[kept_r2_from_position[i]] = S2[kept_r1_from_position[i]]
    );


% Dynamic programming routine decomposition for insertion/deletion/substition cost defined by character
% DP routine first proposed in the paper "The String-to-String Correction Problem", Wagner and Fischer, Journal of the ACM, 1978.
/** @group globals
  Constrains \a ED to be greater than or equal to the minimum edit distance between two strings \a S1 and \a S2.
  This constraint functions in the same way as the constraint \a edit_distance(max_char, S1, S2, ED), however it additionally specifies custom character insertion, deletion and substitution costs for each individual combination of characters.
  All costs should be >= 1, except the substitution costs of a character with itself which should be 0 (W_sub[c,c] = 0).
  Furthermore, the following inequality must hold for all pairs of characters c1 and c2: \a W_sub[c1,c2] <= \a W_del[c1] + \a W_ins[c2]
  
@param max_char: The maximum character that can occur in a string (all characters should be numbered from 1..max_char, 0 represents an empty character)
@param W_ins: Integer array containing the costs for single character insertions
@param W_del: Integer array containing the costs for single character deletions
@param W_sub: Integer array containing the costs for single character substitutions
@param S1: Integer array that represents the first string, the index set has to be 1..length(\a S1 )
@param S2: Integer array that represents the second string, the index set has to be 1..length(\a S2 )
@param ED: Variable that is constrained to be >= than the minimum edit distance between \a S1 to \a S2
*/
predicate edit_distance(int: max_char,
                        array[int] of int: W_ins,
                        array[int] of int: W_del,
                        array[int, int] of int: W_sub,
                        array[int] of var int: S1,
                        array[int] of var int: S2,
                        var int: ED) =
    let { int: l1 = min(index_set(S1)) - 1;
          int: l2 = min(index_set(S2)) - 1;
          int: u1 = max(index_set(S1));
          int: u2 = max(index_set(S2));
          array[int] of int: W_ins_0 = array1d(0..length(W_ins), [0] ++ W_ins);
          array[int] of int: W_del_0 = array1d(0..length(W_del), [0] ++ W_del);
          array[int, int] of int: W_sub_0 = array2d(0..max_char, 0..max_char,
                                                    [ if i == 0 then W_ins_0[j] else if j == 0 then W_del_0[i] else W_sub[i,j] endif endif | i in 0..max_char, j in 0..max_char]);
          array[l1..u1,l2..u2] of var int: T;
		      } in
    forall(i in l1+1..u1-1)(
      S1[i] == 0 -> S1[i+1] == 0
    )
    /\
    forall(i in l2+1..u2-1)(
      S2[i] == 0 -> S2[i+1] == 0
    )
    /\
	   T[l1,l2] = 0 /\
	   T[u1,u2] = ED /\
	   forall(i in l1+1..u1, j in l2+1..u2)(
	     T[i,j] >= 0 /\ T[i,j] <= sum(i2 in 1..i)(W_del_0[S1[i2]]) + sum(j2 in 1..j)(W_ins_0[S2[j2]])
	   )
	   /\
	   forall(j in l2+1..u2)(
	     T[l1,j] = sum(j2 in 1..j)(W_ins_0[S2[j2]])
	   )
	   /\
	   forall(i in l1+1..u1)(
	     T[i,l2] = sum(i2 in 1..i)(W_del_0[S1[i2]])
	   )
	   /\
	  forall(i in l1+1..u1, j in l2+1..u2)(
	    T[i,j] =
	      if S1[i] = S2[j] then
		       T[i-1,j-1]
		     else
		       if S1[i] = 0 then
		         T[i-1,j]
		       else
		       	 if S2[j] = 0 then
		       	   T[i,j-1]
		       	 else
		       	   min([T[i-1,j]+W_del_0[S1[i]],
		       	        T[i,j-1]+W_ins_0[S2[j]], 
		       	        T[i-1,j-1]+W_sub_0[S1[i], S2[j]]
		       	       ])
		       	 endif
  		      endif
			    endif
		);