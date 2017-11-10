.. _sec-efficient:

Prácticas de modelado efectivas en MiniZinc
===========================================

Existen casi siempre múltiples formas de modelar el mismo problema, algunas de las cuales generan modelos que son eficientes de resolver y otros que no son.
En general, es muy difícil determinar a priori qué modelos son los más eficientes para resolver un problema en particular, y de hecho puede depender críticamente del solver subyacente utilizado y de la estrategia de búsqueda. En este capítulo nos concentramos en las prácticas de modelado que evitan la ineficiencia en la generación de modelos y de los modelos generados.

Variable Bounds
---------------

.. index::
  single: variable; bound

Los motores de propagación de dominio finito, que son el principal tipo de solución objetivo de MiniZinc, son más efectivos cuanto más estrictos sean los límites de las variables involucradas. También pueden comportarse mal con problemas que tienen subexpresiones que toman valores enteros grandes, ya que pueden limitar implícitamente el tamaño de las variables enteras.

.. literalinclude:: examples/grocery_es.mzn
  :language: minizinc
  :name: ex-grocery
  :caption: Un modelo con variables no acotadas (:download:`grocery_es.mzn <examples/grocery_es.mzn>`).

The grocery problem shown in :numref:`ex-grocery` finds 4 items whose prices in dollars add up to 7.11 and multiply up to 7.11. The variables are declared unbounded. Running

El problema de comestibles que se muestra en :numref:`ex-grocery` encuentra 4 elementos cuyos precios en dólares suman 7.11 y se multiplican hasta 7.11. Las variables son declaradas no acotadas. Corriendo

.. code-block:: bash

  $ mzn-g12fd grocery_es.mzn

yields

::

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

::

  {120,125,150,316}
  ----------

Sin embargo, tenga en cuenta que incluso el modelo mejorado puede ser demasiado difícil para algunos solucionadores.
Corriendo

.. code-block:: bash

  $ mzn-g12lazy grocery_es.mzn

no devuelve una respuesta, ya que el solucionador crea una gran representación para las variables intermedias del producto.

.. defblock:: Bounding variables

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

::

  mark = [0, 1, 4, 6];
  diffs = [0, 0, 0, 0, 1, 0, 0, 0, 4, 3, 0, 0, 6, 5, 2, 0];
  ----------

y todo parece estar bien con el modelo.

Pero si requerimos todas las soluciones utilizando

.. code-block:: bash

  $ mzn-g12fd -a golomb_es.mzn -D "n = 4; m = 6;"

¡Se nos presenta una lista interminable de la misma solución!

¿Qué está pasando? Para que el solucionador de dominio finito termine, debe de  corregir todas las variables, incluidas las variables :mzn:`diff [i, j]` donde :mzn:`i <= j`, lo que significa que hay innumerables formas de generar un solución, simplemente cambiando estas variables para tomar valores arbitrarios.

We can avoid problems with unconstrained variables, by modifying the model so that they are fixed to some value. For example replacing the lines marked :mzn:`% (diff}` in :numref:`ex-unc` to

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

::

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

You can use the builitin :mzn:`trace` :index:`function <trace>` to help
determine what is happening inside generators.

Puedes usar builitin :mzn:`trace` :index:`function <trace>` para ayudar
determinar qué está sucediendo dentro de los generadores

.. defblock:: Tracing

La función :mzn:`trace (s, e)` imprime la cadena :mzn:`s` antes de evaluar la expresión :mzn:`e` y devuelve su valor.
Se puede usar en cualquier contexto.


Por ejemplo, podemos ver cuántas veces se realiza la prueba en el interior
bucle para ambas versiones del cálculo.

.. literalinclude:: examples/count1_es.mzn
  :language: minizinc
  :lines: 8-15

Produce el resultado:

::

  ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  ----------

indicando el bucle interno se evalúa 64 veces mientras

.. literalinclude:: examples/count2_es.mzn
  :language: minizinc
  :lines: 13-14

Produce el resultado:

::

  ++++++++++++++++
  ----------

indicando el bucle interno se evalúa 16 veces.

Tenga en cuenta que puede usar las cadenas dependientes en :mzn:`trace` para comprender lo que está sucediendo durante la creación del modelo.

.. literalinclude:: examples/count3_es.mzn
  :language: minizinc
  :lines: 13-15


imprimirá cada uno de los triángulos que se encuentran en el cálculo.

Produce la salida:

::

  (1,2,3)
  ----------




Restricciones redundantes
-------------------------

.. index::
  single: constraint; redundant

