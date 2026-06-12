/***
!Test
solvers: [gecode]
expected: !Result
  status: SATISFIED
  solution: !Solution
    _output_item: "upper=103 lower=203"
***/

% Regression test for name-only overloads used inside an output item. Building
% the par "output model" re-resolves the calls in the output expression by
% identifier and (par-coerced) argument types. Resolving by type alone cannot
% tell two name-only siblings apart, so both `pick` calls collapsed onto the
% same sibling and the output printed `upper=103 lower=103`. Re-resolution must
% honour the call's parameter names so each sibling keeps its own body.

function var int: pick(var int: upper) = upper + 100;
function var int: pick(var int: lower) = lower + 200;

var 0..5: x;
constraint x = 3;

output ["upper=", show(pick(upper: x)), " lower=", show(pick(lower: x))];
