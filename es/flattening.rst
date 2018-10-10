.. _sec-flattening:

FlatZinc y Flattening
=======================

.. \pjs{Maybe show the toolset at this point?}


Los solucionadores de restricciones no son compatibles directamente con los modelos MiniZinc. Sino que para ejecutar un modelo MiniZinc se debe de traducir en un subconjunto simple de MiniZinc llamado FlatZinc. FlatZinc refleja el hecho de que la mayoría de los solucionadores de restricciones solo resuelven problemas de satisfacción de la forma :math:`\bar{exists } c_1 \wedge \cdots \wedge c_m` o problemas de optimización de la forma :math:`\text {minimize} z \text{ sujeto a } c_1 \wedge \cdots \wedge c_m`,
donde :math:`c_i` son restricciones primitivas y :math:`z` es un entero o expresión flotante en una forma restringida.

.. index::
  single: mzn2fzn


La herramienta ``mzn2fzn`` toma un modelo de MiniZinc y los archivos de datos, posteriormente crea un modelo FlatZinc aplanado que es equivalente al modelo MiniZinc con los datos dados, y que aparece en la forma restringida discutida anteriormente.
Normalmente, la construcción de un modelo de FlatZinc que se envía a un solucionador está oculta para el usuario, pero puede ver el resultado de un modelo ``model.mzn`` con sus datos ``data.dzn`` de la siguiente manera:

.. code-block:: bash

  mzn2fzn model.mzn data.dzn

El cual crea un modelo FlatZinc llamado ``model.fzn``.

En este capítulo, exploramos el proceso de traducción de MiniZinc a FlatZinc.



Flattening Expressiones
-----------------------

Las restricciones del solucionador subyacente significan que las expresiones complejas en MiniZinc deben *flattened* (aplanarse o aplanar) para usar solo conjunciones de restricciones primitivas que no contienen por sí mismas términos estructurados.

Considere el siguiente modelo para asegurarse de que dos círculos en un recuadro rectangular no se superpongan:

.. literalinclude:: examples/cnonoverlap_es.mzn
  :language: minizinc
  :caption: Modelado de no superposición de dos círculos (:download:`cnonoverlap_es.mzn <examples/cnonoverlap_es.mzn>`).
  :name: fig-nonoverlap


Simplificación y Evaluación
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Dado el siguiente archivo de datos:

.. code-block:: minizinc

  width = 10.0;
  height = 8.0;
  r1 = 2.0;
  r2 = 3.0;

La traducción a FlatZinc primero simplifica el modelo reemplazando todos los parámetros por sus valores y evaluando cualquier expresión fija.
Después de esta simplificación, los valores de los parámetros ya no son necesarios.
Una excepción a esto, son las grandes matrices de valores paramétricos. Si se usan más de una vez, el parámetro se conserva para evitar la duplicación expresiones grandes.


Después de la simplificación, las partes de declaraciones de variables y parámetros del modelo de :numref:`fig-nonoverlap` se transforman en

.. literalinclude:: examples/cnonoverlap_es.fzn
  :language: minizinc
  :start-after: % Variables
  :end-before: %

.. _sec-flat-sub:



Definiendo Subexpresiones
~~~~~~~~~~~~~~~~~~~~~~~~~

Ahora, ningún solucionador de restricciones maneja directamente expresiones de restricciones complejas como la de :numref:`fig-nonoverlap`. En cambio, cada subexpresión en la expresión es nombrada, y posteriormente creamos una restricción para construir el valor de cada expresión. Examinemos las subexpresiones de la expresión de restricción.
:mzn:`(x1 - x2)` es una subexpresión, si nombramos :mzn:`FLOAT01` podemos definirlo como :mzn:`constraint FLOAT01 = x1 - x2;`. Observe que esta expresión aparece dos veces en el modelo. Solo necesitamos calcular el valor una vez, luego podemos reutilizarlo. Esto se llama *eliminación de subexpresiones comunes*. La subexpresión :mzn:`(x1 - x2) * (x1 - x2)` se puede llamar :mzn:`FLOAT02` y podemos definirla como :mzn:`constraint FLOAT02 = FLOAT01 * FLOAT01;`. Podemos nombrar de forma similar :mzn:`restricción FLOAT03 = y1 - y2;` y :mzn:`restricción FLOAT04 = FLOAT03 * FLOAT03;` y finalmente :mzn:`restricción FLOAT05 = FLOAT02 * FLOAT04;`. La restricción de desigualdad se convierte en :mzn:`restricción FLOAT05> = 25.0;` desde :mzn:`(r1 + r2) * (r1 + r2)` se calcula como :mzn:`25.0`.

La restricción aplanada es por lo tanto:

.. code-block:: minizinc

  constraint FLOAT01 = x1 - x2;
  constraint FLOAT02 = FLOAT01 * FLOAT01;
  constraint FLOAT03 = y1 - y2;
  constraint FLOAT04 = FLOAT03 * FLOAT03;
  constraint FLOAT05 = FLOAT02 * FLOAT04;
  constraint FLOAT05 >= 25.0

.. _sec-flat-fzn:


Forma de restricciones FlatZinc
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

