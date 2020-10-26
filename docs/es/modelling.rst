.. _sec-modelling:

Modelamiento básico en MiniZinc
===============================

.. highlight:: minizinc
  :linenothreshold: 5

En esta sección se presenta la estructura básica de un modelo MiniZinc utilizando dos ejemplos simples.

Nuestro primer ejemplo
----------------------

.. _fig-aust:

.. figure:: figures/aust.*

  Estados de Australia

Como nuestro primer ejemplo, imagine que se desea colorear el mapa de Australia, tal como se ve en la figura :numref:`fig-aust`.

Esta figura se compone de siete estados diferentes, cada uno de los estados debe de tener un color y cada región adyacentes tiene que poseer diferentes colores.

.. literalinclude:: examples/aust_es.mzn
  :language: minizinc
  :caption: Un modelo de MiniZinc :download:`aust_es.mzn <examples/aust_es.mzn>` para colorear los estados y territorios en Australia
  :name: ex-aust

Este problema se puede modelar muy fácil en MiniZinc. El modelo se muestra en :numref:`ex-aust`.

La primera línea del modelo corresponde a un comentario. Un comentario comienza con un ``%``, que indica que el resto de la línea es un comentario.

MiniZinc también tiene comentarios de bloque similares al estilo del lenguaje de programación C. Este tipo de comentario comienzan con ``/*`` y finalizan con un ``*/``.

La siguiente parte del modelo declara las variables en el modelo.

La línea

::

  int: nc = 3;

especifica un :index:`parámetro` en el problema, el cual es el número de colores a ser utilizados.

Los parámetros son similares a las variables (constantes) de la mayoría de los lenguajes de programación.

Estos deben de ser declarados y se debe de especificar un :index:`tipo` de dato.
En este caso, el tipo de dato es :mzn:`int`.

Además se le debe de dar un valor al parámetro mediante una :index:`asignación`.

MiniZinc permite incluir la asignación como parte de la declaración (como en la línea anterior) o ser una sentencia de asignación separada.
Así, la siguiente asignación de dos líneas, es equivalente a la asignación de una línea.

::

  int: nc;
  nc = 3;

A diferencia de las variables, en muchos lenguajes de programación a un parámetro sólo se le puede asignar un solo valor. En ese sentido, son denominadas constantes. Es un error que un parámetro posea más de una asignación.


Lo tipos de parámetros básicos son: :index:`enteros <integer>` (:mzn:`int`),
números de punto flotante  (:mzn:`float`),
:index:`boleanos <Boolean>` (:mzn:`bool`) y
:index:`cadenas <string>` (:mzn:`string`).
Arreglos y conjuntos también son soportados.

.. index::
  see: decision variable; variable


Los modelos MiniZinc también pueden contener otro tipo de variable llamadas *variables de decisión*.
Las :index:`variables de decisión <variable>` son variables en el sentido de las matemáticas o variables lógicas.
A diferencia de parámetros y variables en un lenguaje de programación estándar, el modelador no necesita darles un valor.

Más bien, el valor de una variable de decisión es desconocido. En consecuencia, es solamente cuando se ejecuta el modelo en MiniZinc, que el sistema de resolución determina si la variable de decisión se puede asignar un valor que satisface las restricciones en el modelo.

En nuestro modelo de ejemplo, nosotros asociamos una *variable de decisión* a cada estado:
``wa``, ``nt``, ``sa``, ``q``, ``nsw``, ``v`` y ``t`` que representa el color (desconocido) que se utilizará para llenar la región.

.. index::
  single: domain

Para cada variable de decisión se necesita dar el conjunto de los posibles valores que la variable puede tomar.
Esto se llama *dominio* de la variable. Esto se puede dar como parte de la :index:`declaración de la variable <variable; declaration>` y el :index:`tipo` de la variable de decisión se infiere a partir del tipo de los valores en el dominio.

En MiniZinc, las variables de decisión pueden ser booleanas, enteras, números de punto flotante o conjuntos.
También se admiten las matrices cuyos elementos son variables de decisión. En nuestro modelo MiniZinc usamos números enteros para modelar los diferentes colores.
Por lo tanto, cada una de las variables de decisión declarada posee el dominio :mzn:`1..nc`.
Este dominio declarado es una expresión de un intervalo de tipo entero indicado por el conjunto :math:`\{ 1, 2, \dots, nc \}` usado en la declaración :mzn:`var`. El tipo de los valores es entero, por lo que todas las variables del modelo son variables de decisión enteras.


