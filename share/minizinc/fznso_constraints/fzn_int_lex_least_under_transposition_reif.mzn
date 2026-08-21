% The constraint `int_lex_least_under_transposition_reif`, as the FZnSO registry
% defines it.
%
% This file is a solver's override point: declare
% `int_lex_least_under_transposition_reif` and the body below is dropped in
% favour of the propagator, or ship a file of this name with a decomposition of
% your own.

include "fzn_int_lex_least_under_permutation.mzn";

predicate fzn_int_lex_least_under_transposition_reif(list of var int: xs, int: columns, var bool: b) =
  b <-> (
    let {
      int: n = columns;
      int: l = n * n;
      constraint assert(length(xs) = l, "int_lex_least_under_transposition: `xs` is not a square matrix");
      array [1..8, 1..l] of 1..l: p =
        array2d(
          1..8,
          1..l,
          [
            if k == 1 then
              i * n + j - n
            elseif k == 2 then
              (n - j) * n + i % r270
            elseif k == 3 then
              (n - i) * n + (n - j) + 1 % r180
            elseif k == 4 then
              (j * n - n) + (n - i + 1) % r90
            elseif k == 5 then
              (n - i) * n + j % x flip
            elseif k == 6 then
              (i * n - n) + (n - j + 1) % y flip
            elseif k == 7 then
              (n - j) * n + (n - i + 1) % d2 flip
            else
              (j * n - n) + i % d1 flip
            endif |
              k in 1..8,
              i, j in 1..n,
          ],
        );
    } in fzn_int_lex_least_under_permutation(xs, array1d(p))
  );