El flatening como su paso final, convierte la forma de la restricción en una forma estándar de FlatZinc que siempre es :math:`p(a_1, \ldots, a_n)`, donde
:mzn:`p` es el nombre de la restricción primitiva y :math:`a_1, \ldots, a_n` son los argumentos.
FlatZinc intenta usar un mínimo de diferentes formas de restricción, por ejemplo, la restricción :mzn:`FLOAT01 = x1 - x2` se reescribe primero como :mzn:`FLOAT01 + x2 = x1` y luego se genera usando la restricción primitiva:mzn:`float_plus`.
La forma de restricción resultante es la siguiente:

.. literalinclude:: examples/cnonoverlap_es.fzn
  :language: minizinc
  :start-after: % Restricciones
  :end-before: %

Análisis de límites
~~~~~~~~~~~~~~~~~~~

Todavía nos falta una cosa, las declaraciones de las variables introducidas :mzn:`FLOAT01`, ..., :mzn:`FLOAT05`. Si bien estos podrían simplemente declararse como :mzn:`var float`, para facilitar la tarea del solucionador MiniZinc intenta determinar los límites superiores e inferiores de las variables recién introducidas, mediante un simple análisis de límites.
Por ejemplo desde :mzn:`FLOAT01 = x1 - x2` y :math:`2.0 \leq` :mzn:`x1` :math:`\leq 8.0` y :math:`3.0 \leq` :mzn:`x2`
:math:`\leq 7.0` luego podemos ver que
:math:`-5.0 \leq` :mzn:`FLOAT0` :math:`\leq 5.0`. Dada esta información, podemos ver que
:math:`-25.0 \leq` :mzn:`FLOAT02` :math:`\leq 25.0` (aunque tenga en cuenta que si reconociéramos que la multiplicación era en realidad una cuadratura podríamos dar mucho límites más precisos :math:`0.0 \leq` :mzn:`FLOAT02` :math:`\leq 25.0`).

El lector alerta puede haber notado una discrepancia entre la forma aplanada de las restricciones en :ref:`sec-flat-sub` y :ref:`sec-flat-fzn`. En este último, no hay restricción de desigualdad. Como las desigualdades unarias pueden representarse completamente por los límites de una variable, la desigualdad fuerza al límite inferior de :mzn:`FLOAT05` a ser :mzn:`25.0` y luego es redundante. La forma aplanada final del modelo de :numref:`fig-nonoverlap` es:

.. literalinclude:: examples/cnonoverlap_es.fzn
  :language: minizinc

Objetivos
~~~~~~~~~~

MiniZinc aplana los objetivos de minimización o maximización al igual que las restricciones. La expresión objetivo se aplana y se crea una variable para ella, al igual que para otras expresiones. En la salida de FlatZinc, el elemento de resolver siempre está en una sola variable. Ver :ref:`sec-let` para un ejemplo.

.. \pjs{Do we need an example here?}

Expresiones lineales
--------------------

Una de las restricciones más importantes, ampliamente utilizada para el modelado, son las restricciones lineales de la forma

.. math:: a_1 x_1 + \cdots + a_n x_n \begin{array}[c]{c} = \\ \leq \\ < \end{array} a_0

En donde :math:`a_i` son constantes de coma flotante o entero, y :math:`x_i` son variables enteras o de coma flotante. Son altamente expresivos, y son la única clase de restricción soportada por solucionadores de restricciones de programación lineal (entero). El traductor de MiniZinc a FlatZinc intenta crear restricciones lineales, en lugar de dividir las restricciones lineales en muchas subexpresiones.

.. \pjs{Maybe use the equation from SEND-MORE-MONEY instead?}

.. literalinclude:: examples/linear_es.mzn
  :language: minizinc
  :caption: Un modelo MiniZinc para ilustrar el aplanamiento de restricciones lineales (:download:`linear_es.mzn <examples/linear_es.mzn>`).
  :name: fig-lflat

Considere el modelo que se muestra en :numref:`fig-lflat`. En lugar de crear variables para todas las subexpresiones :math:`3 * x`, :math:`3 * x - y`, :math:`x * z`, :math:`3 * x - y + x * z `, :math:`x + y + z`, :math:`d * (x + y + z)`, :math:`19 + d * (x + y + z)`, y :math:`19 + d * (x + y + z) - 4 * d` la traducción intentará crear una restricción lineal grande que capture la mayor cantidad posible de la restricción en una única restricción FlatZinc.


El flatening crea expresiones lineales como una sola unidad en lugar de generar variables intermedias para cada subexpresión. También simboliza la expresión lineal creada. La extracción de la expresión lineal de las restricciones conduce a:

.. code-block:: minizinc

  var 0..80: INT01;
  constraint 4*x + z + INT01 <= 23;
  constraint INT01 = x * z;

Observe cómo la *expresión no lineal* :math:`x \times z` se extrae como una nueva subexpresión y se le da un nombre. Mientras que los términos restantes se recopilan juntos para que cada variable aparezca exactamente una vez (y de hecho variable :math:`y` cuyos términos cancelar se eliminan).

Finalmente, cada restricción se escribe en la forma FlatZinc obteniendo:

.. code-block:: minizinc

  var 0..80: INT01;
  constraint int_lin_le([1,4,1],[INT01,x,z],23);
  constraint int_times(x,z,INT01);

.. _sec-unroll:



Desenrollar expresiones
-----------------------

La mayoría de los modelos requieren la creación de una serie de restricciones que dependen de los datos de entrada. MiniZinc admite estos modelos con tipos de matriz, listas y conjuntos de comprensiones y funciones de agregación.

Considere la siguiente expresión desde el ejemplo de programación de planificación :numref:`ex-prod-planning`.

.. code-block:: minizinc

  int: mproducts = max (p in Products)
                       (min (r in Resources where consumption[p,r] > 0)
                                       (capacity[r] div consumption[p,r]));

Como esto utiliza la sintaxis de llamada del generador, podemos reescribirla en forma equivalente, que es procesada por ``mzn2fzn``:

.. code-block:: minizinc

  int: mproducts = max([ min [ capacity[r] div consumption[p,r]
                             | r in Resources where consumption[p,r] > 0])
                       | p in Products]);

Teniendo en cuenta los datos

.. code-block:: minizinc

  nproducts = 2;
  nresources = 5;
  capacity = [4000, 6, 2000, 500, 500];
  consumption= [| 250, 2, 75, 100, 0,
                | 200, 0, 150, 150, 75 |];

Esto primero construye la matriz de :mzn:`p = 1`

.. code-block:: minizinc

  [ capacity[r] div consumption[p,r]
                             | r in 1..5 where consumption[p,r] > 0]

Que es :mzn:`[16, 3, 26, 5]` y luego calcula el mínimo como 3.
Luego construye la misma matriz para :mzn:`p = 2` que es :mzn:`[20, 13, 3, 6]` y calcula el mínimo como 3. Luego construye la matriz :mzn:`[3, 3]` y calcula el máximo como 3. No hay representación de :mzn:`mproducts` en la salida FlatZinc, esta evaluación se usa simplemente para reemplazar :mzn:`mproducts` por el valor calculado 3.

La forma más común de expresión agregada en un modelo de restricción es :mzn:`forall`. Todas las expresiones se desenrollan en múltiples restricciones.

Considere el siguiente fragmento de MiniZinc

.. code-block:: minizinc

  array[1..8] of var 0..9: v = [S,E,N,D,M,O,R,Y];
  constraint forall(i,j in 1..8 where i < j)(v[i] != v[j])

Que surge del ejemplo SEND-MORE-MONEY de :numref:`ex-smm` utilizando una descomposición predeterminada para :mzn:`alldifferent`.
La expresión :mzn:`forall` crea una restricción para cada par :math:`i, j` que cumple con el requisito :math:`i <j`, creando así:

.. code-block:: minizinc

  constraint v[1] != v[2]; % S != E
  constraint v[1] != v[3]; % S != N
  ...
  constraint v[1] != v[8]; % S != Y
  constraint v[2] != v[3]; % E != N
  ...
  constraint v[7] != v[8]; % R != Y

En la forma de FlatZinc esto es:

.. code-block:: minizinc

  constraint int_neq(S,E);
  constraint int_neq(S,N);
  ...
  constraint int_neq(S,Y);
  constraint int_neq(E,N);
  ...
  constraint int_neq(R,Y);

Observe cómo las variables temporales del arreglo :mzn:`v[i]` son reemplazadas por las variables originales en el resultado FlatZinc.


Arreglos
--------

Los arrays unidimensionales en MiniZinc pueden tener índices arbitrarios siempre que sean enteros contiguos. En FlatZinc todas las matrices están indexadas desde :mzn:`1..l` donde :mzn:`l` es la longitud de la matriz. Esto significa que las búsquedas de matriz deben traducirse a la vista de índices de FlatZinc.

Consider the following MiniZinc model for balancing a seesaw of length :mzn:`2 * l2`, with a child of weight :mzn:`cw` kg using exactly :mzn:`m` 1kg weights.

Considere el siguiente modelo MiniZinc para equilibrar un balance de longitud :mzn:`2 * l2`, con un pequeño de peso :mzn:`cw` kg usando exactamente un peso :mzn:`m` de 1kg.

.. code-block:: minizinc

  int: cw;                                    % Pequeño peso
  int: l2;                                    % La mitad de la longitud del balance
  int: m;                                     % Cantidad de 1 kg de peso
  array[-l2..l2] of var 0..max(m,cw): w;      % Peso en cada punto
  var -l2..l2: p;                             % Posición del peso.
  constraint sum(i in -l2..l2)(i * w[i]) = 0; % Balance
  constraint sum(i in -l2..l2)(w[i]) = m + cw;% Todos los pesos usados.
  constraint w[p] = cw;                       % El pequeño peso está en la posición p
  solve satisfy;

Dado :mzn:`cw = 2`, :mzn:`l2 = 2`, y :mzn:`m = 3` el desenrollado produce las restricciones

