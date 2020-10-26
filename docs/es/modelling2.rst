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
Podemos acceder al elemento de la matriz en la fila :math:`i^{th}` y columna :math:`j^{th}` usando una expresión :mzn:`t[i,j]`.


La ecuación de Laplace indica que cuando la placa alcanza un estado estacionario la temperatura en cada punto interno es la media de sus vecinos ortogonales.

La restricción

.. literalinclude:: examples/laplace_es.mzn
  :language: minizinc
  :lines: 16-18

Asegura que cada punto interno :math:`(i,j)` es el promedio de sus cuatro vecinos ortogonales.

Las restricciones

.. literalinclude:: examples/laplace_es.mzn
  :language: minizinc
  :lines: 20-24

Restringir las temperaturas en cada borde para ser iguales, y da estos nombres de las temperaturas: ``left``, ``right``, ``top`` and ``bottom``.

Si bien las limitaciones

.. literalinclude:: examples/laplace_es.mzn
  :language: minizinc
  :lines: 26-30

Asegurarse que las esquinas (que son irrelevantes) se ajustan a 0.0.
Podemos determinar las temperaturas en una placa dividida en 5 :math:`\times` 5 elementos con temperatura izquierda, derecha e inferior 0 y temperatura superior 100 con el modelo mostrado en:numref:`ex-laplace`.


.. literalinclude:: examples/laplace_es.mzn
  :language: minizinc
  :name: ex-laplace
  :caption: Modelo de placa de elementos finitos para determinar las temperaturas en estado estable (:download:`laplace_es.mzn <examples/laplace_es.mzn>`).

Ejecutando el comando

.. code-block:: bash

  $ mzn-cbc laplace_es.mzn

Da la salida

.. code-block:: none

    0.00 100.00 100.00 100.00   0.00
    0.00  42.86  52.68  42.86   0.00
    0.00  18.75  25.00  18.75   0.00
    0.00   7.14   9.82   7.14   0.00
    0.00   0.00   0.00   0.00   0.00
  ----------

.. defblock:: Conjuntos

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

  Donde la matriz tiene columnas ``m`` y ``n``.

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

En primer lugar, MiniZinc proporciona comprensiones de listas similares a las provistas en muchos lenguajes de programación funcionales, o Python. Por ejemplo, la lista de comprensión :mzn:`[i + j | i, j in 1..3 where j < i]` evalúa a :mzn:`[2 + 1, 3 + 1, 3 + 2]` que es :mzn:`[3, 4, 5]`. Por supuesto :mzn:`[3, 4, 5]` es simplemente una matriz con conjunto de índices :mzn:`1..3`.

MiniZinc también proporciona conjuntos de comprensiones que tienen una sintaxis similar: por ejemplo, :mzn:`{i + j | i, j in 1..3 where j < i}` evalúa al conjunto :mzn:`{3, 4, 5}`.

.. defblock:: Enumeraciones y Establecer comprensiones

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

  :index:`Conjuntos de comprensiones <comprehension; set>` son casi idénticos a las comprensiones de la lista. La única diferencia es el uso de ``{`` y ``}`` para adjuntar la expresión en lugar de ``[`` y ``]``. Los elementos generados por una comprensión de conjunto deben ser :index:`fixed`, es decir, libre de variables de decisión. Del mismo modo, los generadores y opcional :mzndef:`<bool-exp>` para establecer las comprensiones deben ser corregidos.

.. index::
  single: forall

En segundo lugar, MiniZinc proporciona una serie de funciones integradas que toman una matriz unidimensional y que agregan los elementos. Probablemente el más útil de estos es :mzn:``forall``. Esto toma una matriz de expresión booleana (es decir, restricciones) y devuelve una única expresión booleana que es la conjunción lógica de las expresiones booleanas en la matriz.

Por ejemplo, considera la expresión:

.. code-block:: minizinc

  forall( [a[i] != a[j] | i,j in 1..3 where i < j])

Donde ``a`` es una matriz aritmética con el conjunto de índices ``1..3``.
Esto limita los elementos en ``a`` para que sean diferentes por pares. La lista de comprensión se evalúa como :mzn:`[a [1] != a[2], a[1] != a[3], a[2] != a[3]]` y entonces el :mzn:`forall` devuelve la conjunción de logica mzn:`a[1] != a[2] /\ a[1] != a[3] /\ a[2] != a[3]`.

