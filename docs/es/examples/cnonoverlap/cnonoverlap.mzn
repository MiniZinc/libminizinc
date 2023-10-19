% Ancho del rectángulo para contener círculos.
float: width;

% Altura del rectángulo para contener círculos.
float: height;

% (x1, y1) es el centro del círculo de radio r1.
float: r1;
var r1..width-r1:  x1;
var r1..height-r1: y1;

% (x2, y2) es el centro del círculo de radio r2.
float: r2;
var r2..width-r2:  x2;
var r2..height-r2: y2;

% Los centros son al menos r1 + r2.
constraint (x1-x2)*(x1-x2) + (y1-y2)*(y1-y2) >= (r1+r2)*(r1+r2);
solve satisfy;
