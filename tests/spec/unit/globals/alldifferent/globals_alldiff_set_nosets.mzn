/***
!Test
expected:
- !Result
  solution: !Solution
    alldiff_avsi2: !Unordered
    - !!set {}
    - !!set {1}
    - !Range 1..2
    - !Range 1..3
    - !!set {1, 3}
    - !!set {2}
    - !Range 2..3
    - !!set {3}
***/

% Test all_different for var set of int.

include "all_different.mzn";
include "nosets.mzn";

% set of int version
% alldiff_avsi2   = [{1, 2, 3}, {1, 2}, {1, 3}, {1}, {2, 3}, {2}, {3}, {}]

set of int: aa0 = {};

array[1..8] of var set of 1..3: alldiff_avsi2 :: add_to_output = [ aa0, _,_,_,_,_,_,_, ];

constraint alldiff_avsi2[1] = {};

constraint all_different(alldiff_avsi2);

constraint forall(i in index_set( alldiff_avsi2 ) diff { max( index_set( alldiff_avsi2 ) ) } )
  ( alldiff_avsi2[i] <= alldiff_avsi2[i+1] );

array[int] of var bool: adsab = setarray2bools( alldiff_avsi2 );

solve satisfy;

output ["alldiff_avsi2 = array1d(1..8, ", show(alldiff_avsi2), ");\n",
%  "setaray2bools(alldiff_avsi2) = ", show(adsab), ";\n" 
  ]
  ++ [ show(set2bools( alldiff_avsi2[i], ub_array(alldiff_avsi2) )) | i in index_set( alldiff_avsi2 ) ]
  ++ [ "\n" ]
;
