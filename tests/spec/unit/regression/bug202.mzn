/***
!Test
expected: !Error
***/

% Regression test for bug #220.  The assignment of the let-variable 'capacity'
% below should result in an error message being produced during flattening
% because there are different array indexes on the LHS and RHS.
% mzn2fzn 1.3.1 and before do not detect this and instead produced an array
% with an incorrect index set.

int: K = 3;
int: R = 3;

constraint
    forall ( k in 1..K, j in 1..R) 
              (let {
              array[0..K] of int: capacity = [100000] ++ [k | k in 1..K]
            }
          in 
            trace( show(capacity) ++ " " ++ show(min(index_set(capacity))) ++"\n",
	       true)
          );

solve satisfy;