.. code-block:: minizinc

  array[-2..2] of var 0..3: w;
  var -2..2: p
  constraint -2*w[-2] + -1*w[-1] + 0*w[0] + 1*w[1] + 2*w[2] = 0;
  constraint w[-2] + w[-1] + w[0] + w[1] + w[2] = 5;
  constraint w[p] = 2;

Pero FlatZinc insiste en que la matriz :mzn:`w` comienza en el índice 1.
Esto significa que necesitamos reescribir todos los accesos a la matriz para usar el nuevo valor de índice. Para las búsquedas de arreglos fijos esto es fácil, para las búsquedas de arreglos variables, podemos necesitar crear una nueva variable. El resultado para las ecuaciones anteriores es

.. code-block:: minizinc

  array[1..5] of var 0..3: w;
  var -2..2: p
  var 1..5: INT01;
  constraint -2*w[1] + -1*w[2] + 0*w[3] + 1*w[4] + 2*w[5] = 0;
  constraint w[1] + w[2] + w[3] + w[4] + w[5] = 5;
  constraint w[INT01] = 2;
  constraint INT01 = p + 3;

Finalmente reescribimos las restricciones en forma de FlatZinc. Observe cómo la búsqueda del índice de matriz variable está mapeada a :mzn:`array_var_int_element`.

.. code-block:: minizinc

  array[1..5] of var 0..3: w;
  var -2..2: p
  var 1..5: INT01;
  constraint int_lin_eq([2, 1, -1, -2], [w[1], w[2], w[4], w[5]], 0);
  constraint int_lin_eq([1, 1, 1, 1, 1], [w[1],w[2],w[3],w[4],w[5]], 5);
  constraint array_var_int_element(INT01, w, 2);
  constraint int_lin_eq([-1, 1], [INT01, p], -3);

Las matrices multidimensionales son compatibles con MiniZinc, pero FlatZinc solo soporta las matrices de una sola dimensión (en este momento).
Esto significa que las matrices multidimensionales se deben asignar a matrices de una dimensión, y el acceso de matriz multidimensional se debe asignar al acceso a una matriz de una dimensión.

Considere las restricciones de ecuación de Laplace definidas para un modelo de placa de elementos finitos en :numref:`ex-laplace`:

.. literalinclude:: examples/laplace_es.mzn
  :language: minizinc
  :start-after: % Array declaration.
  :end-before: % Lados.

Asumiendo :mzn:`w = 4` y :mzn:`h = 4` esto crea las restricciones:

.. code-block:: minizinc

  array[0..4,0..4] of var float: t; % temperatura en el punto (i,j)
  constraint 4.0*t[1,1] = t[0,1] + t[1,0] + t[2,1] + t[1,2];
  constraint 4.0*t[1,2] = t[0,2] + t[1,1] + t[2,2] + t[1,3];
  constraint 4.0*t[1,3] = t[0,3] + t[1,2] + t[2,3] + t[1,4];
  constraint 4.0*t[2,1] = t[1,1] + t[2,0] + t[3,1] + t[2,2];
  constraint 4.0*t[2,2] = t[1,2] + t[2,1] + t[3,2] + t[2,3];
  constraint 4.0*t[2,3] = t[1,3] + t[2,2] + t[3,3] + t[2,4];
  constraint 4.0*t[3,1] = t[2,1] + t[3,0] + t[4,1] + t[3,2];
  constraint 4.0*t[3,2] = t[2,2] + t[3,1] + t[4,2] + t[3,3];
  constraint 4.0*t[3,3] = t[2,3] + t[3,2] + t[4,3] + t[3,4];

La matriz bidimensional de 25 elementos se convierte en una matriz unidimensional y los índices :mzn:`[i, j]` se convierte en :mzn:`[i * 5 + j + 1]`.

.. code-block:: minizinc

  array [1..25] of var float: t;
  constraint 4.0*t[7] = t[2] + t[6] + t[12] + t[8];
  constraint 4.0*t[8] = t[3] + t[7] + t[13] + t[9];
  constraint 4.0*t[9] = t[4] + t[8] + t[14] + t[10];
  constraint 4.0*t[12] = t[7] + t[11] + t[17] + t[13];
  constraint 4.0*t[13] = t[8] + t[12] + t[18] + t[14];
  constraint 4.0*t[14] = t[9] + t[13] + t[19] + t[15];
  constraint 4.0*t[17] = t[12] + t[16] + t[22] + t[18];
  constraint 4.0*t[18] = t[13] + t[17] + t[23] + t[19];
  constraint 4.0*t[19] = t[14] + t[18] + t[24] + t[20];


Reificación
-----------

.. index::
  single: reification

Los modelos FlatZinc implican solo variables y declaraciones de parámetros y una serie de restricciones primitivas. Por lo tanto, cuando modelamos en MiniZinc con conectivas booleanas distintas a la conjunción, algo tiene que hacerse. El enfoque principal para manejar fórmulas complejas que usan conectivos que no sean la conjunción es por *reificación*. Reificar una restricción :math:`c` crea una nueva restricción equivalente a :math:`b \leftrightarrow c` donde la variable booleana :math:`b` es :mzn:`true` si la restricción se cumple y :mzn:`false` si no se sostiene.

