/***
!Test
expected:
- !Result
  status: OPTIMAL_SOLUTION
  solution: !Solution
    A: [2, 2, 0, 0, 3, 3, 2, 2, 1, 1, 0, 0, 2, 2, 0]
    solved: 15
- !Result
  status: OPTIMAL_SOLUTION
  solution: !Solution
    A: [2, 2, 0, 0, 1, 1, 2, 2, 3, 3, 0, 0, 2, 2, 0]
    solved: 15
***/

% wolfgoatetc.mzn
% ft=zinc ts=4 sw=4 et
%



%
% XXX why is lazyfd so slow with this?
%
% The wolf, goat, and cabbage problem.
%
% A farmer has to transport his wolf, goat, and cabbage from his farm, across
% the river in his rowboat, to the market on the far bank.  He can carry only
% one item at a time in his boat.  He cannot leave the wolf alone with the
% goat or the goat alone with the cabbage.

set of int: loc = 1..3;
loc: farm = 1;
loc: boat = 2;
loc: market = 3;

set of int: obj = 0..3;
obj: F = 0;
obj: W = 1;
obj: G = 2;
obj: C = 3;

int: max_steps = 15;

set of int: step = 1..max_steps;

array [step, obj] of var loc: L;

% Initial conditions.
% 
constraint L[1, W] = farm;
constraint L[1, G] = farm;
constraint L[1, C] = farm;
constraint L[1, F] = farm;

% The goal: move the wolf, goat, and cabbage to the market.
%
var step: solved :: add_to_output;

constraint
    L[solved, W] = market
/\  L[solved, G] = market
/\  L[solved, C] = market;

% We can't leave the wolf alone with the goat
% or the goat alone with the cabbage.
%
constraint
    forall (s in step) (
        L[s, F]  = L[s, G]
    \/  (   L[s, G] != L[s, C]
        /\  L[s, G] != L[s, W]
        )
    );

% The actions.  At every step the farmer moves from one bank to the other;
% he may take something with him.
%
constraint
    forall (s in step diff {max_steps - 1, max_steps} where s mod 2 = 1) (
        exists (x in {W, G, C, F}, a, b in {farm, market} where a != b) (
            move(s, x, a, b)
        )
    );

% Each move goes via the boat.  Any object not moving stays where it is.
%
predicate move(step: s, obj: x, loc: from, loc: to) =
    L[s,     F] = from
/\  L[s,     x] = from
/\  L[s + 1, F] = boat
/\  L[s + 1, x] = boat
/\  L[s + 2, F] = to
/\  L[s + 2, x] = to
/\  forall (y in {W, G, C} diff {x}) (
        L[s + 1, y] = L[s, y]
    /\  L[s + 2, y] = L[s, y]
    );

% The plan is the move (other than the farmer) at each step.
%
array [step] of var obj: A :: add_to_output =
    [ bool2int(s < solved) *
      ( W * bool2int(L[s, W] != L[s + 1, W])
      + G * bool2int(L[s, G] != L[s + 1, G])
      + C * bool2int(L[s, C] != L[s + 1, C])
      )
    | s in step diff {max_steps}
    ] ++ [0];

solve
    :: int_search(
    	[L[s, x] | s in step, x in obj],
        input_order, indomain_min, complete
    )
    minimize solved;

