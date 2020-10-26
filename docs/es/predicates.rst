.. index::
  single: predicate
  single: function

.. _sec-predicates:

Predicados y Funciones
======================

Los predicados en MiniZinc nos permiten capturar restricciones complejas de nuestro modelo de manera sucinta. Los predicados en MiniZinc se utilizan para modelar con restricciones globales predefinidas y para capturar y definir nuevas restricciones complejas por el modelador.

Las funciones son usadas en MiniZinc para capturar estructuras comunes de modelos. De hecho, un predicado es solo una función con tipo de salida :mzn:`var bool`.


.. _sec-globals:

Restricciones Globales
----------------------

.. index::
  single: global constraint

Hay muchas restricciones globales definidas en MiniZinc para su uso en el modelado.
La lista definitiva se encuentra en la documentación de la versión de liberación (release), ya que la lista está creciendo lentamente.
A continuación discutimos algunas de las restricciones globales más importantes.



Alldifferent
~~~~~~~~~~~~

.. index::
  single: alldifferent
  single: global constraint; alldifferent

La restricción :mzn:`alldifferent` toma una matriz de variables y las restringe para tomar valores diferentes.
Un uso de :mzn:`alldifferent` tiene la forma

.. code-block:: minizinc

  alldifferent(array[int] of var int: x)

El argumento es una matriz de variables enteras.

La restricción :mzn:`alldifferent` es una de las restricciones globales más estudiadas y utilizadas en la programación de restricciones. Se usa para definir subproblemas de asignación y existen propagadores globales eficientes para :mzn:`alldifferent`.
Los modelos :download:`send-more-money_es.mzn <examples/send-more-money_es.mzn>` (:numref:`ex-smm`) y :download:`sudoku_es.mzn <examples/sudoku_es.mzn>` (:numref:`ex-sudoku`) son ejemplos de modelos que usan :mzn:`alldifferent`.



Cumulative
~~~~~~~~~~

.. index::
  single: cumulative
  single: global constraint; cumulative

La restricción :mzn:`cumulative` se usa para describir el uso acumulado de recursos.

.. code-block:: minizinc

  cumulative(array[int] of var int: s, array[int] of var int: d,
             array[int] of var int: r, var int: b)

Requiere que un conjunto de tareas proporcionadas por las horas de inicio :mzn:`s`, duraciones :mzn:`d` y requisitos de recursos :mzn:`r`, nunca requiera más que un límite global de recursos :mzn:`b` en cualquier momento.

.. literalinclude:: examples/moving_es.mzn
  :language: minizinc
  :name: ex-moving
  :caption: Modelo para mover muebles usando ``cumulative`` (:download:`moving_es.mzn <examples/moving_es.mzn>`).

.. literalinclude:: examples/moving_es.dzn
  :language: minizinc
  :name: ex-movingd
  :caption: Datos para mover muebles usando ``cumulative`` (:download:`moving_es.dzn <examples/moving_es.dzn>`).

El modelo en :numref:`ex-moving` encuentra un cronograma para mover los muebles de modo que cada mueble tenga suficientes manipuladores (personas) y suficientes carritos disponibles durante el movimiento. Se proporciona el tiempo disponible, los manipuladores y los carros, y los datos proporcionan para cada objeto la duración del movimiento, el número de manipuladores y la cantidad de carros requeridos.

Usando los datos mostrados en :mzn:`ex-movingd`, el comando

.. code-block:: bash

  $ mzn-gecode moving_es.mzn moving_es.dzn

Puede dar como resultado la salida

.. code-block:: none

  start = [0, 60, 60, 90, 120, 0, 15, 105]
  end = 140
  ----------
  ==========

:numref:`fig-histogram-a` y :numref:`fig-histogram-b` muestra los requisitos para manipuladores y carros en cada momento del movimiento para esta solución.

.. _fig-histogram-a:

.. figure:: figures/handlers_es.*

  Histograma de uso de los manipuladores en movimiento.

.. _fig-histogram-b:

.. figure:: figures/trolleys_es.*

  Histograma de uso de carritos en el movimiento.

Table
~~~~~

.. index::
  single: table
  single: global constraint; table

La restricción :mzn:`table` impone que una tupla de variables tome un valor de un conjunto de tuplas. Como no hay tuplas en MiniZinc, esto se codifica utilizando matrices.

El uso de :mzn:`table` tiene una de las formas

.. code-block:: minizinc

  table(array[int] of var bool: x, array[int, int] of bool: t)
  table(array[int] of var int:  x, array[int, int] of int:  t)