Una vez que tenemos la capacidad de *reificar* las restricciones, el tratamiento de fórmulas complejas no es diferente de las expresiones aritméticas. Creamos un nombre para cada subexpresión y una restricción plana para restringir el nombre y tomar el valor de su subexpresión.

Considere la siguiente expresión de restricción que ocurre en el ejemplo de programación de jobshop de :numref:`ex-jobshop`.




.. code-block:: minizinc

  constraint %% Asegurar que no haya superposición de tareas
      forall(j in 1..tasks) (
          forall(i,k in 1..jobs where i < k) (
              s[i,j] + d[i,j] <= s[k,j] \/
              s[k,j] + d[k,j] <= s[i,j]
      ) );

Dado el archivo de datos:

.. code-block:: minizinc

  jobs = 2;
  tasks = 3;
  d = [| 5, 3, 4 | 2, 6, 3 |]

Entonces el desenrollar crea:

.. code-block:: minizinc

  constraint s[1,1] + 5 <= s[2,1] \/ s[2,1] + 2 <= s[1,1];
  constraint s[1,2] + 3 <= s[2,2] \/ s[2,2] + 6 <= s[1,2];
  constraint s[1,3] + 4 <= s[2,3] \/ s[2,3] + 3 <= s[1,3];

La reificación de las restricciones que aparecen en la disyunción crea nuevas variables booleanas para definir los valores de cada expresión.

.. code-block:: minizinc

  array[1..2,1..3] of var 0..23: s;
  constraint BOOL01 <-> s[1,1] + 5 <= s[2,1];
  constraint BOOL02 <-> s[2,1] + 2 <= s[1,1];
  constraint BOOL03 <-> s[1,2] + 3 <= s[2,2];
  constraint BOOL04 <-> s[2,2] + 6 <= s[1,2];
  constraint BOOL05 <-> s[1,3] + 4 <= s[2,3];
  constraint BOOL06 <-> s[2,3] + 3 <= s[1,3];
  constraint BOOL01 \/ BOOL02;
  constraint BOOL03 \/ BOOL04;
  constraint BOOL05 \/ BOOL06;

Cada restricción primitiva ahora se puede asignar a la forma FlatZinc.
Observe cómo la matriz de dos dimensiones :mzn:`s` está mapeada a una forma unidimensional.
Cada restricción se puede asignar a la forma FlatZinc.
Observe cómo la matriz de dos dimensiones :mzn:`s` está mapeada a una forma unidimensional.

.. code-block:: minizinc

  array[1..6] of var 0..23: s;
  constraint int_lin_le_reif([1, -1], [s[1], s[4]], -5, BOOL01);
  constraint int_lin_le_reif([-1, 1], [s[1], s[4]], -2, BOOL02);
  constraint int_lin_le_reif([1, -1], [s[2], s[5]], -3, BOOL03);
  constraint int_lin_le_reif([-1, 1], [s[2], s[5]], -6, BOOL04);
  constraint int_lin_le_reif([1, -1], [s[3], s[6]], -4, BOOL05);
  constraint int_lin_le_reif([-1, 1], [s[3], s[6]], -3, BOOL06);
  constraint array_bool_or([BOOL01, BOOL02], true);
  constraint array_bool_or([BOOL03, BOOL04], true);
  constraint array_bool_or([BOOL05, BOOL06], true);

El :mzn:`int_lin_le_reif` es la forma reificada de la restricción lineal :mzn:` int_lin_le`.


La mayoría de las restricciones primitivas de FlatZinc :math:`p(\bar{x})` tiene una forma reificada :math:`\mathit{p\_reif}(\bar{x},b)` que toma un argumento final adicional :math:`b` y define la restricción :math:`b \leftrightarrow p(\bar{x})`. Las restricciones primitivas de FlatZinc que definen relaciones funcionales, como :mzn:`int_plus` y :mzn:`int_plus`, no necesitan admitir la reificación. En cambio, la igualdad con el resultado de la función se reifica.

Otro uso importante de la reificación surge cuando usamos la función de coerción :mzn:`bool2int` (explícita o implícitamente usando una expresión booleana como una expresión entera). El aplanamiento crea una variable booleana para contener el valor del argumento de expresión booleana, así como una variable entera (restringida a :mzn:`0..1`) para mantener este valor.


Considere el problema de la serie mágica de :numref:`ex-magic-series`.

.. literalinclude:: examples/magic-series_es.mzn
  :language: minizinc
  :end-before: solve satisfy

Dado :mzn:`n = 2` el desenrollado crea

.. code-block:: minizinc

  constraint s[0] = bool2int(s[0] = 0) + bool2int(s[1] = 0);
  constraint s[1] = bool2int(s[0] = 1) + bool2int(s[1] = 1);

y el aplanamiento crea:

.. code-block:: minizinc

  constraint BOOL01 <-> s[0] = 0;
  constraint BOOL03 <-> s[1] = 0;
  constraint BOOL05 <-> s[0] = 1;
  constraint BOOL07 <-> s[1] = 1;
  constraint INT02 = bool2int(BOOL01);
  constraint INT04 = bool2int(BOOL03);
  constraint INT06 = bool2int(BOOL05);
  constraint INT08 = bool2int(BOOL07);
  constraint s[0] = INT02 + INT04;
  constraint s[1] = INT06 + INT08;