.. defblock:: Identificadores

  Los identificadores, son utilizados para nombrar parámetros y variables,
  son secuencias de caracteres alfabéticos de minúsculas y mayúsculas, dígitos y el carácter ``_`` de subrayado .
  Además, deben de comenzar con un carácter alfabético. Así, ``myName_2`` es un identificador válido.

  Las *palabras clave* en MiniZinc no se pueden utilizar como nombres de identificador. Estas, se encuentran listadas en: ref:`spec-identifiers`.

  Tampoco se permite que los *operadores* de MiniZinc se utilicen como identificadores; el listado aparece en :ref:`spec-Operators`.


MiniZinc distingue cuidadosamente entre los dos tipos de variables del modelo: los parámetros y las variables de decisión. Los tipos de expresiones que se pueden construir usando variables de decisión son más restringidos que los que se pueden construir a partir de parámetros. Sin embargo, en cualquier lugar en que se pueda utilizar una variable de decisión, también se puede tener un parámetro del mismo tipo.


.. defblock:: Declaraciones de variables enteras

  Una :index:`variable de parámetro entero <variable; declaration; integer>` se declara como:

  .. code-block:: minizincdef

    int : <var-name>
    <l> .. <u> : <var-name>

  en donde :mzndef:`<l>` y :mzndef:`<u>` son expresiones enteras fijas.

  Una variable de decisión entera se declara como:

  .. code-block:: minizincdef

    var int : <var-name>
    var <l>..<u> : <var-name>

  en donde :mzndef:`<l>` y :mzndef:`<u>` son expresiones enteras fijas.


Formalmente la distinción entre parámetros y variables de decisión se denomina *instanciación* de la variable. La combinación de la instanciación variable y el tipo se llama tipo-instanciación (:index:`type-inst`).
A medida de que usted empiece a usar MiniZinc, sin duda verá ejemplos de errores de tipo-instanciación (:index:`type-inst`).

.. index::
  single: constraint

El siguiente componente del modelo son las *restricciones*. Estas especifican las expresiones booleanas que las variables de decisión deben satisfacer para ser una solución válida para el modelo. En este ejemplo, existe un número de restricciones no iguales entre las variables de decisión, las cuales hacen cumplir que si dos estados son adyacentes, entonces deben tener colores diferentes.

.. defblock:: Operadores relacionales

  MiniZinc proporciona los :index:`operadores relacionales <operator; relational>`:

  .. index::
    single: =
    single: ==
    single: !=
    single: <
    single: <=
    single: >
    single: >=

  igual (``=`` or ``==``), no igual
  (``!=``),
  estrictamente menor que (``<``),
  estrictamente mayor que (``>``),
  menor o igual a (``<=``), y
  mayor o igual que (``>=``).


La siguiente línea del modelo:

::

  solve satisfy;


indica que el tipo de problema que es. En este caso, es un problema de :index:`satisfacción`: es decir, se debe de encontrar un valor para las variables de decisión que satisface las restricciones pero no nos importa cuál.

.. index::
  single: output

La parte final del modelo es la declaración de *salida*. Esto le indica a MiniZinc lo que debe de imprimir cuando se ejecute el modelo y encuentre una :index:`solución`.

