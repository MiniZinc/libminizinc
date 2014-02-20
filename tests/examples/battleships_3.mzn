% RUNS ON mzn_mer_fd
% RUNS ON mzn_mer_lazyfd
% RUNS ON mzn_mer_lp
% RUNS ON zinc_fdic_mznlib
% RUNS ON minizinc_fd
% RUNS ON minizinc_cpx
% Example battleships problem for battleships.mzn.
%
include "battleships.mzn.model";

n = 10;

a = array2d(ROW, COL,
    [| 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
     | 0, _, _, _, _, _, _, _, _, _, _, 0
     | 0, _, _, _, _, _, _, _, _, _, _, 0
     | 0, _, _, _, _, _, _, _, _, _, _, 0
     | 0, _, _, _, _, _, _, _, _, _, _, 0
     | 0, _, _, _, _, _, _, _, _, _, _, 0
     | 0, _, _, _, _, _, _, _, _, _, _, 0
     | 0, _, _, _, _, _, _, _, _, _, _, 0
     | 0, _, _, _, _, _, _, _, _, _, _, 0
     | 0, _, _, _, _, _, _, _, _, _, _, 0
     | 0, _, _, _, _, _, _, _, _, _, _, 0
     | 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
     |]
);

constraint a[2, 7] > 0;
constraint a[6, 4] > 0;

row_sums = [4, 4, 0, 0, 4, 4, 0, 0, 4, 0];

col_sums = [3, 2, 4, 2, 2, 2, 2, 1, 1, 1];

n_classes = 4;

class_sizes = [4, 3, 2, 1];

