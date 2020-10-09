/***
!Test
solvers: [] # Remove once set ordering has been fixed
expected:
- !Result
  solution: !Solution
    sets:
    - !Range 5..7
    - !!set {3, 4, 7}
    - !!set {2, 4, 6}
    - !!set {2, 3, 5}
    - !!set {1, 4, 5}
    - !!set {1, 3, 6}
    - !!set {1, 2, 7}
- !Result
  solution: !Solution
    sets:
    - !Range 4..6
    - !!set {3, 6, 7}
    - !!set {2, 4, 7}
    - !!set {2, 3, 5}
    - !!set {1, 5, 7}
    - !!set {1, 3, 4}
    - !!set {1, 2, 6}
- !Result
  solution: !Solution
    sets:
    - !!set {4, 6, 7}
    - !!set {3, 5, 7}
    - !!set {2, 5, 6}
    - !Range '2..4'
    - !!set {1, 4, 5}
    - !!set {1, 3, 6}
    - !!set {1, 2, 7}
- !Result
  solution: !Solution
    sets:
    - !!set {4, 6, 7}
    - !Range 3..5
    - !!set {2, 5, 6}
    - !!set {2, 3, 7}
    - !!set {1, 5, 7}
    - !!set {1, 3, 6}
    - !!set {1, 2, 4}
***/

%-----------------------------------------------------------------------------%
% Steiner Triples (CSPlib problem 44)
%
% March 2008; Mark Wallace, based on the Eclipse version by Joachim Schimpf
%
% The following program computes so-called Steiner triplets.  These are
% triplets of numbers from 1 to n such that any two triplets have at most one
% element in common.
%
% One possible solution for n=7 is
%   { {1, 2, 3}, {1, 4, 5}, {1, 6, 7}, {2, 4, 6},
%     {2, 5, 7}, {3, 4, 7}, {3, 5, 6} }.
%-----------------------------------------------------------------------------%

n = 7;

%-----------------------------------------------------------------------------%

int: n;

int: nb = n * (n-1) div 6 ;

array[1..nb] of var set of 1..n: sets;

constraint forall(i in 1..nb) ( card(sets[i]) = 3 );

constraint
    forall(i in 1..nb, j in i+1..nb) ( card(sets[i] intersect sets[j]) <= 1 );

% Symmetry breaking:
constraint forall(i in 1..nb-1) ( sets[i] >= sets[i+1] );

solve :: set_search(sets, input_order, indomain_min, complete) satisfy;

output [ " " ++ show(sets[i]) | i in 1..nb ] ++ ["\n"];

%-----------------------------------------------------------------------------%
%-----------------------------------------------------------------------------%
