.. _sec-efficient:

Prácticas de modelado efectivas en MiniZinc
===========================================

Existen casi siempre múltiples formas de modelar el mismo problema, algunas de las cuales generan modelos que son eficientes de resolver y otros que no son.
En general, es muy difícil determinar a priori qué modelos son los más eficientes para resolver un problema en particular, y de hecho puede depender críticamente del solver subyacente utilizado y de la estrategia de búsqueda. En este capítulo nos concentramos en las prácticas de modelado que evitan la ineficiencia en la generación de modelos y de los modelos generados.

Límites de las Variables
------------------------

.. index::
  single: variable; bound

Los motores de propagación de dominio finito, que son el principal tipo de solución objetivo de MiniZinc, son más efectivos cuanto más estrictos sean los límites de las variables involucradas. También pueden comportarse mal con problemas que tienen subexpresiones que toman valores enteros grandes, ya que pueden limitar implícitamente el tamaño de las variables enteras.

.. literalinclude:: examples/grocery_es.mzn
  :language: minizinc
  :name: ex-grocery
  :caption: Un modelo con variables no acotadas (:download:`grocery_es.mzn <examples/grocery_es.mzn>`).


El problema de comestibles que se muestra en :numref:`ex-grocery`, encuentra 4 elementos cuyos precios en dólares suman 7.11 y se multiplican hasta 7.11. Las variables son declaradas no acotadas. Corriendo

.. code-block:: bash

  $ mzn-g12fd grocery_es.mzn

Salida:

.. code-block:: none

  =====UNSATISFIABLE=====
  % grocery.fzn:11: warning: model inconsistency detected before search.

Esto se debe a que las expresiones intermedias en la multiplicación también son :mzn:`var int` y tienen límites predeterminados en el solver :math:`-1,000,000 \dots 1,000,000`, y estos rangos son demasiado pequeños para contener los valores que las expresiones intermedias puede necesitar tomar.

Modificar el modelo para que las variables se declaren con límites estrechos.

.. code-block:: minizinc

  var 1..711: item1;
  var 1..711: item2;
  var 1..711: item3;
  var 1..711: item4;

da como resultado un mejor modelo, ya que ahora MiniZinc puede inferir los límites en las expresiones intermedias y usar estos en lugar de los límites predeterminados. Con esta modificación, la ejecución del modelo da

.. code-block:: none

  {120,125,150,316}
  ----------

Sin embargo, tenga en cuenta que incluso el modelo mejorado puede ser demasiado difícil para algunos solucionadores.
Corriendo

.. code-block:: bash

  $ mzn-g12lazy grocery_es.mzn

no devuelve una respuesta, ya que el solucionador crea una gran representación para las variables intermedias del producto.

.. defblock:: Variables de Delimitación

  .. index::
    single: variable; bound

  Siempre trate de usar variables limitadas en los modelos.
  Al usar declaraciones :mzn:`let` para introducir nuevas variables, siempre intente definirlas con límites correctos y ajustados. Esto hará que su modelo sea más eficiente y evitará la posibilidad de desbordamientos inesperados.
  Una excepción es cuando introduce una nueva variable que se define inmediatamente como igual a una expresión. Por lo general, MiniZinc podrá inferir límites efectivos a partir de la expresión.

Variables sin restricciones
---------------------------

.. index::
  single: variable; unconstrained

A veces, cuando se modela, es más fácil introducir más variables de las que realmente se requieren para modelar el problema.

.. literalinclude:: examples/golomb_es.mzn
  :language: minizinc
  :name: ex-unc
  :caption: Un modelo para los gobernantes de Golomb con variables sin restricciones (:download:`golomb_es.mzn <examples/golomb_es.mzn>`).

Considere el modelo de reglas de Golomb que se muestra en :numref:`ex-unc`.
Una regla Golomb de :mzn:`n` marcas es una donde las diferencias absolutas entre cualesquiera de dos marcas son diferentes
Crea una matriz bidimensional de variables de diferencia, pero solo usa aquellas de la forma :mzn:`diff[i,j]` donde :mzn:`i > j`.
Ejecutando el modelo como

.. code-block:: bash

  $ mzn-g12fd golomb_es.mzn -D "n = 4; m = 6;"

resulta en la salida

