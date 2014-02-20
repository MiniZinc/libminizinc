% RUNS ON mzn_mer_fd
% RUNS ON mzn_mer_std_fd
% RUNS ON mzn_mer_lazyfd
% RUNS ON minizinc_mip
% RUNS ON zinc_fdic_mznlib
% RUNS ON zinc_mip_hl
% RUNS ON minizinc_fd
% RUNS ON minizinc_cpx
%-----------------------------------------------------------------------------
% Placing people on a photo
%
% Guido Tack
% 2007-02-22
%
% Ported from the Gecode example
%
%-----------------------------------------------------------------------------

% A group of people wants to take a group photo. Each person can give
% preferences next to whom he or she wants to be placed on the
% photo. The problem to be solved is to find a placement that
% satisfies as many preferences as possible.

include "globals.mzn";

%-----------------------------------------------------------------------------
% Specification

int: n_names;
int: n_prefs;
array[1..n_prefs,0..1] of int: prefs;

% Instance

n_names = 9;
n_prefs = 17;
prefs = array2d(1..n_prefs, 0..1, 
  [| 0,2 | 0,4 | 0,7 | 1,4 | 1,8 | 2,3 | 2,4 | 3,0 | 3,4 |
     4,5 | 4,0 | 5,0 | 5,8 | 6,2 | 6,7 | 7,8 | 7,6 |]
);

%-----------------------------------------------------------------------------
% Model

array[0..n_names-1] of var 0..n_names-1: pos;
var 0..n_names-1: satisfies;

array[1..n_prefs] of var bool: ful;

constraint
  forall (i in 1..n_prefs) (
    let {
      int: pa = prefs[i,0],
      int: pb = prefs[i,1]
    } in
    ful[i] = (pos[pb]-pos[pa] == 1 xor pos[pa]-pos[pb] == 1)
  );

constraint
  sum (i in 1..n_prefs) (bool2int(ful[i])) = satisfies;

constraint
  alldifferent(pos);

% Break some symmetry
constraint
  pos[0] < pos[1];

% Justification for annotation, from Guido:
%   I've had a closer look at the FlatZinc file for photo again. The real
%   problem is branching! Currently, if nothing is annotated, I branch on
%   all the variables, just naively in the order they're given in the source
%   file.  For photo, this is apparently the worst you can do. You have to
%   branch on the pos array (that's what the Gecode version does), but this
%   array appears only after loads of Boolean variables.  I've added a
%   search annotation to photo.mzn, which should solve this issue.
solve :: int_search(pos, first_fail, indomain, complete) 
    maximize satisfies;

output ["Positions: ", show(pos), "\n",
	"Preferences satisfied: ", show(satisfies), "\n"];