.. defblock:: Funciones de Agregación

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

.. defblock:: Generador de Expresiones de Llamada

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

.. code-block:: none

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
  :caption: Modelo para el problema criptoaritméticos SEND + MORE = MONEY (:download:`send-more-money_es.mzn <examples/send-more-money_es.mzn>`)

El problema SEND + MORE = MONEY requiere asignar un dígito diferente a cada letra para que la restricción aritmética se mantenga. El modelo que se muestra en :numref:`ex-smm` usa la expresión de restricción :mzn:`alldifferent([S,E,N,D,M,O,R,Y])` para garantizar que cada letra tome un dígito diferente valor. La restricción global está disponible en el modelo utilizando el elemento *include*

.. code-block:: minizinc

  include "alldifferent.mzn";

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

.. defblock:: Expresiones condicionales

  .. index::
    single: expression; conditional

  La forma de una expresión condicional es

  .. code-block:: minizincdef

    if <bool-exp> then <exp-1> else <exp-2> endif

  Es una expresión verdadera en lugar de una declaración de flujo de control y, por lo tanto, se puede usar en otras expresiones. Se evalúa como :mzndef:`<exp-1>` si la expresión booleana :mzndef:`<bool-exp>` es verdadera y :mzndef:`<exp-2>` en caso contrario. El tipo de expresión condicional es el de :mzndef:`<exp-1>` y :mzndef:`<exp-2>` que debe tener el mismo tipo.

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

Inserta un espacio extra entre los grupos de tamaño :mzn:`S`. La expresión de salida también usa expresiones condicionales para agregar líneas en blanco después de cada una :mzn:`S` lines. El resultado resultante es altamente legible.

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

.. code-block:: none

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

Tipos enumerados
----------------

.. index::
  single: type; enumerated

Los tipos enumerados nos permiten construir modelos que dependen de un conjunto de objetos que forman parte de los datos, o se nombran en el modelo, y por lo tanto hacen que los modelos sean más fáciles de comprender y depurar. Hemos introducido tipos enumerados o enumeraciones brevemente, en esta subsección exploraremos cómo podemos usarlos más completamente y mostraremos algunas de las funciones integradas para tratar con tipos enumerados.

Volvamos al problema de colorear el gráfico de Australia en :ref:`sec-modelling`.


.. literalinclude:: examples/aust-enum_es.mzn
  :language: minizinc
  :name: ex-aust-enum
  :caption: Modelo para colorear Australia usando tipos enumerados (:download:`aust-enum_es.mzn <examples/aust-enum_es.mzn>`).

El modelo que se muestra en :numref:`ex-aust-enum` declara un tipo enumerado :mzn:`Color` que debe definirse en el archivo de datos. Cada una de las variables de estado se declara para tomar un valor de este tipo enumerado.

Ejecutando este programa usando:

.. code-block:: bash

  $ mzn-gecode -D "Color = { red, yellow, blue };" aust-enum_es.mzn

Puede resultar en la salida:

.. code-block:: none

  wa = yellow;
  nt = blue;
  sa = red;
  q = yellow;
  nsw = blue;
  v = yellow;
  t = red;


.. defblock:: Declaraciones de variables de Tipo Enumeradas

  .. index::
    single: variable; declaration; enum

  Un parámetro de tipo enumerado se declara como:

  .. code-block:: minizincdef

    <enum-name> : <var-name>
    <l>..<u> : <var-name>

  Donde :mzndef:`<enum-name>` es el nombre de un tipo enumerado, :mzndef:`<l>` y :mzndef:`<u>` son expresiones de tipos enumerados fijos del mismo tipo enumerado.

  Una variable de decisión de tipo enumerada se declara como:

  .. code-block:: minizincdef

    var <enum-name> : <var-name>
    var <l>..<u> : <var-name>

  Donde :mzndef:`<enum-name>` es el nombre de un tipo enumerado, :mzndef:`<l>` y :mzndef:`<u>` son expresiones de tipos enumerados fijos del mismo tipo enumerado.

Un comportamiento clave de los tipos enumerados es que son coercionados automáticamente a enteros cuando se usan en una posición que espera un entero. Por ejemplo, esto nos permite usar restricciones globales definidas en enteros, como