Dependiendo de si las tuplas son booleanas o enteras.

La restricción impone :math:`x \in t` donde consideramos :math:`x` y cada fila en :math:`t` para ser una tupla, y :math:`t` para ser un conjunto de tuplas.

.. literalinclude:: examples/meal_es.mzn
  :language: minizinc
  :name: ex-meal
  :caption: Modelo para la planificación de comidas usando la restricción ``table`` (:download:`meal_es.mzn <examples/meal_es.mzn>`).

.. literalinclude:: examples/meal_es.dzn
  :language: minizinc
  :name: ex-meald
  :caption: Datos para la planificación de comidas que definen la tabla ``table`` usada (:download:`meal_es.dzn <examples/meal_es.dzn>`).

El modelo en :numref:`ex-meal` busca comidas balanceadas.
Cada elemento de comida tiene un nombre (codificado como un entero), un kilojulio, proteínas en gramos, sal en miligramos y grasa en gramos, así como un costo en centavos. La relación entre estos elementos se codifica utilizando una restricción :mzn:`table`.
El modelo busca una comida de costo mínimo que tenga un recuento mínimo de kilojulios :mzn:`min_energy`, una cantidad mínima de proteína :mzn:`min_protein`, cantidad máxima de sal :mzn:`max_salt` y grasa :mzn:`max_fat`.


Regular
~~~~~~~

.. index::
  single: regular
  single: global constraint; regular

La restricción :mzn:`regular` se usa para exigir que una secuencia de variables tome un valor definido por un autómata finito.
El uso de :mzn:`regular` tiene la forma


.. code-block:: minizinc

  regular(array[int] of var int: x, int: Q, int: S,
          array[int,int] of int: d, int: q0, set of int: F)

Restringe que la secuencia de valores en array :mzn:`x` (que debe estar en el :index:`range` :mzn:`1..S`) es aceptado por el :index:`DFA` de :mzn:`Q` estados con entrada :mzn:`1..S` y función de transición :mzn:`d` (que asigna :mzn:`<1..Q, 1..S>` a :mzn:`0..Q`) y el estado inicial :mzn:`q0` (que debe estar en :mzn:`1..Q`) y estados de aceptación :mzn:`F` (que deben estar todos en :mzn:`1..Q`).

El estado 0 está reservado para ser un estado siempre fallido.

.. _fig-dfa:

.. figure:: figures/dfa.*

  Un DFA determina las listas correctas.

Considere un problema de lista de enfermeras. Cada enfermera está programada para cada día como:
(d) en el turno de día,
(n) en el turno de noche, o
(o) para ninguno.
En cada período de cuatro días, una enfermera debe tener al menos un día libre y ninguna enfermera puede programar 3 turnos nocturnos seguidos.
Esto se puede codificar utilizando el DFA incompleto que se muestra en :numref:`fig-dfa`.
Podemos codificar este DFA teniendo como estado de inicial :mzn:`1`, los estados finales :mzn:`1..6`, y la función de transición.

.. cssclass:: table-nonfluid table-bordered

+---+---+---+---+
|   | d | n | o |
+===+===+===+===+
| 1 | 2 | 3 | 1 |
+---+---+---+---+
| 2 | 4 | 4 | 1 |
+---+---+---+---+
| 3 | 4 | 5 | 1 |
+---+---+---+---+
| 4 | 6 | 6 | 1 |
+---+---+---+---+
| 5 | 6 | 0 | 1 |
+---+---+---+---+
| 6 | 0 | 0 | 1 |
+---+---+---+---+

Tenga en cuenta que el estado 0 en la tabla indica un estado de error.
El modelo que se muestra en :numref:`ex-nurse` encuentra un cronograma para :mzn:`num_nurses` enfermeras sobre el :mzn:`num_days` días, donde requerimos :mzn:`req_day` enfermeras en el turno de día cada día, y enfermeras :mzn:`req_night` en el turno de noche. Cada cada enfermera toma al menos :mzn:`min_night` turnos nocturnos.

.. literalinclude:: examples/nurse_es.mzn
  :language: minizinc
  :name: ex-nurse
  :caption: Modelo para la formación de la enfermera usando la restricción ``regular`` (:download:`nurse_es.mzn <examples/nurse_es.mzn>`)

Ejecutando el comando

.. code-block:: bash

  $ mzn-gecode nurse_es.mzn nurse_es.dzn

