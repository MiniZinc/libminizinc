int: x = 3;
int: y = 4;
predicate smallx(var int:y) = -x <= y /\ y <= x;
predicate p(int: u, var bool: y) = 
      exists(x in 1..u)(y \/ smallx(x));
constraint p(x,false);
solve satisfy;