.. code-block:: none

  mark = [0, 1, 4, 6];
  diffs = [0, 0, 0, 0, 1, 0, 0, 0, 4, 3, 0, 0, 6, 5, 2, 0];
  ----------

y todo parece estar bien con el modelo.

Pero si requerimos todas las soluciones utilizando:

.. code-block:: bash

  $ mzn-g12fd -a golomb_es.mzn -D "n = 4; m = 6;"

¡Se nos presenta una lista interminable de la misma solución!

¿Qué está pasando? Para que el solucionador de dominio finito termine, debe de  corregir todas las variables, incluidas las variables :mzn:`diff [i, j]` donde :mzn:`i <= j`, lo que significa que hay innumerables formas de generar un solución, simplemente cambiando estas variables para tomar valores arbitrarios.

We can avoid problems with unconstrained variables, by modifying the model so that they are fixed to some value. For example replacing the lines marked :mzn:`%(diff}` in :numref:`ex-unc` to

Podemos evitar problemas con variables no restringidas, modificando el modelo para que se fijen a algún valor. Por ejemplo en :numref:`ex-unc`, reemplazando las líneas marcadas como :mzn:`% (diff}` a

.. code-block:: minizinc

  constraint forall(i,j in 1..n)
                   (diffs[i,j] = if (i > j) then mark[i] - mark[j]
                                 else 0 endif);

asegura que las variables adicionales estén todas fijadas en 0. Con este cambio, el solucionador nos devuelve solo una solución.

MiniZinc eliminará automáticamente las variables que no están restringidas y no se utilizan en la salida. Una solución alternativa al problema anterior es simplemente eliminar la salida de la matriz :mzn:`diffs` cambiando la declaración de salida a

.. code-block:: minizinc

  output ["mark = \(mark);\n"];

Con este cambio funcionando

.. code-block:: bash

  $ mzn-g12fd -a golomb_es.mzn -D "n = 4; m = 6;"

Simplemente se traduce en

.. code-block:: none

  mark = [0, 1, 4, 6];
  ----------
  ==========

Ilustrando una solución única.


.. defblock:: Unconstrained Variables

  .. index::
    single: variable; unconstrained

  Los modelos nunca deben tener variables sin restricciones. Algunas veces es difícil modelar sin variables innecesarias. Si este es el caso, agregue restricciones para corregir las variables innecesarias, de modo que no puedan influir en la resolución.




Generadores efectivos
---------------------

.. index::
  single: generator

Imagine que queremos contar el número de triángulos (:math:`K_3` subgrafos) que aparecen en un grafo. Supongamos que el grafo está definido por una matriz de adyacencia: :mzn:`adj[i, j]` es verdadero si los nodos :mzn:`i` y :mzn:`j` son adyacentes.

Podríamos escribir

.. code-block:: minizinc

  int: count = sum ([ 1 | i,j,k in NODES where i < j  /\ j < k
                         /\ adj[i,j] /\ adj[i,k] /\ adj[j,k]]);

lo cual es ciertamente correcto, pero examina todos los triples de nodos.
Si el gráfico es escaso, podemos hacerlo mejor al darnos cuenta de que algunas pruebas se pueden aplicar tan pronto como seleccionamos :mzn:`i` y :mzn:`j`.

.. code-block:: minizinc

  int: count = sum( i,j in NODES where i < j /\ adj[i,j])
       (sum([1 | k in NODES where j < k /\ adj[i,k] /\ adj[j,k]]));

Puedes usar builitin :mzn:`trace` :index:`function <trace>` para ayudar determinar qué está sucediendo dentro de los generadores

.. defblock:: Rastreo (Tracing)

  La función :mzn:`trace(s,e)` imprime la cadena :mzn:`s` antes de evaluar la expresión :mzn:`e` y devuelve su valor.
  Se puede usar en cualquier contexto.

Por ejemplo, podemos ver cuántas veces se realiza la prueba en el interior bucle para ambas versiones del cálculo.

.. literalinclude:: examples/count1_es.mzn
  :language: minizinc
  :lines: 8-15

Produce el resultado:

.. code-block:: none

  ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  ----------

Indicando el bucle interno se evalúa 64 veces mientras

.. literalinclude:: examples/count2_es.mzn
  :language: minizinc
  :lines: 13-14

