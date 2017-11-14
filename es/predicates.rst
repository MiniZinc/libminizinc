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
Los modelos :download:`send-more-money_es.mzn <examples/send-more-money_es.mzn>` (:numref:`ex-smm`) y :download:`sudoku_es.mzn <examples/sudoku_es.mzn> `(:numref:`ex-sudoku`) son ejemplos de modelos que usan :mzn:`alldifferent`.



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
  :caption: Model for moving furniture using ``cumulative`` (:download:`moving_es.mzn <examples/moving_es.mzn>`).

.. literalinclude:: examples/moving_es.dzn
  :language: minizinc
  :name: ex-movingd
  :caption: Data for moving furniture using ``cumulative`` (:download:`moving_es.dzn <examples/moving_es.dzn>`).

El modelo en :numref:`ex-moving` encuentra un cronograma para mover los muebles de modo que cada mueble tenga suficientes manipuladores (personas) y suficientes carritos disponibles durante el movimiento. Se proporciona el tiempo disponible, los manipuladores y los carros, y los datos proporcionan para cada objeto la duración del movimiento, el número de manipuladores y la cantidad de carros requeridos.

Usando los datos mostrados en :mzn:`ex-movingd`, el comando

.. code-block:: bash

  $ mzn-gecode moving_es.mzn moving_es.dzn

Puede dar como resultado la salida

::

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
  :caption: Model for meal planning using ``table`` constraint (:download:`meal_es.mzn <examples/meal_es.mzn>`).

.. literalinclude:: examples/meal_es.dzn
  :language: minizinc
  :name: ex-meald
  :caption: Data for meal planning defining the ``table`` used (:download:`meal_es.dzn <examples/meal_es.dzn>`).

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
  :caption: Model for nurse rostering using ``regular`` constraint (:download:`nurse_es.mzn <examples/nurse_es.mzn>`)

Ejecutando el comando

.. code-block:: bash

  $ mzn-gecode nurse_es.mzn nurse_es.dzn

Encuentra un horario de 10 días para 7 enfermeras, que requiere 3 en cada turno de día y 2 en cada turno de noche, con un mínimo de 2 turnos de noche por enfermera.
Un posible resultado es:

::

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
  :caption: Model for job shop scheduling using predicates (:download:`jobshop2_es.mzn <examples/jobshop2_es.mzn>`)

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

.. defblock:: Predicate definitions

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
  :caption: Model for a number placement problem illustrating the use of functions (:download:`manhattan_es.mzn <examples/manhattan_es.mzn>`).

.. defblock:: Function definitions

  .. index::
    single: function; definition

Las funciones se definen mediante una declaración de la forma

  .. code-block:: minizincdef

    function <ret-type> : <func-name> ( <arg-def>, ..., <arg-def> ) = <exp>

  The :mzndef:`<func-name>` must be a valid MiniZinc identifier, and each :mzndef:`<arg-def>` is a valid MiniZinc type declaration.
  The :mzndef:`<ret-type>` is the return type of the function which must be the type of :mzndef:`<exp>`. Arguments have the same restrictions as in predicate definitions.

Funciones en MiniZinc pueden tener cualquier tipo de retorno, no solo tipos de retorno fijos.

Las funciones son útiles para definir y documentar expresiones complejas que se usan varias veces en un modelo.


Funciones de reflexión
----------------------

To help write generic tests and predicates, various reflection functions return information about array index sets, var set domains and decision variable ranges. Those for index sets are :mzndef:`index_set(<1-D array>)`, :mzndef:`index_set_1of2(<2-D array>)`, :mzndef:`index_set_2of2(<2-D array>)`, and so on for higher dimensional arrays.

A better model of the job shop conjoins all the non-overlap constraints for a single machine into a single disjunctive constraint.
An advantage of this approach is that while we may initially model this simply as a conjunction of :mzn:`non-overlap` constraints, if the underlying solver has a better approach to solving disjunctive constraints we can use that instead, with minimal changes to our model. The model is shown in :numref:`ex-jobshop3`.


.. literalinclude:: examples/jobshop3_es.mzn
  :language: minizinc
  :name: ex-jobshop3
  :caption: Model for job shop scheduling using ``disjunctive`` predicate (:download:`jobshop3_es.mzn <examples/jobshop3_es.mzn>`).

.. index::
  single: global constraint; disjunctive

The :mzn:`disjunctive` constraint takes an array of start times for each task and an array of their durations and makes sure that only one task is active at any one time. We define the disjunctive constraint as a :index:`predicate <predicate; definition>` with signature

