%% Variables.

% Reembolso trimestral.
var float: R;

% Capital prestado inicialmente.
var float: P;

% Tasa de interés
var 0.0 .. 10.0: I;

%% Variables intermedias.

% saldo después de un trimestre.
var float: B1;

% Saldo después de dos trimestres.
var float: B2;

% Saldo después de tres trimestres.
var float: B3;

% Saldo adeudado al final.
var float: B4;

% Restricciones.
constraint B1 = P * (1.0 + I) - R;
constraint B2 = B1 * (1.0 + I) - R;
constraint B3 = B2 * (1.0 + I) - R;
constraint B4 = B3 * (1.0 + I) - R;

solve satisfy;

% Salida
output [
"Préstamo ", show_float(0, 2, P), " al ", show(I*100.0),
"% interés y reembolso ", show_float(0, 2, R),
"\npor trimestre para 1 año deja ",
show_float(0, 2, B4), " debido\n"
];
