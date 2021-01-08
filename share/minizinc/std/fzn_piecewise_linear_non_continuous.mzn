/*
 * Function fzn_piecewise_linear_base creates
 * auxiliary variables for the intervals {x_start[i]..x_end[i]},
 * and constrains them to correspond to the value of x.
 * The auxiliaries are reusable if we have several pwls
 * on the same set of intervals and x.
 */
function array[int, int] of var 0.0..1.0: fzn_piecewise_linear_base(var float: x,
               array[int] of float: x_start, array[int] of float: x_end)  ::promise_total =
    let {
        set of int: is = index_set(x_start),
        array[is] of var 0.0..1.0: s,          %% Segment variables
        array[is] of var 0..1: f,
        constraint 1 == sum(f),
        constraint forall(i in is)(
          if x_start[i] < x_end[i] then f[i] >= s[i] 
          elseif x_start[i] == x_end[i] then s[i] == 0.0 
          else abort("piecewise linear discontinuous: interval \(i): x_end < x_start") endif),
        constraint x == sum(i in is)(x_start[i] * f[i] + (x_end[i]-x_start[i]) * s[i]),
    } in
      array2d(1..2, is, f++s);
   

predicate fzn_piecewise_linear(var float: x, var float: y,
              array[int] of float: x_start, array[int] of float: x_end,
              array[int] of float: v_start, array[int] of float: v_end) = 
    let {
        set of int: is = index_set(x_start),
        constraint assert(is == index_set(v_start) 
            /\ is == index_set(v_end) /\ is == index_set(x_end) /\ 0<card(is) ,
                             "fzn_pwl: index sets unequal or empty"),
                                     
        array[int, int] of var float: fs = fzn_piecewise_linear_base(x, x_start,x_end),
    } in
      y == sum(i in is)(v_start[i] * fs[1, i] + (v_end[i]-v_start[i]) * fs[2, i]);

%-----------------------------------------------------------------------------%