Encuentra un horario de 10 días para 7 enfermeras, que requiere 3 en cada turno de día y 2 en cada turno de noche, con un mínimo de 2 turnos de noche por enfermera.
Un posible resultado es:

.. code-block:: none

  o d n n o n n d o o
  d o n d o d n n o n
  o d d o d o d n n o
  d d d o n n d o n n
  d o d n n o d o d d
  n n o d d d o d d d
  n n o d d d o d d d
  ----------

Hay una forma alternativa de la restricción regular :mzn:`regular_nfa` que especifica la expresión regular usando un NFA (sin arcos :mzn:`\epsilon` arcs).

Esta restricción tiene la forma:

.. code-block:: minizinc

  regular_nfa(array[int] of var int: x, int: Q, int: S,
          array[int,int] of set of int: d, int: q0, set of int: F)

Se restringe que la secuencia de valores en la matriz :mzn:`x` (que debe estar en el rango :mzn:`1..S`) sea aceptada por el :index:`NFA` de :mzn:`Q` estados con entrada :mzn:`1..S` y una función de transición :mzn:`d` (que mapea :mzn:`<1..Q, 1..S>` a subconjuntos de :mzn:`1..Q`) y un estado inicial :mzn:`q0` (que debe estar en :mzn:`1..Q`) y estados de aceptación :mzn:`F` (todos deben estar en :mzn:`1..Q`).

No es necesario un estado de falla 0, ya que la función de transición se puede asignar a un conjunto vacío de estados.



Definición de Predicados
------------------------

.. index::
  single: predicate; definition

Una de las características de modelado más potentes de MiniZinc, es la capacidad del modelador para definir sus propias restricciones de alto nivel. Esto les permite abstraer y modularizar su modelo. También permite la reutilización de restricciones en diferentes modelos y permite el desarrollo de bibliotecas específicas de aplicaciones que definen los tipos y restricciones estándar.

.. literalinclude:: examples/jobshop2_es.mzn
  :language: minizinc
  :name: ex-jobshop2
  :caption: Modelo para la programación de taller de trabajo usando predicados (:download:`jobshop2_es.mzn <examples/jobshop2_es.mzn>`)

Comenzamos con un ejemplo simple, revisando el problema de programación de la tienda de trabajo de la sección anterior. El modelo se muestra en :numref:`ex-jobshop2`. El elemento de interés es el elemento :mzn:`predicate`:

.. literalinclude:: examples/jobshop2_es.mzn
  :language: minizinc
  :lines: 12-13

Esto define una nueva restricción que impone que una tarea con hora de inicio :mzn:`s1` y duración :mzn:`d1` no se superpone con una tarea con hora de inicio :mzn:`s2` y duración :mzn:`d2`. Esto ahora se puede usar dentro del modelo en cualquier otro lugar :index:`Expresión booleana <expresión; Booleana>` (que involucra variables de decisión) donde se pueda usar.

Además de los predicados, el modelador puede definir nuevas restricciones que solo involucran parámetros. Estos son útiles para escribir pruebas fijas para una expresión condicional. Estos se definen con la palabra clave :mzn:`test`.

Por ejemplo:

.. code-block:: minizinc

  test even(int:x) = x mod 2 = 0;

.. \ignore{ % for capture for testing!
.. $ mzn-g12fd jobshop2_es.mzn jobshop_es.dzn
.. } % $

.. defblock:: Definiciones de Predicado

  .. index::
    single: predicate; definition

  Los predicados se definen mediante una declaración con la forma

  .. code-block:: minizincdef

    predicate <pred-name> ( <arg-def>, ..., <arg-def> ) = <bool-exp>

  El :mzndef:`<pred-name>` debe ser un identificador de MiniZinc válido, y cada uno de las :mzndef:`<arg-def>` es una válida declaración del :index:`type` en MiniZinc.


  .. \ignore{The type-insts\index{type-inst}
  .. of arguments may include type-inst variables\index{type-inst!variable}
  .. which are of the
  .. form \texttt{\$T} or \texttt{any \$T} with \texttt{T} an identifier. A type-inst
  .. variable \texttt{\$T}\ttindexdef{\$T}
  .. can match any fixed type-inst, whereas a type-inst
  .. variable \texttt{any \$T} can
  .. also match non-fixed type-insts\index{type-index!non-fixed}
  .. (such as \texttt{var int}).
  .. Because predicate arguments\index{argument}
  .. receive an assignment when calling the predicate, the
  .. argument type-insts may include
  .. implicitly indexed arrays\index{array!index set!implicit},
  .. as well as set variables with a
  .. non-finite element type.}

  Una relajación de las definiciones :index:`argument` es que los tipos de índice para matrices pueden ser :index:`unbounded <array; conjunto de índices; sin límites> `, escrito en :mzn:`int`.

  .. code-block:: minizincdef

    test <pred-name> ( <arg-def>, ..., <arg-def> ) = <bool-exp>

  El :mzndef:`<bool-expo>` del cuerpo debe ser reparado.

  También presentamos una nueva forma del comando :mzn:`assert` para usarlo en predicados.

  .. code-block:: minizincdef

    assert ( <bool-exp>, <string-exp>, <exp> )

