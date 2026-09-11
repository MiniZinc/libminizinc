% The constraint `bool_lex_chain_lesseq`, as the FZnSO registry defines it.
%
% This file is a solver's override point: declare `bool_lex_chain_lesseq` and
% the body below is dropped in favour of the propagator, or ship a file of this
% name with a decomposition of your own.

% A FlatZinc array is one-dimensional, so the matrix is passed flat, one row
% after another, with the number of columns alongside — `std` reads that off an
% `index_set` the solver never receives. The order is on the *columns*.

predicate fzn_bool_lex_chain_lesseq(list of var bool: xs, int: columns) =
  if columns < 2 then
    true
  else
    let {
      array [1..length(xs) div columns, 1..columns] of var bool: m =
        array2d(1..length(xs) div columns, 1..columns, xs);
    } in forall (j in 1..columns - 1) (lex_lesseq(col(m, j), col(m, j + 1)))
  endif;
