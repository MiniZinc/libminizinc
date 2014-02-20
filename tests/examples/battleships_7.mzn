% RUNS ON mzn_mer_fd
% RUNS ON mzn_mer_lazyfd
% RUNS ON zinc_fdic_mznlib
% RUNS ON zinc_mip_hl
% RUNS ON minizinc_fd
% RUNS ON minizinc_cpx
% Example battleships problem for battleships.mzn.
%
include "battleships.mzn.model";

n = 10;

a = array2d(ROW, COL,
    [| 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
     | 0, _, 0, 0, 0, 0, 0, _, _, _, _, 0
     | 0, _, 0, _, 0, _, _, _, _, _, _, 0
     | 0, 0, _, _, _, _, _, _, _, _, _, 0
     | 0, _, _, _, _, _, _, _, _, _, _, 0
     | 0, _, _, _, _, _, _, _, _, _, _, 0
     | 0, _, _, _, _, 0, 0, _, _, _, _, 0
     | 0, _, _, _, _, 0, _, _, _, _, _, 0
     | 0, _, _, _, _, 0, 0, _, _, _, _, 0
     | 0, _, _, _, _, _, _, _, _, _, 0, 0
     | 0, _, _, _, _, _, _, _, _, _, _, 0
     | 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
     |]
);

constraint a[1, 1] > 0;
constraint a[2, 3] > 0;
constraint a[7, 6] > 0;

row_sums = [3, 2, 2, 1, 4, 0, 3, 1, 3, 1];

col_sums = [3, 1, 4, 3, 2, 2, 1, 1, 2, 1];

n_classes = 4;

class_sizes = [4, 3, 2, 1];

