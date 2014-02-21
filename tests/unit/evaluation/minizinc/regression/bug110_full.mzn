% RUNS ON mzn_fd
% RUNS ON mzn_mer_fd
% RUNS ON mzn_mer_lp

% Regression test for MiniZinc bug 110.
% mzn2fzn 1.1 was generating out of range array indicies in reified contexts,
% when the result of the reification should have just been "false".

% Peacable Army of queens in MiniZinc.
%
% Translation of the ESSENCE' model in the Minion Translator examples:
% http://www.cs.st-andrews.ac.uk/~andrea/examples/peacableArmyOfQueens/peaceableArmyOfQueens.eprime
% """
% Place 2 equally-sized armies of queens (white and black)
% on a chess board without attacking each other
% Maximise the size of the armies.
%
% 'occurrence' representation 
% """

% 
% Model created by Hakan Kjellerstrand, hakank@bonetmail.com
% See also my MiniZinc page: http://www.hakank.org/minizinc
%
% There are 18 solutions for n=5
% For n=6: 560 solutions


% board width
% given        n : int(1..1000)
int: n;
% letting      N be domain int(1..n)
set of int: N = 1..n;


% 0: empty field, 1:white queen, 2: black queen
array[N,N] of var 0..2: board;
var 1..(n*n) div 2: amountOfQueens;


solve :: int_search([board[i,j] | i,j in N], first_fail, indomain, complete) maximize amountOfQueens;

constraint
   board[1,1] = 1  % let's place a white queen in the north-west corner    
   /\

   % we have the same amount of white as black queens
   sum(row, col in N) ( bool2int(board[row,col] = 1)) = amountOfQueens

   /\

   sum(row, col in N) ( bool2int(board[row,col] = 2) ) = amountOfQueens

   /\


   % if we have a white queen at position row and column
   % there is no field on the same row/column/diagonal
   % that holds a black queen
   forall(row,col in N) ( 
      (board[row, col] = 1) ->
            (forall(i in N) (
               ((i != row) ->
                  (board[i,col] < 2))
               /\
               ((i!=col) ->
                  (board[row,i] < 2))
	       /\ 
               (((row+i <= n) /\ (col+i <= n)) ->
                (board[row+i,col+i] < 2)	  
		)  
	       /\ 
               (((row-i > 0) /\ (col-i > 0)) ->
                (board[row-i,col-i] < 2)	  
		)  
	       /\ 
               (((row+i <= n) /\ (col-i > 0)) ->
                (board[row+i,col-i] < 2)	  
		)  
	       /\ 
               (((row-i > 0) /\ (col+i <= n)) ->
                (board[row-i,col+i] < 2)	  
		))
            )
     )

   /\

   % if we have a black queen at position row and column
   % there is no field on the same row/column/diagonal
   % that holds a white queen

   % original ESSENCE' comment:
   % we cannot use != 1 since diseq is not reifiable
   forall(row,col in N) (
      (board[row, col] = 2) ->
            forall(j in N) (
               ((j != row) ->
                  ((board[j,col] < 1) \/ (board[j,col] > 1) ))
               /\
               ((j!=col) ->
                  ((board[row,j] < 1) \/ (board[row,j] > 1)))
	       /\ 
               (((row+j <= n) /\ (col+j <= n)) ->
                ((board[row+j,col+j] < 1) \/ (board[row+j,col+j] > 1))	  
		)  
	       /\ 
               (((row-j > 0) /\ (col-j > 0)) ->
                ((board[row-j,col-j] < 1) \/ (board[row-j,col-j] > 1))	  
		)  
	       /\ 
               (((row+j <= n) /\ (col-j > 0)) ->
                ((board[row+j,col-j] < 1) \/  (board[row+j,col-j] > 1))	   
		)  
	       /\ 
               (((row-j > 0) /\ (col+j <= n)) ->
                ((board[row-j,col+j] < 1) \/   (board[row-j,col+j] > 1))
		)  
            )
    )

;

%
% data 
%
n = 5;
% n = 6;

output [
    "board = ", show(board), ";\n",
    "amountOfQueens = ", show(amountOfQueens), ";\n"
];