Produce el resultado:

.. code-block:: none

  ++++++++++++++++
  ----------

Indicando el bucle interno se evalúa 16 veces.

Tenga en cuenta que puede usar las cadenas dependientes en :mzn:`trace` para comprender lo que está sucediendo durante la creación del modelo.

.. literalinclude:: examples/count3_es.mzn
  :language: minizinc
  :lines: 13-15


imprimirá cada uno de los triángulos que se encuentran en el cálculo.

Produce la salida:

.. code-block:: none

  (1,2,3)
  ----------




Restricciones redundantes
-------------------------

.. index::
  single: constraint; redundant


La forma de un modelo afectará qué tan bien puede resolverlo el solucionador de restricciones.
En muchos casos, la adición de restricciones que son redundantes, es decir, están lógicamente implícitas en el modelo existente, puede mejorar la búsqueda de soluciones al hacer que el solucionador tenga más información antes.


Considere el problema de la serie mágica de :ref:`sec-complex`.
Ingresando para :mzn:`n = 16` de la siguiente manera:

.. code-block:: bash

  $ mzn-g12fd --all-solutions --statistics magic-series_es.mzn -D "n=16;"

puede resultar en la salida

.. code-block:: none

  s = [12, 2, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0];
  ----------
  ==========

y las estadísticas muestran 174 puntos de elección requeridos.

Podemos agregar restricciones redundantes al modelo. Como cada número en la secuencia cuenta el número de ocurrencias de un número, sabemos que suman :mzn:`n`. Del mismo modo, sabemos que la suma de :mzn:`s[i] * i` también debe sumar hasta :mzn:`n` porque la secuencia es mágica.

Agregar estas restricciones dadas al modelo en :numref:`ex-magic-series2`.

.. literalinclude:: examples/magic-series2_es.mzn
  :language: minizinc
  :name: ex-magic-series2
  :caption: Modelo que resuelve el problema de la serie mágica con restricciones redundantes (:download:`magic-series2_es.mzn <examples/magic-series2_es.mzn>`).

Ejecutando el mismo problema que antes

.. code-block:: bash

  $ mzn-g12fd --all-solutions --statistics magic-series2_es.mzn -D "n=16;"

da como salida el mismo resultado, pero con estadísticas que muestran solo 13 puntos de elección explorados. Las restricciones redundantes han permitido al solucionador podar la búsqueda mucho antes.


Opciones de modelado
--------------------

Hay muchas maneras de modelar el mismo problema en MiniZinc, aunque algunas pueden ser más naturales que otras.
Los diferentes modelos pueden tener una eficacia de resolución muy diferente, y lo que es peor, diferentes modelos pueden ser mejores o peores para los diferentes backends de resolución.
Sin embargo, hay algunas pautas para producir generalmente mejores modelos:

.. defblock:: Elegir entre Modelos

  El mejor modelo es probable que tenga algunas de las siguientes características:

  - Menor número de variables, o al menos aquellas que no están definidas funcionalmente por otras variables.
  - Tamaños de dominio más pequeños de variables.
  - Definición más sucinta o directa de las limitaciones del modelo.
  - Usar restricciones globales tanto como sea posible.

  En realidad, todo esto tiene que ser atenuado por cuán efectiva es la búsqueda para el modelo. Por lo general, la efectividad de la búsqueda es difícil de juzgar excepto por experimentación.

Considere el problema de encontrar permutaciones de :math:`n` números del 1 al :math:`n`, tal que las diferencias entre números adyacentes también forman una permutación de los números de 1 a :math:`n-1`.
Tenga en cuenta que las variables :mzn:`u` están definidas funcionalmente por las variables :mzn:`x`, por lo que el espacio de búsqueda sin formato es :math:`n^n`.
La forma obvia de modelar este problema se muestra en :numref:`ex-allint`.

.. literalinclude:: examples/allinterval_es.mzn
  :language: minizinc
  :name: ex-allint
  :caption: Un modelo natural para el problema de todas las series de intervalos ``prob007`` en CSPlib (:download:`allinterval_es.mzn <examples/allinterval_es.mzn>`).

En este modelo, la matriz :mzn:`x` representa la permutación de los números :mzn:`n` y las restricciones se representan naturalmente usando :mzn:`alldifferent`.

