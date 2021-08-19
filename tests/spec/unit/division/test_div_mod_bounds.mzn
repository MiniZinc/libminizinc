/***
!Test
solvers: [gecode, gurobi, scip]
expected:
- !Result
  solution: !Solution
    x: 20
    y: -3
    z: -6
    zbnd: [-20, 20]
    zfbnd: [!Approx 0.15, !Approx 1.333333333333333]
    zif: !Approx -6.666666666666667
    tf: 1
    tfbnd: [!Approx 0.1125, !Approx 8.888888888888889]
    zmbnd: [-6, 6]
    zm2lb: -6
***/


var 3..20: x; 
var -5..-3 union 5..7:  y; 
var int: z;
constraint z == x div y;
array[1..2] of var int: zbnd;
constraint zbnd == [lb(z), ub(z)];

var 0.3..2.0: xf; 
var 1.5..2.0: yf; 
var float: zf;
constraint zf == xf / yf;
constraint trace(" Set \( compute_float_div_bounds(xf, yf) ) \n");
constraint trace(" Dom \( lb(zf)..ub(zf) ) \n");
array[1..2] of var float: zfbnd;
constraint zfbnd == [lb(zf), ub(zf)];

var float: zif;
constraint zif == x / y;
constraint trace(" Set(zif) \( compute_float_div_bounds(x, y) ) \n");
constraint trace(" dom(zif) = \( lb(zif)..ub(zif) ) \n");
var float: xif = x;
constraint trace(" dom(xif) = \( lb(xif)..ub(xif) ) \n");

var float: tf;
constraint tf = zf / (xf / yf);
array[1..2] of var float: tfbnd;
constraint tfbnd == [lb(tf), ub(tf)];
constraint trace(" Set \( compute_float_div_bounds(zf, (xf/yf) ) ) \n");

var 3..20: xm; 
var 5..7:  ym; 
var int: zm;
constraint zm == xm mod ym;
array[1..2] of var int: zmbnd;
constraint zmbnd == [lb(zm), ub(zm)];

var 3..20: xm2; 
var 5..7:  ym2; 
var int: zm2lb;
constraint zm2lb == lb(xm2 mod ym2);

solve minimize x / y;

