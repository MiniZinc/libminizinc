% Tortas de hornear para la fiesta de la escuela (con archivo de datos)

% Número de gramos de harina disponible.
int: flour;

% Número de bananas de harina disponibles.
int: banana;

% Número de gramos de azúcar disponible.
int: sugar;

% Número de gramos de mantequilla disponible.
int: butter;

% Número de gramos de cacao disponible.
int: cocoa;

constraint assert(flour >= 0,"Archivo inválido: " ++
                  "La cantidad de harina no debe ser negativa");
constraint assert(banana >= 0,"Archivo inválido: " ++
                  "La cantidad de plátanos no debe ser negativa");
constraint assert(sugar >= 0,"Archivo inválido: " ++
                  "La cantidad de azúcar no debe ser negativa");
constraint assert(butter >= 0,"Archivo inválido: " ++
                  "La cantidad de mantequilla no debe ser negativa");
constraint assert(cocoa >= 0,"Archivo inválido: " ++
                  "La cantidad de cacao no debe ser negativa");

% Número de tortas de plátano.
var 0..100: b;

% Número de tortas de chocolate.
var 0..100: c;

% Harina
constraint 250*b + 200*c <= flour;

% Plátanos
constraint 2*b  <= banana;

% Azúcar
constraint 75*b + 150*c <= sugar;

% Mantequilla
constraint 100*b + 150*c <= butter;

% Chocolate
constraint 75*c <= cocoa;

% Maximizar nuestro beneficio
solve maximize 400*b + 450*c;

output ["Número de tortas de plátano = \(b)\n",
"Número de tortas de chocolate = \(c)\n"];
