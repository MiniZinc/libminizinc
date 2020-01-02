/***
!Test
solvers:
- gecode
- cbc # Can't check as floats are imprecise
expected: !Result
  status: SATISFIED
  solution: !Solution
    P: !Approx 373.0277986476333
    mortgage: [!Approx 260.3305785123967, !Approx 136.3636363636363, !Approx 0.0]
***/

% Constraint Programming: Mortgage
%
%
% This Minizinc program is written by Hakan Kjellerstrand, hakank@bonetmail.com,
% and is commented in the (swedish) blog post
% Constraint Programming: Minizinc, Gecode/flatzinc och ECLiPSe/minizinc
% http://www.hakank.org/webblogg/archives/001209.html
%
% See also my MiniZinc page: http://www.hakank.org/minizinc
%
% Marriot & Stuckey: Programming with Constraints, page 175f
% (famous early example)
%
% Since Minizinc don't allow recursion, it is implemented as an array.
%
% This example shows the flexibility of Minizinc: 
% the same code can calculate P, R _or_ I with just a simple change of the declarations.
%
%
% Calculates P, given I and R:
% P: 373.02779864763335
%
% Calculates R, given P and I:
%  R: 149.944102252456 (which is close to 150)
%
% Calculates I, given R and P:
% I: 0.099995922287248337__0.10000424331679297 (close to 0.1)

% Note: As of 2008-06-23 this don't work in version >= 0.8.
% It may work with later version, though.

int: T = 3; % time period

% comment one of the initiations to calculate it:
var 0.0..10000.0: I = 10.0/100.0;
var 0.0..10000.0: R = 150.0; 
var 0.0..10000.0: P; % = 373.02779864763318;

array[1..T] of var float: mortgage;

solve satisfy;
% solve minimize P;

constraint
   % start value:
   mortgage[1] = P + (P  * I) - R /\
   forall(i in 2..T) (
     % calculate the next value using a local variable
     let {
        var float: NP = mortgage[i-1] + (mortgage[i-1]  * I) - R 
     }
     in
     mortgage[i] =  NP /\ NP >= 0.0
   )
;

output [
   "P: ", show(P), "\n",
   "I: ", show(I), "\n",
   "R: ", show(R), "\n",
   "mortgage: ", show(mortgage),"\n" % is not especially interesting
];