.. defblock:: Salida y cadenas

  .. index::
    single: output
    single: string
    single: show

  Una instrucción de salida es seguida por una *lista* de cadenas. Estos son típicamente :index:`string literales <string; literal>`
  que se escriben entre comillas dobles y utilizan una notación C para caracteres especiales,
  o una expresión de la forma :mzn:`show(e)`
  en donde :mzn:`e` es una espresión de MiniZinc.
  En el ejemplo ``\n`` representa el carácter de nueva línea y ``\t`` un tabulador.

  También hay variedades de formato :mzn:`show` para números:
  :mzn:`show_int(n,X)`
  salidas de valores enteros
  ``X`` en al menos :math:`|n|` carácteres, justificados a la derecha
  si :math:`n > 0` y de otro modo, justificado a la izquierda;
  :mzn:`show_float(n,d,X)`
  salidas de los valores de punto flotante (mzn:`float`) ``X`` en al menos :math:`|n|` carácteres, justificados a la derecha
  si :math:`n > 0` y de otro modo, justificado a la izquierda, con :math`d` carácteres después del punto decimal.

  Los :index:`Literales de cadenas <string; literal>` deben encajar en una sola línea. Los literales de cadena más largos se pueden dividir en varias líneas utilizando el operador de concatenación de cadenas ``++``.
  Por ejemplo, el literal

  ::

    "Archivo de datos no válido: La cantidad de harina no es negativa"

  es equivalente a la expresión literal de cadena

  ::

    "Archivo de datos no válido: " ++
    "La cantidad de harina no es negativa"

  MiniZinc soporta
  :index:`cadenas interpoladas <string; literal; interpolated>`.
  Las expresiones pueden ser incorporados directamente en las cadenas literales,
  donde una subcadena del forma :mzn:`"\(e)"`
  es reemplazada por el resultado :mzn:`show(e)`.
  Por ejemplo, :mzn:`"t=\(t)\n"` produce la misma cadena como :mzn:`"t=" ++ show(t) ++ "\n"`.

  Un modelo puede contener varias declaraciones de salida. En ese caso, todas las salidas se concatenan en el orden en que aparecen en el modelo.

Podemos evaluar nuestro modelo escribiendo

.. code-block:: bash

  $ mzn-gecode aust_es.mzn

en donde :download:`aust_es.mzn <examples/aust_es.mzn>` es el nombre del archivo que contiene nuestro modelo MiniZinc.
Se debe de utilizar la extensión de archivo ``.mzn`` para indicar un modelo MiniZinc.
El comando ``mzn-gecode`` utiliza el *solver* de dominios finitos Gecode para evaluar nuestro modelo.

Cuando ejecutamos esto obtenemos el resultado:

.. code-block:: none

  wa=2	 nt=3	 sa=1
  q=2	 nsw=3	 v=2
  t=1
  ----------


.. index::
  single: solution; separator ----------

La línea de 10 guiones ``----------`` es automáticamente agregada por la salida de MiniZinc, la cual indica que se ha encontrado una solución.


Un ejemplo de optimización aritmética
-------------------------------------

El segundo ejemplo está motivado por la necesidad de hornear pasteles para una fiesta en nuestra escuela local. Sabemos cómo hacer dos tipos de tortas. \footnote{Advertencia: por favor no utilice estas recetas en casa}
Un pastel de plátano que toma 250g de harina, 2 plátanos para puré de plátano, 75g de azúcar y 100g de mantequilla. Para un pastel de chocolate se necesita 200g de harina, 75g de cacao, 150g de azúcar y 150g de mantequilla.

Podemos vender un pastel de chocolate por $4.50 y un pastel de plátano por $4.00. Además, tenemos 4kg de harina, 6 plátanos, 2kg de azúcar, 500g de mantequilla y 500g de cacao.

La pregunta es, ¿cuántos de cada tipo de pastel debemos hornear para la fiesta para maximizar el beneficio?.

A continuación se muestra el posible modelo de MiniZinc en :numref:`ex-cakes`.

.. literalinclude:: examples/cakes_es.mzn
  :language: minizinc
  :caption: Modelo para determinar cuántos pasteles de banana y chocolate para hornear para la fiesta de la escuela (:download:`cakes_es.mzn <examples/cakes_es.mzn>`)
  :name: ex-cakes

.. index::
  single: expression; arithmetic

La primera nueva característica es el uso de *expresiones aritméticas*.

.. defblock:: Operadores Aritméticos Enteros

  .. index::
    single: operator; integer
    single: +
    single: -
    single: div
    single: *
    single: mod

  MiniZinc proporciona los estándar operadores aritméticos enteros.

  Adición (``+``),
  subtracción (``-``),
  multiplicación (``*``),
  división entera (:mzn:`div`)
  y
  módulo entero (:mzn:`mod`).
  También proporciona ``+`` y ``-``
  como operadores unarios.

  El módulo entero se define para dar un resultado :math:`a` :mzn:`mod` :math:`b`
  que tiene el mismo signo que el dividendo :math:`a`. La división entera se define de modo que
  :math:`a = b ` ``*`` :math:`(a` :mzn:`div` :math:`b) + (a` :mzn:`mod` :math:`b)`.

  MiniZinc proporciona funciones enteras estándar para el valor absoluto (:mzn:`abs`) y función de potencia (:mzn:`pow`).
  Por ejemplo, :mzn:`abs(-4)` y :mzn:`pow(2,5)` es evaluado como ``4`` and ``32`` respectivamente.

  La sintaxis para literales aritméticos es razonablemente estándar. Los literales enteros pueden ser decimales, hexadecimales u octales. Por ejemplo, ``0``, ``5``, ``123``, ``0x1b7``, ``0o777``.

