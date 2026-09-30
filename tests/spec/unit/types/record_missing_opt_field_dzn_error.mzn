/***
!Test
extra_files:
- record_missing_opt_field_dzn_error.dzn
solvers: [gecode]
expected: !Error
  type: TypeError
***/

% Only optional fields can be omitted in data files: y is not optional
array[1..2] of record(opt int: x, int: y): a;
