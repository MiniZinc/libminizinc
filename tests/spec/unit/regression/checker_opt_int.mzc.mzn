opt int: x;
opt int: y;
output [if x = 2 /\ absent(y) then "CORRECT" else "INCORRECT" endif];
