/***
!Test
expected:
- !Result
  solution: !Solution
    a:
    - [false, false, false, false]
    - [false, false, false, false]
    - [false, false, false, false]
    - [false, false, false, false]
    d: [3, 3, 3, 3]
***/

% Regression test for bug #380 - the body of the lex_leq_bool_half was being
% flattened in a reified context.  (The reification context was not being
% correctly reset after the application arguments had been flattened.)

int: n = 4;
set of int: N = 1..n;

array[N,N] of var bool: a;
array[N] of var 3..n-1: d;

int: i = 2;

constraint 
	   lex_lesseq_bool_half([ a[i,j+1] | j in N where j != i /\ j != i+1 ],
			         [ a[i,j]   | j in N where j != i /\ j != i+1 ],
			          d[i] = d[i+1] );

solve satisfy;

output ["d = array1d(1..4, ", show(d), ");\n"];		  


% half reified version of lex_lesseq for Booleans, that is
% h -> lex_lesseq(x,y)
predicate lex_lesseq_bool_half(array[int] of var bool: x,
                         array[int] of var bool: y,
			 var bool: h) =
    let { int: lx = min(index_set(x)),
          int: ux = max(index_set(x)),
          int: ly = min(index_set(y)),
          int: uy = max(index_set(y)),
          int: size = max(ux - lx, uy - ly),
          array[0..size] of var bool: b }
          % b[i] is true if the lexicographical order holds from position i on.
    in
    (h -> b[0])
    /\
    forall(i in 0..size) (
        b[i] = ( x[lx + i] <= y[ly + i]
                 /\
                 if i = size then true
                 else x[lx + i] <  y[ly + i] \/ b[i+1] endif
               )
    );