El formulario final de FlatZinc es:

.. code-block:: minizinc

  var bool: BOOL01;
  var bool: BOOL03;
  var bool: BOOL05;
  var bool: BOOL07;
  var 0..1: INT02;
  var 0..1: INT04;
  var 0..1: INT06;
  var 0..1: INT08;
  array [1..2] of var 0..2: s;
  constraint int_eq_reif(s[1], 0, BOOL01);
  constraint int_eq_reif(s[2], 0, BOOL03);
  constraint int_eq_reif(s[1], 1, BOOL05);
  constraint int_eq_reif(s[2], 1, BOOL07);
  constraint bool2int(BOOL01, INT02);
  constraint bool2int(BOOL03, INT04);
  constraint bool2int(BOOL05, INT06);
  constraint bool2int(BOOL07, INT08);
  constraint int_lin_eq([-1, -1, 1], [INT02, INT04, s[1]], 0);
  constraint int_lin_eq([-1, -1, 1], [INT06, INT08, s[2]], 0);
  solve satisfy;

Predicados
----------

Un factor importante en el soporte para MiniZinc por muchos solucionadores diferentes es que las restricciones globales (y de hecho las restricciones de FlatZinc) pueden ser especializadas para el solucionador particular.

Cada solucionador especificará un predicado sin una definición, o con una definición. Por ejemplo, un solucionador que tiene un predicado global integrado :mzn:`alldifferent`, incluirá la definición

.. code-block:: minizinc

  predicate alldifferent(array[int] of var int:x);

en su biblioteca global, mientras que un solucionador que usa la descomposición por defecto tendrá la definición

.. code-block:: minizinc

  predicate alldifferent(array[int] of var int:x) =
      forall(i,j in index_set(x) where i < j)(x[i] != x[j]);

Las llamadas a predicados :math:`p(\bar{t})` se aplanan construyendo primero las variables :math:`v_i` para cada argumento :math:`t_i`. Si el predicado no tiene definición, simplemente usamos una llamada al predicado con los argumentos construidos: :math:`p(\bar{v})`.

Si el predicado tiene una definición :math:`p(\bar{x}) = \phi(\bar{x})` luego reemplazamos la llamada del predicado :math:`p(\bar{t})` con el cuerpo del predicado con los argumentos formales reemplazados por las variables del argumento, es decir :math:`\phi(\bar{v})`.

Tenga en cuenta que si una llamada de predicado :math:`p(\bar{t})` aparece en una posición reificada y no tiene definición, verificamos la existencia de una versión reificada del predicado :math:`\mathit{p\_reif}(\bar{x},b)` en cuyo caso usamos eso.

Consideremos la restricción :mzn:`alldifferent` en el ejemplo de SEND-MORE-MONEY de :numref:`ex-smm`

.. code-block:: minizinc

  constraint alldifferent([S,E,N,D,M,O,R,Y]);

Si el solucionador tiene una función incorporada :mzn:`alldifferent` simplemente construimos una nueva variable para el argumento y la reemplazamos en la llamada.

.. code-block:: minizinc

  array[1..8] of var 0..9: v = [S,E,N,D,M,O,R,Y];
  constraint alldifferent(v);

Observe que el análisis de límites intenta encontrar límites estrechos en la nueva variable de matriz. La razón para construir el argumento de la matriz es si usamos la misma matriz dos veces, el solucionador FlatZinc no tiene que construirla dos veces.
En este caso, dado que no se usa dos veces, una etapa posterior de la traducción reemplazará :mzn:`v` por su definición.

¿Qué ocurre si el solucionador usa la definición predeterminada de :mzn:`alldifferent`?.

Entonces la variable :mzn:`v` se define como de costumbre, y la llamada del predicado se reemplaza por una copia renombrada donde :mzn:`v` reemplaza el argumento formal :mzn:`x`.

El código resultante es:

.. code-block:: minizinc

  array[1..8] of var 0..9: v = [S,E,N,D,M,O,R,Y];
  constraint forall(i,j in 1..8 where i < j)(v[i] != v[j])

Que examinamos en :ref:`sec-unroll`.

Considere la siguiente restricción, donde :mzn:`alldifferent` aparece en una posición reificada.

.. code-block:: minizinc

  constraint alldifferent([A,B,C]) \/ alldifferent([B,C,D]);

Si el solucionador tiene una forma reificada de :mzn:`alldifferent` esto será flattend a:

.. code-block:: minizinc

  constraint alldifferent_reif([A,B,C],BOOL01);
  constraint alldifferent_reif([B,C,D],BOOL02);
  constraint array_bool_or([BOOL01,BOOL02],true);

Usando la descomposición por defecto, el reemplazo del predicado creará primero:

.. code-block:: minizinc

  array[1..3] of var int: v1 = [A,B,C];
  array[1..3] of var int: v2 = [B,C,D];
  constraint forall(i,j in 1..3 where i<j)(v1[i] != v1[j]) \/
             forall(i,j in 1..3 where i<j)(v2[i] != v2[j]);

