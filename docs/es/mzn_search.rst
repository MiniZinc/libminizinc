.. _sec-search:

Búsqueda
========

.. index::
  single: annotation

Por defecto en MiniZinc no hay una declaración de cómo queremos buscar soluciones. Esto deja la búsqueda completamente al solucionador subyacente. Pero a veces, en particular para los problemas de entero combinatorio, es posible que deseemos especificar cómo debe realizarse la búsqueda. Esto requiere que nos comuniquemos con el solucionador a una estrategía :index:`search` strategy. Tenga en cuenta que la estrategia de búsqueda *no* es realmente parte del modelo. De hecho, no se requiere que cada solucionador implemente todas las posibles estrategias de búsqueda. MiniZinc usa un enfoque consistente para comunicar información adicional al solucionador de restricciones usando *anotaciones*.

Búsqueda de dominio finito
--------------------------

.. index::
  single: search; finite domain

La búsqueda en un solucionador de dominio finito implica examinar los restantes valores posibles de las variables y elegir restringir aún más algunas variables. La búsqueda agrega una nueva restricción que restringe los valores restantes de la variable (en efecto, adivinar dónde podría estar la solución), y luego aplica la propagación para determinar qué otros valores todavía son posibles en las soluciones. Para garantizar la integridad, la búsqueda deja otra opción que es la negación de la nueva restricción. La búsqueda finaliza cuando el solucionador de dominio finito detecta que se cumplen todas las restricciones y, por lo tanto, se ha encontrado una solución o que las restricciones no son satisfactorias. Cuando se detecta insatisfacción, la búsqueda debe continuar por un conjunto diferente de opciones. Normalmente, los solucionadores de dominio finito usan :index:`depth first search <search; depth first>` donde deshacer la última elección realizada y luego intentar hacer una nueva elección.

.. literalinclude:: examples/nqueens_es.mzn
  :language: minizinc
  :name: ex-queens
  :caption: Modelo para n-reinas (:download:`nqueens_es.mzn <examples/nqueens_es.mzn>`).




Un ejemplo simple de un problema de dominio finito es el problema matemático :math:`n` reinas, que requiere que pongamos :math:`n` reinas en un tablero de ajedrez :math:`n \times n` para que ninguno pueda atacar a otro.

La variable :mzn:`q[i]` registra en qué fila se coloca la reina en la columna :mzn:`i`. Las restricciones :mzn:`alldifferent` aseguran que no haya dos reinas en la misma fila o diagonal.

Un árbol de búsqueda típico (parcial) para :mzn:`n = 9` se ilustra en :numref:` fig-9q-a`.

Primero establecemos :mzn:`q[1] = 1`, esto elimina los valores de los dominios de otras variables. De modo que, por ejemplo :mzn:`q[2]` no puede tomar los valores 1 o 2.

Luego establecemos :mzn:`q[2] = 3`, esto elimina los valores de los dominios de otras variables. Establecemos :mzn:`q[3] = 5` (su valor más temprano posible).

El estado del tablero de ajedrez después de estas tres decisiones se muestra en :numref:`fig-9q-b` donde las reinas indican la posición de las reinas ya fijadas y las estrellas indican las posiciones donde no podemos colocar una reina ya que podría tomar una reina ya colocada.

.. _fig-9q-a:

.. figure:: figures/tree-4.*

  Árboles de búsqueda parcial para 9 reinas

.. _fig-9q-b:

.. figure:: figures/chess9x9-3.*

  El estado después de la adición de ``q[1] = 1``, ``q[2] = 4``, ``q[3] = 5``

.. _fig-9q-c:

.. figure:: figures/chess9x9-4.*

  La propagación inicial al agregar más ``q[6] = 4``

Una estrategia de búsqueda determina qué opciones tomar. Las decisiones que hemos tomado hasta ahora siguen la simple estrategia de elegir la primera variable que aún no se ha solucionado e intentar establecerla en su menor valor posible. Siguiendo esta estrategia, la siguiente decisión sería :mzn:`q[4] = 7`.

Una estrategia alternativa para la selección de variables es elegir la variable cuyo conjunto actual de valores posibles (*dominio*) sea más pequeño.

En virtud de la llamada estrategia de selección de variables *first-fail*, la siguiente decisión sería :mzn:`q[6] = 4`.


Si tomamos esta decisión, inicialmente la propagación elimina los valores adicionales que se muestran en :numref:`fig-9q-c`. Pero esto deja solo un valor para :mzn:`q[8]`, :mzn:`q[8] = 7`, entonces esto es forzado, pero esto deja solo un valor posible para :mzn:`q[7]` y :mzn:`q[9]`, eso es 2. Por lo tanto, se debe violar una restricción. Hemos detectado insatisfacción, y el solucionador debe retroceder deshaciendo la última decisión :mzn:`q[6] = 4` y agregando su negación :mzn:`q[6]! = 4` (llevándonos al estado (c) en el árbol en :numref:`fig-9q-a`) que fuerza :mzn:`q[6] = 8`. Esto elimina algunos valores del dominio y luego reinvocamos la estrategia de búsqueda para decidir qué hacer.

