.. _sec-optiontypes:

Tipos de Opciones
=================

.. index::
  single: option types

Tipo de opciones son una abstraccion poderosa que permite modelar de forma concisa. Un tipo de opcion de variable de decicion representa una decicion que tiene otra posibilidad :math:`\top`, representada en MiniZinc como :mzn:`<>` indicando que la variable es *absent*.
Las deciciones de tipo de opcion son utiles para modelar problemas donde una decicion no es significativa a menos que otras deciciones sean hechas primero.


Declarando y Utilizando Tipos de Opciones
-----------------------------------------

.. defblock:: Tipo de Opción de Variables

  .. index::
    single: variable; option type

  Una variable de tipo de opción se declara como:

  .. code-block:: minizincdef

    var opt <type> : <var-name:

  Donde :mzndef:`<type>` es uno de :mzn:`int`, :mzn:`float` o :mzn:`bool` o una expresion de rango fijo.

  Las variables de tipo de opción pueden ser parámetros, pero esto rara vez es útil.
  Una variable de tipo de opción puede tomar el valor adicional :mzn:`<>` indicando *absent*.

  Se proporcionan tres funciones integradas para las variables de tipo de opción: :mzn:`absent(v)` retorna :mzn:`true` si la opción de tipo variable :mzn:`v` toma el valor :mzn:`<>`, :mzn:`occurs(v)` retorna :mzn:`true` si la opción de tipo variable :mzn:`v` no *not* toma el valor :mzn:`<>`, y :mzn:`deopt(v)` devuelve el valor normal de :mzn:`v` o falla si toma el valor :mzn:`<>`.

El uso más común de tipo de opciones es para tareas opcionales en una planificación. En el problema de la planificación de trabajo flexible en una tienda, tenemos :mzn:`n` tarea que ejecutar en la máquina :mzn:`k` ,y el tiempo para completar cada tarea en cada maquina puede ser diferente. El objetivo es minimizar el tiempo requerido para completar todas las tareas. Un modelo usando tipo de opciones para codificar el problema es dado en :numref:`ex-flexible-js`. Nosotros modelamos el problema usando :math:`n \times k` las tareas opcionales aquí representan la posibilidad de que cada tarea sea llevada a cabo en cada maquina. Nosotros requerimos que el tiempo de partida de cada tarea y su duración abarque las tareas optativas que las constituye, y de hecho requerimos que solo una sea ejecutada usando :mzn:`alternative` restricción global. Requerimos que como máximo una tarea sea ejecutada en una maquina usando el :mzn:`disjunctive` restricción global extendida a tareas opcionales. Finalmente restringimos que como máximo :mzn:`k` tareas sean ejecutadas en cualquier momento, una restricción redundante que se mantiene en la tarea actual, no las opcionales.

.. literalinclude:: examples/flexible-js_es.mzn
  :language: minizinc
  :name: ex-flexible-js
  :caption: Modelo para la programación de taller de trabajo flexible utilizando tipos de opciones (:download:`flexible-js_es.mzn <examples/flexible-js_es.mzn>`).

.. \pjs{Finish the damn section!}



Tipos de opciones ocultos
-------------------------

La variable de tipo de opción surge implícitamente cuando las comprensiones de lista se construyen con iteración sobre conjuntos de variables, o cuando las expresiones en las cláusulas :mzn:`where` no son fijas.

Por ejemplo el fragmento modelo:

.. code-block:: minizinc

  var set of 1..n: x;
  constraint sum(i in x)(i) <= limit;

Es una sintaxis para:

.. code-block:: minizinc

  var set of 1..n: x;
  constraint sum(i in 1..n)(if i in x then i else <> endif) <= limit;

La función :mzn:`sum` built-in realmente funciona en una lista de tipo-instanciación (type-inst) :mzn:`var opt int`. Dado que :mzn:`<>` actúa como la identidad 0 para +, esto da los resultados esperados.

Del mismo modo, el fragmento del modelo

.. code-block:: minizinc

  array[1..n] of var int: x;
  constraint forall(i in 1..n where x[i] >= 0)(x[i] <= limit);

Es una sintaxis para:

.. code-block:: minizinc

  array[1..n] of var int: x;
  constraint forall(i in 1..n)(if x[i] >= 0 then x[i] <= limit else <> endif);

Nuevamente la función :mzn:`forall` en realidad opera en una lista de tipo-instanciación :mzn:`var opt bool`. Dado que :mzn:`<>` actúa como identidad :mzn:`true` para :mzn:`/\ ` esto da los resultados esperados.

Sin embargo, los usos ocultos pueden conducir a un comportamiento inesperado, por lo que se requiere cuidado. Considerar:

.. code-block:: minizinc

  var set of 1..9: x;
  constraint card(x) <= 4;
  constraint length([ i | i in x]) > 5;
  solve satisfy;

Que parece ser insatisfactorio. Esto retorna :mzn:`x = {1,2,3,4}` como ejemplo de respuesta. Esto es correcto ya que la segunda restricción es equivalente a:

.. code-block:: minizinc

  constraint length([ if i in x then i else <> endif | i in 1..9 ]) > 5;

Y el largo de la lista de enteros opcionales es siempre 9 por lo tanto la restricción siempre se mantiene!

Se pueden evitar los tipos de opciones ocultos al no construir iteraciones sobre conjuntos de variables o usar cláusulas no fijadas :mzn:`where`.
Por ejemplo, los dos ejemplos anteriores podrían reescribirse sin tipos de opciones como:

.. code-block:: minizinc

  var set of 1..n: x;
  constraint sum(i in 1..n)(bool2int(i in x)*i) <= limit;

Y

.. code-block:: minizinc

  array[1..n] of var int: x;
  constraint forall(i in 1..n)(x[i] >= 0 -> x[i] <= limit);
