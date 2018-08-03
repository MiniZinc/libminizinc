include "alldifferent.mzn";

int: S;
int: N = S * S;
int: digs = ceil(log(10.0,int2float(N))); % 输出的数字

set of int: PuzzleRange = 1..N;
set of int: SubSquareRange = 1..S;

array[1..N,1..N] of 0..N: start; %% 板初始0 = 空
array[1..N,1..N] of var PuzzleRange: puzzle;

% 填充初始板
constraint forall(i,j in PuzzleRange)(
    if start[i,j] > 0 then puzzle[i,j] = start[i,j] else true endif );

% 每行中取值各不相同
constraint forall (i in PuzzleRange) (
                   alldifferent( [ puzzle[i,j] | j in PuzzleRange ]) ); 

% 每列中取值各不相同
constraint forall (j in PuzzleRange) (
                   alldifferent( [ puzzle[i,j] | i in PuzzleRange ]) ); 

% 每个子方格块中取值各不相同
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