Muchas búsquedas de dominio finito se definen de esta manera: elija una variable para restringir aún más, y luego elija cómo restringirla aún más.




Anotaciones de búsqueda
-----------------------

.. index::
  single: search; annotation
  single: solve

Las anotaciones de búsqueda en MiniZinc especifican cómo buscar para encontrar una solución al problema. La anotación se adjunta al elemento de resolver, después de la palabra clave :mzn:`solve`.

La anotación de búsqueda

.. literalinclude:: examples/nqueens_es.mzn
  :language: minizinc
  :lines: 11-12


Aparece en el elemento de resolver. Las anotaciones se adjuntan a las partes del modelo utilizando el conector :mzn:`::`.

Esta anotación de búsqueda significa que debemos buscar seleccionando de la matriz de variables enteras :mzn:`q`, la variable con el dominio actual más pequeño (esta es la regla :mzn:`first_fail`), e intenta configurarlo en su valor más pequeño valor posible (selección del valor :mzn:`indomain_min`), mirando a través de todo el árbol de búsqueda (búsqueda :mzn:`complete`).



.. % \begin{tabular}{|c|c|c|c|c|c|c|c|c|}
.. % \hline
.. % Q & . & . & . & . & . & . & . & . \\ \hline
.. % . & . & . &   &   & . &   &   &   \\ \hline
.. % . & Q & . & . & . & . & . & . & . \\ \hline
.. % . & . & . & . &   &   &   &   &   \\ \hline
.. % . & . & Q & . & . & . & . & . & . \\ \hline
.. % . & . & . & . & . & . &   &   &   \\ \hline
.. % . & . & . &   & . & . & . &   &   \\ \hline
.. % . & . & . &   &   & . & . & . &   \\ \hline
.. % . & . & . &   &   &   & . & . & . \\ \hline
.. % \end{tabular}

.. defblock:: Anotaciones de Búsqueda Básicas

  .. index::
    single: int_search
    single: bool_search
    single: set_search

  Hay tres anotaciones de búsqueda básicas correspondientes a diferentes tipos de variables básicas:

  - :mzndef:`int_search( <variables>, <varchoice>, <constrainchoice>, <strategy> )`
    where :mzndef:`<variables>` is a one dimensional array of :mzn:`var int`,
    :mzndef:`<varchoice>` is a variable choice annotation discussed below,
    :mzndef:`<constrainchoice>` is a choice of how to constrain a variable, discussed
    below, and :mzndef:`<strategy>` is a search strategy which we will assume for now
    is :mzn:`complete`.
  - :mzndef:`bool_search( <variables>, <varchoice>, <constrainchoice>, <strategy> )`
    where :mzndef:`<variables>` is a one dimensional array of :mzn:`var bool`
    and the rest are as above.
  - :mzndef:`set_search( <variables>, <varchoice>, <constrainchoice>, <strategy> )`
    where :mzndef:`<variables>` is a one dimensional array of :mzn:`var set of int`
    and the rest are as above.
  - :mzndef:`float_search( <variables>, <precision>, <varchoice>, <constrainchoice>, <strategy> )`
    where :mzndef:`<variables>` is a one dimensional array of :mzn:`var float`,
    :mzndef:`<precision>` is a fixed float specifying the :math:`\epsilon` below which
    two float values are considered equal,
    and the rest are as above.

  .. index::
    single: search; variable choice
    single: input_order
    single: first_fail
    single: smallest

  Ejemplo de anotaciones de elección de variable son:

  - :mzn:`input_order`: choose in order from the array
  - :mzn:`first_fail`: choose the variable with the smallest domain size, and
  - :mzn:`smallest`: choose the variable with the smallest value in its domain.

  .. index::
    single: search; constrain choice
    single: indomain_min
    single: indomain_median
    single: indomain_random
    single: indomain_split

  Ejemplos de formas de restringir una variable son:

  - :mzn:`indomain_min`: asignar a la variable su valor de dominio más pequeño,
  - :mzn:`indomain_median`: asignar la variable su valor de dominio mediano,
  - :mzn:`indomain_random`: asignarle a la variable un valor aleatorio de su dominio, y
  - :mzn:`indomain_split` bisectar el dominio de variables excluyendo la mitad superior.

  El :mzndef:`<strategy>` casi siempre es :mzn:`complete` para una búsqueda completa.
  Para obtener una lista completa de las anotaciones de opciones de restricciones y restricciones, consulte la especificación FlatZinc en la documentación de referencia MiniZinc.

Podemos construir estrategias de búsqueda más complejas utilizando anotaciones de constructor de búsqueda. Solo hay una anotación de este tipo en el presente:

.. index::
  single: search; sequential
  single: seq_search

.. code-block:: minizinc

  seq_search([ <search-ann>, ..., <search-ann> ])

El constructor de búsqueda secuencial primero emprende la búsqueda dada por la primera anotación en su lista, cuando todas las variables en esta anotación son fijas, emprende la segunda anotación de búsqueda, etc. hasta que todas las anotaciones de búsqueda estén completas.


