% RUNS ON mzn_mer_fd
% RUNS ON mzn_mer_lazyfd
% RUNS ON zinc_fdic_mznlib
% RUNS ON zinc_mip_hl
% RUNS ON mzn_mer_lp
% RUNS ON minizinc_fd
% RUNS ON minizinc_cpx
% Example battleships problem for battleships.mzn.model
%
include "battleships.mzn.model";

n = 9;

a = array2d(ROW, COL,
    [| 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
     | 0, 0, _, _, _, _, 0, _, _, _, 0
     | 0, _, 0, _, _, _, _, _, _, _, 0
     | 0, _, _, _, _, _, _, _, _, _, 0
     | 0, 0, _, _, _, _, _, _, 0, _, 0
     | 0, _, _, _, _, _, _, _, _, _, 0
     | 0, _, 0, _, 0, _, _, _, _, _, 0
     | 0, _, _, 0, _, _, _, _, _, _, 0
     | 0, _, _, _, _, _, _, _, _, _, 0
     | 0, _, _, _, _, _, _, _, _, _, 0
     | 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
     |]
);

constraint a[2, 1] > 0;
constraint a[6, 3] > 0;
constraint a[7, 8] > 0;

row_sums = [2, 2, 3, 2, 2, 1, 5, 1, 2];

col_sums = [4, 0, 3, 2, 2, 2, 1, 4, 2];


n_classes = 4;

class_sizes = [4, 3, 2, 1];