.. code-block:: minizinc

  predicate disjunctive(array[int] of var int:s, array[int] of int:d);

We can use the disjunctive constraint to define the non-overlap of tasks as shown in :numref:`ex-jobshop3`.
We assume a definition for the :mzn:`disjunctive` predicate is given by the file :download:`disjunctive_es.mzn <examples/disjunctive_es.mzn>` which is included in the model.
If the underlying system supports :mzn:`disjunctive` directly, it will include a file :download:`disjunctive_es.mzn <examples/disjunctive_es.mzn>` in its globals directory (with contents just the signature definition above).

If the system we are using does not support disjunctive directly we can give our own definition by creating the file :download:`disjunctive_es.mzn <examples/disjunctive_es.mzn>`.

The simplest implementation simply makes use of the :mzn:`no_overlap` predicate defined above.
A better implementation is to make use of a global :mzn:`cumulative` constraint assuming it is supported by the underlying solver. :numref:`ex-disj` shows an implementation of :mzn:`disjunctive`.

Note how we use the :mzn:`index_set` reflection function to (a) check that the arguments to :mzn:`disjunctive` make sense, and (b) construct the array of resource utilisations of the appropriate size for :mzn:`cumulative`.
Note also that we use a ternary version of :mzn:`assert` here.

.. literalinclude:: examples/disjunctive_es.mzn
  :language: minizinc
  :name: ex-disj
  :caption: Defining a ``disjunctive`` predicate using ``cumulative`` (:download:`disjunctive_es.mzn <examples/disjunctive_es.mzn>`).

.. \ignore{ % for capture for testing!
.. $ mzn-g12fd jobshop3_es.mzn jobshop_es.dzn
.. } % $



Local Variables
---------------

.. index::
  single: variable; local
  single: let

It is often useful to introduce *local variables* in a predicate, function or test.
The :mzn:`let` expression allows you to do so.
It can be used to introduce both decision :index:`variables <variable>` and :index:`parameters <parameter>`, but parameters must be initialised. For example:

.. code-block:: minizinc

  var s..e: x;
  let {int: l = s div 2; int: u = e div 2; var l .. u: y;} in x = 2*y

introduces parameters :mzn:`l` and :mzn:`u` and variable :mzn:`y`.

While most useful in :index:`predicate`, :index:`function` and test definitions, :mzn:`let` expressions can also be used in other expressions, for example for eliminating common subexpressions:

.. code-block:: minizinc

  constraint let { var int: s = x1 + x2 + x3 + x4 } in
             l <= s /\ s <= u;

Local variables can be used anywhere and can be quite useful for simplifying complex expressions.
:numref:`ex-wedding2` gives a revised version of the wedding model, using local variables to define the :index:`objective` function, rather than adding lots of variables to the model explicitly.

.. literalinclude:: examples/wedding2_es.mzn
  :language: minizinc
  :name: ex-wedding2
  :caption: Using local variables to define a complex objective function (:download:`wedding2_es.mzn <examples/wedding2_es.mzn>`).


Context
-------

.. index::
  single: context
  single: context; negative
  single: predicate
  single: function

One limitation is that predicates and functions containing decision variables that are not initialised in the declaration cannot be used inside a negative context.
The following is illegal:

.. code-block:: minizinc

  predicate even(var int:x) =
            let { var int: y } in x = 2 * y;

  constraint not even(z);

The reason for this is that solvers only solve existentially constrained problems, and if we introduce a local variable in a negative context, then the variable is *universally quantified* and hence out of scope of the underlying solvers. For example the :math:`\neg \mathit{even}(z)` is equivalent to :math:`\neg \exists y. z = 2y` which is equivalent to :math:`\forall y. z \neq 2y`.

If local variables are given values, then they can be used in negative contexts. The following is legal

.. code-block:: minizinc

  predicate even(var int:x) =
            let { var int: y = x div 2; } in x = 2 * y;

  constraint not even(z);

Note that the meaning of :mzn:`even` is correct, since if :mzn:`x` is even then :math:`x = 2 * (x ~\mbox{div}~ 2)`. Note that for this definition :math:`\neg \mathit{even}(z)` is equivalent to :math:`\neg \exists y. y = z ~\mbox{div}~ 2 \wedge z = 2y` which is equivalent to :math:`\exists y. y = z ~\mbox{div}~ 2 \wedge \neg z \neq 2y`, because :math:`y` is functionally defined by :math:`z`.


