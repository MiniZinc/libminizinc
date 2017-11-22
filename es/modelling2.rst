Modelos más complejos
=====================

En la última sección presentamos la estructura básica de un modelo MiniZinc. En esta sección presentamos la matriz, las estructuras de datos establecidas, los tipos enumerados y las restricciones más complejas.

.. _sec-arrayset:

Las matrices y conjuntos
------------------------

Casi siempre estamos interesados en construir modelos donde el número de restricciones y variables depende de los datos de entrada. Para ello, usualmente utilizaremos index: `matrices <array>`.

Consideremos un modelo simple de elementos finitos para modelar las temperaturas en una chapa de metal rectangular. Aproximamos las temperaturas a través de la hoja, rompiendo la hoja en un número finito de elementos en una matriz bidimensional.

Un modelo es mostrado en :numref:`ex-laplace`.
Declara el ancho ``w`` y la altura ``h`` del modelo de elementos finitos.

La declaración

.. literalinclude:: examples/laplace_es.mzn
  :language: minizinc
  :lines: 5-9

declara cuatro conjuntos fijos de enteros que describen las dimensiones del modelo de elementos finitos:
``HEIGHT`` es toda la altura del modelo, mientras que ``CHEIGHT`` es el centro de la altura omitiendo la parte superior e inferior,
``WIDTH`` es el ancho total del modelo, mientras que ``CWIDTH`` es el centro de la anchura omitiendo la izquierda y derecha,

Finalmente, una matriz bidimensional de variables flotantes ``t`` con filas numeradas :math:`0` to :math:`h` (``HEIGHT``) y columnas :math:`0` to :math:`w` (``WIDTH``), para representar las temperaturas en cada punto de la placa metálica.
Podemos acceder al elemento de la matriz en la :math: fila `i^{th}` y columna :math:`j^{th}` usando una expresión :mzn:`t[i,j]`.


La ecuación de Laplace indica que cuando la placa alcanza un estado estacionario la temperatura en cada punto interno es la media de sus vecinos ortogonales.

La restricción

.. literalinclude:: examples/laplace_es.mzn
  :language: minizinc
  :lines: 16-18

asegura que cada punto interno :math:`(i,j)` es el promedio de sus cuatro vecinos ortogonales.

Las restricciones

.. literalinclude:: examples/laplace_es.mzn
  :language: minizinc
  :lines: 20-24

restringir las temperaturas en cada borde para ser iguales, y da estos nombres de las temperaturas: ``left``, ``right``, ``top`` and ``bottom``.

Si bien las limitaciones

.. literalinclude:: examples/laplace_es.mzn
  :language: minizinc
  :lines: 26-30

asegurarse que las esquinas (que son irrelevantes) se ajustan a 0.0.
Podemos determinar las temperaturas en una placa dividida en 5 :math:`\times` 5 elementos con temperatura izquierda, derecha e inferior 0 y temperatura superior 100 con el modelo mostrado en:numref:`ex-laplace`.


.. literalinclude:: examples/laplace_es.mzn
  :language: minizinc
  :name: ex-laplace
  :caption: Finite element plate model for determining steady state temperatures (:download:`laplace_es.mzn <examples/laplace_es.mzn>`).

Ejecutando el comando

.. code-block:: bash

  $ mzn-cbc laplace_es.mzn

da la salida

::

    0.00 100.00 100.00 100.00   0.00
    0.00  42.86  52.68  42.86   0.00
    0.00  18.75  25.00  18.75   0.00
    0.00   7.14   9.82   7.14   0.00
    0.00   0.00   0.00   0.00   0.00
  ----------

.. defblock:: Sets

  .. index::
    single: set

  Las variables de conjunto se declaran con una declaración de la forma

  .. code-block:: minizincdef

    set of <type-inst> : <var-name> ;

  donde se permiten conjuntos de enteros, enumeraciones (ver más adelante), flotantes o booleanos.
  El único tipo permitido para conjuntos de variables de decisión son conjuntos variables de enteros o enumeraciones.

  Los literales establecidos son de la forma

  .. code-block:: minizincdef

    { <expr-1>, ..., <expr-n> }

  o son expresiones :index:`range` sobre enteros, enumeraciones o flotantes de la forma

  .. code-block:: minizincdef

    <expr-1> .. <expr-2>

  El estandar :index:`set operations <operator; set>` están provistos por:
  pertenencia elemento (:mzn:`in`),
  (no-estricto) relación de subconjuntos (:mzn:`subset`),
  (no-estricto) relación superconjunto (:mzn:`superset`),
  union (:mzn:`union`),
  intersecciñon (:mzn:`intersect`),
  diferencia de conjuntos (:mzn:`diff`),
  diferencia de conjunto simétrico (:mzn:`symdiff`)
  y la cantidad de elementos en el conjunto (:mzn:`card`).

Como hemos visto, las variables establecidas y los literales establecidos (incluidos los rangos), se pueden usar como un tipo implícito en las declaraciones de variables. En cuyo caso la variable tiene el tipo de los elementos en el conjunto y la variable está implícitamente restringida para ser miembro del conjunto.

Nuestro problema de horneado de pastele, es un ejemplo de un tipo muy simple de problema de planificación de la producción. En este tipo de problema, deseamos determinar qué cantidad de cada tipo de producto se debe hacer para maximizar los beneficios cuando la fabricación de un producto consume cantidades variables de algunos recursos fijos. Podemos generalizar el modelo MiniZinc en :numref:`ex-cakes2` para manejar este tipo de problema con un modelo que sea genérico en los tipos de recursos y productos. El modelo se muestra en :numref:`ex-prod-planning` y un archivo de datos de muestra (para el ejemplo de bicarbonato) se muestra en :numref:`fig-prod-planning-data`.


.. literalinclude:: examples/simple-prod-planning_es.mzn
  :language: minizinc
  :name: ex-prod-planning
  :caption: Modelo para planificación de producción simple (:download:`simple-prod-planning_es.mzn <examples/simple-prod-planning_es.mzn>`).