El tipo de :mzn:`assert` :index:`expression <expression; assert>` es el mismo que el tipo del último argumento.
La expresión :mzn:`assert` verifica si el primer argumento es falso, y de ser así, imprime la segunda cadena de argumento. Si el primer argumento es verdadero, devuelve el tercer argumento.


Tenga en cuenta que :index:`assert expressions <expression; assert>` son flojos (lazy) en el tercer argumento, es decir, si el primer argumento es falso, no serán evaluados.
Por lo tanto, se pueden usar para verificar:

.. code-block:: minizinc

  predicate lookup(array[int] of var int:x, int: i, var int: y) =
      assert(i in index_set(x), "index out of range in lookup"
             y = x[i]
      );

This code will not evaluate :mzn:`x[i]` if :mzn:`i` is out of the range of the array :mzn:`x`.

Este código no evaluará :mzn:`x[i]` si :mzn:`i` está fuera del rango de la matriz :mzn:`x`.



Definiendo Funciones
--------------------

.. index::
  single: function; definition

Las funciones se definen en MiniZinc de manera similar a los predicados, pero con un tipo de retorno más general.

La función siguiente define la fila en una matriz de Sudoku de la fila :math:`a1^{th}` de subcuadrados :math:`a^{th}`.

.. code-block:: minizinc

  function int: posn(int: a, int: a1) = (a-1) * S + a1;

Con esta definición podemos reemplazar la última restricción en el problema de Sudoku que se muestra en :numref:`ex-sudoku` por:

.. code-block:: minizinc

  constraint forall(a, o in SubSquareRange)(
                    alldifferent([ puzzle [ posn(a,a0), posn(o,o1) ] |
                                           a1,o1 in SubSquareRange ] ) );

Las funciones son útiles para codificar expresiones complejas que se utilizan con frecuencia en el modelo. Por ejemplo, imagina colocando los números 1 en :math:`n` en diferentes posiciones en una grilla :math:`n \times n`, como la distancia de Manhattan entre dos números cualquiera :math:`i` y :math:`j` es mayor que el máximo de los dos números menos 1.

El objetivo es minimizar el total de las distancias de Manhattan entre los pares. La función de distancia de Manhattan se puede expresar como:

.. literalinclude:: examples/manhattan_es.mzn
  :language: minizinc
  :lines: 12-14

El modelo completo se muestra en :numref:`ex-manhattan`.


.. literalinclude:: examples/manhattan_es.mzn
  :language: minizinc
  :name: ex-manhattan
  :caption: Modelo para un problema de ubicación numérica que ilustra el uso de funciones (:download:`manhattan_es.mzn <examples/manhattan_es.mzn>`).

.. defblock:: Function definitions

  .. index::
    single: function; definition

  Las funciones se definen mediante una declaración de la forma

  .. code-block:: minizincdef

    function <ret-type> : <func-name> ( <arg-def>, ..., <arg-def> ) = <exp>

  El :mzndef:`<func-name>` debe ser un identificador MiniZinc válido, y cada uno de los :mzndef:`<arg-def>` es una declaración de tipo MiniZinc válida.
  El :mzndef:`<ret-type>` es el tipo de retorno de la función que debe ser el tipo :mzndef:`<exp>`. Los argumentos tienen las mismas restricciones que en las definiciones de predicados.

Funciones en MiniZinc pueden tener cualquier tipo de retorno, no solo tipos de retorno fijos.

Las funciones son útiles para definir y documentar expresiones complejas que se usan varias veces en un modelo.


Funciones de Reflexión
----------------------


Para ayudar a escribir pruebas genéricas y predicados, varias funciones de reflexión devuelven información sobre conjuntos de índices de matriz, dominios de conjuntos de variables y rangos de variables de decisión. Los de los conjuntos de índices son :mzndef:`index_set(<1-D array>)`, :mzndef:`index_set_1of2(<2-D array>)`, :mzndef:`index_set_2of2(<2-D array>)`, y así sucesivamente para matrices de mayor dimensión.

