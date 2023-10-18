%-----------------------------------------------------------------------------%
% Requires that the array 'x' is in increasing order iff b is true
%-----------------------------------------------------------------------------%

predicate fzn_increasing_float_opt_reif(array[int] of var opt float: x, var bool: b) =
  let {
    array[int] of var opt float: xx = array1d(x);
    array[1..length(xx)] of var float: y;
    constraint forall(i in 1..length(xx)) (
      y[i] = if occurs(xx[i]) then
          deopt(xx[i])
        elseif i = 1 then
          lb_array(xx)
        else
          y[i-1]
        endif
    );
  } in b <-> forall (i in 2..length(y) where occurs(xx[i])) (
    deopt(xx[i]) >= y[i-1]
  );