.. index::
  single: optimization
  single: objective
  single: maximize
  single: minimize

La segunda característica mostrada en el ejemplo es la optimización. La línea

::

  solve maximize 400 * b + 450 * c;

especifica que queremos encontrar una solución que maximice la expresión declarada, esto se denomina *objetivo*.

El objetivo puede ser cualquier tipo de expresión aritmética. Se puede reemplazar la palabra clave
:mzn:`maximize`
por :mzn:`minimize` para especificar un problema de minimización.

Cuando ejecutamos esto obtenemos el resultado:

.. code-block:: none

  Número de tortas de plátano = 2
  Número de tortas de chocolate = 2
  ----------
  ==========

.. index::
  single: solution; end `==========`

La línea ``==========``
se muestra automáticamente para problemas de optimización cuando el sistema ha demostrado que una solución es óptima.


.. index::
  single: data file


Archivos de datos y afirmaciones
--------------------------------

Un inconveniente de este modelo es que si queremos resolver un problema similar, la próxima vez que necesitamos hornear pasteles para la escuela (que es a menudo) tenemos que modificar las restricciones en el modelo para reflejar los ingredientes que tenemos en la despensa. Si queremos reutilizar el modelo entonces es más conveniente dejar que la cantidad de cada ingrediente sea un parámetro del modelo y luego fijar sus valores en la parte superior del modelo.

Incluso mejor sería establecer el valor de estos parámetros en un archivo de datos separado (*data file*). MiniZinc (como la mayoría de los otros lenguajes de modelado) permite el uso de archivos de datos para establecer el valor de los parámetros declarado en el modelo original. Esto permite que el mismo modelo se utilice fácilmente con diferentes datos ejecutándolo con diferentes archivos de datos.


Los archivos de datos deben tener la extensión de archivo ``.dzn`` para indicar un archivo de datos MiniZinc y un modelo se puede ejecutar con cualquier número de archivos de datos (aunque una variable/parámetro sólo se puede asignar un valor en un archivo).

.. literalinclude:: examples/cakes2_es.mzn
  :language: minizinc
  :caption: Modelo independiente de los datos para determinar cuántos pasteles de banana y chocolate para hornear para la fiesta de la escuela (:download:`cakes2_es.mzn <examples/cakes2_es.mzn>`)
  :name: ex-cakes2

Nuestro nuevo modelo se muestra en :numref:`ex-cakes2`.
Podemos ejecutarlo usando el comando

.. code-block: bash

  $ mzn-gecode cakes2_es.mzn pantry.dzn

donde el archivo de datos :download:`pantry_es.dzn <examples/pantry_es.dzn>` es definido en
:numref:`fig-pantry1`. Esto da el mismo resultado que :download:`cakes_es.mzn <examples/cakes_es.mzn>`.
El resultado de la ejecución del comando

.. code-block:: bash

  $ mzn-gecode cakes2_es.mzn pantry2_es.dzn

con un conjunto de datos alternativo definido en
:numref:`fig-pantry2` is

.. code-block:: none

  Número de tortas de plátano = 3
  Número de tortas de chocolate = 8
  ----------
  ==========

Si eliminamos la instrucción de salida de :download:`cakes_es.mzn <examples/cakes_es.mzn>` entonces MiniZinc utilizará una salida predeterminada. En este caso, la salida resultante será

.. code-block:: none

  b = 3;
  c = 8;
  ----------
  ==========

.. defblock:: Salida por defecto

  A MiniZinc model with no output will output a line for each decision variable with its value, unless it is assigned an expression on its declaration. Note how the output is in the form of a correct datafile.

.. literalinclude:: examples/pantry_es.dzn
  :language: minizinc
  :caption: Ejemplo de archivo de datos para :download:`cakes2_es.mzn <examples/cakes2_es.mzn>` (:download:`pantry_es.dzn <examples/pantry_es.dzn>`)
  :name: fig-pantry1

