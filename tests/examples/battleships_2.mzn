% RUNS ON mzn_mer_fd
% RUNS ON mzn_mer_lazyfd
% RUNS ON zinc_fdic_mznlib
% RUNS ON zinc_mip_hl
% RUNS ON minizinc_fd
% RUNS ON minizinc_cpx
% Example battleships problem for battleships.mzn.
%
include "battleships.mzn.model";

n = 6;

a = array2d(ROW, COL,
    [| 0, 0, 0, 0, 0, 0, 0, 0
     | 0, _, _, _, _, _, _, 0
     | 0, _, _, _, _, _, _, 0
     | 0, _, _, _, _, _, _, 0
     | 0, _, _, _, _, _, _, 0
     | 0, _, _, _, _, _, _, 0
     | 0, _, _, _, _, _, _, 0
     | 0, 0, 0, 0, 0, 0, 0, 0
     |]
);

constraint a[1, 3] > 0;
constraint a[2, 6] > 0;
constraint a[5, 6] > 0;

row_sums = [2, 2, 1, 1, _, 1];

col_sums = [2, 0, _, _, _, _];

n_classes = 3;

class_sizes = [3, 2, 1];