.. code-block:: minizinc

  global_cardinality_low_up([wa,nt,sa,q,nsw,v,t],
                            [red,yellow,blue],[2,2,2],[2,2,3]);

Esto requiere al menos dos estados para colorear cada color y tres para ser de color azul.

.. defblock:: Operaciones de Tipo enumeradas

  Hay una serie de operaciones integradas en tipos enumerados:

  - :mzn:`enum_next(X, x)`: devuelve el siguiente valor después de :mzn:`x` en el tipo enumerado :mzn:`X`. Esta es una función parcial, si :mzn:`x` es el último valor en el tipo enumerado :mzn:`X` luego la función devuelve :math:`\bot` haciendo que la expresión booleana que contiene la expresión evalúe :mzn:`false`.
  - :mzn:`enum_prev(X, x)`: devuelve el valor anterior antes de :mzn:`x` en el tipo enumerado :mzn:`X`. Del mismo modo :mzn:`enum_prev` es una función parcial.
  - :mzn:`to_enum(X, i)`: asigna una expresión entera :mzn:`i` a un valor de tipo enumerado en tipo :mzn:`X` o evalúa a :math:`\bot` si :mzn:`i` es menor o igual que 0 o mayor que la cantidad de elementos en :mzn:`X`.

Tenga en cuenta también que una serie de funciones estándar son aplicables a los tipos enumerados:

  - :mzn:`card(X)`: devuelve la cardinalidad de un tipo enumerado :mzn:`X`.
  - :mzn:`min(X)`: devuelve el elemento mínimo de un tipo enumerado :mzn:`X`.
  - :mzn:`max(X)`: devuelve el elemento máximo de un tipo enumerado :mzn:`X`.

.. _sec-complex:

Restricciones complejas
-----------------------

.. index::
  single: constraint; complex

Las restricciones son el núcleo del modelo MiniZinc. Hemos visto expresiones relacionales simples, pero las restricciones pueden ser considerablemente más poderosas que esto. Se permite que una restricción sea cualquier expresión booleana. Imagine un problema de programación en el que tenemos dos tareas que no se pueden superponer en el tiempo. Si :mzn:`s1` y :mzn:`s2` son los tiempos de inicio correspondientes y :mzn:`d1` y :mzn:`d2` son las duraciones correspondientes, podemos expresar esto como:

.. code-block:: minizinc

  constraint s1 + d1 <= s2  \/ s2 + d2 <= s1;

Que asegura que las tareas no se superponen.

.. defblock:: Booleanos

  .. index::
    single: Boolean
    single: expression; Boolean
    single: true
    single: false
    single: operator; Boolean
    single: bool2int

  Las expresiones booleanas en MiniZinc se pueden escribir usando una sintaxis matemática estándar. Los literales booleanos son :mzn:`true`, :mzn:`false` y los operadores booleanos son conjunción, es decir, (``/\``), disyunción, es decir o (``\/``), only-if (:mzn:`<-`), implica (:mzn:`->`), if-and-only-if (:mzn:`<->`) y negación (:mzn:`not`). Los booleanos se pueden forzar automáticamente a enteros, pero para que esta coerción sea explícita la función incorporada :mzn:`bool2int` se puede usar: devuelve 1 si su argumento es verdadero y 0 en caso contrario.

.. literalinclude:: examples/jobshop_es.mzn
  :language: minizinc
  :name: ex-jobshop
  :caption: Modelo para problemas de planificación de tiendas de trabajo (:download:`jobshop_es.mzn <examples/jobshop_es.mzn>`).

.. literalinclude:: examples/jdata_es.dzn
  :language: minizinc
  :name: ex-jdata
  :caption: Datos para problemas de planificación de tiendas de trabajo (:download:`jdata_es.dzn <examples/jdata_es.dzn>`).

El modelo de planificación de tiendas de trabajo dado en :numref:`ex-jobshop` brinda un ejemplo realista del uso de esta capacidad de modelado disyuntivo. En la planificación de tiendas de trabajo tenemos un conjunto de trabajos, cada uno de los cuales consiste en una secuencia de tareas en máquinas separadas: por lo que la tarea :mzn:`[i, j]` es la tarea en el trabajo :math:`i^{th}` realizado en la máquina :math:`j^{th}`. Cada secuencia de tareas debe completarse en orden, y no hay dos tareas en la misma máquina que puedan superponerse en el tiempo. Incluso pequeñas instancias de este problema pueden ser bastante desafiantes para encontrar soluciones óptimas.