Ejecutando el modelo

.. code-block:: bash

  $ mzn-g12fd -all-solutions --statistics allinterval_es.mzn -D "n=10;"

Encuentra todas las soluciones en 84598 puntos de elección y 3s.

Un modelo alternativo usa una matriz :mzn:`y`, donde :mzn:`y[i]` da la posición del número :mzn:`i` en la secuencia.

También modelamos las posiciones de las diferencias usando variables :mzn:`v`.
:mzn:`v[i]` es la posición en la secuencia donde se produce la diferencia absoluta :mzn:`i`. Si los valores de :mzn:`y[i]` y :mzn:`y[j]` difieren en uno donde :mzn:`j > i`, lo que significa que las posiciones son adyacentes, entonces :mzn:`v[j-i]` es la restricción para ser el primero de estos puestos.

Podemos agregar dos restricciones redundantes a este modelo: dado que sabemos que debe producirse una diferencia de :mzn:`n-1`, sabemos que las posiciones de 1 y :mzn:`n` deben ser adyacentes (:mzn:`abs(y [1] - y [n]) = 1`), que también nos dice que la posición de la diferencia :mzn:`n-1` es la anterior de :mzn:`y [1]` y :mzn:`y[n]`, es decir :mzn:`v[n-1] = min (y [1], y [n])`.

Con esto podemos modelar el problema como se muestra en :numref:`ex-allint2`. La instrucción de salida recrea la secuencia original :mzn:`x` de la matriz de posiciones :mzn:`y`.

.. literalinclude:: examples/allinterval2_es.mzn
  :language: minizinc
  :name: ex-allint2
  :caption: Un modelo inverso para el problema de toda la serie de intervalos ``prob007`` en CSPlib (:download:`allinterval2_es.mzn <examples/allinterval2_es.mzn>`).

El modelo inverso tiene el mismo tamaño que el modelo original, en términos de cantidad de variables y tamaños de dominio. Pero el modelo inverso tiene una forma mucho más indirecta de modelar la relación entre las variables :mzn:`y` y :mzn:`v` en oposición a la relación entre las variables :mzn:`x` y :mzn:`u`.

Por lo tanto, podemos esperar que el modelo original sea mejor.

El comando

.. code-block:: bash

  $ mzn-g12fd --all-solutions --statistics allinterval2_es.mzn -D "n=10;"

Encuentra todas las soluciones en 75536 puntos de elección y 18s.

Curiosamente, aunque el modelo no es tan breve aquí, la búsqueda en las variables :mzn:`y` es mejor que buscar en las variables :mzn:`x`.
La falta de concisión significa que, aunque la búsqueda requiere menos opciones, es mucho más lenta.

.. _sec-multiple-modelling-and-channels:


Múltiples modelos y canales
---------------------------

Cuando tenemos dos modelos para el mismo problema, puede ser útil utilizar ambos modelos juntos al vincular las variables en los dos modelos, ya que cada uno puede proporcionar información diferente al solucionador.

.. literalinclude:: examples/allinterval3_es.mzn
  :language: minizinc
  :name: ex-allint3
  :caption: Un modelo dual para el problema de toda la serie de intervalos ``prob007`` en CSPlib (:download:`allinterval3_es.mzn <examples/allinterval3_es.mzn>`).

:numref:`ex-allint3` gives a dual model combining features of :download:`allinterval_es.mzn <examples/allinterval_es.mzn>` and :download:`allinterval2_es.mzn <examples/allinterval2_es.mzn>`.

El comienzo del modelo está tomado de :download:`allinterval_es.mzn <examples/allinterval_es.mzn>`.

Luego presentamos el :mzn:`y` y :mzn:`v` variables de :download:`allinterval2_es.mzn <examples/allinterval2_es.mzn>`.

Vinculamos las variables utilizando la restricción global :mzn:`inverse`, :mzn:`inverse(x,y)` si mantiene :mzn:`y` es la función inversa de :mzn:`x` (y vice versa), esto es :mzn:`x[i] = j <-> y[j] = i`. Una definición se muestra en :numref:`ex-inverse`.

El modelo no incluye las restricciones que relacionan las variables :mzn:`y` y :mzn:`v`, son redundantes (y de hecho la propagación es redundante) por lo que no agregan información para un solucionador de propagación.