.. literalinclude:: examples/simple-prod-planning-data_es.dzn
  :language: minizinc
  :name: fig-prod-planning-data
  :caption: Archivo de datos de ejemplo para el problema simple de planificación de la producción (:download:`simple-prod-planning-data_es.dzn <examples/simple-prod-planning-data_es.dzn>`).

La nueva característica de este modelo es el uso de :index:`enumerated types <type; enumerated>`.
Esto nos permite tratar la elección de recursos y productos como parámetros para el modelo.
El primer artículo en el modelo

.. code-block:: minizinc

  enum Products;

Declara ``Products`` como un conjunto de productos *desconocido*.

.. defblock:: Enumerated Types

  .. index::
    single: enumerated type
    single enum

Los tipos enumerados, a los que nos referiremos como ``enumeraciones``, se declaran con una declaración de la forma:

  .. code-block:: minizincdef

    enum <var-name> ;

Un tipo enumerado se define mediante una asignación del formulario


  .. code-block:: minizincdef

    enum <var-name> = { <var-name-1>, ..., <var-name-n> } ;


Donde :mzndef:`<var-name-1>`, ..., :mzndef:`<var-name-n>` son los elementos del tipo enumerado, con el nombre :mzndef:`<var-name>`.
Cada uno de los elementos del tipo enumerado también es declarado efectivamente por esta definición como una nueva constante de ese tipo.
La declaración y la definición se pueden combinar en una línea como de costumbre.

El segundo elemento declara una matriz de enteros:

.. code-block:: minizinc

  array[Products] of int: profit;

El :index:`index set <array; index set>` del array ``profit`` es ``Products``.
Esto significa que solo los elementos del conjunto ``Products`` se pueden usar para indexar la matriz.

Los elementos de un tipo enumerado de :math:`n` elements actúan de forma muy similar a los enteros :math:`1 \dots n`. Se pueden comparar, están ordenados, por el orden en que aparecen en la definición del tipo enumerado, se pueden volver a procesar, pueden aparecer como índices de matrices, de hecho, pueden aparecer en cualquier lugar donde pueda aparecer un número entero.

En el archivo de datos de ejemplo, hemos inicializado la matriz usando una lista de enteros

.. code-block:: minizinc

  Products = { BananaCake, ChocolateCake };
  profit = [400,450];

Lo que significa que la ganancia de un pastel de plátano es de 400, mientras que para un pastel de chocolate es de 450. Internamente, ``BananaCake`` se tratará como el número entero 1, mientras que ``ChocolateCake`` se tratará como el número entero 2.

Aunque MiniZinc no proporciona un tipo de lista explícito, las matrices unidimensionales con un conjunto de índices :mzn:`1..n` se comportan como listas, y a veces nos referiremos a ellas como :index:`lists <list>`.

In a similar fashion, in the next two items we declare a set of resources
``Resources``, and an array ``capacity`` which gives the amount of
each resource that is available.

De manera similar, en los siguientes dos elementos declaramos un conjunto de recursos ``Recursos``, y una matriz ``capacidad`` que da la cantidad de
cada recurso que está disponible.

Más interesante, el artículo

.. code-block:: minizinc

  array[Products, Resources] of int: consumption;

Declara una matriz 2-D ``consumption``. El valor de :mzn:`consumption[p, r]` es la cantidad de recurso :mzn:`r` requerido para producir una unidad de producto :mzn:`p`. Tenga en cuenta que el primer índice es la fila y el segundo es la columna.

El archivo de datos contiene una inicialización de ejemplo de una matriz 2-D:

.. code-block:: minizinc

  consumption= [| 250, 2, 75,  100, 0,
                | 200, 0, 150, 150, 75 |];

Observe cómo el delimitador ``|`` se usa para separar filas.

.. defblock:: Arrays

  .. index:
    single: array


Por lo tanto, MiniZinc proporciona matrices de una y varias dimensiones que se declaran utilizando el tipo:

  .. code-block:: minizincdef

    array [ <index-set-1>, ..., <index-set-n> ] of <type-inst>

MiniZinc requiere que la declaración de matriz contenga el conjunto de índices de cada dimensión y que el conjunto de índices sea un rango entero, una variable establecida inicializada a un rango entero, o un :index:`enumeration type <enumerated type>`.

Las matrices pueden contener cualquiera de los tipos base: enteros, enumeraciones, booleanos, flotantes o cadenas. Estos pueden ser fijos o no, a excepción de cadenas que solo pueden ser parámetros. Las matrices también pueden contener conjuntos, pero no pueden contener matrices.

  :index:`One-dimensional array literals <array; literal; 1D>` son de la forma:

  .. code-block:: minizincdef

    [ <expr-1>, ..., <expr-n> ]

Mientras que :index:`two-dimensional array literals <array; literal; 2D>` son de la forma:

  .. code-block:: minizincdef

    [| <expr-1-1>, ..., <expr-1-n> |
       ...                         |
       <expr-m-1>, ..., <expr-m-n> |]

donde la matriz tiene columnas ``m`` y ``n``.

La familia de funciones incorporadas :mzn:`array1d`, :mzn:`array2d`, etc., se puede utilizar para inicializar una matriz de cualquier dimensión de una lista (o más exactamente una matriz unidimensional).

La llamada:

  .. code-block:: minizincdef

    array<n>d(<index-set-1>, ..., <index-set-n>, <list>)

  returns an ``n`` dimensional array with index sets given by the first ``n``
  arguments and the last argument contains the elements of the array. For
  instance, :mzn:`array2d(1..3, 1..2, [1, 2, 3, 4, 5, 6])` is equivalent to
  :mzn:`[|1, 2 |3, 4 |5, 6|]`.

Devuelve una matriz dimensional ``n`` con conjuntos de índices dados por los primeros argumentos ``n`` y el último argumento contiene los elementos de la matriz.
Por ejemplo, :mzn:`array2d (1..3, 1..2, [1, 2, 3, 4, 5, 6])` es equivalente a :mzn:`[| 1, 2 | 3, 4 | 5, 6 |]`.

Los elementos de la matriz son :index:`accessed <array; access>` de la forma habitual: :mzn:`a[i, j]` da el elemento en la fila :math:`i^{th}` y en la columna :math:`j^{th}`.

  .. \pjs{New array functions!}

