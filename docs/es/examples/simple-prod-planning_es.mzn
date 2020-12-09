% Productos a producir.
enum Products;

% Beneficio por unidad para cada producto.
array[Products] of int: profit;

% Recursos a utilizar.
enum Resources;

% Cantidad de cada recurso disponible.
array[Resources] of int: capacity;

% Unidades de cada recurso requerido para producir 1 unidad de producto.
array[Products, Resources] of int: consumption;
constraint assert(forall (r in Resources, p in Products)
           (consumption[p,r] >= 0), "Error: consumo negativo");

% Sobre el número de productos.
int: mproducts = max (p in Products)
                     (min (r in Resources where consumption[p,r] > 0)
                          (capacity[r] div consumption[p,r]));

% Variables: La cantidad que se debe de producir de cada producto.
array[Products] of var 0..mproducts: produce;
array[Resources] of var 0..max(capacity): used;

% La producción no puede utilizar más que los recursos disponibles.
constraint forall (r in Resources) (
      used[r] = sum (p in Products)(consumption[p, r] * produce[p])
);
constraint forall (r in Resources) (
      used[r] <= capacity[r]
);

% Maximizar los beneficios.
solve maximize sum (p in Products) (profit[p] * produce[p]);

output [ "\(p) = \(produce[p]);\n" | p in Products ] ++
       [ "\(r) = \(used[r]);\n" | r in Resources ];
