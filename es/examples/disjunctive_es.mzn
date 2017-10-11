include "cumulative_es.mzn";

predicate disjunctive(array[int] of var int:s, array[int] of int:d) =
          assert(index_set(s) == index_set(d), "disjunctive: " ++
          "Los argumentos primero y segundo adeben tener el mismo conjunto de índices",
            cumulative(s, d, [ 1 | i in index_set(s) ], 1)
          );