.. literalinclude:: examples/pantry2_es.dzn
  :language: minizinc
  :caption: Ejemplo de archivo de datos para :download:`cakes2_es.mzn <examples/cakes2_es.mzn>` (:download:`pantry2_es.dzn <examples/pantry2_es.dzn>`)
  :name: fig-pantry2

Se pueden introducir pequeños archivos de datos sin crear un archivo de datos ``.dzn``, usando una :index:`bandera en la línea de comandos <data file;command line>`
``-D`` *cadena*,
en donde *cadena* es el contenido del archivo de datos. Por ejemplo, el comando

.. code-block:: bash

  $ mzn-gecode cakes2_es.mzn -D \
       "flour=4000;banana=6;sugar=2000;butter=500;cocoa=500;"

dará idénticos resultados que

.. code-block:: bash

  $ mzn-g12fd cakes2_es.mzn pantry_es.dzn

Los archivos de datos sólo pueden contener instrucciones de asignación para variables de decisión y parámetros en el modelo o modelos a los que están destinados.

.. index::
  single: assert

La programación defensiva sugiere que debemos comprobar que los valores en el archivo de datos son razonables. Para nuestro ejemplo es razonable comprobar que la cantidad de todos los ingredientes no es negativa y generar un error en tiempo de ejecución si esto no es cierto.

MiniZinc proporciona un operador (*built-in*) booleano incorporado para comprobar los valores de los parámetros.
La forma es :mzn:`assert(B,S)`. La expresión booleana ``B`` es evaluada, y si la ejecución es falsa se aborta y la expresión de cadena ``S`` se evalúa e imprime como un mensaje de error. Para comprobar y generar un mensaje de error apropiado si la cantidad de harina es negativa podemos simplemente agregar la línea

::

  constraint assert(flour >= 0,"La cantidad de harina no debe ser negativa");

a nuestro modelo. Observe que la espresión :mzn:`assert` es una expresión booleana y por lo tanto es considerado como un tipo de restricción. Podemos agregar líneas similares para comprobar que la cantidad de los otros ingredientes no es negativa.



Resolviendo números reales
--------------------------

Minizinc también soporta la resolución de restricciones con números reales usado en variables de punto flotante y restricciones.

Considere un problema de tomar un préstamo a corto por un año, el cual será reembolsado en 4 cuotas trimestrales.

Un modelo para esto se muestra en :numref:`ex-loan`. Utiliza un cálculo de interés simple para calcular el saldo después de cada trimestre.

.. literalinclude:: examples/loan_es.mzn
  :language: minizinc
  :caption: Modelo para determinar las relaciones entre un préstamo de 1 año y lo reembolsado cada trimestre (:download:`loan_es.mzn <examples/loan_es.mzn>`)
  :name: ex-loan

Tenga en cuenta que declaramos una variable flotante ``f`` similar a una variable entera utilizando la palabra clave :mzn:`float` en lugar de :mzn:`int`.

Podemos utilizar el mismo modelo para responder a una serie de preguntas diferentes.
La primera pregunta es: Si tomo prestado $1000 al 4% y debo pagar $260 por trimestre, cuánto puedo terminar debiendo?.
Esta pregunta está codificada por el archivo de datos :download:`loan1_es.dzn <examples/loan1_es.dzn>`.

Puesto que deseamos utilizar variables de números reales y restricción, necesitamos usar un *solver* que soporte este tipo de problema.
Un solver adecuado sería uno que soporta la programación lineal entera mixta.
La distribución MiniZinc contiene un *solver*.
La salida al ejecutar el comando ``mzn-cbc``:

.. code-block:: bash

  $ mzn-cbc loan_es.mzn loan1_es.dzn

La salida es

.. code-block:: none

  Préstamo 1000.00 al 4.0% interés y reembolso 260.00
  por trimestre para 1 año deja 65.78 debido
  ----------

La segunda pregunta es: Si tomo prestado $1000 al 4% y no debo nada al final, ¿cuánto tengo que pagar?.
Esta pregunta está codificada por el archivo de datos :download:`loan2_es.dzn <examples/loan2_es.dzn>`.
La salida al ejecutar el comando

.. code-block:: bash

  $ mzn-cbc loan_es.mzn loan2_es.dzn

La salida es

