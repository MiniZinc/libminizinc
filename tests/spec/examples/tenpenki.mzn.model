% tenpenki.mzn
% vim: ft=zinc ts=4 sw=4 et
% Ralph Becket
% Thu Aug 14 15:23:11 EST 2008
%
% A tenpenki puzzle involves filling out the pixels in a grid to make
% a picture.  Each row/column constraint specifies the distinct
% runs of "black" pixels in that row/column.  For example,
%
%       1       1
%       1 2   1 1
%       1 1 5 2 1
%     +-----------+
% 1 3 | # . # # # |
% 2   | . # # . . |
% 5   | # # # # # |
% 2   | . . # # . |
% 3 1 | # # # . # |
%     +-----------+

% The dimensions of the board.
%
int: nrows;
int: ncols;

set of int: row = 1..nrows;
set of int: col = 1..ncols;
set of int: row_plus_one = 1..(nrows + 1);
set of int: col_plus_one = 1..(ncols + 1);

% The grid.
%
array [row, col] of var bool: a;

% A generic row constraint.
%
predicate row_constraint(row: r, array [int] of col: x) =
    if index_set(x) = {} then
        forall (c in col) (a[r, c] = false)
    else
        let {
            int: n = max(index_set(x)),
            array [1..n] of var col: s % The starting col of each block.
        } in (
            % There must be at least one white cell between each black block.
            %
            forall (i in 1..(n - 1)) (s[i + 1] > s[i] + x[i])
        /\
            % The final black block mustn't overrun the row.
            %
            s[n] + x[n] <= ncols + 1
        /\
            % Ensure that the pixels in this row are coloured in appropriately.
            %
            forall (c in col) (
                a[r, c]
            <->
                exists (i in 1..n) (s[i] <= c /\ c < s[i] + x[i])
            )
        )
    endif;

% A generic col constraint.
%
predicate col_constraint(col: c, array [int] of row: x) =
    if index_set(x) = {} then
        forall (r in row) (a[r, c] = false)
    else
        let {
            int: n = max(index_set(x)),
            array [1..n] of var row: s % The starting row of each block.
        } in (
            % There must be at least one white cell between each black block.
            %
            forall (i in 1..(n - 1)) (s[i + 1] > s[i] + x[i])
        /\
            % The final black block mustn't overrun the col.
            %
            s[n] + x[n] <= ncols + 1
        /\
            % Ensure that the pixels in this col are coloured in appropriately.
            %
            forall (r in row) (
                a[r, c]
            <->
                exists (i in 1..n) (s[i] <= r /\ r < s[i] + x[i])
            )
        )
    endif;

solve satisfy;

output  [   if fix(a[r, c]) then "# " else ". " endif ++
            if c = ncols then "\n" else "" endif
        |   r in row, c in col
        ];
