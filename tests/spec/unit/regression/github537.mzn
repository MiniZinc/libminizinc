/***
!Test
solvers: [gecode]
options:
  all_solutions: true
expected: !Result
  status: ALL_SOLUTIONS
  solution: !SolutionSet
  - !Solution
    b: true
***/

% This model fragment caused an assertion failure. 
% Implementation would consider all branches for partiallity even though considerations
% of the conditions would have already excluded the final branch.

var bool: b;
constraint 1 = if b then
	1
elseif true then 
	let {
		var 1..2: i = sum (j in 1..2) (j) % invalid domain
	} in 1 
else 
	1
endif;
