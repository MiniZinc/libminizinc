%--------------------------------------------------------------------%
% Requiere que un conjunto de tareas dadas por los tiempos de inicio 's',
% duraciones 'd' y requerimientos de recursos 'r',
% nunca requieren más de un recurso global vinculado 'b' en cualquier momento.
% Suposiciones:
% - forall i, d[i] >= 0 and r[i] >= 0
%--------------------------------------------------------------------%
predicate cumulative(array[int] of var int: s,
                     array[int] of var int: d,
                     array[int] of var int: r, var int: b) =
   assert(index_set(s) == index_set(d) /\
          index_set(s) == index_set(r),
     "cumulativo: los argumentos del array deben tener conjuntos de índices idénticos",
   assert(lb_array(d) >= 0 /\ lb_array(r) >= 0,
     "acumulativo: las duraciones y usos de los recursos deben ser no negativos",
           let {
               set of int: times =
                 lb_array(s) ..
                 max([ ub(s[i]) + ub(d[i]) | i in index_set(s) ])
               }
           in
              forall( t in times ) (
                 b >= sum( i in index_set(s) ) (
                    bool2int( s[i] <= t /\ t < s[i] + d[i] ) * r[i]
                 )
              )
       )
   );
