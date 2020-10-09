/***
!Test
expected:
- !Result
  solution: !Solution
    allsym: [P, L, E, A, S, E, ' ', S, O, L, V, E, P, U, Z, Z, L, E, '?', '?', '?', '?', '?', '?']
    num_symbols2: 11
    sym2: [A, ' ', S, O, V, P, U, Z, L, E, '?']
    x: 0
- !Result
  solution: !Solution
    allsym: [P, L, E, A, S, E, ' ', S, O, L, V, E, P, U, Z, Z, L, E, '?', '?', '?', '?', '?', '?']
    num_symbols2: 11
    sym2: [A, ' ', S, O, V, P, U, Z, L, E, '?']
    x: 1
***/

% Regression test for a flattening problem reported by pjs in version 1.1.5
% of MiniZinc.

int: size_codex;
array[1..size_codex] of 0..9: codex;
array[1..size_codex] of string: fixed;

int: num_symbols;
array[1..num_symbols] of string: symbols;

int: rows;
int: cols;
array[1..rows,1..cols] of string: puzzle;

array[1..rows*cols] of string: allsym :: add_to_output = 
		    [ puzzle[i,j] |  i in 1..rows, j in 1..cols ]; 
int: num_symbols2 :: add_to_output = length( [ allsym[i] | i in 1..rows*cols 
                             where not exists(j in i+1..rows*cols)(
			     	           allsym[i] == allsym[j]) ]);
array[1..num_symbols] of string: sym2 :: add_to_output =  [ allsym[i] | i in 1..rows*cols 
                             where not exists(j in i+1..rows*cols)(
			     	           allsym[i] == allsym[j]) ];

var 0..1:x :: add_to_output;

solve satisfy;

output [show(x),"\n"]
        ++ [show(num_symbols2),"\n"]
        ++ [show(allsym),"\n"]
        ++ [show(sym2),"\n"]
	;

size_codex = 10;
codex = [0,  1,  2,  3,  5,  6,  7,  7,  8,  9];
fixed = [" "," "," "," "," "," ","Z","?"," "," "];

num_symbols = 11;
symbols = ["P","L","E","A","S","O","V","U","Z","?"," "];

rows = 4;
cols = 6;
puzzle = [| "P", "L", "E", "A", "S", "E" 
          | " ", "S", "O", "L", "V", "E"
          | "P", "U", "Z", "Z", "L", "E" 
	  | "?", "?", "?", "?", "?", "?" |];