El comando

.. code-block:: bash

  $ mzn-gecode --all-solutions jobshop_es.mzn jdata_es.dzn

Resuelve un pequeño problema de planificación de tiendas de trabajo e ilustra el comportamiento de \texttt{all-solutions} para problemas de optimización. Aquí el solucionador genera cada una de las mejores soluciones tal como la encuentra, en lugar de todas las soluciones óptimas posibles. El resultado de este comando es:

.. code-block:: none

  Fin = 39
   5  9 13 22 30
   6 13 18 25 36
   0  4  8 12 16
   4  8 12 16 22
   9 16 25 27 38
  ----------
  Fin = 37
   4  8 12 17 20
   5 13 18 26 34
   0  4  8 12 16
   8 12 17 20 26
   9 16 25 27 36
  ----------
  Fin = 34
   0  1  5 10 13
   6 10 15 23 31
   2  6 11 19 27
   1  5 10 13 19
   9 16 22 24 33
  ----------
  Fin = 30
   5  9 13 18 21
   6 13 18 25 27
   1  5  9 13 17
   0  1  2  3  9
   9 16 25 27 29
  ----------
  ==========

Lo que indica que finalmente se encuentra una solución óptima con el tiempo de finalización 30, y resultó ser óptima. Podemos generar todas las *soluciones óptimas* agregando una restricción que :mzn:`end = 30` y cambiando el elemento de resolver a :mzn:`solve satisfy`.

Posteriormente se debe de ejecutar el siguiente comando:

.. code-block:: bash

  $ mzn-gecode --all-solutions jobshop_es.mzn jobshop_es.dzn

Para este problema, hay 3,444,375 soluciones óptimas.

.. literalinclude:: examples/stable-marriage_es.mzn
  :language: minizinc
  :name: ex-stable-marriage
  :caption: Modelo para el problema del matrimonio estable (:download:`stable-marriage_es.mzn <examples/stable-marriage_es.mzn>`).

.. literalinclude:: examples/stable-marriage_es.dzn
  :language: minizinc
  :name: ex-sm-data
  :caption: Datos de ejemplo para el problema de matrimonio estable (:download:`stable-marriage_es.dzn <examples/stable-marriage_es.dzn>`).

Otra poderosa característica de modelado en MiniZinc es que las variables de decisión se pueden usar para :index:`array access <array; access>`. Como por ejemplo, considere el (anticuado) *problema de matrimonio estable*.
Tenemos que :mzn:`n` (hétero) mujeres y :mzn:`n` (straight) men. Cada hombre tiene una lista clasificada de mujeres y viceversa. Queremos encontrar un esposo/esposa para cada mujer/hombre para que todos los matrimonios sean *estables* en el sentido de que:

- Siempre que :mzn:`m` prefiera a otra mujer :mzn:`o` a su esposa :mzn:`w`, :mzn:`o` prefiere a su marido a :mzn:`m`.
- Siempre que :mzn:`w` prefiera a otro hombre :mzn:`o` a su marido :mzn:`m`, :mzn:`o` prefiere a su mujer a :mzn:`w`.

Esto se puede modelar elegantemente en MiniZinc.
El modelo y los datos de muestra se muestran en :numref:`ex-stable-marriage` y :numref:`ex-sm-data`.

Los tres primeros elementos en el modelo declaran el número de hombres/mujeres y el conjunto de hombres y mujeres. Aquí presentamos el uso de *tipos enumerados anónimos*. Ambos :mzn:`Men` y :mzn:`Women` son conjuntos de tamaño :mzn:`n`, pero no deseamos mezclarlos, así que utilizamos un tipo enumerado anónimo. Esto permite que MiniZinc detecte errores de modelado cuando usamos :mzn:`Men` para :mzn:`Women` o viceversa.