Every expression in MiniZinc appears in one of the four *contexts*: :index:`root <context; !root>`, :index:`positive <context; !positive>`, :index:`negative <context; !negative>`, or :index:`mixed <context; !mixed>`.
The context of a non-Boolean expression is simply the context of its nearest enclosing Boolean expression. The one exception is that the objective expression appears in a root context (since it has no enclosing Boolean expression).

For the purposes of defining contexts we assume implication expressions :mzn:`e1 -> e2` are rewritten equivalently as :mzn:`not e1 \/ e2`, and similarly :mzn:`e1 <- e2` is rewritten as  :mzn:`e1 \/ not e2`.

The context for a Boolean expression is given by:

Root:
root context is the context for any expression $e$ appearing as the argument of :mzn:`constraint` or as an :index:`assignment` item, or appearing as a sub expression :mzn:`e1` or :mzn:`e2` in an expression :mzn:`e1 /\ e2` occuring in a root context.

Root context Boolean expressions must hold in any model of the problem.

Positive:
positive context is the context for any expression appearing as a sub expression :mzn:`e1` or :mzn:`e2` in an expression :mzn:`e1 \/ e2` occuring in a root or positive context, appearing as a sub expression :mzn:`e1` or :mzn:`e2` in an expression :mzn:`e1 /\ e2` occuring in a positive context, or appearing as a sub expression :mzn:`e` in an expression :mzn:`not e` appearing in a negative context.

Positive context Boolean expressions need not hold in a model, but making them hold will only increase the possibility that the enclosing constraint holds. A positive context expression has an even number of negations in the path from the enclosing root context to the expression.

Negative:
negative context is the context for any expression appearing as a sub expression :mzn:`e1` or :mzn:`e2` in an expression :mzn:`e1 \/ e2` or :mzn:`e1 /\ e2` occuring in a negative context, or appearing as a sub expression :mzn:`e` in an expression :mzn:`not e` appearing in a positive context.

Negative context Boolean expressions need not hold in a model, but making them false will increase the possibility that the enclosing constraint holds. A negative context expression has an odd number of negations in the path from the enclosing root context to the expression.

Mixed:
mixed context is the context for any Boolean expression appearing as a subexpression :mzn:`e1` or :mzn:`e2` in :mzn:`e1 <-> e2`, :mzn:`e1 = e2`, or :mzn:`bool2int(e)`.

Mixed context expression are effectively both positive and negative. This can be seen from the fact that :mzn:`e1 <-> e2` is equivalent to :mzn:`(e1 /\ e2) \/ (not e1 /\ not e2)` and :mzn:`x = bool2int(e)` is equivalent to :mzn:`(e /\ x=1) \/ (not e /\ x=0)`.

Consider the code fragment

.. code-block:: minizinc

  constraint x > 0 /\ (i <= 4 -> x + bool2int(x > i) = 5);

Then :mzn:`x > 0` is in the root context, :mzn:`i <= 4}` is in a negative context, :mzn:`x + bool2int(x > i) = 5` is in a positive context, and :mzn:`x > i` is in a mixed context.



Local Constraints
-----------------

.. index::
  single: constraint; local

Let expressions can also be used to include local constraints, usually to constrain the behaviour of local variables.
For example, consider defining a square root function making use of only multiplication:

.. code-block:: minizinc

  function var float: mysqrt(var float:x) =
           let { var float: y;
                 constraint y >= 0;
                 constraint x = y * y; } in y;

The local constraints ensure :mzn:`y` takes the correct value; which is then returned by the function.

Local constraints can be used in any let expression, though the most common usage is in defining functions.


.. defblock:: Let expressions

  .. index::
    single: expression; let

  :index:`Local variables <variable;local>`
  can be introduced in any expression with a *let expression*
  of the form:

  .. code-block:: minizincdef

    let { <dec>; ... <dec> ; } in <exp>

The declarations :mzndef:`<dec>` can be declarations of decision variables and parameters (which must be initialised) or constraint items.
No declaration can make use of a newly declared variable before it is introduced.

Note that local variables and constraints cannot occur in tests.
Local variables cannot occur in predicates or functions that appear in a :index:`negative <context; negative>` or :index:`mixed <context; mixed>` context, unless the variable is defined by an expression.


Domain Reflection Functions
---------------------------

.. index::
  single: domain; reflection

