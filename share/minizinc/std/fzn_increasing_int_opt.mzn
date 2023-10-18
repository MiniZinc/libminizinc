%-----------------------------------------------------------------------------%
% Requires that the array 'x' is in increasing order
%-----------------------------------------------------------------------------%

predicate fzn_increasing_int_opt(array[int] of var opt int: x) =
  let {
    array[int] of var opt int: xx = array1d(x);
    array[1..length(xx)] of var int: y;
    constraint forall(i in 1..length(xx)) (
      y[i] = if occurs(xx[i]) then
          deopt(xx[i])
        elseif i = 1 then
          lb_array(xx)
        else
          y[i-1]
        endif
    );
  } in forall (i in 2..length(y) where occurs(xx[i])) (
    deopt(xx[i]) >= y[i-1]
  );
