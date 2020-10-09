/***
!Test
expected:
- !Result
  solution: !Solution
    p:
    - 1
    - 2
    - 1
- !Result
  solution: !Solution
    p:
    - 2
    - 2
    - 1
***/

% Regression test for MiniZinc bug #109.
% mzn2fzn 1.1 was incorrectly flattening predicate arguments in a reifying
% context.  Thiw as leading to the generated model instance being
% inconsistent.

% Smullyan Knights and Knaves problems in MiniZinc.
%
%
% These are the problems 26 to 35 from Raymond Smullyan's wonderful book
% "What is the name of this book? - The riddle of dracula and 
% other logical puzzles".
%
% 
% Model created by Hakan Kjellerstrand, hakank@bonetmail.com
% See also my MiniZinc page: http://www.hakank.org/minizinc

int: knight = 1; % alway tells the truth
int: knave  = 2; % always lies

array[1..3] of var {knight, knave}: p;

% a knight always speaks the truth
% a knave always lies
%
% says(kind of person, what the person say)
%
predicate says(var int: kind, var bool: says) =
   (kind = knight /\ says = true )
   \/
   (kind = knave  /\ says = false )
;

constraint
    %% Problem 27
    %% B: A said that there are exactly 1 knights
    %% C: B is a knave      
    %% What are B and C
    %% Solution: B: knave, C: knight
    says(p[2], says(p[1], 1 = sum(i in 1..3) (bool2int(p[i] = 1))))
    /\
    says(p[3], p[2] = knave);

solve satisfy;

output ["p = ", show(p), ";\n"];
