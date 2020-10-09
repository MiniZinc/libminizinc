/***
!Test
expected:
- !Result
  solution: !Solution
    Num: [1, 25, 2, 16, 3, 22, 4, 13, 19, 5, 17, 26, 6, 14, 23, 10, 20, 18, 7, 15, 11, 27, 8, 24, 21, 12, 9]
    Pos: [1, 3, 5, 7, 10, 13, 19, 23, 27, 16, 21, 26, 8, 14, 20, 4, 11, 18, 9, 17, 25, 6, 15, 24, 2, 12, 22]
- !Result
  solution: !Solution
    Num: [19, 13, 7, 22, 16, 25, 8, 14, 20, 10, 9, 17, 23, 15, 11, 26, 21, 4, 18, 12, 5, 24, 1, 6, 2, 27, 3]
    Pos: [23, 25, 27, 18, 21, 24, 3, 7, 11, 10, 15, 20, 2, 8, 14, 5, 12, 19, 1, 9, 17, 4, 13, 22, 6, 16, 26]
- !Result
  solution: !Solution
    Num: [1, 25, 2, 4, 3, 22, 5, 10, 16, 6, 19, 26, 11, 13, 23, 17, 7, 12, 20, 14, 8, 27, 18, 24, 9, 15, 21]
    Pos: [1, 3, 5, 4, 7, 10, 17, 21, 25, 8, 13, 18, 14, 20, 26, 9, 16, 23, 11, 19, 27, 6, 15, 24, 2, 12, 22]
- !Result
  solution: !Solution
    Num: [1, 22, 2, 25, 3, 13, 4, 16, 19, 5, 23, 14, 6, 26, 17, 10, 20, 15, 7, 24, 11, 18, 8, 27, 21, 12, 9]
    Pos: [1, 3, 5, 7, 10, 13, 19, 23, 27, 16, 21, 26, 6, 12, 18, 8, 15, 22, 9, 17, 25, 2, 11, 20, 4, 14, 24]
- !Result
  solution: !Solution
    Num: [7, 10, 19, 22, 8, 25, 11, 13, 9, 16, 20, 12, 23, 14, 4, 26, 17, 5, 21, 15, 6, 24, 1, 18, 2, 27, 3]
    Pos: [23, 25, 27, 15, 18, 21, 1, 5, 9, 2, 7, 12, 8, 14, 20, 10, 17, 24, 3, 11, 19, 4, 13, 22, 6, 16, 26]
- !Result
  solution: !Solution
    Num: [7, 10, 19, 25, 8, 16, 11, 22, 9, 13, 20, 12, 17, 26, 4, 14, 23, 5, 21, 18, 6, 15, 1, 27, 2, 24, 3]
    Pos: [23, 25, 27, 15, 18, 21, 1, 5, 9, 2, 7, 12, 10, 16, 22, 6, 13, 20, 3, 11, 19, 8, 17, 26, 4, 14, 24]
***/

%-----------------------------------------------------------------------------%
% Langford's Problem  (CSPlib problem 24)
%
% June 2006; Sebastian Brand
%
% Instance L(k,n):
% Arrange k sets of numbers 1 to n so that each appearance of the number m is m
% numbers on from the last.  For example, the L(3,9) problem is to arrange 3
% sets of the numbers 1 to 9 so that the first two 1's and the second two 1's
% appear one number apart, the first two 2's and the second two 2's appear two
% numbers apart, etc.
%-----------------------------------------------------------------------------%
% MiniZinc version
% Peter Stuckey September 30

include "globals.mzn";

%-----------------------------------------------------------------------------%
% Instance
%-----------------------------------------------------------------------------%

% int: n = 10;                            % numbers 1..n
% int: k = 2;                             % sets 1..k
int: n = 9;
int: k = 3;

%-----------------------------------------------------------------------------%
% Input
%-----------------------------------------------------------------------------%

set of int: numbers = 1..n;             % numbers
set of int: sets    = 1..k;             % sets of numbers
set of int: num_set = 1..n*k;

set of int: positions = 1..n*k;         % positions of (number, set) pairs

%-----------------------------------------------------------------------------%
% Primal model
%-----------------------------------------------------------------------------%

array[num_set] of var positions: Pos;
					% Pos[ns]: position of (number, set)
                                        % pair in the sought sequence
constraint
        forall(i in 1..n, j in 1..k-1) (
            Pos[k*(i-1) + j+1] - Pos[k*(i-1) + j] = i+1
        );

constraint
        alldifferent(Pos);

%-----------------------------------------------------------------------------%
% Dual model (partial)
%-----------------------------------------------------------------------------%

array[positions] of var num_set: Num;   % Num[p]: (number, set) pair at
                                        % position p in the sought sequence
constraint
        alldifferent(Num);

%-----------------------------------------------------------------------------%
% Channelling between primal model and dual model
%-----------------------------------------------------------------------------%

constraint
        forall(i in numbers, j in sets, p in positions) (
                (Pos[k*(i-1) + j] = p) <-> (Num[p] = k*(i-1) + j)
        );

%-----------------------------------------------------------------------------%

	% Without specifying a sensible search order this problem takes
	% forever to solve.
	%
solve	:: int_search(Pos, first_fail, indomain_split, complete)
	satisfy;

output
	[ if j = 1 then "\n" ++ show(i) ++ "s at " else ", " endif ++
	  show(Pos[k*(i-1) + j])
	| i in 1..n, j in 1..k
	] ++
	[ "\n" ];
