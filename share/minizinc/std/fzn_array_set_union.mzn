/* Constrain z to be the union of the elements in x */
predicate fzn_array_set_union(array[int] of var set of int: x, var set of int: z) =
    if length(x)=0 then z={}
    elseif length(x)=1 then z=x[min(index_set(x))]
    else
      let {
        int: l=min(index_set(x));
        int: u=max(index_set(x));
        array[l..u-1] of var set of ub_array(x): y;
      } in
      y[l]=x[l] union x[l+1] /\
      forall (i in l+2..u) (y[i-1]=y[i-2] union x[i]) /\
      z=y[u-1]
    endif;