The form of a model will affect how well the constraint solver can solve it.
In many cases adding constraints which are redundant, i.e. are logically implied by the existing model, may improve the search for solutions by making more information available to the solver earlier.

La forma de un modelo afectará qué tan bien puede resolverlo el solucionador de restricciones.
En muchos casos, la adición de restricciones que son redundantes, es decir, están lógicamente implícitas en el modelo existente, puede mejorar la búsqueda de soluciones al hacer que el solucionador tenga más información antes.


Considere el problema de la serie mágica de :ref:`sec-complex`.
Ingresando para :mzn:`n = 16` de la siguiente manera:

.. code-block:: bash

  $ mzn-g12fd --all-solutions --statistics magic-series_es.mzn -D "n=16;"

puede resultar en la salida

::

  s = [12, 2, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0];
  ----------
  ==========

y las estadísticas muestran 174 puntos de elección requeridos.
We can add redundant constraints to the model. Since each number
in the sequence counts the number of occurrences of a number we know
that they sum up to :mzn:`n`. Similarly we know that the sum of
:mzn:`s[i] * i` must also add up to :mzn:`n` because the sequence is magic.
Adding these constraints
gives the model in
:numref:`ex-magic-series2`.

.. literalinclude:: examples/magic-series2_es.mzn
  :language: minizinc
  :name: ex-magic-series2
  :caption: Model solving the magic series problem with redundant constraints (:download:`magic-series2_es.mzn <examples/magic-series2_es.mzn>`).

Running the same problem as before

.. code-block:: bash

  $ mzn-g12fd --all-solutions --statistics magic-series2_es.mzn -D "n=16;"

results in the same output, but with statistics showing just 13 choicepoints
explored. The redundant constraints have allowed the solver to prune the
search much earlier.


Modelling Choices
-----------------

There are many ways to model the same problem in MiniZinc,
although some may be more natural than others.
Different models may have very different efficiency of solving, and worse
yet, different models may be better or worse for different solving backends.
There are however some guidelines for usually producing better models:

.. defblock:: Choosing between models

  The better model is likely to have some of the following features

  - smaller number of variables, or at least those that are not
    functionally defined by other variables
  - smaller domain sizes of variables
  - more succinct, or direct, definition of the constraints of the model
  - uses global constraints as much as possible

  In reality all this has to be tempered by how effective the search is for
  the model.  Usually the effectiveness of search is hard to judge except by
  experimentation.

Consider the problem of finding permutations of :math:`n` numbers
from 1 to :math:`n` such that the differences between adjacent numbers
also form a permutation of numbers 1 to :math:`n-1`.
Note that the :mzn:`u` variables are functionally defined by
the :mzn:`x` variables so the raw search space is :math:`n^n`.
The obvious way to model this problem is shown in :numref:`ex-allint`.

.. literalinclude:: examples/allinterval_es.mzn
  :language: minizinc
  :name: ex-allint
  :caption: A natural model for the all interval series problem ``prob007`` in CSPlib (:download:`allinterval_es.mzn <examples/allinterval_es.mzn>`).

In this model the array :mzn:`x` represents the permutation of the :mzn:`n`
numbers and the constraints are naturally represented using :mzn:`alldifferent`.

Running the model

.. code-block:: bash

  $ mzn-g12fd -all-solutions --statistics allinterval_es.mzn -D "n=10;"

finds all solutions in 84598 choice points and 3s.

An alternate model uses array :mzn:`y` where :mzn:`y[i]` gives the
position of the number :mzn:`i` in the sequence.
We also model the positions of the differences using variables
:mzn:`v`. :mzn:`v[i]` is the position in the sequence where the absolute difference
:mzn:`i` occurs.  If the values of :mzn:`y[i]` and :mzn:`y[j]` differ by one
where :mzn:`j > i`, meaning the
positions are adjacent, then :mzn:`v[j-i]` is constrained to be the earliest
of these positions.
We can add two redundant constraints to this model:
since we know that a difference of :mzn:`n-1` must result, we know that
the positions of 1 and :mzn:`n` must be adjacent (:mzn:`abs( y[1] - y[n] ) = 1`),
which also tell us that the position of difference :mzn:`n-1` is
the earlier of :mzn:`y[1]` and :mzn:`y[n]`, i.e.
:mzn:`v[n-1] = min(y[1], y[n])`.
With this we can model the problem
as shown in :numref:`ex-allint2`. The output statement recreates the
original sequence :mzn:`x` from the array of positions :mzn:`y`.

