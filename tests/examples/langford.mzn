% RUNS ON mzn_mer_fd
% RUNS ON mzn_mer_std_fd
% RUNS ON mzn_mer_lazyfd
% RUNS ON zinc_fdic_mznlib
% RUNS ON zinc_mip_hl
% RUNS ON minizinc_fd
% RUSN ON minizinc_cpx
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
