/***
!Test
solvers: [gecode]
expected: !Error
  type: TypeError
***/

int: NUM_W=2;
int: NUM_A=1;
int: NUM_B=1;

enum W = C(1..NUM_W);

enum A = P({ to_enum(W, c) | c in 1..NUM_A });

enum B = S({ to_enum(W, NUM_A+c) | c in 1..NUM_B });

output[show(W) ++ "\n"]; % {C(1), C(2)}
output[show(A) ++ "\n"]; % {P(C(1))}
output[show(B) ++ "\n"]; % {S(C(2))}
output[show((to_enum(A, 1))) ++ "\n"]; % P(C(1))
output[show(to_enum(W, 1) in A) ++ "\n"]; % true   <- makes sense
output[show(to_enum(W, 1) in B) ++ "\n"];  % true <- WHY???
