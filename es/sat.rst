.. _sec-sat:

Modelado de la satisfacción Booleana en MiniZinc
================================================

MiniZinc  puede usarse para modelar problemas de satisfaccion booleanos, donde las variables estan restringidas a ser booleano (:mzn:`bool`).
MiniZinc se puede utilizar con solucionadores de satisfacción booleanos eficientes para resolver los modelos resultantes de manera eficiente.

Modelando Enteros
-----------------

Muchas veces, aunque deseamos utilizar un solucionador de satisfaccion booleano, podriamos necesitar modelar algunas partes enteras de nuestro problema.

Hay tres formas comunes de modelar variables enteras: :math:`I` en el rango de :math:`0 \dots m`, donde :math:`m = 2^{k}-1` utiliza variables booleanas.

- Binary: :math:`I` es representado por :math:`k` variables binarias :math:`i_0, \ldots, i_{k-1}`, donde :math:`I = 2^{k-1} i_{k-1} + 2^{k-2} i_{k-2} + \cdots + 2 i_1 + i_0`.

  Esto puede ser representado en MiniZinc como

  .. code-block:: minizinc

    array[0..k-1] of var bool: i;
    var 0..pow(2,k)-1: I = sum(j in 0..k-1)(bool2int(i[j])*pow(2,j));

