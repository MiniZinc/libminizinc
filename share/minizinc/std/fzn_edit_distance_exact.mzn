% Dynamic programming routine decomposition for insertion/deletion/substition cost defined by character
% DP routine first proposed in the paper "The String-to-String Correction Problem", Wagner and Fischer, Journal of the ACM, 1978.
predicate fzn_edit_distance_exact(int: max_char,
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
	     T[i,j] >= 0 /\ T[i,j] <= sum(i2 in l1+1..i)(W_del_0[S1[i2]]) + sum(j2 in l2+1..j)(W_ins_0[S2[j2]])
	   )
	   /\
	   forall(j in l2+1..u2)(
	     T[l1,j] = sum(j2 in l2+1..j)(W_ins_0[S2[j2]])
	   )
	   /\
	   forall(i in l1+1..u1)(
	     T[i,l2] = sum(i2 in l1+1..i)(W_del_0[S1[i2]])
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