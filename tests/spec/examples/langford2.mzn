/***
!Test
expected:
- !Result
  solution: !Solution
    a: [1, 7, 1, 2, 6, 4, 2, 5, 3, 7, 4, 6, 3, 5]
- !Result
  solution: !Solution
    a: [1, 7, 1, 2, 5, 6, 2, 3, 4, 7, 5, 3, 6, 4]
- !Result
  solution: !Solution
    a: [2, 7, 4, 2, 3, 5, 6, 4, 3, 7, 1, 5, 1, 6]
- !Result
  solution: !Solution
    a: [3, 6, 7, 1, 3, 1, 4, 5, 6, 2, 7, 4, 2, 5]
- !Result
  solution: !Solution
    a: [2, 6, 7, 2, 1, 5, 1, 4, 6, 3, 7, 5, 4, 3]
- !Result
  solution: !Solution
    a: [4, 1, 7, 1, 6, 4, 2, 5, 3, 2, 7, 6, 3, 5]
- !Result
  solution: !Solution
    a: [2, 3, 7, 2, 6, 3, 5, 1, 4, 1, 7, 6, 5, 4]
- !Result
  solution: !Solution
    a: [2, 4, 7, 2, 3, 6, 4, 5, 3, 1, 7, 1, 6, 5]
- !Result
  solution: !Solution
    a: [3, 5, 7, 2, 3, 6, 2, 5, 4, 1, 7, 1, 6, 4]
- !Result
  solution: !Solution
    a: [4, 6, 1, 7, 1, 4, 3, 5, 6, 2, 3, 7, 2, 5]
- !Result
  solution: !Solution
    a: [1, 6, 1, 7, 2, 4, 5, 2, 6, 3, 4, 7, 5, 3]
- !Result
  solution: !Solution
    a: [3, 4, 6, 7, 3, 2, 4, 5, 2, 6, 1, 7, 1, 5]
- !Result
  solution: !Solution
    a: [1, 5, 1, 7, 3, 4, 6, 5, 3, 2, 4, 7, 2, 6]
- !Result
  solution: !Solution
    a: [2, 6, 3, 2, 7, 4, 3, 5, 6, 1, 4, 1, 7, 5]
- !Result
  solution: !Solution
    a: [2, 3, 6, 2, 7, 3, 4, 5, 1, 6, 1, 4, 7, 5]
- !Result
  solution: !Solution
    a: [4, 1, 6, 1, 7, 4, 3, 5, 2, 6, 3, 2, 7, 5]
- !Result
  solution: !Solution
    a: [1, 5, 1, 6, 7, 2, 4, 5, 2, 3, 6, 4, 7, 3]
- !Result
  solution: !Solution
    a: [1, 4, 1, 6, 7, 3, 4, 5, 2, 3, 6, 2, 7, 5]
- !Result
  solution: !Solution
    a: [1, 6, 1, 3, 5, 7, 4, 3, 6, 2, 5, 4, 2, 7]
- !Result
  solution: !Solution
    a: [2, 6, 3, 2, 5, 7, 3, 4, 6, 1, 5, 1, 4, 7]
- !Result
  solution: !Solution
    a: [5, 2, 6, 4, 2, 7, 5, 3, 4, 6, 1, 3, 1, 7]
- !Result
  solution: !Solution
    a: [2, 5, 6, 2, 3, 7, 4, 5, 3, 6, 1, 4, 1, 7]
- !Result
  solution: !Solution
    a: [5, 2, 4, 6, 2, 7, 5, 4, 3, 1, 6, 1, 3, 7]
- !Result
  solution: !Solution
    a: [1, 5, 1, 6, 3, 7, 4, 5, 3, 2, 6, 4, 2, 7]
- !Result
  solution: !Solution
    a: [1, 5, 1, 4, 6, 7, 3, 5, 4, 2, 3, 6, 2, 7]
- !Result
  solution: !Solution
    a: [1, 4, 1, 5, 6, 7, 4, 2, 3, 5, 2, 6, 3, 7]
***/

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
