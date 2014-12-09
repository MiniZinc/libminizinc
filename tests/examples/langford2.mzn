% RUNS ON mzn20_fd
% RUNS ON mzn20_fd_linear

% vim: ft=zinc ts=4 sw=4 et
% Ralph Becket <rafe@csse.unimelb.edu.au>
% Wed Feb 25 16:43:52 EST 2009
%
% Langford's problem (see e.g., http://www.lclark.edu/~miller/langford.html)
% ------------------
% 
% Arrange k sets of 1..n such that successive occurrences of any number, k,
% are separated by k other numbers.

% There should be 26 solutions to this problem.
int: k = 2;
int: n = 7;
int: nk = n * k;

array [1..nk] of var 1..n: a;     % The sequence.
array [1..n] of var 1..nk: Fst;   % Fst[k] is position of first k.

    % Break some symmetry.
    %
constraint a[1] <= a[nk];

    % Prune some domains.
    %
constraint
    forall ( i in 1..n ) ( Fst[i] <= nk - (k - 1) * (i + 1) );

    % The nitty gritty.
    %
constraint
    forall ( i in 1..n, j in 0..(k - 1) ) ( a[Fst[i] + j * (i + 1)] = i );

solve :: int_search(Fst, first_fail, indomain_min, complete) satisfy;

output ["a = ", show(a), ";\n"];
