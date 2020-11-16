/***
!Test
expected: !Result
  solution: !Solution
  arr2d: [[A,B],[C,D]]
  arr3d: [[[E,F],[G,H]],[[I,J],[K,L]]]
***/
include "strictly_increasing.mzn";

enum FOUR = {A,B,C,D};
array[1..2,1..2] of var FOUR: arr2d;
constraint strictly_increasing(arr2d);

enum EIGHT = {E,F,G,H,I,J,K,L};
array[1..2,1..2,1..2] of var EIGHT: arr3d;
constraint strictly_increasing(arr3d);
