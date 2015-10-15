% RUNS ON mzn20_fd

function var int:mydiv(var int: x, var int: y) = 
         let {constraint y != 0 } in 
         safediv(x, y)
;

function var int: safediv(var int: x, var int: y)   :: promise_total = 
  let {
    var 0..ub(x): q;
    var 0..ub(y)-1: r;
    constraint q*y + r = x;
    constraint r < y;
  }  in q;

var int: x;
var int: y; 
var int: z;

constraint x = 7;
constraint y = 1;
constraint z = mydiv(x,y);

solve satisfy;