.. literalinclude:: examples/allinterval2_es.mzn
  :language: minizinc
  :name: ex-allint2
  :caption: An inverse model for the all interval series problem ``prob007`` in CSPlib (:download:`allinterval2_es.mzn <examples/allinterval2_es.mzn>`).

The inverse model has the same size as the original model, in terms of
number of variables and domain sizes.  But the inverse model has a much more
indirect way of modelling the relationship between the :mzn:`y` and :mzn:`v` variables
as opposed to the relationship between :mzn:`x` and :mzn:`u` variables.
Hence we might expect the original model to be better.

The command

.. code-block:: bash

  $ mzn-g12fd --all-solutions --statistics allinterval2_es.mzn -D "n=10;"

finds all the solutions in  75536 choice points and 18s.
Interestingly, although the model is not as succinct here, the search on the
:mzn:`y` variables is better than searching on the :mzn:`x` variables.
The lack of succinctness means that even though the search requires
less choice it is substantially slower.

.. _sec-multiple-modelling-and-channels:

Multiple Modelling and Channels
-------------------------------

When we have two models for the same problem it may be
useful to use both models together by tying the variables in the two models
together, since each can give different information to the solver.

.. literalinclude:: examples/allinterval3_es.mzn
  :language: minizinc
  :name: ex-allint3
  :caption: A dual model for the all interval series problem ``prob007`` in CSPlib (:download:`allinterval3_es.mzn <examples/allinterval3_es.mzn>`).

:numref:`ex-allint3` gives a dual model combining features of
:download:`allinterval_es.mzn <examples/allinterval_es.mzn>` and :download:`allinterval2_es.mzn <examples/allinterval2_es.mzn>`.
The beginning of the model is taken from :download:`allinterval_es.mzn <examples/allinterval_es.mzn>`.
We then introduce the :mzn:`y` and :mzn:`v` variables from :download:`allinterval2_es.mzn <examples/allinterval2_es.mzn>`.
We tie the variables together using the
global
:mzn:`inverse` constraint:
:mzn:`inverse(x,y)` holds if :mzn:`y` is the inverse function of :mzn:`x` (and vice versa),
that is :mzn:`x[i] = j <-> y[j] = i`. A definition
is shown in :numref:`ex-inverse`.
The model does not include the constraints relating the
:mzn:`y` and :mzn:`v` variables, they are redundant (and indeed propagation
redundant)
so they do not add information for a
propagation solver. The :mzn:`alldifferent` constraints are also missing since
they are made redundant (and propagation redundant) by the inverse
constraints.
The only constraints are the relationships of the :mzn:`x` and :mzn:`u` variables
and the redundant constraints on :mzn:`y` and :mzn:`v`.

.. literalinclude:: examples/inverse_es.mzn
  :language: minizinc
  :name: ex-inverse
  :caption: A definition of the ``inverse`` global constraint (:download:`inverse_es.mzn <examples/inverse_es.mzn>`).

One of the benefits of the dual model is that there is more scope for
defining different search strategies.
Running the dual model,

.. code-block:: bash

  $ mzn-g12fd -all-solutions --statistics allinterval3_es.mzn -D "n=10;"

which uses the search strategy of
the inverse model, labelling the :mzn:`y` variables,
finds all solutions in 1714 choice points and 0.5s.
Note that running the same model with labelling on the :mzn:`x` variables
requires 13142 choice points and 1.5s.

Symmetry
--------

Symmetry is very common in constraint satisfaction and optimisation problems. To illustrate this, let us look again at the n-queens problem from :numref:`ex-queens`. The top left chess board in :numref:`fig-queens-sym` shows a solution to the 8-queens problems (labeled "original"). The remaining chess boards show seven symmetric variants of the same solution: rotated by 90, 180 and 270 degrees, and flipped vertically.

.. _fig-queens-sym:

.. figure:: figures/queens_symm.*

  Symmetric variants of an 8-queens solution


If we wanted to enumerate *all* solutions to the 8-queens problem, we could obviously save the solver some work by only enumerating *non-symmetric* solutions, and then generating the symmetric variants ourselves. This is one reason why we want to get rid of symmetry in constraint models. The other, much more important reason, is that the solver may also **explore symmetric variants of non-solution states!**