Un mejor modelo de tienda de trabajo combina todas las restricciones no superpuestas para una sola máquina en una única restricción disyuntiva.
Una ventaja de este enfoque es que si bien inicialmente podemos modelar esto simplemente como una conjunción de: restricciones :mzn:`non-overlap`, si el solucionador subyacente tiene un mejor enfoque para resolver las restricciones disyuntivas, podemos usar eso en su lugar, con cambios mínimos en nuestro modelo. El modelo se muestra en :numref:`ex-jobshop3`.

.. literalinclude:: examples/jobshop3_es.mzn
  :language: minizinc
  :name: ex-jobshop3
  :caption: Modelo para la programación de la tienda de trabajo usando un predicado ``disjunctive`` (:download:`jobshop3_es.mzn <examples/jobshop3_es.mzn>`).

.. index::
  single: global constraint; disjunctive

La restricción :mzn:`disjunctive` toma una matriz de tiempos de inicio para cada tarea y una matriz de sus duraciones y se asegura de que solo una tarea esté activa en un momento dado. Definimos la restricción disyuntiva como un :index:`predicate <predicate; definition>` con firma

.. code-block:: minizinc

  predicate disjunctive(array[int] of var int:s, array[int] of int:d);

Podemos usar la restricción disyuntiva para definir la no superposición de tareas como se muestra en :numref:`ex-jobshop3`.
Suponemos una definición para el predicado :mzn:`disjunctive` está dada por el archivo :download:`disjunctive_es.mzn <examples/disjunctive_es.mzn>` que se incluye en el modelo.

Si el sistema subyacente admite :mzn:`disjunctive` directamente, incluirá un archivo :download:`disjunctive_es.mzn <examples/disjunctive_es.mzn>` en su directorio global (con contenido solo la definición de firma anterior).


Si el sistema que estamos utilizando no es compatible directamente con disyuntivo, podemos dar nuestra propia definición creando el archivo :download:`disjunctive_es.mzn <examples/disjunctive_es.mzn>`.

La implementación más simple simplemente hace uso del predicado :mzn:`no_overlap` definido anteriormente.
Una mejor implementación es hacer uso de una restricción global :mzn:`cumulative` asumiendo que sea compatible con el solucionador subyacente. :numref:`ex-disj` muestra una implementación de :mzn:`disjunctive`.

Observe cómo usamos la función de reflexión :mzn:`index_set` para
(a) verificar que los argumentos para :mzn:`disjunctive` tengan sentido, y
(b) construir la matriz de utilizaciones de recursos del tamaño apropiado para :mzn:`cumulative`.
Tenga en cuenta también que utilizamos una versión ternaria :mzn:`assert` aquí.

.. literalinclude:: examples/disjunctive_es.mzn
  :language: minizinc
  :name: ex-disj
  :caption: Definir un predicado ``disjunctive`` utilizando ``cumulative`` (:download:`disjunctive_es.mzn <examples/disjunctive_es.mzn>`).

.. \ignore{ % for capture for testing!
.. $ mzn-g12fd jobshop3_es.mzn jobshop_es.dzn
.. } % $



Variables Locales
-----------------

.. index::
  single: variable; local
  single: let


A menudo es útil introducir *variables locales* en un predicado, función o prueba.
La expresión :mzn:`let` te permite hacerlo.
Se puede usar para introducir ambas decisiones :index:`variables <variable>` y :index:`parameters <parameter>`, pero los parámetros deben inicializarse. Por ejemplo:

.. code-block:: minizinc

  var s..e: x;
  let {int: l = s div 2; int: u = e div 2; var l .. u: y;} in x = 2*y

Introduce parámetros :mzn:`l`, :mzn:`u` y variable :mzn:`y`.

Si bien es más útil en :index:`predicate`, :index:`function` y en las definiciones de prueba, las expresiones :mzn:`let` también se pueden usar en otras expresiones, por ejemplo para eliminar subexpresiones comunes:

.. code-block:: minizinc

  constraint let { var int: s = x1 + x2 + x3 + x4 } in
             l <= s /\ s <= u;

Las variables locales se pueden usar en cualquier lugar y pueden ser bastante útiles para simplificar expresiones complejas.
:numref:`ex-wedding2` da una versión revisada del modelo de boda, usando variables locales para definir la función :index:`objective`, en lugar de agregar muchas variables al modelo explícitamente.