Las restricciones :mzn:`alldifferent` también faltan porque se vuelven redundantes (y la propagación es redundante) por las restricciones inversas.

Las únicas restricciones son las relaciones de las variables :mzn:`x` y :mzn:`u` y las restricciones redundantes en :mzn:`y` y :mzn:`v`.


.. literalinclude:: examples/inverse_es.mzn
  :language: minizinc
  :name: ex-inverse
  :caption: Una definición de la restricción global ``inverse`` (:download:`inverse_es.mzn <examples/inverse_es.mzn>`).

Uno de los beneficios del modelo dual es que hay más posibilidades para definir diferentes estrategias de búsqueda.

Ejecutando el modelo dual,

.. code-block:: bash

  $ mzn-g12fd -all-solutions --statistics allinterval3_es.mzn -D "n=10;"

Que usa la estrategia de búsqueda del modelo inverso, etiquetando las variables :mzn:`y`, encuentra todas las soluciones en 1714 puntos de elección y 0.5s.

Tenga en cuenta que ejecutar el mismo modelo con etiquetado en las variables :mzn:`x` requiere 13142 puntos de elección y 1.5s.


Simetría
--------

La simetría es muy común en problemas de satisfacción y optimización de restricciones. Para ilustrar esto, veamos nuevamente el problema n-queens en :numref:`ex-queens`. El tablero de ajedrez superior izquierdo en :numref:`fig-queens-sym` muestra una solución a los problemas de 8 reinas (etiquetada como original). Los tableros de ajedrez restantes muestran siete variantes simétricas de la misma solución: rotados por 90, 180 y 270 grados, y volteados verticalmente.

.. _fig-queens-sym:

.. figure:: figures/queens_symm_es.*

  Variantes simétricas de una solución de 8 reinas.

Si quisiéramos enumerar *todas* las soluciones del problema de las 8 reinas, obviamente podríamos ahorrarle algo de trabajo al solucionador enumerando solo las soluciones *no simétricas* y luego generando las variantes simétricas nosotros mismos. Esta es una de las razones por las que queremos deshacernos de la simetría en los modelos de restricción. La otra razón, mucho más importante, es que el solucionador también puede **explorar variantes simétricas de estados sin solución**.

Por ejemplo, un solucionador de restricciones típico puede tratar de colocar a la reina en la columna 1 en la fila 1 (lo cual está bien), y luego tratar de poner la reina en la columna 2 y en la fila 3, que, a primera vista, no viola ninguno de los restricciones Sin embargo, esta configuración no se puede completar con una solución completa (que el solucionador descubre después de una pequeña búsqueda). :numref:`fig-queens-sym-unsat` muestra esta configuración en el tablero de ajedrez superior izquierdo. Ahora nada impide que el solucionador intente, por ejemplo, la segunda configuración desde la izquierda en la fila inferior de :numref:`fig-queens-sym-unsat`, donde la reina en la columna 1 todavía está en la fila 1, y la reina en la columna 3 se coloca en la fila 2. Por lo tanto, incluso cuando solo se busca una solución única, el solucionador puede explorar muchos estados simétricos que ya ha visto y probado como insatisfactorios.

.. _fig-queens-sym-unsat:

.. figure:: figures/queens_symm_unsat.*

  Symmetric variants of an 8-queens unsatisfiable partial assignment


Rompiendo la simetría estática
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

La técnica de modelado para tratar con simetría se llama *ruptura de simetría*, y en su forma más simple, implica agregar restricciones al modelo que descarta todas las variantes simétricas de una asignación (parcial) a las variables excepto a uno. Estas restricciones se llaman *restricciones de ruptura de simetría estática*.

La idea básica detrás de la ruptura de simetría es imponer un *orden*. Por ejemplo, para descartar cualquier giro vertical del tablero de ajedrez, simplemente podríamos agregar la restricción de que la dama en la primera columna debe estar en la mitad superior del tablero:

.. code-block:: minizinc

  constraint q[1] <= n div 2;

Convencete de que esto eliminaría exactamente la mitad de las variantes simétricas en :numref:`fig-queens-sym`. Para eliminar *toda* la simetría, tenemos que trabajar un poco más duro.