For example, a typical constraint solver may try to place the queen in column 1 into row 1 (which is fine), and then try to put the column 2 queen into row 3, which, at first sight, does not violate any of the constraints. However, this configuration cannot be completed to a full solution (which the solver finds out after a little search). :numref:`fig-queens-sym-unsat` shows this configuration on the top left chess board. Now nothing prevents the solver from trying, e.g., the second configuration from the left in the bottom row of :numref:`fig-queens-sym-unsat`, where the queen in column 1 is still in row 1, and the queen in column 3 is placed in row 2. Therefore, even when only searching for a single solution, the solver may explore many symmetric states that it has already seen and proven unsatisfiable before!

.. _fig-queens-sym-unsat:

.. figure:: figures/queens_symm_unsat.*

  Symmetric variants of an 8-queens unsatisfiable partial assignment

Static Symmetry Breaking
~~~~~~~~~~~~~~~~~~~~~~~~

The modelling technique for dealing with symmetry is called *symmetry breaking*, and in its simplest form, involves adding constraints to the model that rule out all symmetric variants of a (partial) assignment to the variables except one. These constraints are called *static symmetry breaking constraints*.

The basic idea behind symmetry breaking is to impose an *order*. For example, to rule out any vertical flips of the chess board, we could simply add the constraint that the queen in the first column must be in the top half of the board:

.. code-block:: minizinc

  constraint q[1] <= n div 2;

Convince yourself that this would remove exactly half of the symmetric variants in :numref:`fig-queens-sym`. In order to remove *all* symmetry, we need to work a bit harder.

Whenever we can express all symmetries as permutations of the array of variables, a set of *lexicographic ordering constraints* can be used to break all symmetry. For example, if the array of variables is called :mzn:`x`, and reversing the array is a symmetry of the problem, then the following constraint will break that symmetry:

.. code-block:: minizinc

  constraint lex_lesseq(x, reverse(x));

How about two-dimensional arrays? Lexicographic ordering works just the same, we only have to coerce the arrays into one dimension. For example, the following breaks the symmetry of flipping the array along one of the diagonals (note the swapped indices in the second comprehension):

.. code-block:: minizinc

  array[1..n,1..n] of var int: x;
  constraint lex_lesseq([ x[i,j] | i,j in 1..n ], [ x[j,i] | i,j in 1..n ]);

The great thing about using lexicographic ordering constraints is that we can add multiple ones (to break several symmetries simultaneously), without them interfering with each other, as long as we keep the order in the first argument the same.

For the n-queens problem, unfortunately this technique does not immediately apply, because some of its symmetries cannot be described as permutations of the :mzn:`q` array. The trick to overcome this is to express the n-queens problem in terms of Boolean variables that model, for each field of the board, whether it contains a queen or not. Now all the symmetries can be modeled as permutations of this array. Since the main constraints of the n-queens problem are much easier to express with the integer :mzn:`q` array, we simply use both models together and add channeling constraints between them, as explained in :ref:`sec-multiple-modelling-and-channels`.

The full model, with added Boolean variables, channeling constraints and symmetry breaking constraints is shown in :numref:`ex-queens-sym`. We can conduct a little experiment to check whether it successfully breaks all the symmetry. Try running the model with increasing values for :mzn:`n`, e.g. from 1 to 10, counting the number of solutions (e.g., by using the ``-s`` flag with the Gecode solver, or selecting "Print all solutions" as well as "Statistics for solving" in the IDE). You should get the following sequence of numbers of solutions: 1, 0, 0, 1, 2, 1, 6, 12, 46, 92. To verify the sequence, you can search for it in the *On-Line Encyclopedia of Integer Sequences* (http://oeis.org).

.. literalinclude:: examples/nqueens_sym_es.mzn
  :language: minizinc
  :name: ex-queens-sym
  :start-after: % Alternative
  :end-before: % search
  :caption: Partial model for n-queens with symmetry breaking (full model: :download:`nqueens_sym_es.mzn <examples/nqueens_sym_es.mzn>`).


Other Examples of Symmetry
~~~~~~~~~~~~~~~~~~~~~~~~~~

Many other problems have inherent symmetries, and breaking these can often make a significant difference in solving performance. Here is a list of some common cases:

- Bin packing: when trying to pack items into bins, any two bins that have
  the same capacity are symmetric.
- Graph colouring: When trying to assign colours to nodes in a graph such
  that adjacent nodes must have different colours, we typically model
  colours as integer numbers. However, any permutation of colours is again a
  valid graph colouring.
- Vehicle routing: if the task is to assign customers to certain vehicles,
  any two vehicles with the same capacity may be symmetric (this is similar
  to the bin packing example).
- Rostering/time tabling: two staff members with the same skill set may be
  interchangeable, just like two rooms with the same capacity or technical
  equipment.