.. literalinclude:: examples/wedding2_es.mzn
  :language: minizinc
  :name: ex-wedding2
  :caption: Usar variables locales para definir una función objetivo compleja (:download:`wedding2_es.mzn <examples/wedding2_es.mzn>`).


Contexto
--------

.. index::
  single: context
  single: context; negative
  single: predicate
  single: function

Una limitación es que los predicados y las funciones que contienen variables de decisión que no se inicializan en la declaración no se pueden usar dentro de un contexto negativo.
Lo siguiente es ilegal:

.. code-block:: minizinc

  predicate even(var int:x) =
            let { var int: y } in x = 2 * y;

  constraint not even(z);


La razón de esto es que los solucionadores solo resuelven problemas existencialmente restringidos, y si introducimos una variable local en un contexto negativo, entonces la variable es *universalmente cuantificada*. Por lo tanto, fuera del alcance de los solucionadores subyacentes. Por ejemplo, :math:`\neg \mathit{even}(z)` es equivalente a :math:`\neg \exists y. z = 2y`, que es equivalente a :math:`\forall y. z \neq 2y`.

Si las variables locales reciben valores, entonces pueden usarse en contextos negativos. Lo siguiente es legal:


.. code-block:: minizinc

  predicate even(var int:x) =
            let { var int: y = x div 2; } in x = 2 * y;

  constraint not even(z);


Tenga en cuenta que el significado de :mzn:`even` es correcto, ya que si :mzn:`x` es par :math:`x = 2 * (x ~\mbox{div}~ 2)`.
Tenga en cuenta que para esta definición :math:`\neg \mathit{even}(z)` es equivalente a :math:`\neg \exists y. y = z ~\mbox{div}~ 2 \wedge z = 2y` que es equivalente a :math:`\exists y. y = z ~\mbox{div}~ 2 \wedge \neg z \neq 2y`, porque :math:`y` se define funcionalmente por :math:`z`.

Cada expresión en MiniZinc aparece en uno de los cuatro *contextos* :index:`root <context; !root>`, :index:`positive <context; !positive>`, :index:`negative <context; !negative>`, o :index:`mixed <context; !mixed>`.
El contexto de una expresión no booleana es simplemente el contexto de su expresión booleana más cercana. La única excepción es que la expresión objetivo aparece en un contexto raíz (ya que no tiene una expresión booleana adjunta).

Para los propósitos de definir contextos asumimos expresiones de implicación :mzn:`e1 -> e2` se reescriben de forma equivalente como :mzn:`not e1 \/ e2`, y de manera similar :mzn:`e1 <- e2` se reescribe como :mzn:`e1 \/ no e2`.

El contexto para una expresión booleana viene dado por:

Raíz:
El contexto raíz es el contexto para cualquier expresión $e$ que aparece como el argumento de :mzn:`constraint` o como un elemento :index:`assignment`, o que aparece como una subexpresión :mzn:`e1` o :mzn:`e2` en una expresión :mzn:`e1 /\ e2` que ocurre en un contexto raíz.

Las expresiones booleanas de contexto raíz deben mantenerse en cualquier modelo del problema.

Positivo:
Un contexto positivo es el contexto para cualquier expresión que aparece como una expresión secundaria :mzn:`e1` o :mzn:`e2` en una expresión :mzn:`e1 \/ e2` que ocurre en un contexto raíz o positivo, el cual aparece como una subexpresión :mzn:`e1` o :mzn:`e2` en una expresión :mzn:`e1 /\ e2` que aparece en un contexto positivo, o que aparece como una subexpresión :mzn:`e` en una expresión:mzn:`not e` que aparece en un contexto negativo.

Las expresiones booleanas de contexto positivo no necesitan mantenerse en un modelo, pero hacer que se sostengan solo aumentará la posibilidad de que se mantenga la restricción adjunta. Una expresión de contexto positiva tiene un número par de negaciones en la ruta desde el contexto raíz adjunto a la expresión.

Negativo:
El contexto negativo es el contexto para cualquier expresión que aparece como una subexpresión :mzn:`e1` o :mzn:`e2` en una expresión :mzn:`e1 \/ e2` o :mzn:`e1 /\ e2` que ocurre en un negativo contexto, o que aparece como una subexpresión :mzn:`e` en una expresión :mzn:`not e` que aparece en un contexto positivo.