El operador de concatenación ``++`` se puede usar para concatenar dos matrices unidimensionales juntas. El resultado es una lista, es decir, una matriz unidimensional cuyos elementos están indexados desde 1.

Por ejemplo, :mzn:`[4000, 6] ++ [2000, 500, 500]` evalúa a :mzn:`[4000, 6 , 2000, 500, 500]`.

La función incorporada :mzn:`length` devuelve la longitud de una matriz unidimensional.

El siguiente elemento en el modelo define el parámetro :mzn:`mproducts`. Esto se establece en un límite superior en la cantidad de productos de cualquier tipo que se pueden producir. Este es un ejemplo bastante complejo de comprensiones de conjuntos anidados y operadores de agregación. Los presentaremos antes de tratar de entender este artículo y el resto del modelo.

En primer lugar, MiniZinc proporciona comprensiones de listas similares a las provistas en muchos lenguajes de programación funcionales, o Python. Por ejemplo, la lista de comprensión :mzn:`[i + j | i, j in 1..3 where j < i]` evalúa a :mzn:`[1 + 2, 1 + 3, 2 + 3]` que es :mzn:`[3, 4, 5]`. Por supuesto :mzn:`[3, 4, 5]` es simplemente una matriz con conjunto de índices :mzn:`1..3`.

MiniZinc también proporciona conjuntos de comprensiones que tienen una sintaxis similar: por ejemplo, :mzn:`{i + j | i, j in 1..3 where j < i}` evalúa al conjunto :mzn:`{3, 4, 5}`.

.. defblock:: List and Set Comprehensions

  .. index:
    single: comprehension
    single: comprehension; list

La forma genérica de una lista de comprensión es

  .. code-block:: minizincdef

    [ <expr> | <generator-exp> ]

La expresión :mzndef:`<expr>` especifica cómo construir elementos en la lista de salida a partir de los elementos generados por :mzndef:`<generator-exp>`. El generador :mzndef:`<generator-exp>` consiste en una secuencia separada por comas de expresiones generadoras opcionalmente, seguidas de una expresión booleana. Las dos formas son:

  .. code-block:: minizincdef

    <generator>, ..., <generator>
    <generator>, ..., <generator> where <bool-exp>

El opcional :mzndef:`<bool-exp>` en la segunda forma actúa como un filtro en la expresión del generador. Solo los elementos que satisfacen la expresión booleana se usan para construir elementos en la lista de salida.
Un :index:`generator <comprehension; generator>` :mzndef:`<generator>` tiene la forma:

  .. code-block:: minizincdef

    <identifier>, ..., <identifier> in <array-exp>

Cada identificador es un *iterador* que toma los valores de la expresión de la matriz a su vez, con el último identificador que varía más rápidamente.

Los generadores de una lista de comprensión y :mzndef:`<bool-exp>` generalmente no involucran variables de decisión. Si implican variables de decisión, la lista producida es una lista de :mzndef:`var opt <T>` donde :mzndef:`<T>` es el tipo de :mzndef:`<expr>`. Vea la discusión de :index:`option types <option type>` en :ref:`sec-optiontypes` para más detalles.

:index:`Set comprehensions <comprehension; set>` son casi idénticos a las comprensiones de la lista. La única diferencia es el uso de ``{`` y ``}`` para adjuntar la expresión en lugar de ``[`` y ``]``. Los elementos generados por una comprensión de conjunto deben ser :index:`fixed`, es decir, libre de variables de decisión. Del mismo modo, los generadores y opcional :mzndef:`<bool-exp>` para establecer las comprensiones deben ser corregidos.

.. index::
  single: forall

En segundo lugar, MiniZinc proporciona una serie de funciones integradas que toman una matriz unidimensional y que agregan los elementos. Probablemente el más útil de estos es :mzn:``forall``. Esto toma una matriz de expresión booleana (es decir, restricciones) y devuelve una única expresión booleana que es la conjunción lógica de las expresiones booleanas en la matriz.

Por ejemplo, considera la expresión:

.. code-block:: minizinc

  forall( [a[i] != a[j] | i,j in 1..3 where i < j])

Donde ``a`` es una matriz aritmética con el conjunto de índices ``1..3``.
Esto limita los elementos en ``a`` para que sean diferentes por pares. La lista de comprensión se evalúa como :mzn:`[a [1] != a[2], a[1] != a[3], a[2] != a[3]]` y entonces el :mzn:`forall` devuelve la conjunción de logica mzn:`a[1] != a[2] /\ a[1] != a[3] /\ a[2] != a[3]`.

.. defblock:: Aggregation functions

  .. index::
    single: aggregation function
    single: sum
    single: product
    single: min
    single: max
    single: forall
    single: exists
    single: xorall
    single: iffall
    single: aggregation function; sum
    single: aggregation function; product
    single: aggregation function; min
    single: aggregation function; max
    single: aggregation function; forall
    single: aggregation function; exists
    single: aggregation function; xorall
    single: aggregation function; iffall

Las *funciones de agregación* para matrices aritméticas son:
:mzn:`sum` que agrega los elementos,
:mzn:`product` que las multiplica,
y :mzn:`min` y :mzn:`max` que devuelven respectivamente el menor y mayor elemento en la matriz.
Cuando se aplica a un conjunto vacío, :mzn:`min` y :mzn:`max` dan un error en tiempo de ejecución, :mzn:`sum` devuelve 0 y :mzn:`product` devuelve 1.

MiniZinc proporciona cuatro funciones de agregación para matrices que contienen expresiones booleanas. Como hemos visto, el primero de ellos, :mzn:`forall`, devuelve una única restricción que es la conjunción lógica de las restricciones.
La segunda función, :mzn:`exists`, devuelve la disyunción lógica de las restricciones. Por lo tanto, :mzn:`forall` impone que todas las restricciones en la matriz se mantengan, mientras que :mzn:`exists` asegura que al menos una de las restricciones se mantenga. La tercera función, :mzn:`xorall`, asegura que se mantenga un número impar de restricciones. La cuarta función, :mzn:`iffall`, garantiza que se mantenga un número par de restricciones.