.. code-block:: none

  Préstamo 1000.00 al 4.0% interés y reembolso 275.49
  por trimestre para 1 año deja 0.00 debido
  ----------

La tercera pregunta, es si puedo pagar $250 por trimestre, ¿cuánto puedo pedir prestado al 4% para terminar por no pagar nada?.
Esta pregunta está codificada por el archivo de datos :download:`loan3_es.dzn <examples/loan3_es.dzn>`.
La salida al ejecutar el comando

.. code-block:: bash

  $ mzn-g12mip loan_es.mzn loan3_es.dzn

La salida es:

.. code-block:: none

  Préstamo 907.47 at 4.0% interés y reembolso 250.00
  por trimestre para 1 año deja 0.00 debido
  ----------

.. literalinclude:: examples/loan1_es.dzn
  :language: minizinc
  :caption: Ejemplo de archivo de datos para :download:`loan_es.mzn <examples/loan_es.mzn>` (:download:`loan1_es.dzn <examples/loan1_es.dzn>`)

.. literalinclude:: examples/loan2_es.dzn
  :language: minizinc
  :caption: Ejemplo de archivo de datos para :download:`loan_es.mzn <examples/loan_es.mzn>` (:download:`loan2.dzn <examples/loan2_es.dzn>`)

.. literalinclude:: examples/loan3_es.dzn
  :language: minizinc
  :caption: Ejemplo de archivo de datos para :download:`loan_es.mzn <examples/loan_es.mzn>` (:download:`loan3_es.dzn <examples/loan3_es.dzn>`)


.. defblock:: Operadores de Coma Flotante

  .. index:
    single: operator; float
    single: +
    single: -
    single: *
    single: /
    single: abs
    single: sqrt
    single: ln
    single: log2
    single: log10
    single: exp
    single: sin
    single: cos
    single: tan
    single: asin
    single: acos
    single: atan
    single: sinh
    single: cosh
    single: tanh
    single: asinh
    single: acosh
    single: atanh
    single: pow

  MiniZinc proporciona los siguiente operadores  estándar aritméticos de punto flotante:
  adición (``+``),
  subtracción (``-``),
  multiplicación (``*``)
  and floating point division (``/``).
  and floating point division ``+`` y ``-`` as unary operators.

  MiniZinc puede coaccionar de forma automática números enteros a los números de punto flotante. Pero para hacer la coerción explícita, la función incorporada
  :mzn:`int2float` puede ser usada. Obsérvese que una consecuencia de la coerción automática es que una expresión :mzn:`a / b` siempre se considera una división de punto flotante. Si necesita una división entera, asegúrese de usar el operador :mzn:`div`!.

  MiniZinc proporciona además las siguientes funciones de coma flotante:
  valor absoluto (``abs``),
  raíz cuadrada (``sqrt``),
  logaritmo natural (``ln``),
  logaritmo en base 2 (``log2``),
  logaritmo en base 10 (``log10``),
  exponenciación de $e$ (``exp``),
  seno (``sin``),
  coseno (``cos``),
  tangente (``tan``),
  arcoseno (``asin``),
  arco-coseno (``acos``),
  arco-tangente (``atan``),
  seno hiperbólico (``sinh``),
  coseno hiperbólico(``cosh``),
  tangente hiperbólico(``tanh``),
  arcoseno hiperbólico(``asinh``),
  arco-coseno hiperbólico(``acosh``),
  arco-tangente hiperbólico(``atanh``),
  y potencia (``pow``) que es la única función binaria, el resto son unarios.

  La sintaxis para literales aritméticos es razonablemente estándar. Ejemplo literales flotantes son ``1.05``, ``1.3e-5`` and ``1.3E+5``.

.. \pjs{Should do multiple solutions????}

Estructura básica de un modelo
------------------------------