Considere el modelo de planificación de puestos de trabajo que se muestra en :numref:`ex-jobshop3`.

Podríamos reemplazar el elemento de resolver con:

.. code-block:: minizinc

  solve :: seq_search([
               int_search(s, smallest, indomain_min, complete),
               int_search([end], input_order, indomain_min, complete)])
        minimize end

Que intenta establecer tiempos de inicio :mzn:`s` al elegir el trabajo que puede comenzar más temprano y establecerlo en ese momento. Cuando se completan todos los tiempos de inicio, el tiempo de finalización :mzn:`end` no se puede arreglar. Por lo tanto, lo establecemos en su valor mínimo posible.


Anotaciones
-----------

.. index::
  single: annotation

Las anotaciones son un objeto de primera clase en MiniZinc. Podemos declarar nuevas anotaciones en un modelo, declarar y asignar a las variables de anotación.

.. defblock:: Anotaciones

  .. index::
    single: ann

  Las anotaciones tienen un tipo :mzn:`ann`.
  Puede declarar una anotación :index:`parameter` (con asignación opcional):

  .. code-block:: minizincdef

    ann : <ident>;
    ann : <ident> = <ann-expr> ;

  Y asignar a una variable de anotación como cualquier otro parámetro.

  Los elementos :index:`Expressions <expression>`, :index:`variable declarations <variable; declaration>`, y :mzn:`solve` se pueden anotar utilizando el operador :mzn:`::`.

  Podemos declarar un nuevo :index:`annotation` utilizando el :mzn:`annotation` :index:`item <item; annotation>`:

  .. code-block:: minizincdef

    annotation <annotation-name> ( <arg-def>, ..., <arg-def> ) ;

.. literalinclude:: examples/nqueens-ann_es.mzn
  :language: minizinc
  :name: ex-queens-ann
  :caption: Modelo anotado para n-reinas (:download:`nqueens-ann_es.mzn <examples/nqueens-ann_es.mzn>`).

El programa en :numref:`ex-queens-ann` ilustra el uso de declaraciones de anotación, anotaciones y variables de anotación.

Declaramos una nueva anotación :mzn:`bitdomain`, que pretende decirle al solucionador que los dominios variables deben representarse mediante matrices de bits de tamaño :mzn:`nwords`.

La anotación se adjunta a las declaraciones de las variables :mzn:`q`.

Cada una de las restricciones :mzn:`alldifferent` se anota con la anotación incorporada :mzn:`domain` que indica al solucionador que use la versión de propagación de dominio de :mzn:`alldifferent` si tiene una.

Una variable de anotación :mzn:`search_ann` se declara y se usa para definir la estrategia de búsqueda. Podemos dar el valor a la estrategia de búsqueda en un archivo de datos separado.

Las anotaciones de búsqueda de ejemplo pueden ser las siguientes (donde imaginamos que cada línea está en un archivo de datos separado).

.. code-block:: minizinc

  search_ann = int_search(q, input_order, indomain_min, complete);
  search_ann = int_search(q, input_order, indomain_median, complete);
  search_ann = int_search(q, first_fail, indomain_min, complete);
  search_ann = int_search(q, first_fail, indomain_median, complete);

El primero solo prueba las reinas para establecerlas en el valor mínimo, el segundo prueba las variables reinas en orden, pero las establece en su valor mediano, el tercero prueba la variable reina con el dominio más pequeño y lo establece en el valor mínimo, y la estrategia final prueba la variable reinas con el dominio más pequeño configurándola en su valor mediano.

Las diferentes estrategias de búsqueda pueden marcar una diferencia significativa en lo fácil que es encontrar soluciones. En la siguiente tabla se muestra una pequeña comparación de la cantidad de elecciones realizadas para encontrar la primera solución de los problemas de n-reinas utilizando las 4 estrategias de búsqueda diferentes (donde --- significa más de 100,000 opciones). Claramente, la estrategia de búsqueda correcta puede marcar una diferencia significativa.

.. cssclass:: table-nonfluid table-bordered

+-----+-----------+--------------+--------+-----------+
|  n  | input-min | input-median | ff-min | ff-median |
+=====+===========+==============+========+===========+
| 10  | 28        |  15          |  16    | 20        |
+-----+-----------+--------------+--------+-----------+
| 15  | 248       |  34          |  23    | 15        |
+-----+-----------+--------------+--------+-----------+
| 20  | 37330     |  97          |  114   | 43        |
+-----+-----------+--------------+--------+-----------+
| 25  | 7271      |  846         |  2637  | 80        |
+-----+-----------+--------------+--------+-----------+
| 30  | ---       |  385         |  1095  | 639       |
+-----+-----------+--------------+--------+-----------+
| 35  | ---       |  4831        |  ---   | 240       |
+-----+-----------+--------------+--------+-----------+
| 40  | ---       |  ---         |  ---   | 236       |
+-----+-----------+--------------+--------+-----------+