Siempre que podamos expresar todas las simetrías como permutaciones de la matriz de variables, se puede usar un conjunto de *restricciones de ordenamiento lexicográfico* para romper toda la simetría. Por ejemplo, si la matriz de variables se llama :mzn:`x`, e invertir la matriz es una simetría del problema, la siguiente restricción romperá esa simetría:

.. code-block:: minizinc

  constraint lex_lesseq(x, reverse(x));

¿Qué hay de arreglos bidimensionales? El orden lexicográfico funciona de la misma manera, solo tenemos que forzar las matrices en una dimensión. Por ejemplo, lo siguiente rompe la simetría de voltear el conjunto a lo largo de una de las diagonales (observe los índices intercambiados en la segunda comprensión):

.. code-block:: minizinc

  array[1..n,1..n] of var int: x;
  constraint lex_lesseq([ x[i,j] | i,j in 1..n ], [ x[j,i] | i,j in 1..n ]);

Lo mejor del uso de restricciones de ordenamiento lexicográfico es que podemos agregar múltiples (para romper varias simetrías simultáneamente), sin que interfieran entre sí, siempre que mantengamos el mismo orden en el primer argumento.

Para el problema de n-queens, lamentablemente esta técnica no se aplica de inmediato, porque algunas de sus simetrías no se pueden describir como permutaciones de la matriz :mzn:`q`. El truco para superar esto es expresar el problema de n-queens en términos de variables booleanas que modelan, para cada campo del tablero, si contiene una reina o no.

Ahora todas las simetrías se pueden modelar como permutaciones de esta matriz. Dado que las principales restricciones del problema n-queens son mucho más fáciles de expresar con una matriz entera mzn:`q`, simplemente usamos ambos modelos juntos y agregamos restricciones de canal entre ellos, como se explica en :ref:`sec-multiple-modelling-and-channels`.

El modelo completo, con variables booleanas añadidas, restricciones de canalización y restricciones de ruptura de simetría se muestra en :numref:`ex-queens-sym`. Podemos realizar un pequeño experimento para verificar si rompe con éxito toda la simetría. Intente ejecutar el modelo con valores crecientes para :mzn:`n`. Ejemplo, desde 1 a 10, contando el número de soluciones (por ejemplo, utilizando el indicador ``-s`` con el solucionador Gecode, o seleccionando "Imprimir todas las soluciones", así como también "Estadísticas para resolver" en el IDE). Debe obtener la siguiente secuencia de números de soluciones: 1, 0, 0, 1, 2, 1, 6, 12, 46, 92. Para verificar la secuencia, puede buscarla en la *Enciclopedia en línea de Secuencias Enteras* (http://oeis.org).

.. literalinclude:: examples/nqueens_sym_es.mzn
  :language: minizinc
  :name: ex-queens-sym
  :start-after: % Modelo booleano alternativo:
  :end-before: % Búsqueda.
  :caption: Modelo parcial para n-reinas con ruptura de simetría (full model: :download:`nqueens_sym_es.mzn <examples/nqueens_sym_es.mzn>`).


Otros ejemplos de simetría
~~~~~~~~~~~~~~~~~~~~~~~~~~

Muchos otros problemas tienen simetrías inherentes, y romperlos a menudo puede hacer una diferencia significativa en la resolución del rendimiento. Aquí hay una lista de algunos casos comunes:

- Embalaje del contenedor (Bin packing) : Cuando se intenta empacar elementos en contenedores, los dos contenedores que tienen la misma capacidad son simétricos.
- Coloreado de gráficos (Graph colouring) : Cuando intentamos asignar colores a nodos en un grafo de manera que los nodos adyacentes deben tener diferentes colores. Generalmente modelamos los colores como números enteros. Sin embargo, cualquier permutación de colores es nuevamente un grafo de coloración válida.

- Enrutamiento de vehículos (Vehicle routing) : Si la tarea es asignar clientes a ciertos vehículos, dos vehículos con la misma capacidad pueden ser simétricos (esto es similar al ejemplo de empaque de contenedores).

- Nómina/Lista de horarios (Rostering/time tabling): Dos miembros del personal con el mismo conjunto de habilidades pueden ser intercambiables, al igual que dos salas con la misma capacidad o equipamiento técnico.
