/***
!Test
solvers: [gecode, chuffed]
expected:
- !Result
  status: SATISFIED
  solution: !Solution
    s: !Range 1..9
    t: !!set {45}
    s_total: 45
    t_total: 45
- !Result
  status: SATISFIED
  solution: !Solution
    s: !!set {100}
    t: !!set {49, 51}
    s_total: 100
    t_total: 100
- !Result
  status: SATISFIED
  solution: !Solution
    s: !!set {1, 2, 40, 56, 94}
    t: !!set {3, 93, 97}
    s_total: 193
    t_total: 193
***/

% Regression test for a bug in mzn2fzn 1.2.  The optimisation pass was leaving
% dangling references to variables it had "eliminated".  The symptom was the
% following error from the FlatZinc interpreter:
%
%    subsets_100.fzn:413:
%    symbol error: `INT____407' undeclared
%
% (This model is from the original bug report.)

% Subsets 100 puzzle in MiniZinc.
% 
% From rec.puzzle FAQ
% http://brainyplanet.com/index.php/Subsets?PHPSESSID=051ae1e2b6df794a5a08fc7b5ecf8028
% """
% Out of the set of integers 1,...,100 you are given ten different integers. 
% From this set, A, of ten integers you can always find two disjoint non-empty 
% subsets, S & T, such that the sum of elements in S equals the sum of elements 
% in T. Note: S union T need not be all ten elements of A. Prove this.
% """

% 
% This MiniZinc model was created by Hakan Kjellerstrand, hakank@bonetmail.com
% See also my MiniZinc page: http://www.hakank.org/minizinc
%

% Note that this model is not run using CBC
% This is because the solutions listed are not exhaustive, so we would usually check against another solver
% However we cannot do this because when giving values for s or t then sum_set does not work
% As the ub for the set will simply be the set itself, summing the wrong numbers

include "globals.mzn"; 
int: n = 100;
int: m = 10;
var set of 1..n: s;
var set of 1..n: t;
var int: s_total;
var int: t_total;

%
% sums the integer in set ss
%
predicate sum_set(var set of int: ss, var int: total) =
  let {
    int: m = card(ub(ss)), % NOTE: This prevents checking of solutions since when fixing ss then card(ub(ss))=card(ss) so numbers are not summed correctly when checking
    array[1..m] of var 0..1: tmp
  }
  in
  forall(i in 1..m) (
    i in ss <-> tmp[i] = 1
  )
  /\
  total = sum(i in 1..m) (i*tmp[i])
;


solve :: set_search([s,t], 
        input_order, indomain_min, complete) satisfy;

constraint
  card(s union t) <= m
  /\
  card(s union t) > 0
  /\
  disjoint(s, t)
  /\
  sum_set(s, s_total)
  /\
  sum_set(t, t_total)
  /\
  s_total = t_total
  % /\
  % t_total = n
;