que eventualmente será aplanado a la forma FlatZinc:

.. code-block:: minizinc

  constraint int_neq_reif(A,B,BOOL01);
  constraint int_neq_reif(A,C,BOOL02);
  constraint int_neq_reif(B,C,BOOL03);
  constraint array_bool_and([BOOL01,BOOL02,BOOL03],BOOL04);
  constraint int_neq_reif(B,D,BOOL05);
  constraint int_neq_reif(C,D,BOOL06);
  constraint array_bool_and([BOOL03,BOOL05,BOOL06],BOOL07);
  constraint array_bool_or([BOOL04,BOOL07],true);


Observe cómo la eliminación de subexpresiones común reutiliza la desigualdad reificada :mzn:`B! = C` (aunque hay una traducción mejor que eleva la restricción común a la conjunción de nivel superior).

.. _sec-let:



Expresiones Let
---------------

Las expresiones ``let`` son una poderosa herramienta de MiniZinc para introducir nuevas variables. Esto es útil para crear sub expresiones comunes y para definir variables locales para predicados.

Durante el aplanamiento, las expresiones de ``let`` se traducen a declaraciones de variables y restricciones. La semántica relacional de MiniZinc significa que estas restricciones deben aparecer como si estuvieran conjuntas en la primera expresión booleana adjunta.

Una característica clave de las expresiones ``let`` es que cada vez que se utilizan crean nuevas variables.

Considere el aplanamiento del código:

.. code-block:: minizinc

  constraint even(u) \/ even(v);
  predicate even(var int: x) =
            let { var int: y } in x = 2 * y;

Primero, las llamadas de predicados se reemplazan por su definición.

.. code-block:: minizinc

  constraint (let { var int: y} in u = 2 * y) \/
             (let { var int: y} in v = 2 * y);

A continuación, las variables ``let`` se renombran aparte como:

.. code-block:: minizinc

  constraint (let { var int: y1} in u = 2 * y1) \/
             (let { var int: y2} in v = 2 * y2);

Finalmente, las declaraciones variables se extraen al nivel superior

.. code-block:: minizinc

  var int: y1;
  var int: y2;
  constraint u = 2 * y1 \/ v = 2 * y2;

Una vez que se elimina la expresión ``let`` podemos aplanar como de costumbre.

Recuerde que las expresiones ``let`` pueden definir valores para las variables recién introducidas (y de hecho deben hacerlo para los parámetros).
Estos definen implícitamente restricciones que también deben aplastarse.

Considere la compleja función objetivo para el problema de asignación de asientos en una bodas :numref:`ex-wedding2`.

.. code-block:: minizinc

  solve maximize sum(h in Hatreds)(
        let {  var Seats: p1 = pos[h1[h]],
               var Seats: p2 = pos[h2[h]],
               var 0..1: same = bool2int(p1 <= 6 <-> p2 <= 6) } in
        same * abs(p1 - p2) + (1-same) * (abs(13 - p1 - p2) + 1));

Por concisión asumimos solo los dos primeros Hatreds (odios), entonces:

.. code-block:: minizinc

  set of int: Hatreds = 1..2;
  array[Hatreds] of Guests: h1 = [groom, carol];
  array[Hatreds] of Guests: h2 = [clara, bestman];


El primer paso de flattening es desenrollar la expresión :mzn:`sum`, mantenemos los nombres y parámetros de los invitados :mzn:`Seats` solo para mayor claridad, en realidad serían reemplazados por su definición:

.. code-block:: minizinc

  solve maximize
        (let { var Seats: p1 = pos[groom],
               var Seats: p2 = pos[clara],
               var 0..1: same = bool2int(p1 <= 6 <-> p2 <= 6) } in
         same * abs(p1 - p2) + (1-same) * (abs(13 - p1 - p2) + 1))
        +
        (let { var Seats: p1 = pos[carol],
               var Seats: p2 = pos[bestman],
               var 0..1: same = bool2int(p1 <= 6 <-> p2 <= 6) } in
         same * abs(p1 - p2) + (1-same) * (abs(13 - p1 - p2) + 1));

A continuación, cada nueva variable en una expresión ``let`` se renombra para ser distinta

.. code-block:: minizinc

  solve maximize
        (let { var Seats: p11 = pos[groom],
               var Seats: p21 = pos[clara],
               var 0..1: same1 = bool2int(p11 <= 6 <-> p21 <= 6) } in
         same1 * abs(p11 - p21) + (1-same1) * (abs(13 - p11 - p21) + 1))
        +
        (let { var Seats: p12 = pos[carol],
               var Seats: p22 = pos[bestman],
               var 0..1: same2 = bool2int(p12 <= 6 <-> p22 <= 6) } in
         same2 * abs(p12 - p22) + (1-same2) * (abs(13 - p12 - p22) + 1));

Las variables en la expresión ``let`` se extraen al nivel superior y las restricciones de definición se extraen al nivel correcto (que en este caso también es el nivel superior).

