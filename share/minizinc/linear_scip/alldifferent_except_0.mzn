/**  @group globals.alldifferent
  Constrain the array of integers \a vs to be all different except those
  elements that are assigned the value 0.
*/
predicate alldifferent_except_0(array[int] of var int: vs) = 
%    forall(i, j in index_set(vs) where i < j) (
%       (vs[i] != 0 /\ vs[j] != 0) -> vs[i] != vs[j]
%    );
    if length(vs)<=1 then true
    else
    let {
        array[int,int] of var 0..1: x_eq_d = eq_encode(vs)
    } in (
%        my_trace("  alldifferent_except_0: x[" ++ show(index_set(vs)) ++ "]\n") /\
        forall(d in index_set_2of2(x_eq_d) diff {0})( sum(i in index_set_1of2(x_eq_d))( x_eq_d[i,d] ) <= 1 )
    )
    endif;
