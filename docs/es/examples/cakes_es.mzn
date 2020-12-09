% Tortas de hornear para la fiesta de la escuela.

% Número de tortas de plátano.
var 0..100: b;

% Número de tortas de chocolate.
var 0..100: c;

% Harina
constraint 250*b + 200*c <= 4000;

% Plátanos
constraint 2*b  <= 6;

% Azúcar
constraint 75*b + 150*c <= 2000;

% Mantequilla
constraint 100*b + 150*c <= 500;

% Cacao
constraint 75*c <= 500;

% Maximizar el beneficio
solve maximize 400*b + 450*c;

% Salida
output ["Número de tortas de plátano = \(b)\n",
"Número de tortas de chocolate = \(c)\n"];
