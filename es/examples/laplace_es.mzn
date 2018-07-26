int: w = 4;
int: h = 4;

% Array declaration.
set of int: HEIGHT = 0..h;
set of int: CHEIGHT = 1..h-1;
set of int: WIDTH = 0..w;
set of int: CWIDTH = 1..w-1;

% Temperatura en el punto (i, j).
array[HEIGHT,WIDTH] of var float: t;

% Temperatura del borde izquierdo.
var float: left;

% Temperatura del borde derecho.
var float: right;

% Temperatura del borde superior.
var float: top;

% Temperatura del borde inferior.
var float: bottom;

% Ecuación.
% Ecuación de Laplace: Cada temperatura interna. es la media de sus vecinos.
constraint forall(i in CHEIGHT, j in CWIDTH)(
              4.0*t[i,j] = t[i-1,j] + t[i,j-1] + t[i+1,j] + t[i,j+1]);
% Lados.
% Restricciones de los bordes.
constraint forall(i in CHEIGHT)(t[i,0] = left);
constraint forall(i in CHEIGHT)(t[i,w] = right);
constraint forall(j in CWIDTH)(t[0,j] = top);
constraint forall(j in CWIDTH)(t[h,j] = bottom);

% Esquinas.
% Restricciones de esquinas.
constraint t[0,0]=0.0;
constraint t[0,w]=0.0;
constraint t[h,0]=0.0;
constraint t[h,w]=0.0;
left = 0.0;
right = 0.0;
top = 100.0;
bottom = 0.0;

solve satisfy;

output [ show_float(6, 2, t[i,j]) ++
         if j == h then "\n" else " " endif |
         i in HEIGHT, j in WIDTH
];