La tercera y última pieza del rompecabezas es que MiniZinc permite una sintaxis especial para funciones de agregación cuando se usa con una comprensión de matriz. En lugar de escribir:

.. code-block:: minizinc

  forall( [a[i] != a[j] | i,j in 1..3 where i < j])

El modelador puede en cambio escribir el aspecto más matemático

.. code-block:: minizinc

  forall (i,j in 1..3 where i < j) (a[i] != a[j])

Las dos expresiones son completamente equivalentes: el modelador es libre de usar el que parezca más natural.

.. defblock:: Generator call expressions

  .. index::
    single: generator call
    single: expression; generator call

Una *expresión de llamada del generador* tiene forma:

  .. code-block:: minizincdef

    <agg-func> ( <generator-exp> ) ( <exp> )


Los corchetes alrededor de la expresión del generador :mzndef:`<generator-exp>` y la expresión del constructor :mzndef:`<exp>` no son opcionales, deben estar allí.
Esto es equivalente a escribir:

  .. code-block:: minizincdef

    <agg-func> ( [ <exp> | <generator-exp> ] )

El :index:`aggregation function` :mzndef:`<agg-func>` es cualquier función de MiniZinc que espera una única matriz como argumento.

Ahora estamos en condiciones de comprender el resto del modelo de planificación de producción simple que se muestra en :numref:`ex-prod-planning`. Por el momento, ignore el elemento que define :mzn:`mproducts`. El elemento más tarde:

.. code-block:: minizinc

  array[Products] of var 0..mproducts: produce;

Define una matriz unidimensional :mzn:`produce` de variables de decisión. El valor de :mzn:`produce[p]` se establecerá en la cantidad de producto :mzn:`p` en la solución óptima.

El siguiente punto

.. code-block:: minizinc

  array[Resources] of var 0..max(capacity): used;

Define un conjunto de variables auxiliares que registran la cantidad de cada recurso que se utiliza.
Las siguientes dos restricciones

.. code-block:: minizinc

  constraint forall (r in Resources)
             (used[r] = sum (p in Products) (consumption[p, r] * produce[p]));
  constraint forall (r in Resources)(used[r] <= capacity[r] );


Calcular en :mzn:`used[r]` el consumo total del recurso :mzn:`r` y garantizar que sea menor que la cantidad disponible.
Finalmente, el artículo

.. code-block:: minizinc

  solve maximize sum (p in Products) (profit[p]*produce[p]);

Indica que este es un problema de maximización y que el objetivo a maximizar es el beneficio total.

Ahora volvemos a la definición de :mzn:`mproducts`. Para cada producto
:mzn:`p` la expresión:

.. code-block:: minizinc

  (min (r in Resources where consumption[p,r] > 0)
                                   (capacity[r] div consumption[p,r])

determines the maximum amount of :mzn:`p` that can be produced taking into account the amount of each resource :mzn:`r` and how much of :mzn:`r` is required to produce the product. Notice the use of the filter :mzn:`where consumption[p,r] > 0` to ensure that only resources required to make the product are considered so as to avoid a division by zero error.  Thus, the complete expression

Determina la cantidad máxima de :mzn:`p` que se puede producir teniendo en cuenta la cantidad de cada recurso :mzn:`r` y la cantidad de :mzn:`r` que se requiere para producir el producto. Tenga en cuenta el uso del filtro :mzn:`where consumption[p,r] > 0` para garantizar que solo los recursos necesarios para fabricar el producto se consideran a fin de evitar una división por error cero.

Por lo tanto, la expresión completa:

.. code-block:: minizinc

  int: mproducts = max (p in Products)
                       (min (r in Resources where consumption[p,r] > 0)
                                   (capacity[r] div consumption[p,r]));

Calcula la cantidad máxima de *cualquier* producto que se puede producir, y así se puede usar como un límite superior en el dominio de las variables de decisión en :mzn:`produce`.

Finalmente observe que el ítem de salida es más complejo y usa :index:`list comprehensions <comprehension; list>` para crear una salida comprensible.

Ejecutando el código:

.. code-block:: bash

  $ mzn-gecode simple-prod-planning_es.mzn simple-prod-planning-data_es.dzn

Resulta en la salida:

::

  BananaCake = 2;
  ChocolateCake = 2;
  Flour = 900;
  Banana = 4;
  Sugar = 450;
  Butter = 500;
  Cocoa = 150;
  ----------
  ==========


Restricciones Globales
----------------------

.. \index{constraint!global|see{global constraint}}
.. \index{global constraint}


MiniZinc incluye una biblioteca de restricciones globales que también se puede usar para definir modelos. Un ejemplo es la restricción :mzn:`alldifferent` que requiere que todas las variables que aparecen en su argumento sean diferentes por pares.

.. literalinclude:: examples/send-more-money_es.mzn
  :language: minizinc
  :name: ex-smm
  :caption: Model for the cryptarithmetic problem SEND+MORE=MONEY (:download:`send-more-money_es.mzn <examples/send-more-money_es.mzn>`)

El problema SEND + MORE = MONEY requiere asignar un dígito diferente a cada letra para que la restricción aritmética se mantenga. El modelo que se muestra en :numref:`ex-smm` usa la expresión de restricción :mzn:`alldifferent([S,E,N,D,M,O,R,Y])` para garantizar que cada letra tome un dígito diferente valor. La restricción global está disponible en el modelo utilizando el elemento *include*

.. code-block:: minizinc

  include "alldifferent_es.mzn";

Lo que hace que la restricción global :mzn:`alldifferent` sea utilizable por el modelo. Uno podría reemplazar esta línea por

.. code-block:: minizinc

  include "globals.mzn";

Que incluye todos los globales.

En la documentación de la versión se incluye una lista de todas las restricciones globales definidas para MiniZinc. Ver :ref:`sec-globals` para una descripción de algunas restricciones globales importantes.




Expresiones condicionales
-------------------------

.. \index{expression!conditional}


MiniZinc proporciona una expresión condicional *if-then-else-endif*.
Un ejemplo de su uso es:

.. code-block:: minizinc

  int: r = if y != 0 then x div y else 0 endif;

Que establece :mzn:`r` a :mzn:`x` dividido por :mzn:`y` a menos que :mzn:`y` sea cero, en cuyo caso lo pone a cero.

.. defblock:: Conditional expressions

  .. index::
    single: expression; conditional

La forma de una expresión condicional es

  .. code-block:: minizincdef

    if <bool-exp> then <exp-1> else <exp-2> endif

Es una expresión verdadera en lugar de una declaración de flujo de control y, por lo tanto, se puede usar en otras expresiones. Se evalúa como :mzndef:`<exp-1>` si la expresión booleana :mzndef:`<bool-exp>` es verdadera y :mzndef:`<exp-2>` en caso contrario. El tipo de expresión condicional es el de :mzndef:`<exp-1>` y :mzndef:`<exp-2>` que debe tener el mismo tipo.

  If the :mzndef:`<bool-exp>` contains decision variables, then the type-inst of the expression is :mzndef:`var <T>` where :mzndef:`<T>` is the type of :mzndef:`<exp-1>` and :mzndef:`<exp-2>` even if both expressions are fixed.

Si el :mzndef:`<bool-exp>` contiene variables de decisión, entonces el tipo-instansación (type-inst) de la expresión es :mzndef:`var <T>` donde :mzndef:`<T>` es el tipo de :mzndef:`<exp-1>` y :mzndef:`<exp-2>` incluso si ambas expresiones son correctas.

.. literalinclude:: examples/sudoku_es.mzn
  :language: minizinc
  :name: ex-sudoku
  :caption: Modelo para el problema generalizado de Sudoku (:download:`sudoku_es.mzn <examples/sudoku_es.mzn>`)


.. literalinclude:: examples/sudoku_es.dzn
  :language: minizinc
  :name: ex-sudokud
  :caption: Ejemplo de archivo de datos para el problema generalizado de Sudoku (:download:`sudoku_es.dzn <examples/sudoku_es.dzn>`)

.. _fig-sudoku:

.. figure:: figures/sudoku.*

  El problema representado por el archivo de datos :download:`sudoku_es.dzn <examples/sudoku_es.dzn>`

Las expresiones condicionales son muy útiles en la creación de modelos complejos, o en resultados complejos. Considere el modelo de problemas de Sudoku que se muestra en :numref:`ex-sudoku`. Las posiciones iniciales de la placa están dadas por el parámetro :mzn:`start`, donde 0 representa una posición de tablero vacío. Esto se convierte en restricciones en las variables de decisión :mzn:`puzzle` utilizando la expresión condicional

.. code-block:: minizinc

  constraint forall(i,j in PuzzleRange)(
       if start[i,j] > 0 then puzzle[i,j] = start[i,j] else true endif );

Las expresiones condicionales también son muy útiles para definir el complejo :index:`output`. En el modelo Sudoku de :numref:`ex-sudoku` la expresión

.. code-block:: minizinc

  if j mod S == 0 then " " else "" endif

inserta un espacio extra entre los grupos de tamaño :mzn:`S`. La expresión de salida también usa expresiones condicionales para agregar líneas en blanco después de cada una :mzn:`S` lines. El resultado resultante es altamente legible.

Las restricciones restantes aseguran que los números que aparecen en cada fila, columna y :math:`S \times S` sub-cuadrado son todos diferentes.

.. index::
  single: runtime flag; -a
  single: runtime flag; --all-solutions
  single: solution; all

One can use MiniZinc to search for all solutions to a satisfaction problem (:mzn:`solve satisfy`) by using the flag ``-a`` or ``--all-solutions``. Running

Uno puede usar MiniZinc para buscar todas las soluciones a un problema de satisfacción (:mzn:`solve satisfy`) usando la bandera ``-a`` o ``--all-solutions``.
Ejecutando

.. code-block:: bash

  $ mzn-g12fd --all-solutions sudoku_es.mzn sudoku_es.dzn

Resulta en:

::

   5 9 3  7 6 2  8 1 4
   2 6 8  4 3 1  5 7 9
   7 1 4  9 8 5  2 3 6

   3 2 6  8 5 9  1 4 7
   1 8 7  3 2 4  9 6 5
   4 5 9  1 7 6  3 2 8

   9 4 2  6 1 8  7 5 3
   8 3 5  2 4 7  6 9 1
   6 7 1  5 9 3  4 8 2
  ----------
  ==========

La línea ``==========`` se emite cuando el sistema ha generado todas las soluciones posibles, y aquí verifica que hay exactamente una.


.. _sec-enum:

Enumerated Types
----------------

.. index::
  single: type; enumerated

Enumerated types allows us to build models that depend on a set of objects which are part of the data, or are named in the model, and hence make models easier to understand and debug. We have introduce enumerated types or enums briefly, in this subsection we will explore how we can use them more fully, and show some of the built in functions for dealing with enumerated types.

Let's revisit the problem of coloring the graph of Australia from :ref:`sec-modelling`.


.. literalinclude:: examples/aust-enum_es.mzn
  :language: minizinc
  :name: ex-aust-enum
  :caption: Model for coloring Australia using enumerated types (:download:`aust-enum_es.mzn <examples/aust-enum_es.mzn>`).

The model shown in :numref:`ex-aust-enum` declares an enumerated type :mzn:`Color` which must be defined in the data file.  Each of the state variables is declared to take a value from this enumerated type. Running this program using

.. code-block:: bash

  $ mzn-gecode -D "Color = { red, yellow, blue };" aust-enum_es.mzn

might result in output

::

  wa = yellow;
  nt = blue;
  sa = red;
  q = yellow;
  nsw = blue;
  v = yellow;
  t = red;


.. defblock:: Enumerated Type Variable Declarations

  .. index::
    single: variable; declaration; enum

An enumerated type parameter is declared as either:

  .. code-block:: minizincdef

    <enum-name> : <var-name>
    <l>..<u> : <var-name>

where :mzndef:`<enum-name>` is the name of a enumerated type, and :mzndef:`<l>` and :mzndef:`<u>` are fixed enumerated type expressions of the same enumerated type.

An enumerated type decision variable is declared as either:

  .. code-block:: minizincdef

    var <enum-name> : <var-name>
    var <l>..<u> : <var-name>

where :mzndef:`<enum-name>` is the name of a enumerated type, and :mzndef:`<l>` and :mzndef:`<u>` are fixed enumerated type expressions of the same enumerated type.

A key behaviour of enumerated types is that they are automatically coerced to integers when they are used in a position expecting an integer. For example, this allows us to use global constraints defined on integers, such as

.. code-block:: minizinc

  global_cardinality_low_up([wa,nt,sa,q,nsw,v,t],
                            [red,yellow,blue],[2,2,2],[2,2,3]);

This requires at least two states to be colored each color and three to be colored blue.


.. defblock:: Enumerated Type Operations

There are a number of built in operations on enumerated types:

- :mzn:`enum_next(X,x)`: returns the next value in after :mzn:`x` in the enumerated type :mzn:`X`. This is a partial function, if :mzn:`x` is the last value in the enumerated type :mzn:`X` then the function returns :math:`\bot` causing the Boolean expression containing the expression to evaluate to :mzn:`false`.
- :mzn:`enum_prev(X,x)`: returns the previous value before :mzn:`x` in the enumerated type :mzn:`X`. Similarly :mzn:`enum_prev` is a partial function.
- :mzn:`to_enum(X,i)`: maps an integer expression :mzn:`i` to an enumerated type value in type :mzn:`X` or evaluates to :math:`\bot` if :mzn:`i` is less than or equal to 0 or greater than the number of elements in :mzn:`X`.

Note also that a number of standard functions are applicable to enumerated types:

- :mzn:`card(X)`: returns the cardinality of an enumerated type :mzn:`X`.
- :mzn:`min(X)`: returns the minimum element of of an enumerated type :mzn:`X`.
- :mzn:`max(X)`: returns the maximum element of of an enumerated type :mzn:`X`.





.. _sec-complex:

Complex Constraints
-------------------

.. index::
  single: constraint; complex

Constraints are the core of the MiniZinc model. We have seen simple relational expressions but constraints can be considerably more powerful than this. A constraint is allowed to be any Boolean expression. Imagine a scheduling problem in which we have two tasks that cannot overlap in time. If :mzn:`s1` and :mzn:`s2` are the corresponding start times and :mzn:`d1` and :mzn:`d2` are the corresponding durations we can express this as:

.. code-block:: minizinc

  constraint s1 + d1 <= s2  \/ s2 + d2 <= s1;

which ensures that the tasks do not overlap.


.. defblock:: Booleans

  .. index::
    single: Boolean
    single: expression; Boolean
    single: true
    single: false
    single: operator; Boolean
    single: bool2int

Boolean expressions in MiniZinc can be written using a standard mathematical syntax. The Boolean literals are :mzn:`true` and :mzn:`false` and the Boolean operators are conjunction, i.e. and  (``/\``), disjunction, i.e. or  (``\/``), only-if (:mzn:`<-`), implies (:mzn:`->`), if-and-only-if (:mzn:`<->`) and negation (:mzn:`not`). Booleans can be automatically coerced to integers, but to make this coercion explicit the built-in function :mzn:`bool2int` can be used: it returns 1 if its argument is true and 0 otherwise.

.. literalinclude:: examples/jobshop_es.mzn
  :language: minizinc
  :name: ex-jobshop
  :caption: Model for job-shop scheduling problems (:download:`jobshop_es.mzn <examples/jobshop_es.mzn>`).

.. literalinclude:: examples/jdata_es.dzn
  :language: minizinc
  :name: ex-jdata
  :caption: Data for job-shop scheduling problems (:download:`jdata_es.dzn <examples/jdata_es.dzn>`).

The job shop scheduling model given in :numref:`ex-jobshop` gives a realistic example of the use of this disjunctive modelling capability. In job shop scheduling we have a set of jobs, each consisting of a sequence of tasks on separate machines: so task :mzn:`[i,j]` is the task in the :math:`i^{th}` job performed on the :math:`j^{th}` machine. Each sequence of tasks must be completed in order, and no two tasks on the same machine can overlap in time. Even small instances of this problem can be quite challenging to find optimal solutions.

The command

.. code-block:: bash

  $ mzn-gecode --all-solutions jobshop_es.mzn jdata_es.dzn

solves a small job shop scheduling problem, and illustrates the behaviour of \texttt{all-solutions} for optimisation problems.  Here the solver outputs each better solutions as it finds it, rather than all possible optimal solutions. The output from this command is:

::

  end = 39
   5  9 13 22 30
   6 13 18 25 36
   0  4  8 12 16
   4  8 12 16 22
   9 16 25 27 38
  ----------
  end = 37
   4  8 12 17 20
   5 13 18 26 34
   0  4  8 12 16
   8 12 17 20 26
   9 16 25 27 36
  ----------
  end = 34
   0  1  5 10 13
   6 10 15 23 31
   2  6 11 19 27
   1  5 10 13 19
   9 16 22 24 33
  ----------
  end = 30
   5  9 13 18 21
   6 13 18 25 27
   1  5  9 13 17
   0  1  2  3  9
   9 16 25 27 29
  ----------
  ==========

indicating an optimal solution with end time 30 is finally found, and proved optimal. We can generate all *optimal solutions* by adding a constraint that :mzn:`end = 30` and changing the solve item to :mzn:`solve satisfy` and then executing

.. code-block:: bash

  $ mzn-gecode --all-solutions jobshop_es.mzn jobshop_es.dzn

For this problem there are 3,444,375 optimal solutions.

.. literalinclude:: examples/stable-marriage_es.mzn
  :language: minizinc
  :name: ex-stable-marriage
  :caption: Model for the stable marriage problem (:download:`stable-marriage_es.mzn <examples/stable-marriage_es.mzn>`).

.. literalinclude:: examples/stable-marriage_es.dzn
  :language: minizinc
  :name: ex-sm-data
  :caption: Example data for the stable marriage problem (:download:`stable-marriage_es.dzn <examples/stable-marriage_es.dzn>`).

Another powerful modelling feature in MiniZinc is that decision variables can be used for :index:`array access <array; access>`. As an example, consider the (old-fashioned) *stable marriage problem*. We have :mzn:`n` (straight) women and :mzn:`n` (straight) men. Each man has a ranked list of women and vice versa. We want to find a husband/wife for each women/man so that all marriages are *stable* in the sense that:

- whenever :mzn:`m` prefers another women :mzn:`o` to his wife :mzn:`w`, :mzn:`o` prefers her husband to :mzn:`m`, and
- whenever :mzn:`w` prefers another man :mzn:`o` to her husband :mzn:`m`, :mzn:`o` prefers his wife to :mzn:`w`.

This can be elegantly modelled in MiniZinc.
The model and sample data is shown in :numref:`ex-stable-marriage` and :numref:`ex-sm-data`.

The first three items in the model declare the number of men/women and the set of men and women. Here we introduce the use of *anonymous enumerated types*. Both :mzn:`Men` and :mzn:`Women` are sets of size :mzn:`n`, but we do not wish to mix them up so we use an anonymous enumerated type. This allows MiniZinc to detect modelling errors where we use :mzn:`Men` for :mzn:`Women` or vice versa.


The matrices :mzn:`rankWomen` and :mzn:`rankMen`, respectively, give the women's ranking  of the men and the men's ranking of the women. Thus, the entry  :mzn:`rankWomen[w,m]` gives the ranking by woman :mzn:`w` of man :mzn:`m`. The lower the number in the ranking, the more the man or women is preferred.

There are two arrays of decision variables: :mzn:`wife` and :mzn:`husband`. These, respectively, contain the wife of each man and the husband of each women.

The first two constraints

.. literalinclude:: examples/stable-marriage_es.mzn
  :language: minizinc
  :lines: 13-14

ensure that the assignment of husbands and wives is consistent: :mzn:`w` is the wife of :mzn:`m` implies :mzn:`m` is the husband of :mzn:`w` and vice versa. Notice how in :mzn:`husband[wife[m]]` the index expression :mzn:`wife[m]` is a decision variable, not a parameter.

The next two constraints are a  direct encoding of the stability condition:

.. literalinclude:: examples/stable-marriage_es.mzn
  :language: minizinc
  :lines: 16-22

This natural modelling of the stable marriage problem is made possible by the ability to use decision variables as array indices and to construct constraints using the standard Boolean connectives. The alert reader may be wondering at this stage, what happens if the array index variable takes a value that is outside the index set of the array. MiniZinc treats this as failure: an array access :mzn:`a[e]` implicitly adds the constraint :mzn:`e in index_set(a)` to the closest surrounding Boolean context where :mzn:`index_set(a)` gives the index set of :mzn:`a`.

.. defblock:: Anonymous Enumerated Types

  .. index::
    single: type; enumerated; anonymous

An *anonymous enumerated type* is of the form :mzndef:`anon_enum(<n>)` where :mzndef:`<n>` is a fixed integer expression defining the size of the enumerated type.

An anonymous enumerated type is just like any other enumerated type except that we have no names for its elements. When printed out, they are given unique names based on the enumerated type name.

Thus for example, consider the variable declarations

.. code-block:: minizinc

  array[1..2] of int: a= [2,3];
  var 0..2: x;
  var 2..3: y;

The constraint :mzn:`a[x] = y` will succeed with :math:`x=1 \wedge y=2` and :math:`x=2 \wedge y=3`. And the constraint :mzn:`not a[x] = y` will succeed with :math:`x=0 \wedge y=2`, :math:`x=0 \wedge y=3`, :math:`x=1 \wedge y=3` and :math:`x=2 \wedge y=2`.

In the case of invalid array accesses by a parameter, the formal semantics of MiniZinc treats this as failure so as to ensure that the treatment of parameters and decision variables is consistent, but a warning is issued since it is almost always an error.

.. literalinclude:: examples/magic-series_es.mzn
  :language: minizinc
  :name: ex-magic-series
  :caption: Model solving the magic series problem (:download:`magic-series_es.mzn <examples/magic-series_es.mzn>`).

.. index::
  single: bool2int
  single: constraint; higher order

The coercion function :mzn:`bool2int` can be called with any Boolean expression. This allows the MiniZinc modeller to use so called *higher order constraints*. As a simple example consider the *magic series problem*: find a list of numbers :math:`s= [s_0,\ldots,s_{n-1}]` such that :math:`s_i` is the number of occurrences of :math:`i` in :math:`s`. An example is :math:`s = [1,2,1,0]`. A MiniZinc model for this problem is shown in :numref:`ex-magic-series`. The use of :mzn:`bool2int` allows us to sum up the number of times the constraint :mzn:`s[j]=i` is satisfied. Executing the command

.. code-block:: bash

  $ mzn-gecode --all-solutions magic-series_es.mzn -D "n=4;"

leads to the output

::

  s = [1, 2, 1, 0];
  ----------
  s = [2, 0, 2, 0];
  ----------
  ==========

indicating exactly two solutions to the problem.

Note that MiniZinc will automatically coerce Booleans to integers and integers to floats when required. We could replace the the constraint item in :numref:`ex-magic-series` with

.. code-block:: minizinc

  constraint forall(i in 0..n-1) (
     s[i] = (sum(j in 0..n-1)(s[j]=i)));

and get identical results, since the Boolean expression :mzn:`s[j] = i` will be automatically coerced to an integer, effectively by the MiniZinc system automatically adding the missing :mzn:`bool2int`.

.. defblock:: Coercion

  .. index::
    single: coercion; automatic
    single: coercion; bool2int
    single: coercion; int2float

In MiniZinc one can *coerce* a Boolean value to an integer value using the :mzn:`bool2int` function. Similarly one can coerce an integer value to a float value using :mzn:`int2float`. The instantiation of the coerced value is the same as the argument, e.g. :mzn:`par bool` is coerced to :mzn:`par int`, while :mzn:`var bool` is coerced to :mzn:`var int` MiniZinc automatically coerces Boolean expressions to integer expressions and integer expressions to float expressions, by inserting :mzn:`bool2int` and :mzn:`int2float` in the model appropriately. Note that it will also coerce Booleans to floats using two steps.


Set Constraints
---------------

.. index::
  single: constraint; set

 Another powerful modelling feature of MiniZinc is that it allows sets containing integers to be decision variables: this means that when the model is evaluated the solver will find which elements are in the set. As a simple example, consider the *0/1 knapsack problem*. This is a restricted form of the knapsack problem in which we can either choose to place the item in the knapsack or not. Each item has a weight and a profit and we want to find which choice of items leads to the maximum profit subject to the knapsack not being too full. It is natural to model this in MiniZinc with a single decision variable: :mzn:`var set of ITEM: knapsack` where :mzn:`ITEM` is the set of possible items. If the arrays :mzn:`weight[i]` and :mzn:`profit[i]` respectively give the weight and profit of item :mzn:`i`, and the maximum weight the knapsack can carry is given by :mzn:`capacity` then a naural model is given in :numref:`ex-knapsack-binary`.

.. literalinclude:: examples/knapsack_es.mzn
  :language: minizinc
  :name: ex-knapsack-binary
  :caption: Model for the 0/1 knapsack problem (:download:`knapsack_es.mzn <examples/knapsack_es.mzn>`).

Notice that the :mzn:`var`
keyword comes before the :mzn:`set` declaration indicating that the set itself is the decision variable. This contrasts with an array in which the :mzn:`var` keyword qualifies the elements in the array rather than the array itself since the basic structure of the array is fixed, i.e. its index set.

.. literalinclude:: examples/social-golfers_es.mzn
  :language: minizinc
  :name: ex-social-golfers
  :caption: Model for the social golfers problems (:download:`social-golfers_es.mzn <examples/social-golfers_es.mzn>`).


As a more complex example of set constraint consider the social golfers problem shown in :numref:`ex-social-golfers`. The aim is to schedule a golf tournament over :mzn:`weeks` using :mzn:`groups` :math:`\times` :mzn:`size` golfers. Each week we have to schedule :mzn:`groups` different groups each of size :mzn:`size`. No two pairs of golfers should ever play in two groups.

The variables in the model are sets of golfers :mzn:`Sched[i,j]` for the :math:`i^{th}` week and :mzn:`j^{th}` group.

The constraints shown in lines 11-32 first enforces an ordering on the first set in each week to remove symmetry in swapping weeks. Next they enforce an ordering on the sets in each week, and make each set have a cardinality of :mzn:`size`. They then ensure that each week is a partition of the set of golfers using the global constraint :mzn:`partition_set`. Finally the last constraint ensures that no two players play in two groups together (since the cardinality of the intersection of any two groups is at most 1).

.. index::
  single: symmetry; breaking

There are also symmetry breaking initialisation constraints shown in lines 34-46: the first week is fixed to have all players in order; the second week is made up of the first players of each of the first groups in the first week; finally the model forces the first :mzn:`size` players to appear in their corresponding group number for the remaining weeks.

Executing the command

.. code-block:: bash

  $ mzn-gecode social-golfers_es.mzn social-golfers_es.dzn

where the data file defines a problem with 4 weeks, with 4 groups of size 3 leads to the output

::

  1..3 4..6 7..9 10..12
  { 1, 4, 7 } { 2, 5, 10 } { 3, 9, 11 } { 6, 8, 12 }
  { 1, 5, 8 } { 2, 6, 11 } { 3, 7, 12 } { 4, 9, 10 }
  { 1, 6, 9 } { 2, 4, 12 } { 3, 8, 10 } { 5, 7, 11 }
  ----------

Notice hows sets which are ranges may be output in range format.


Putting it all together
-----------------------

We finish this section with a complex example illustrating most of the features introduced in this chapter including enumerated types, complex constraints, global constraints, and complex output.

.. literalinclude:: examples/wedding_es.mzn
  :language: minizinc
  :name: ex-wedding
  :caption: Planning wedding seating using enumerated types (:download:`wedding_es.mzn <examples/wedding_es.mzn>`).

The model of :numref:`ex-wedding` arranges seats at the wedding table.
The table has 12 numbered seats in order around the table, 6 on each side. Males must sit in odd numbered seats, and females in even. Ed cannot sit at the end of the table because of a phobia, and the bride and groom must sit next to each other. The aim is to maximize the distance between known hatreds. The distance between seats is the difference in seat number if on the same side, otherwise its the distance to the opposite seat + 1.

Note that in the output statement we consider each seat :mzn:`s` and search for a guest :mzn:`g` who is assigned to that seat. We make use of the built in function :mzn:`fix` which checks if a decision variable is fixed and returns its fixed value, and otherwise aborts. This is always safe to use in output statements, since by the time the output statement is run all decision variables should be fixed.


Running

.. code-block:: bash

  $ mzn-gecode wedding_es.mzn

Results in the output

::

  ted bride groom rona ed carol ron alice bob bridesmaid bestman clara
  ----------
  ==========

The resulting table placement is illustrated in :numref:`fig-wedding` where the lines indicate hatreds. The total distance is 22.

.. _fig-wedding:

.. figure:: figures/wedding.*

  Seating arrangement at the wedding table


.. \pjs{Move the fix definition elsewhere!}

.. defblock:: Fix

  .. index::
    single: fix
    single: fixed

In output items the built-in function :mzn:`fix` checks that the value of a decision variable is fixed and coerces the instantiation from decision variable to parameter.

.. % oil-blending
.. %arrays floats sum forall
.. %more complex datafile
..
.. %suduko
.. %2-D array
.. %complex transformation from data file
..
.. %jobshop
.. %disjunction,
..
.. %talk about other complex constraints--IC example?
..
.. %magic sequence
.. %reification
..
.. %warehouse placement
.. %reification more complex example
..
.. %0/1 knapsack
.. %set constraint
..
.. %social golfers
.. %more complex set constraint
..
.. %finish with larger example from Mark