Other important reflection functions are those that allow us to access the domains of variables. The expression :mzn:`lb(x)` returns a value that is lower than or equal to any value that :mzn:`x` may take in  a solution of the problem. Usually it will just be the declared lower :index:`bound <variable; bound>` of :mzn:`x`.
If :mzn:`x` is declared as a non-finite type, e.g. simply :mzn:`var int` then it is an error.
Similarly the expression :mzn:`dom(x)` returns a (non-strict) superset of the possible values of :mzn:`x` in any solution of the problem.
Again it is usually the declared values, and again if it is not declared as finite then there is an error.

.. \ignore{ % for capture for testing!
.. $ mzn-g12fd reflection_es.mzn
.. } % $


.. literalinclude:: examples/reflection_es.mzn
  :language: minizinc
  :name: ex-reflect
  :caption: Using reflection predicates (:download:`reflection_es.mzn <examples/reflection_es.mzn>`).

For example, the model show in :numref:`ex-reflect` may output

::

  y = -10
  D = -10..10
  ----------

or

::

  y = 0
  D = {0, 1, 2, 3, 4}
  ----------

or any answer with :math:`-10 \leq y \leq 0` and :math:`\{0, \ldots, 4\} \subseteq D \subseteq \{-10, \ldots, 10\}`.

Variable domain reflection expressions should be used in a manner where they are correct for any safe approximations, but note this is not checked!

For example the additional code

.. code-block:: minizinc

  var -10..10: z;
  constraint z <= y;

is not a safe usage of the domain information.
Since using the tighter (correct) approximation leads to more solutions than the weaker initial approximation.

.. TODO: this sounds wrong!

.. defblock:: Domain reflection

  .. index::
    single: domain; reflection

  There are reflection functions to interrogate the possible values of expressions containing variables:

  - :mzndef:`dom(<exp>)`
    returns a safe approximation to the possible values of the expression.
  - :mzndef:`lb(<exp>)`
    returns a safe approximation to the lower bound value of the expression.
  - :mzndef:`ub(<exp>)`
    returns a safe approximation to the upper bound value of the expression.

  The expressions for :mzn:`lb` and :mzn:`ub` can only be of types :mzn:`int`, :mzn:`bool`, :mzn:`float` or :mzn:`set of int`.
  For :mzn:`dom` the type cannot be :mzn:`float`.
  If one of the variables appearing in :mzndef:`<exp>` has a :index:`non-finite declared type <type; non-finite>` (e.g. :mzn:`var int` or :mzn:`var float`) then an error can occur.

  There are also versions that work directly on arrays of expressions (with similar restrictions):

  - :mzndef:`dom_array(<array-exp>)`: returns a safe approximation to the union of all possible values of the expressions appearing in the array.
  - :mzndef:`lb_array(<array-exp>)`: returns a safe approximation to the lower bound of all expressions appearing in the array.
  - :mzndef:`ub_array(<array-exp>)`: returns a safe approximation to the upper bound of all expressions appearing in the array.

The combinations of predicates, local variables and domain reflection allows the definition of complex global constraints by decomposition.
We can define the time based decomposition of the :mzn:`cumulative` constraint using the code shown in :numref:`ex-cumul`.

.. literalinclude:: examples/cumulative_es.mzn
  :language: minizinc
  :name: ex-cumul
  :caption: Defining a ``cumulative`` predicate by decomposition (:download:`cumulative_es.mzn <examples/cumulative_es.mzn>`).

The decomposition uses :mzn:`lb` and :mzn:`ub` to determine the set of times :mzn:`times` over which tasks could range.
It then asserts for each time :mzn:`t` in :mzn:`times` that the sum of resources for the active tasks at time :mzn:`t` is less than the bound :mzn:`b`.

Scope
-----

.. index::
  single: scope

It is worth briefly mentioning the scope of declarations in MiniZinc.
MiniZinc has a single namespace, so all variables appearing in declarations are visible in every expression in the model.
MiniZinc introduces locally scoped variables in a number of ways:

- as :index:`iterator <variable; iterator>`
  variables in :index:`comprehension` expressions
- using :mzn:`let` expressions
- as predicate and function :index:`arguments <argument>`

Any local scoped variable overshadows the outer scoped variables of the same name.

.. literalinclude:: examples/scope_es.mzn
  :language: minizinc
  :name: ex-scope
  :caption: A model for illustrating scopes of variables (:download:`scope_es.mzn <examples/scope_es.mzn>`).

For example, in the model shown in :numref:`ex-scope` the :mzn:`x` in :mzn:`-x <= y` is the global :mzn:`x`, the :mzn:`x` in :mzn:`smallx(x)` is the iterator :mzn:`x in 1..u`, while the :mzn:`y` in the disjunction is the second argument of the predicate.