- Unary: donde :math:`I` es representado por :math:`m` variables binarias :math:`i_1, \ldots, i_m` y :math:`i = \sum_{j=1}^m \mathtt{bool2int}(i_j)`.  Como hay redundancia masiva en la representacion unaria, usualmente requerimos que :math:`i_j \rightarrow i_{j-1}, 1 < j \leq m`.

  Esto puede ser representado en MiniZinc como

  .. code-block:: minizinc

    array[1..m]  of var bool: i;
    constraint forall(j in 2..m)(i[j] -> i[j-1]);
    var 0..m: I = sum(j in 1..m)(bool2int(i[j]);

- Value: donde :math:`I` es representado por :math:`m+1` variables binarias :math:`i_0, \ldots, i_m` donde :math:`i = k \Leftrightarrow i_k`, y como máximo uno de :math:`i_0, \ldots, i_m` es verdadero.

  Esto puede ser representado en MiniZinc como

  .. code-block:: minizinc

    array[0..m] of var bool: i;
    constraint sum(j in 0..m)(bool2int(i[j]) == 1;
    var 0..m: I;
    constraint foall(j in 0..m)(I == j <-> i[j]);

Hay ventajas y desventajas para cada representación. Depende de qué operaciones en enteros se requieran en el modelo y cuál es preferible.

Modelando Desigualdad
---------------------

Consideremos el modelado de un problema de cuadrados latinos. Un cuadrado latino es una matriz :math:`n \ times n` de números de :math:`1..n`, tal que cada número aparece exactamente una vez en cada fila y columna.
Un modelo entero para cuadrados latinos se muestra en :numref:`ex-latin`.

.. literalinclude:: examples/latin_es.mzn
  :language: minizinc
  :name: ex-latin
  :caption: Modelo entero para los cuadrados latino (:download:`latin_es.mzn <examples/latin_es.mzn>`).

La única limitación de los enteros es de hecho desigualdad, que se codifica en la restricción :mzn:`alldifferent`.
La representación del valor es la mejor forma de representar la desigualdad.
Un modelo solo booleano para cuadrados latinos se muestra en :numref:`ex-latinbool`.
Tenga en cuenta cada elemento del conjunto de enteros :mzn:`a[i, j]` se reemplaza por un arreglo de booleanos.
Utilizamos el predicado :mzn:`exactlyone` para codificar que cada valor se use exactamente una vez en cada fila y en cada columna, así como para codificar que exactamente uno de los Booleanos correspondiente al elemento de arreglo entero :mzn:`a[i, j]` es verdadero.

.. literalinclude:: examples/latinbool_es.mzn
  :language: minizinc
  :name: ex-latinbool
  :caption: Modelo booleano para los cuadrados latinos (:download:`latinbool_es.mzn <examples/latinbool_es.mzn>`).


Modelando Cardinalidad
----------------------

Consideremos el modelado del rompecabezas de Light Up. El rompecabezas consiste en una cuadrícula rectangular de cuadrados que están en blanco, o llenos. Cada cuadrado lleno puede contener un número del 1 al 4, o puede no tener ningún número. El objetivo es colocar luces en los cuadrados en blanco para que

- Cada casilla en blanco está "iluminada", es decir, puede ver una luz a través de una línea ininterrumpida de cuadrados en blanco.
- No se pueden ver dos luces a si mismas.
- El número de luces adyacentes a un cuadrado lleno numerado es exactamente el número en el cuadrado lleno.

Un ejemplo de un rompecabezas de Light Up se muestra en :numref:`fig-lightup`
con su solución en :numref:`fig-lightup-sol`.

.. _fig-lightup:

.. figure:: figures/lightup.*

  Un ejemplo de un rompecabezas de Light Up

.. _fig-lightup-sol:

.. figure:: figures/lightup2.*

  La solución completa del rompecabezas de Light Up

Es natural modelar este problema usando variables booleanas para determinar qué cuadrados contienen una luz y cuáles no, pero hay algo de aritmética entera que considerar para los cuadrados llenos.

.. literalinclude:: examples/lightup_es.mzn
  :language: minizinc
  :name: ex-lightup
  :caption: Modelo SAT para el rompecabezas de Light Up (:download:`lightup_es.mzn <examples/lightup_es.mzn>`).

A model for the problem is given in :numref:`ex-lightup`.
A data file for the problem shown in :numref:`fig-lightup`
is shown in :numref:`fig-lightupdzn`.

Un modelo para el problema es dado en :numref:`ex-lightup`.
Un archivo de datos para el problema :numref:`fig-lightup` se muestra en :numref:`fig-lightupdzn`.

.. literalinclude:: examples/lightup_es.dzn
  :language: minizinc
  :name: fig-lightupdzn
  :caption: El archivo de datos para la instancia del rompecabezas de Light Up se muestra en :numref:`fig-lightup`.

El modelo hace uso de un predicado de suma booleana

.. code-block:: minizinc

  predicate bool_sum_eq(array[int] of var bool:x, int:s);

que requiere que la suma de un arreglo de Boolean sea igual a un entero fijo. Existen varias formas de modelar tales restricciones de *cardinalidad* usando Booleanos.

- Sumar redes: podemos usar una red de sumadores para construir una representación booleana binaria de la suma de los booleanos.
- Redes de clasificación: podemos usar una red de clasificación para ordenar el arreglo de Booleanos para crear una representación unaria de la suma de los Booleanos.
- Diagramas de decisiones binarias: podemos crear un diagrama de decisión binario (BDD) que codifica la restricción de cardinalidad.

.. literalinclude:: examples/bboolsum_es.mzn
  :language: minizinc
  :name: ex-bboolsum
  :caption: Restricciones de cardinalidad por las redes de sumador binarias (:download:`bboolsum_es.mzn <examples/bboolsum_es.mzn>`).

.. literalinclude:: examples/binarysum_es.mzn
  :language: minizinc
  :name: ex-binarysum
  :caption: Código para construir redes de adición binaria (:download:`binarysum_es.mzn <examples/binarysum_es.mzn>`).

Podemos implementar :mzn:`bool_sum_eq` usando redes de sumador binarias usando el código que se muestra en :numref:`ex-bboolsum`.
El predicado :mzn:`binary_sum` definido en :numref:`ex-binarysum` crea una representación binaria de la suma de :mzn:`x` al dividir la lista en dos, sumando cada mitad para crear una representación binaria y luego sumando estos dos números binarios usando :mzn:`binary_add`.
Si la lista :mzn:`x` es impar, el último bit se guarda para usar como un complemento a la adición binaria

.. \pjs{Add a picture of an adding network}

.. literalinclude:: examples/uboolsum_es.mzn
  :language: minizinc
  :name: ex-uboolsum
  :caption: Restricciones de Cardinalidad Clasificando Redes (:download:`uboolsum_es.mzn <examples/uboolsum_es.mzn>`).

.. literalinclude:: examples/oesort_es.mzn
  :language: minizinc
  :name: ex-oesort
  :caption: Odd-even Fusionar Redes de Clasificación (:download:`oesort_es.mzn <examples/oesort_es.mzn>`).

Podemos implementar :mzn:`bool_sum_eq` utilizando redes de clasificación unaria utilizando el código que se muestra en :numref:` ex-uboolsum`.
La restricción de cardinalidad se define expandiendo la entrada :mzn:`x` para tener una longitud de una potencia de 2 y clasificar los bits resultantes utilizando una red de clasificación de combinación de pares impares.
El clasificador de combinación impar-par que se muestra en :mzn:`ex-oesort` funciona recursivamente mediante la divición de la lista de entrada en 2, ordenando cada lista y combinando las dos listas ya ordenadas.

.. \pjs{Add much more stuff on sorting networks}



.. \pjs{Add a picture of an adding network}

.. literalinclude:: examples/bddsum_es.mzn
  :language: minizinc
  :name: ex-bddsum
  :caption: Restricciones de cardinalidad por diagramas de decisión binarios (:download:`bddsum_es.mzn <examples/bddsum_es.mzn>`).

Nosotros podemos implementar :mzn:`bool_sum_eq` usando diagramas de decición binaria usando el codigo que se muestra en :mzn:`ex:bddsum`.
La restricción de cardinalidad se divide en dos casos: ya sea en el primer elemento :mzn:`x[1]` es :mzn:`true`, y la suma de los bits restantes es :mzn:`s-1`, o :mzn:`x[1]` es :mzn:`false` y la suma de los bits restantes es :mzn:`s`. Por eficiencia, esto se basa en la eliminación de la subexpresión común para evitar crear muchas restricciones equivalentes.

.. \pjs{Add a picture of a bdd network network}
