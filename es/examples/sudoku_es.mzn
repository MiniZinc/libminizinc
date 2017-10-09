include "alldifferent.mzn";

int: S;
int: N = S * S;

% Dígitos para la salida.
int: digs = ceil(log(10.0,int2float(N)));

set of int: PuzzleRange = 1..N;
set of int: SubSquareRange = 1..S;

% tablero inicial 0 = vacío
array[1..N,1..N] of 0..N: start;
array[1..N,1..N] of var PuzzleRange: puzzle;

% Llenar el tablero inicial.
constraint forall(i,j in PuzzleRange)(
    if start[i,j] > 0 then puzzle[i,j] = start[i,j] else true endif );

% Todos diferentes en las filas.
constraint forall (i in PuzzleRange) (
                   alldifferent( [ puzzle[i,j] | j in PuzzleRange ]) );

% Todos diferentes en las columnas.
constraint forall (j in PuzzleRange) (
                   alldifferent( [ puzzle[i,j] | i in PuzzleRange ]) );

% Todos diferentes en los sub-cuadrados.
constraint
        forall (a, o in SubSquareRange)(
                alldifferent( [ puzzle[(a-1) *S + a1, (o-1)*S + o1] |
                                        a1, o1 in SubSquareRange ] ) );

solve satisfy;

output  [ show_int(digs,puzzle[i,j]) ++ " " ++
         if j mod S == 0 then " " else "" endif ++
         if j == N then
             if i != N then
                if i mod S == 0 then "\n\n" else "\n" endif
         else "" endif else "" endif
         | i,j in PuzzleRange ] ++ ["\n"];