.. code-block:: minizinc

  var Seats: p11;
  var Seats: p21;
  var 0..1: same1;
  constraint p12 = pos[clara];
  constraint p11 = pos[groom];
  constraint same1 = bool2int(p11 <= 6 <-> p21 <= 6);
  var Seats p12;
  var Seats p22;
  var 0..1: same2;
  constraint p12 = pos[carol];
  constraint p22 = pos[bestman];
  constraint same2 = bool2int(p12 <= 6 <-> p22 <= 6) } in
  solve maximize
        same1 * abs(p11 - p21) + (1-same1) * (abs(13 - p11 - p21) + 1))
        +
        same2 * abs(p12 - p22) + (1-same2) * (abs(13 - p12 - p22) + 1));

Ahora hemos construido un código en MiniZinc equivalente sin el uso de expresiones ``let`` y el aplanamiento puede continuar como de costumbre.

Como una ilustración de las expresiones ``let`` que no aparecen en el nivel superior, considere el siguiente modelo:

.. code-block:: minizinc

  var 0..9: x;
  constraint x >= 1 -> let { var 2..9: y = x - 1 } in
                       y + (let { var int: z = x * y } in z * z) < 14;

Extraemos las definiciones de las variables al nivel superior y las restricciones al primer contexto booleano que lo incluye, que aquí está en el lado derecho de la implicación.

.. code-block:: minizinc

  var 0..9: x;
  var 2..9: y;
  var int: z;
  constraint x >= 1 -> (y = x - 1 /\ z = x * y /\ y + z * z < 14);

Tenga en cuenta que si sabemos que la ecuación que define una definición de variable no puede fallar, podemos extraerla al nivel superior. Esto generalmente hará que la resolución sea mucho más rápida.

Para el ejemplo anterior, la restricción :mzn:`y = x - 1` puede fallar ya que el dominio de :mzn:`y` no es lo suficientemente grande para todos los valores posibles de :mzn:`x - 1`. Pero la restricción :mzn:`z = x * y` no puede (de hecho el análisis de límites dará :mzn:`z` límites lo suficientemente grandes como para contener todos los valores posibles de :mzn:`x * y`).

Un mejor aplanamiento dará:

.. code-block:: minizinc

  var 0..9: x;
  var 2..9: y;
  var int: z;
  constraint z = x * y;
  constraint x >= 1 -> (y = x - 1 /\ y + z * z < 14);

Actualmente ``mzn2fzn`` hace esto definiendo siempre los límites declarados de una variable introducida para que sea lo suficientemente grande como para que su ecuación de definición mantenga siempre y luego agregue restricciones de límites en el contexto correcto para la expresión ``let``. En el ejemplo anterior, esto da como resultado


.. code-block:: minizinc

  var 0..9: x;
  var -1..8: y;
  var -9..72: z;
  constraint y = x - 1;
  constraint z = x * y;
  constraint x >= 1 -> (y >= 2 /\ y + z * z < 14);

Esta traducción conduce a una solución más eficiente ya que el cálculo posiblemente complejo de la variable ``let`` no se reifica.

Otra razón para este enfoque es que también funciona cuando las variables introducidas aparecen en contextos negativos (siempre que tengan una definición).

Considere el siguiente ejemplo similar al anterior:

.. code-block:: minizinc

  var 0..9: x;
  constraint (let { var 2..9: y = x - 1 } in
             y + (let { var int: z = x * y } in z * z) > 14) -> x >= 5;

Las expresiones ``let`` aparecen en un contexto negado, pero se define cada variable introducida.

El código aplanado es:

.. code-block:: minizinc

  var 0..9: x;
  var -1..8: y;
  var -9..72: z;
  constraint y = x - 1;
  constraint z = x * y;
  constraint (y >= 2 /\ y + z * z > 14) -> x >= 5;

Tenga en cuenta que el método simple para la eliminación de ``let`` no proporciona una traducción correcta:

.. code-block:: minizinc

  var 0..9: x;
  var 2..9: y;
  var int: z;
  constraint (y = x - 1 /\ z = x * y /\ y + z * z > 14) -> x >= 5;

Da respuestas para todos los valores posibles de :mzn:`x`, mientras que la restricción original elimina la posibilidad de que :mzn:`x = 4`.

El tratamiento de *elementos de restricción* en ``let`` expresiones es análogo a variables definidas. Uno puede pensar en una restricción como equivalente a la definición de una nueva variable booleana. Las definiciones de las nuevas variables booleanas se extraen al nivel superior y las Boolean permanece en el contexto correcto.

.. code-block:: minizinc

  constraint z > 1 -> let { var int: y,
                            constraint (x >= 0) -> y = x,
                            constraint (x < 0)  -> y = -x
                      } in y * (y - 2) >= z;

Es tratado como:

.. code-block:: minizinc

  constraint z > 1 -> let { var int: y,
                            var bool: b1 = ((x >= 0) -> y = x),
                            var bool: b2 = ((x < 0)  -> y = -x),
                            constraint b1 /\ b2
                      } in y * (y - 2) >= z;

Y se aplana a:

.. code-block:: minizinc

  constraint b1 = ((x >= 0) -> y = x);
  constraint b2 = ((x < 0)  -> y = -x);
  constraint z > 1 -> (b1 /\ b2 /\ y * (y - 2) >= z);
