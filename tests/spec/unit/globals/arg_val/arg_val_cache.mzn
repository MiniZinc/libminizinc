/***
!Test
solvers: [gecode]
expected: !Result
  status: SATISFIED
***/
% Repeated lookups in one array build its value-to-index map only once, also through a
% user function and across arg_val, arg_val_weak and first_duplicate. The data dependencies
% make sure the counter is read before and after the lookups.
include "arg_val.mzn";

int: n = 2000;
array [1..n] of string: m = ["k\(i)" | i in 1..n];

function opt int: lookup(array [int] of string: ids, string: s) = arg_val_weak(ids, s);

int: b0 = mzn_arg_val_index_builds();
int: total = sum (i in 1..n) (if b0 >= 0 then deopt(lookup(m, "k\(i)")) else 0 endif);
int: direct = sum (i in 1..n) (if total > 0 then arg_val(m, "k\(i)") else 0 endif);
bool: distinct = if direct > 0 then absent(first_duplicate(m)) else false endif;
int: b1 = if distinct then mzn_arg_val_index_builds() else -1 endif;

constraint assert(total = n * (n + 1) div 2, "wrong lookup result");
constraint assert(direct = total, "wrong direct lookup result");
constraint assert(distinct, "wrong duplicate result");
constraint assert(b1 - b0 = 1, "expected one map build, got \(b1 - b0)");
