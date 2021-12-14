/***
!Test
expected: !Error
  type: TypeError
  regex: .*non-set-array-ti-location\.mzn.*
***/

% GitHub issue #408 - used to not give useful location info

array[2] of bool: x = [false, true];