Las matrices :mzn:`rankWomen` y :mzn:`rankMen`, respectivamente, dan el ranking de las mujeres de los hombres y el ranking de las mujeres de los hombres. Por lo tanto, la entrada :mzn:`rankWomen[w, m]` da la clasificación por mujer :mzn:`w` de hombre :mzn:`m`. Cuanto menor sea el número en el ranking, más se prefiere el hombre o la mujer.

Hay dos matrices de variables de decisión :mzn:`wife` y :mzn:`husband`. Estos, respectivamente, contienen la esposa de cada hombre y el esposo de cada mujer.

Las dos primeras restricciones:

.. literalinclude:: examples/stable-marriage_es.mzn
  :language: minizinc
  :lines: 13-14

Asegúrese de que la asignación de esposos y esposas sea consistente: :mzn:`w` es la esposa de :mzn:`m` implica que :mzn:`m` es el esposo de :mzn:`w` y viceversa. Observe cómo en :mzn:`husband[wife[m]]` la expresión de índice :mzn:`wife[m]` es una variable de decisión, no un parámetro.

Las siguientes dos restricciones son una codificación directa de la condición de estabilidad:

.. literalinclude:: examples/stable-marriage_es.mzn
  :language: minizinc
  :lines: 16-22


Este modelado natural del problema del matrimonio estable es posible gracias a la capacidad de usar variables de decisión como índices de matriz y construir restricciones utilizando las conectivas booleanas estándar. El lector puede preguntarse en esta etapa, qué sucede si la variable de índice de matriz toma un valor que está fuera del conjunto de índices de la matriz. MiniZinc lo trata como una falla: un acceso de matriz :mzn:`a[e]` implícitamente agrega la restricción: :mzn:`e in index_set(a)` al contexto booleano circundante más cercano donde :mzn:`index_set(a)` da el conjunto de índices de :mzn:`a`.

.. defblock:: Tipos enumerados anónimos

  .. index::
    single: type; enumerated; anonymous

  Un *tipo enumerado anónimo* tiene la forma :mzndef:`anon_enum(<n>)` donde :mzndef:`<n>` es una expresión entera fija que define el tamaño del tipo enumerado.

  Un tipo enumerado anónimo es como cualquier otro tipo enumerado excepto que no tenemos nombres para sus elementos. Cuando se imprimen, reciben nombres únicos basados en el nombre del tipo enumerado.

Por lo tanto, por ejemplo, considere las declaraciones de variables

.. code-block:: minizinc

  array[1..2] of int: a= [2,3];
  var 0..2: x;
  var 2..3: y;

La restricción :mzn:`a[x] = y` tendrá éxito con :math:`x=1 \wedge y=2` y :math:`x=2 \wedge y=3`. Y la restricción :mzn:`not a[x] = y` tendrá éxito con :math:`x=0 \wedge y=2`, :math:`x=0 \wedge y=3`, :math:`x=1 \wedge y=3` y :math:`x=2 \wedge y=2`.

En el caso de los accesos de matriz no válida por un parámetro, la semántica formal de MiniZinc trata esto como un fallo para garantizar que el tratamiento de los parámetros y las variables de decisión sea coherente, pero se emite una advertencia ya que casi siempre es un error.

.. literalinclude:: examples/magic-series_es.mzn
  :language: minizinc
  :name: ex-magic-series
  :caption: Modelo que soluciona el problema de la serie mágica (:download:`magic-series_es.mzn <examples/magic-series_es.mzn>`).

.. index::
  single: bool2int
  single: constraint; higher order

La función de coerción :mzn:`bool2int` se puede invocar con cualquier expresión booleana. Esto permite que el modelador MiniZinc use las llamadas *restricciones de orden superior*. Como un simple ejemplo, considere el problema de *magic series*.
Encuentre una lista de números :math:`s= [s_0,\ldots,s_{n-1}]` tal que :math:`s_i` es el número de ocurrencias de :math:`i` en :math:`s`. Un ejemplo es :math:`s = [1,2,1,0]`. Un modelo MiniZinc para este problema se muestra en :numref:`ex-magic-series`. El uso de :mzn:`bool2int` nos permite resumir el número de veces que se cumple la restricción :mzn:`s[j]=i`.

Ejecutando el comando

.. code-block:: bash

  $ mzn-gecode --all-solutions magic-series_es.mzn -D "n=4;"

Conduce a la salida

