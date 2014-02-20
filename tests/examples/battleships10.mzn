% RUNS ON mzn_mer_fd
% RUNS ON mzn_mer_lazyfd
% RUNS ON zinc_fdic_mznlib
% RUNS ON zinc_mip_hl
% RUNS ON mzn_mer_lp
% RUNS ON minizinc_fd
% RUNS ON minizinc_cpx
% Example battleships problem for bs.mzn.
%
include "battleships.mzn.model";

n = 10;

a = array2d(ROW, COL,
    [| 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
     | 0, _, _, _, _, _, _, _, _, _, _, 0
     | 0, _, _, _, _, _, _, _, 0, _, 0, 0
     | 0, 0, _, _, _, _, _, _, _, 0, _, 0
     | 0, _, _, _, _, _, _, _, _, _, _, 0
     | 0, _, _, _, _, _, _, _, _, _, _, 0
     | 0, _, _, _, _, _, _, _, _, _, _, 0
     | 0, _, _, _, _, _, _, _, _, _, _, 0
     | 0, _, _, _, 0, _, _, _, _, _, _, 0
     | 0, _, _, _, _, _, _, _, _, _, _, 0
     | 0, _, _, _, _, _, _, _, _, _, _, 0
     | 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
     |]
);

constraint a[2, 9] > 1;

row_sums = [2, 1, 5, 0, 0, 0, 1, 6, 1, 4];

col_sums = [1, 4, 1, 2, 3, 1, 3, 0, 4, 1];

n_classes = 4;

class_sizes = [4, 3, 2, 1];

