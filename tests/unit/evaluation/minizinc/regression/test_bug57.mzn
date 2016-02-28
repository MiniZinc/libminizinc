% RUNS ON mzn20_fd
% RUNS ON mzn-fzn_fd
set of int: X = {1,2};
function array[int] of int: foo(set of X: x) =
  if card(x)==0 then []
  else let {
    int: y = min(x);
    array[int] of X: res = [1]++foo(x diff {y})
  } in res  endif;
array[int] of X: x = foo(X);
solve satisfy;