Ahora estamos en condiciones de resumir la estructura básica de un modelo MiniZinc. La estructura esta compuesta por varios elementos (*items*), cada uno de los cuales tiene un punto y coma ``;``` al final. Los elementos pueden ocurrir en cualquier orden.

Por ejemplo, los identificadores no necesitan ser declarados antes de ser utilizados.

Hay 8 clases de :index:`elementos <item>`.

- :index:`Include items <item; include>` permiten que el contenido de otro archivo se inserte en el modelo.
  Estos tienen la forma:

  .. code-block:: minizincdef

    include <filename>;

  en donde, :mzndef:`<filename>` es una cadena literal.
  Permite que los modelos grandes se dividan en sub-modelos más pequeños y también la inclusión de restricciones definidas en los archivos de la biblioteca.
  Veremos un ejemplo en :numref:`ex-smm`.

- :index:`Variable declarations <item; variable declaration>` declaran nuevas variables.
  Estas variables son variables globales y pueden ser referidos a desde cualquier parte del modelo.
  Las variables pueden ser de dos tipos.
  Parámetros a los que se asigna un valor fijo en el modelo o en un archivo de datos y variables de decisión cuyo valor se encuentra sólo cuando se resuelve el modelo.
  Decimos que los parámetros son :index:`fixed` y variables de decisión :index:`unfixed`.
  A la variable se le puede asignar opcionalmente un valor como parte de la declaración.
  La forma es:

  .. index:
    single: expression; type-inst
    single: par
    single: var

  .. code-block:: minizincdef

    <type inst expr>: <variable> [ = ] <expression>;

  El :mzndef:`<type-inst expr>` da la instanciación y el tipo de la variable. Estos son uno de los aspectos más complejos de MiniZinc.
  Las instanciaciones se declaran utilizando :mzn:`par` para parámetros y
  :mzn:`var` para variables de decisión. Si no hay una declaración de instanciación explícita entonces la variable es un parámetro.
  El tipo puede ser un tipo base, como :index:`integer o float range <range>` o un arreglo o un conjunto.
  Los tipos bases son :mzn:`float`,
  :mzn:`int`,
  :mzn:`string`,
  :mzn:`bool`,
  :mzn:`ann`
  de los cuales solo
  :mzn:`float`, :mzn:`int` and :mzn:`bool` puede usarse para variables de decisión.
  El tipo de base :mzn:`ann` es una :index:`annotation` --
  Se analiza las anotaciones en :ref:`sec-search`.
  :index:`Integer range expressions <range; integer>` puede ser utilizado en lugar del tipo :mzn:`int`.
  Igualmente :index:`float range expressions <range; float>` puede utilizarse en lugar del tipo :mzn:`float`.
  Éstos se usan típicamente para dar el dominio de una variable de la decisión pero se pueden también utilizar para restringir el rango de un parámetro. Otro uso de las declaraciones de variables es definir :index:`enumerated types`, los cuales se discuten en :ref:`sec-enum`.

- :index:`Assignment items <item; assignment>` asignar un valor a una variable. Tienen la forma:

  .. code-block:: minizincdef

    <variable> = <expression>;

  Los valores se pueden asignar a las variables de decisión, en cuyo caso la asignación es equivalente a la escritura :mzndef:`constraint <variable> = <expression>`.

- :index:`Constraint items <item; constraint>` forman el corazón del modelo. Tienen la forma:

  .. code-block:: minizincdef

    constraint <Boolean expression>;

  Ya hemos visto ejemplos de restricciones simples usando la comparación aritmética y el operador :mzn:`assert` incorporado.
  En la siguiente sección veremos ejemplos de restricciones más complejas.

- :index:`Solve items <item; solve>` especifique exactamente qué tipo de solución se busca.
  Como hemos visto, tienen una de las siguientes tres formas:

  .. code-block:: minizincdef

    solve satisfy;
    solve maximize <arithmetic expression>;
    solve minimize <arithmetic expression>;

  Se requiere un modelo para tener exactamente un elemento de resolución.

- :index:`Output items <item; output>` son para presentar bien los resultados de la ejecución del modelo.
  Estas tienen la forma:

  .. code-block:: minizincdef

    output [ <string expression>, ..., <string expression> ];

  Si no hay ningún elemento de salida,
  por defecto MiniZinc imprimirá los valores de todas las variables de decisión. Estas variables no se les asignará opcionalmente un valor en el formato de los elementos de asignación.

- :index:`Enumerated type declarations <item; enum>`.
  Se discute eso en :ref:`sec-arrayset` y :ref:`sec-enum`.

- :index:`Predicate, function and test items <item; predicate>` son para definir nuevas restricciones, funciones y pruebas booleanas.
  Se discute eso en :ref:`sec-predicates`.

- El :index:`annotation item <item; annotation>` se utiliza para definir una nueva anotación. Se discute eso en :ref:`sec-search`.