Un contexto negativo es el contexto para cualquier expresión que aparezca como una expresión secundaria :mzn:`e1` o :mzn:`e2` en una expresión :mzn:`e1 \/ e2` o :mzn:`e1 /\ e2` que aparece en un contexto negativo, o que aparece como una subexpresión :mzn:`e` en una expresión :mzn:`not e` que aparece en un contexto positivo.

Las expresiones booleanas de contexto negativo no necesitan mantenerse en un modelo, pero al hacerlas falsas aumentará la posibilidad de que se mantenga la restricción de inclusión. Una expresión de contexto negativa tiene un número impar de negaciones en la ruta desde el contexto raíz adjunto a la expresión.

Mixto:
Un contexto mixto es el contexto para cualquier expresión booleana que aparece como una subexpresión :mzn:`e1` o :mzn:`e2` en :mzn:`e1 <-> e2`, :mzn:`e1 = e2`, o :mzn:`bool2int(e)`.

La expresión de contexto mixto es tanto positiva como negativa. Esto se puede ver por el hecho de que :mzn:`e1 <-> e2` es equivalente a :mzn:`(e1 /\ e2) \/ (not e1 /\ not e2)` y :mzn:`x = bool2int(e)` es equivalente a :mzn:`(e /\ x=1) \/ (not e /\ x=0)`.

Considera el fragmento de código

.. code-block:: minizinc

  constraint x > 0 /\ (i <= 4 -> x + bool2int(x > i) = 5);


Entonces :mzn:`x > 0` está en el contexto raíz, :mzn:`i <= 4}` está en un contexto negativo, :mzn:`x + bool2int(x > i) = 5` está en una posición positiva contexto, y :mzn:`x > i` está en un contexto mixto.


Restricciones locales
---------------------

.. index::
  single: constraint; local

Let expressions can also be used to include local constraints, usually to constrain the behaviour of local variables.
For example, consider defining a square root function making use of only multiplication:

Las expresiones ``let`` también se pueden usar para incluir restricciones locales, generalmente para restringir el comportamiento de las variables locales.
Por ejemplo, considere la definición de una función de raíz cuadrada haciendo uso de solo la multiplicación:

.. code-block:: minizinc

  function var float: mysqrt(var float:x) =
           let { var float: y;
                 constraint y >= 0;
                 constraint x = y * y; } in y;

Las restricciones locales aseguran que :mzn:`y` toma el valor correcto; que luego es devuelto por la función.

Las restricciones locales se pueden usar en cualquier expresión de let, aunque el uso más común es en la definición de funciones.


.. defblock:: Expresiones Let

  .. index::
    single: expression; let

  :index:`Local variables <variable;local>`

  Se puede introducir en cualquier expresión con *let expression* de la forma:

  .. code-block:: minizincdef

    let { <dec>; ... <dec> ; } in <exp>

  Las declaraciones :mzndef:`<dec>` pueden ser declaraciones de variables y parámetros de decisión (que deben inicializarse) o elementos de restricción.
  Ninguna declaración puede hacer uso de una variable recientemente declarada antes de ser presentada.

  Tenga en cuenta que las variables locales y las restricciones no pueden ocurrir en las pruebas.
  Las variables locales no pueden ocurrir en predicados o funciones que aparecen en un :index:`negative <context; negative>` o en un contexto :index:`mixed <context; mixed>`, a menos que la variable esté definida por una expresión.


Funciones de Reflexión del Dominio
----------------------------------

.. index::
  single: domain; reflection

Otras funciones de reflexión importantes son aquellas que nos permiten acceder a los dominios de las variables. La expresión :mzn:`lb(x)` devuelve un valor que es menor o igual a cualquier valor :mzn `x` que pueda tomar en una solución del problema. Por lo general, será el más bajo declarado :index:`bound <variable; bound>` de :mzn:`x`

Si :mzn:`x` se declara como un tipo no finito, por ejemplo, :mzn:`var int`, entonces es un error.

Similarly the expression :mzn:`dom(x)` returns a (non-strict) superset of the possible values of :mzn:`x` in any solution of the problem.

De nuevo, generalmente son los valores declarados, y de nuevo si no se declara como finito, entonces hay un error.

.. \ignore{ % for capture for testing!
.. $ mzn-g12fd reflection_es.mzn
.. } % $


.. literalinclude:: examples/reflection_es.mzn
  :language: minizinc
  :name: ex-reflect
  :caption: Usar predicados de reflexión (:download:`reflection_es.mzn <examples/reflection_es.mzn>`).


Por ejemplo, el modelo descrito en :numref:`ex-reflect` puede mostrar:

.. code-block:: none

  y = -10
  D = -10..10
  ----------

O

.. code-block:: none

  y = 0
  D = {0, 1, 2, 3, 4}
  ----------

O cualquier respuesta con :math:`-10 \leq y \leq 0` y :math:`\{0, \ldots, 4\} \subseteq D \subseteq \{-10, \ldots, 10\}`.

Las expresiones de reflexión de dominio variable deben usarse de manera tal que sean correctas para cualquier aproximación segura, ¡pero tenga en cuenta que esto no está verificado!


Por ejemplo, el código adicional

.. code-block:: minizinc

  var -10..10: z;
  constraint z <= y;

No es un uso seguro de la información de dominio.
Dado que el uso de la aproximación más ajustada (correcta) conduce a más soluciones que la aproximación inicial más débil.

.. TODO: this sounds wrong!

.. defblock:: Domain reflection

  .. index::
    single: domain; reflection

  Hay funciones de reflexión para interrogar los posibles valores de las expresiones que contienen variables:

  - :mzndef:`dom(<exp>)` devuelve una aproximación segura a los posibles valores de la expresión.
  - :mzndef:`lb(<exp>)` devuelve una aproximación segura al valor límite inferior de la expresión.
  - :mzndef:`ub(<exp>)` devuelve una aproximación segura al valor límite superior de la expresión.

  Las expresiones para :mzn:`lb` y :mzn:`ub` solo puede ser de tipos :mzn:`int`, :mzn:`bool`, :mzn:`float` o :mzn:`set of int`.
  Para :mzn:`dom` el tipo no puede ser :mzn:`float`.
  Si una de las variables que aparecen en :mzndef:`<exp>` tiene un :index:`non-finite declared type <type; non-finite>` (por ejemplo, :mzn:`var int` o :mzn:`var float`) entonces un error puede ocurrir.

  También hay versiones que funcionan directamente en matrices de expresiones (con restricciones similares):

  - :mzndef:`dom_array(<array-exp>)`: Devuelve una aproximación segura a la unión de todos los valores posibles de las expresiones que aparecen en la matriz.
  - :mzndef:`lb_array(<array-exp>)`: Devuelve una aproximación segura al límite inferior de todas las expresiones que aparecen en la matriz.
  - :mzndef:`ub_array(<array-exp>)`: Devuelve una aproximación segura al límite superior de todas las expresiones que aparecen en la matriz.

Las combinaciones de predicados, variables locales y reflexión de dominio permiten la definición de restricciones globales complejas por descomposición.
Podemos definir la descomposición basada en el tiempo de la restricción :mzn:`cumulative` utilizando el código que se muestra en :numref:`ex-cumul`.

.. literalinclude:: examples/cumulative_es.mzn
  :language: minizinc
  :name: ex-cumul
  :caption: Definiendo un predicado ``cumulative`` por descomposición (:download:`cumulative_es.mzn <examples/cumulative_es.mzn>`).

La descomposición usa :mzn:`lb` y :mzn:`ub` para determinar el conjunto de veces :mzn:`times` sobre las cuales las tareas podrían estar en rango.
A continuación, afirma para cada momento :mzn:`t` en :mzn:`times` que la suma de recursos para las tareas activas en el momento :mzn:`t` es menor que la límite :mzn:`b`.

Alcance
-------

.. index::
  single: scope

Vale la pena mencionar brevemente el alcance de las declaraciones en MiniZinc.
MiniZinc tiene un único espacio de nombre, por lo que todas las variables que aparecen en las declaraciones son visibles en cada expresión del modelo.

MiniZinc presenta variables con ámbito local de varias maneras:

- Como :index:`iterator <variable; iterator>` variables en expresiones :index:`comprehension`.
- Usando las expresiones :mzn:`let`.
- Como predicado y función :index:`arguments <argument>`

Cualquier variable de ámbito local eclipsa las variables de ámbito externo del mismo nombre.

.. literalinclude:: examples/scope_es.mzn
  :language: minizinc
  :name: ex-scope
  :caption: Un modelo para ilustrar alcances de variables (:download:`scope_es.mzn <examples/scope_es.mzn>`).

Por ejemplo, en el modelo que se muestra en :numref:`ex-scope` the :mzn:`x` en :mzn:`-x <= y` es el global :mzn:`x`, el :mzn:`x` en :mzn:`smallx (x)` es el iterador :mzn:`x in 1..u`, mientras que :mzn:`y` en la disyunción es el segundo argumento del predicado.
