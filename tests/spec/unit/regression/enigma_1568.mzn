/***
!Test
expected:
- !Result
  solution: !Solution
    D: 6
    E: 5
    L: 3
    O: 9
    ODD: 966
    P: 1
    PUZZLE: 102235
    U: 0
    Z: 2
    num1: 161
    num2: 635
    num3: 805
    num4: 483
    x: [9, 6, 1, 0, 2, 3, 5]
***/

% Regression test for a bug in mzn2fzn 1.2.  The optimisation pass was leaving
% dangling references to variables it had "eliminated".  The symptom was the
% following error from the FlatZinc interpreter:
%
%  enigma_1568.fzn:28:
%  symbol error: `INT____3' undeclared
%  enigma_1568.fzn:30:
%  symbol error: `INT____3' undeclared
%
% (This model is from the original bug report.)
 
% Enigma problem 1568 (Odd puzzle) in MiniZinc.
% 
% From New Scientist
% http://www.newscientist.com/article/mg20427311.100-enigma-number-1568.html
% """
% 21 October 2009 by Albert Haddad
% 
% Odd puzzle
% 
% In this multiplication sum the digits have been replaced by letters and 
% dots. Different letters stand for different digits, the same letter 
% stands for the same digit, each dot can be any digit, and leading 
% digits cannot be zero.
% 
% [
%         . . .    | num1
%       * . . .    | num2
%       -------
%         . . .    | num3
%       . . .      | num4
%     O D D        | ODD
%     ---------
%   P U Z Z L E    | PUZZLE
%   
% ]
%
% What is the six-figure odd PUZZLE?
% """

% 
% This MiniZinc model was created by Hakan Kjellerstrand, hakank@bonetmail.com
% See also my MiniZinc page: http://www.hakank.org/minizinc
%
include "globals.mzn"; 

int: n = 7; % number of unknown
set of int: Digits = 0..9;
var Digits: O;
var Digits: D;
var Digits: P;
var Digits: U;
var Digits: Z;
var Digits: L;
var Digits: E;
array[1..n] of var Digits: x;

var 100..999: num1;
var 100..999: num2;
var 100..999: num3;
var 100..999: num4;
var 100..999: ODD ;
var 100000..999999: PUZZLE;

predicate cp1d(array[int] of var int: x, array[int] of var int: y) =
  assert(index_set(x) = index_set(y),
           "cp1d: x and y have different sizes",
    forall(i in index_set(x)) ( x[i] = y[i] ) )
; 


% solve satisfy;
solve :: int_search(
             x ++ [num1,num2,num3,num4,ODD,PUZZLE], 
             first_fail, 
             indomain_min, 
             complete) 
         satisfy;

constraint
   cp1d(x, [O,D,P,U,Z,L,E]) /\
   
   all_different(x) /\

   ODD = 100*O + 10*D + D /\
   PUZZLE = 100000*P + 10000*U + 1000*Z + 100*Z + 10*L + E /\

   num1 * num2 = num3 + num4*10 + ODD*100 /\
   PUZZLE = num1 * num2 /\

   PUZZLE mod 2 = 1 /\ % PUZZLE is odd

   % And then code the "long multiplication"
   num1 * (num2 mod 10) = num3 /\
   num1 * ((num2 div 10) mod 10) = num4 /\
   num1 * (num2 div 100) = ODD
;

output [
   "O = ", show(O), ";\n",
   "D = ", show(D), ";\n",
   "P = ", show(P), ";\n",
   "U = ", show(U), ";\n",
   "Z = ", show(Z), ";\n",
   "L = ", show(L), ";\n",
   "E = ", show(E), ";\n",
   "x = ", show(x), ";\n",
   "num1 = ", show(num1), ";\n",
   "num2 = ", show(num2), ";\n",
   "num3 = ", show(num3), ";\n",
   "num4 = ", show(num4), ";\n",
   "ODD = ", show(ODD), ";\n",
   "PUZZLE = ", show(PUZZLE), ";\n"
];