.. code-block:: none

  s = [1, 2, 1, 0];
  ----------
  s = [2, 0, 2, 0];
  ----------
  ==========

Indicando exactamente dos soluciones al problema.

Tenga en cuenta que MiniZinc forzará automáticamente booleanos a enteros y enteros a flotantes cuando sea necesario. Podríamos reemplazar el elemento de restricción en :numref:`ex-magic-series` con:

.. code-block:: minizinc

  constraint forall(i in 0..n-1) (
     s[i] = (sum(j in 0..n-1)(s[j]=i)));

Y obtenga resultados idénticos, ya que la expresión booleana :mzn:`s[j] = i` se forzará automáticamente a un entero, efectivamente mediante el sistema MiniZinc que agrega automáticamente la falta :mzn:`bool2int`.

.. defblock:: Coercion

  .. index::
    single: coercion; automatic
    single: coercion; bool2int
    single: coercion; int2float

  En MiniZinc uno puede *forzar* un valor booleano a un valor entero usando la función :mzn:`bool2int`. De manera similar, se puede forzar un valor entero a un valor flotante usando :mzn:`int2float`. La instanciación del valor coaccionado es el mismo que el argumento. Por ejemplo, :mzn:`par bool` se fuerza a :mzn:`par int`, mientras que :mzn:`var bool` se fuerza a :mzn:`var int` MiniZinc coacciona automáticamente expresiones booleanas a expresiones enteras y expresiones enteras a expresiones flotantes, insertando :mzn:`bool2int` y :mzn:`int2float` en el modelo de forma apropiada. Tenga en cuenta que también forzará booleanos a flotantes utilizando dos pasos.



Establecer Restricciones
------------------------

.. index::
  single: constraint; set

Otra poderosa característica de modelado de MiniZinc es que permite que los conjuntos que contienen enteros sean variables de decisión. Esto significa que cuando se evalúa el modelo, el solucionador encontrará qué elementos están en el conjunto. Como un ejemplo simple, considere el problema *0/1 de la mochila*.

Esta es una forma restringida del problema de la mochila en la que podemos elegir colocar el artículo en la mochila o no. Cada artículo tiene un peso y una ganancia y queremos encontrar qué opción de artículos conduce a la ganancia máxima sujeta a que la mochila no esté demasiado llena.

Es natural modelar esto en MiniZinc con una única variable de decisión: :mzn:`var set of ITEM: knapsack` donde :mzn:`ITEM` es el conjunto de elementos posibles. Si los arreglos :mzn:`weight[i]` y :mzn:`profit[i]` respectivamente dan el peso y el beneficio del ítem :mzn:`i`, y el peso máximo que puede llevar la mochila viene dado por :mzn:`capacity` luego se da un modelo naural en :numref:`ex-knapsack-binary`.

.. literalinclude:: examples/knapsack_es.mzn
  :language: minizinc
  :name: ex-knapsack-binary
  :caption: Modelo para el problema de mochila 0/1 (:download:`knapsack_es.mzn <examples/knapsack_es.mzn>`).

Observe que la palabra clave :mzn:`var` viene antes de la declaración :mzn:`set` que indica que el conjunto en sí es la variable de decisión.

Esto contrasta con una matriz en la que la palabra clave :mzn:`var` califica los elementos en la matriz en lugar de la matriz en sí, ya que la estructura básica de la matriz es fija, es decir, su conjunto de índices.

.. literalinclude:: examples/social-golfers_es.mzn
  :language: minizinc
  :name: ex-social-golfers
  :caption: Modelo para los problemas de los golfistas sociales (:download:`social-golfers_es.mzn <examples/social-golfers_es.mzn>`).

Como un ejemplo más complejo de restricción de conjuntos, considere el problema de los golfistas sociales que se muestra en :numref:`ex-social-golfers`. El objetivo es programar un torneo de golf sobre :mzn:`weeks` usando :mzn:`groups` :math:`\times` :mzn:`size` de golfistas. Cada semana tenemos que programar :mzn:`groups` diferentes grupos cada uno de tamaño :mzn:`size`. No hay dos pares de golfistas que jueguen en dos grupos

Las variables en el modelo son conjuntos de golfistas :mzn:`Sched[i,j]` para el grupo de la semana :math:`i^{th}` del grupo :mzn:`j^{th}`.


