/***
!Test
expected:
- !Result
  solution: !Solution
    ful: [false, false, false, true, false, true, false, false, true, false, false, false, true, true, true, true, true]
    objective: 8
    pos: [0, 1, 4, 3, 2, 8, 5, 6, 7]
    satisfies: 8
- !Result
  solution: !Solution
    ful: [false, true, true, false, true, false, false, false, false, true, true, false, false, true, true, false, true]
    objective: 8
    pos: [3, 8, 6, 0, 2, 1, 5, 4, 7]
    satisfies: 8
- !Result
  solution: !Solution
    ful: [false, true, false, false, false, false, false, true, false, false, true, false, true, true, true, true, true]
    objective: 8
    pos: [1, 3, 4, 2, 0, 8, 5, 6, 7]
    satisfies: 8
- !Result
  solution: !Solution
    ful: [false, true, true, false, true, true, false, false, true, false, true, false, false, true, false, true, false]
    objective: 8
    pos: [5, 8, 2, 3, 4, 0, 1, 6, 7]
    satisfies: 8
***/

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
array[1..n_prefs,1..2] of int: prefs;

% Instance

n_names = 9;
n_prefs = 17;
prefs = array2d(1..n_prefs, 1..2, 
  [| 1,3 | 1,5 | 1,8 | 2,5 | 2,9 | 3,4 | 3,5 | 4,1 | 4,5 |
     5,6 | 5,1 | 6,1 | 6,9 | 7,3 | 7,8 | 8,9 | 8,7 |]
);

%-----------------------------------------------------------------------------
% Model

array[1..n_names] of var 0..n_names-1: pos;
var 0..n_names-1: satisfies;

array[1..n_prefs] of var bool: ful;

constraint
  forall (i in 1..n_prefs) (
    let {
      int: pa = prefs[i,1],
      int: pb = prefs[i,2]
    } in
    ful[i] = (pos[pb]-pos[pa] == 1 xor pos[pa]-pos[pb] == 1)
  );

constraint
  sum (i in 1..n_prefs) (bool2int(ful[i])) = satisfies;

constraint
  alldifferent(pos);

% Break some symmetry
constraint
  pos[1] < pos[2];

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