Las restricciones que se muestran en las líneas 11-32 primero imponen un orden en el primer conjunto de cada semana para eliminar la simetría en el intercambio de semanas. A continuación, imponen un orden en los conjuntos en cada semana y hacen que cada conjunto tenga una cardinalidad de :mzn:`size`. Luego se aseguran de que cada semana sea una partición del conjunto de golfistas que utilizan la restricción global :mzn:`partition_set`. Finalmente, la última restricción asegura que no hay dos jugadores que jueguen en dos grupos juntos (ya que la cardinalidad de la intersección de dos grupos es como máximo 1).

.. index::
  single: symmetry; breaking

También hay restricciones de inicialización de ruptura de simetría que se muestran en las líneas 34-46: la primera semana se fija para tener todos los jugadores en orden; la segunda semana se compone de los primeros jugadores de cada uno de los primeros grupos en la primera semana; finalmente, el modelo obliga a los primeros jugadores :mzn:`size` a aparecer en su número de grupo correspondiente durante las semanas restantes.

Ejecutando el comando:

.. code-block:: bash

  $ mzn-gecode social-golfers_es.mzn social-golfers_es.dzn

Donde el archivo de datos define un problema con 4 semanas, con 4 grupos de tamaño 3 y que conduce a la salida:

.. code-block:: none

  1..3 4..6 7..9 10..12
  { 1, 4, 7 } { 2, 5, 10 } { 3, 9, 11 } { 6, 8, 12 }
  { 1, 5, 8 } { 2, 6, 11 } { 3, 7, 12 } { 4, 9, 10 }
  { 1, 6, 9 } { 2, 4, 12 } { 3, 8, 10 } { 5, 7, 11 }
  ----------

Observe cómo conjuntos que son rangos pueden salir en formato de rango.


Poniendolo Todo Junto
---------------------

Terminamos esta sección con un ejemplo complejo que ilustra la mayoría de las características presentadas en este capítulo, incluidos los tipos enumerados, las restricciones complejas, las restricciones globales y los resultados complejos.

.. literalinclude:: examples/wedding_es.mzn
  :language: minizinc
  :name: ex-wedding
  :caption: Planificación de asientos de bodas utilizando tipos enumerados (:download:`wedding_es.mzn <examples/wedding_es.mzn>`).

El modelo de :numref:`ex-wedding` organiza los asientos en la mesa de la boda.
La mesa tiene 12 asientos numerados en orden alrededor de la mesa, 6 en cada lado. Los hombres deben sentarse en asientos con números impares y las mujeres en parejas. Ed no puede sentarse al final de la mesa debido a una fobia, y la novia y el novio deben sentarse uno al lado del otro. El objetivo es maximizar la distancia entre los odios conocidos. La distancia entre asientos es la diferencia en el número de asiento si está en el mismo lado, de lo contrario es la distancia al asiento opuesto + 1.

Tenga en cuenta que en la declaración de salida consideramos cada asiento :mzn:`s` y la búsqueda de un invitado :mzn:`g` que está asignado a ese asiento. Hacemos uso de la función incorporada :mzn:`fix` que verifica si una variable de decisión es fija y devuelve su valor fijo, y de lo contrario lo cancela. Esto es siempre seguro de usar en declaraciones de salida, ya que para el momento en que se ejecuta la declaración de salida, todas las variables de decisión deben ser corregidas.


Ejecutando:

.. code-block:: bash

  $ mzn-gecode wedding_es.mzn

Resultados en la salida:

.. code-block:: none

  ted bride groom rona ed carol ron alice bob bridesmaid bestman clara
  ----------
  ==========


La colocación de la tabla resultante se ilustra en :numref:`fig-wedding` donde las líneas indican odios. La distancia total es 22.

.. _fig-wedding:

.. figure:: figures/wedding_es.*

  Disposición de los asientos en la mesa de la boda


.. \pjs{Move the fix definition elsewhere!}

.. defblock:: Fijar (Fix)

  .. index::
    single: fix
    single: fixed

  En los elementos de salida, la función incorporada :mzn:`fix` comprueba que el valor de una variable de decisión sea fijo y coacciona la instanciación de la variable de decisión al parámetro.

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
