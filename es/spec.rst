.. |coerce| replace:: :math:`\stackrel{c}{\rightarrow}`

.. |varify| replace:: :math:`\stackrel{v}{\rightarrow}`

.. |TyOverview| replace:: *Overview.*

.. |TyInsts| replace:: *Allowed Insts.*

.. |TySyntax| replace:: *Syntax.*

.. |TyFiniteType| replace:: *Finite?*

.. |TyVarifiable| replace:: *Varifiable?*

.. |TyOrdering| replace:: *Ordering.*

.. |TyInit| replace:: *Initialisation.*

.. |TyCoercions| replace:: *Coercions.*


Especificación de MiniZinc
==========================

Introducción
------------

Este documento define a MiniZinc, el cual es un lenguaje para modelar satisfacción de restricciones y problemas de optimización.


MiniZinc es un lenguaje de modelado de alto nivel, mecanografiado, sobre todo de primer orden, funcional.
El lenguaje provee de:

- Sintaxis como una notación matemática (coerciones automáticas, sobrecarga, iteraciones, conjuntos, arreglos);
- Restricciones expresivas (dominio finito, conjunto, aritmética lineal, entera);
- Soporte para diferentes tipos de problemas (satisfacción, optimización explicita);
- Separación del modelo y de los datos;
- Estructuras de datos de alto-nivel y encapsulamiento de datos (conjuntos, arreglos, tipos de enumeración restriciones de tipo-instanciación);
- Extensibilidad (usuarios definen funciones y predicados);
- Confiabilidad (verificación de tipo, comprobación de instancias, aserciones);
- Modelado independiente del solver;
- Semántica declarativa y simple;

Minizinc es similar a OPL y más cercano a los lenguajes de CLP como ECLIPSe.

Este documento tiene la siguiente estructura
:ref:`spec-syntax-notation` introduce la notación de sintaxis utilizado en toda la especificación.
:ref:`spec-Overview` ofrece una visión general de alto nivel de los modelos MiniZinc.
:ref:`spec-Syntax-Overview` cubre los fundamentos de la sintaxis.
:ref:`spec-High-level-Model-Structure` cubre las estructuras de alto nivel: elementos, modelos de archivos múltiples, espacios de nombres y ámbitos.
:ref:`spec-Types` cubre los tipos y los tipo-instanciación.
:ref:`spec-Expressions` cubre las expresiones.
:ref:`spec-Items` describe en detalle los elementos de nivel-superior.
:ref:`spec-Annotations` describe las anotaciones.
:ref:`spec-Partiality` describe como se maneja la parcialidad en varios casos.
:ref:`spec-builtins` describe el lenguaje incorporado.
:ref:`spec-Grammar` da la gramática de MiniZinc.
:ref:`spec-Content-types` define los tipos de contenido utilizados en esta especificación.


Este documento también proporciona una explicación de por qué se tomaron ciertas decisiones de diseño. Dichas explicaciones están marcadas por la palabra *Justificación* y están escritas en cursiva, y no constituyen parte de la especificación como tal.
*Justificación: estas explicaciones están presentes porque son útiles tanto para los diseñadores como para los usuarios de MiniZinc.*


Autores originales
~~~~~~~~~~~~~~~~~~

La versión original de este documento fue preparada por Nicholas Nethercote, Kim Marriott, Reza Rafeh, Mark Wallace y Maria Garcia de la Banda. Sin embargo, MiniZinc está evolucionando, al igual que este documento.

Para ver un documento publicado formalmente sobre el lenguaje MiniZinc y el lenguaje Zinc superconjunto, consulte:

- N. Nethercote, P.J. Stuckey, R. Becket, S. Brand, G.J. Duck, and G. Tack.
  Minizinc: Towards a standard CP modelling language.
  In C. Bessiere, editor, *Proceedings of the 13th International
  Conference on Principles and Practice of Constraint Programming*, volume 4741
  of *LNCS*, pages 529--543. Springer-Verlag, 2007.
- K. Marriott, N. Nethercote, R. Rafeh, P.J. Stuckey,
  M. Garcia de la Banda, and M. Wallace.
  The Design of the Zinc Modelling Language.
  *Constraints*, 13(3):229-267, 2008.

.. _spec-syntax-notation:



Notación
--------

Los conceptos básicos del EBNF utilizados en esta especificación son los siguientes.

- Los no terminales se escriben entre corchetes angulares, :mzndef:`<item>`.
- Las terminales están escritas entre comillas dobles. Por ejemplo, :mzndef:`"constraint"`. Una terminal de doble cita se escribe como una secuencia de tres comillas dobles :mzndef:`"""`.
- Los elementos opcionales están escritos entre corchetes, por ejemplo, :mzndef:`[ "var" ]`.
- Las secuencias de cero o más elementos se escriben con paréntesis y una estrella, por ejemplo :mzndef:`( "," <ident> )*`.
- Las secuencias de uno o más elementos se escriben con paréntesis y un signo más (+), por ejemplo :mzndef:`( <msg> )+`.
- Las listas no vacías se escriben con un elemento, un terminal separador/terminador, y tres puntos.  Por ejemplo:

.. code-block:: minizincdef

    <expr> "," ...

Es la abreviatura de:

.. code-block:: minizincdef

    <expr> ( "," <expr> )* [ "," ]


El terminal final siempre es opcional en listas no vacías.

- Las expresiones regulares se usan en algunas producciones. Por ejemplo, :mzndef:`[-+]?[0-9]+`.

La gramática de MiniZinc se presenta pieza por pieza a lo largo de este documento. También está disponible como un todo en :ref:`spec-Grammar`.

La gramática de salida también incluye algunos detalles del uso del espacio en blanco.
Se usan las siguientes convenciones:

- Un carácter de nueva línea o una secuencia CRLF es escrito ``\n``.


.. - A sequence of space characters of length :math:`n` is written ``nSP``, e.g., ``2SP``.


.. _spec-Overview:


Descripción general de un modelo
--------------------------------

Conceptualmente, una especificación de problema MiniZinc tiene dos partes.

1. El *modelo*: la parte principal de la especificación del problema, que describe la estructura de una clase particular de problemas.
2. Los *datos*: los datos de entrada para el modelo, que especifica un problema particular dentro de esta clase de problemas.

El emparejamiento de un modelo con un conjunto de datos particular es una *instancia de modelo* (a veces abreviado como *instancia*).

El modelo y los datos pueden estar separados, o los datos pueden estar "cableados" en el modelo.
:ref:`spec-Model-Instance-Files` especifica cómo el modelo y los datos se pueden estructurar dentro de archivos en una instancia modelo.

Hay dos clases amplias de problemas: satisfacción y optimización.
En los problemas de satisfacción, todas las soluciones se consideran igualmente buenas, mientras que en los problemas de optimización las soluciones se ordenan según un objetivo y el objetivo es encontrar una solución cuyo objetivo sea óptimo.
:ref:`spec-Solve-Items` especifica cómo se elige la clase de problema.


Fases de evaluación
~~~~~~~~~~~~~~~~~~~

Una instancia de modelo MiniZinc se evalúa en dos fases distintas.

1. Tiempo de instancia: Comprobación estática de la instancia del modelo.
2. Tiempo de ejecución: Evaluación de la instancia (ejemplo, resolución de restricciones).

Es posible que la instancia del modelo no se compile debido a un problema con el modelo y/o los datos que se detectaron en el momento de la instancia.
Esto podría deberse a un error de sintaxis, a un error de
instanciación de tipo, al uso de una función u operación no admitida, etc.
En este caso, el resultado de la evaluación es un error estático, esto se debe informar antes del tiempo de ejecución.
La forma de salida para los errores estáticos depende de la implementación, aunque dicha salida debería ser fácilmente reconocible como un error estático.

Una implementación puede producir advertencias durante todas las fases de evaluación.
Por ejemplo, una implementación puede ser capaz de determinar que existen restricciones insatisfactorias antes del tiempo de ejecución, y la advertencia resultante dada al usuario puede ser más útil que si se detecta la insatisfacción en el tiempo de ejecución.

Una implementación debe producir una advertencia si el objetivo para un problema de optimización no tiene límites.


.. _spec-run-time-outcomes:

Resultados en tiempo de ejecución
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Suponiendo que no hay errores estáticos, el resultado de la fase de tiempo de ejecución tiene la siguiente forma abstracta:

.. literalinclude:: output.mzn
  :language: minizincdef
  :start-after: % Output
  :end-before: %

Si se produce una solución en la salida, entonces debe ser factible.
Para problemas de optimización, cada solución debe ser estrictamente mejor que cualquier solución anterior.

Si no hay soluciones en el resultado, el resultado debe indicar que no hay soluciones.

Si la búsqueda está completa, la salida puede indicar esto después de las soluciones.
La ausencia del mensaje de completitud indica que la búsqueda está incompleta.

Cualquier advertencia producida durante el tiempo de ejecución se debe resumir después de la declaración de integridad.
En particular, si hubo advertencias durante el tiempo de ejecución, entonces el resumen debe indicar este hecho.

La implementación puede producir texto en cualquier formato después de las advertencias.
Por ejemplo, puede imprimir un resumen de estadísticas de benchmarking o recursos utilizados.

.. _spec-output:




Salida
~~~~~~

Las implementaciones deben ser capaces de producir resultados de tipo de contenido ``application/x-zinc-output``, que se describe a continuación y también en :ref:`spec-Content-types`.
Las implementaciones también pueden producir resultados en formatos alternativos.
Cualquier salida debe ajustarse al formato abstracto de la sección anterior y debe tener la semántica descrita allí.

Tipo de contenido ``application/x-zinc-output`` extiende la sintaxis de la sección anterior de la siguiente manera:

.. literalinclude:: output.mzn
  :language: minizincdef
  :start-after: % Solutions
  :end-before: %

El texto de la solución para cada solución debe ser como se describe en :ref:`spec-Output-Items`.
Se debe agregar una nueva línea si el texto de la solución no finaliza con una nueva línea.
*Justificación: Esto permite que las soluciones se extraigan de la salida sin saber necesariamente cómo se formatean las soluciones.*
Las soluciones terminan con una secuencia de diez guiones seguidos de una nueva línea.

.. literalinclude:: output.mzn
  :language: minizincdef
  :start-after: % Unsatisfiable
  :end-before: %

El resultado de integridad se imprime en una línea separada. *Justificación: las cadenas están diseñadas para indicar claramente el final de las soluciones.*

.. literalinclude:: output.mzn
  :language: minizincdef
  :start-after: % Complete
  :end-before: %

Si la búsqueda está completa, se imprime una declaración correspondiente al resultado.
Para un resultado sin soluciones, el enunciado es que la instancia del modelo no es satisfactoria. Para el resultado de no más soluciones, la declaración es que el conjunto de soluciones está completo. Finalmente, para un resultado sin mejores soluciones, la afirmación es que la última solución es óptima .
*Justificación: estas son las implicaciones lógicas de una búsqueda completa.*

.. literalinclude:: output.mzn
  :language: minizincdef
  :start-after: % Messages
  :end-before: %

Si la búsqueda es incompleta, se pueden imprimir uno o más mensajes que describen los motivos de la falta de cumplimiento.
Del mismo modo, si se produjeron advertencias durante la búsqueda, se repiten después del mensaje de integridad.
Ambos tipos de mensajes deben tener líneas que comiencen con ``%`` para que se reconozcan como comentarios por postprocesamiento.
*Justificación: Esto permite que los mensajes individuales sean fácilmente reconocibles.*

Por ejemplo, se puede generar lo siguiente para un problema de optimización:

.. code-block:: bash

  =====UNSATISFIABLE=====
  % trentin.fzn:4: warning: model inconsistency detected before search.

Tenga en cuenta que, como en este caso, un objetivo ilimitado no se considera una fuente de incompletud.

.. _spec-syntax-overview:


Descripción general de la sintaxis
----------------------------------

Conjunto de caracteres
~~~~~~~~~~~~~~~~~~~~~~

Los archivos de entrada a MiniZinc deben codificarse como UTF-8.

MiniZinc es sensible a mayúsculas y minúsculas. No hay lugares donde se deben usar letras mayúsculas o minúsculas.

MiniZinc no tiene restricciones de diseño, es decir, cualquier espacio en blanco (que contiene espacios, pestañas y líneas nuevas) es equivalente a cualquier otro.


Comentarios
~~~~~~~~~~~

Un ``%`` indica que el resto de la línea es un comentario.  MiniZinc también tiene comentarios de bloque, usando los símbolos ``/*`` and ``*/`` para marcar el comienzo y el final de un comentario.


.. _spec-identifiers:


Identificadores
~~~~~~~~~~~~~~~

Los identificadores tienen la siguiente sintaxis:

.. code-block:: minizincdef

  <ident> ::= [A-Za-z][A-Za-z0-9_]*       % excluyendo las palabras clave
            | "'" [^'\xa\xd\x0]* "'"

.. code-block:: minizinc

  mi_nombre_2
  MiNombre2
  'Un identificador arbitrario'

Se reservan varias palabras clave y no se pueden usar como identificadores. Las palabras clave son:
:mzn:`ann`,
:mzn:`annotation`,
:mzn:`any`,
:mzn:`array`,
:mzn:`bool`,
:mzn:`case`,
:mzn:`constraint`,
:mzn:`diff`,
:mzn:`div`,
:mzn:`else`,
:mzn:`elseif`,
:mzn:`endif`,
:mzn:`enum`,
:mzn:`false`,
:mzn:`float`,
:mzn:`function`,
:mzn:`if`,
:mzn:`in`,
:mzn:`include`,
:mzn:`int`,
:mzn:`intersect`,
:mzn:`let`,
:mzn:`list`,
:mzn:`maximize`,
:mzn:`minimize`,
:mzn:`mod`,
:mzn:`not`,
:mzn:`of`,
:mzn:`op`,
:mzn:`output`,
:mzn:`par`,
:mzn:`predicate`,
:mzn:`record`,
:mzn:`satisfy`,
:mzn:`set`,
:mzn:`solve`,
:mzn:`string`,
:mzn:`subset`,
:mzn:`superset`,
:mzn:`symdiff`,
:mzn:`test`,
:mzn:`then`,
:mzn:`true`,
:mzn:`tuple`,
:mzn:`type`,
:mzn:`union`,
:mzn:`var`,
:mzn:`where`,
:mzn:`xor`.

Una serie de identificadores se utilizan para integradas; ver :ref:`spec-builtins` para más detalles.

.. _spec-High-level-Model-Structure:


Estructura de modelo de alto nivel
----------------------------------

Un modelo MiniZinc consta de múltiples *elementos*:

.. literalinclude:: grammar.mzn
  :language: minizincdef
  :start-after: % A MiniZinc model
  :end-before: %

Los elementos pueden disponerse en cualquier orden; los identificadores no necesitan ser declarados antes de ser usados. Los elementos tienen la siguiente sintaxis de nivel superior:

.. literalinclude:: grammar.mzn
  :language: minizincdef
  :start-after: % Items
  :end-before: %

Incluir elementos proporciona una forma de combinar múltiples archivos en una sola instancia. Esto permite que un modelo se divida en varios archivos
(:ref:`spec-Include-Items`).

Los ítems de declaración variables introducen nuevas variables globales y posiblemente las vinculen a un valor (:ref:`spec-Declarations`).

Los elementos de asignación vinculan valores a variables globales (:ref:`spec-Assignments`).

Los elementos de restricción describen las restricciones del modelo (:ref:`spec-Constraint-Items`).

Los elementos de resolución son el "punto de partida" de un modelo y especifican exactamente qué tipo de solución se está buscando:
simple satisfacción, o la minimización/maximización de una expresión.  Cada modelo debe tener exactamente un elemento de solución (:ref:`spec-Solve-Items`).

Los elementos de salida se utilizan para presentar muy bien el resultado de una ejecución del modelo (:ref:`spec-Output-Items`).

Elementos de predicados, elementos de prueba (que son solo un tipo especial de predicado) y los elementos de función introducen nuevos predicados y funciones definidos por el usuario que pueden invocarse en expresiones (:ref:`spec-preds-and-fns`).
Los predicados, las funciones y los operadores incorporados se describen colectivamente como *operaciones*.

Los elementos de anotación aumentan el tipo :mzn:`ann`, cuyos valores pueden especificar información no declarativa y/o específica del solucionador en un modelo.

.. _spec-model-instance-files:




Archivos de instancias modelo
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Los modelos en MiniZinc se pueden construir a partir de múltiples archivos utilizando elementos incluidos (ver :ref:`spec-Include-Items`).  MiniZinc no tiene un sistema de módulos como tal; todos los archivos incluidos simplemente se concatenan y procesan como un todo, exactamente como si todos hubieran sido parte de un solo archivo.
*Razón fundamental: No hemos encontrado mucha necesidad de uno hasta el momento.  Si los modelos más grandes se vuelven comunes y el espacio de nombres global único se convierte en un problema, esto debería reconsiderarse.*

Cada modelo puede emparejarse con uno o más archivos de datos. Los archivos de datos son más restringidos que los archivos de modelo. Solo pueden contener asignaciones de variables (ver :ref:`spec-Assignments`).

Los archivos de datos pueden no incluir llamadas a operaciones definidas por el usuario.

Los modelos no contienen los nombres de los archivos de datos. Al hacerlo, se corregirá el archivo de datos utilizado por el modelo y se frustrará el propósito de permitir archivos de datos separados. En cambio, una implementación debe permitir que uno o más archivos de datos se combinen con un modelo para su evaluación a través de un mecanismo como la línea de comandos.

Al verificar un modelo con datos, se deben asignar todas las variables globales con inserciones de tipo fijo, a menos que no se utilicen (en cuyo caso pueden eliminarse del modelo sin efecto).

Un archivo de datos solo se puede verificar en busca de errores estáticos junto con un modelo, ya que el modelo contiene las declaraciones que incluyen los tipos de las variables asignadas en el archivo de datos.

Un único archivo de datos puede compartirse entre varios modelos, siempre que las definiciones sean compatibles con todos los modelos.

.. _spec-namespaces:


Espacios de nombres (Namespaces)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Todos los nombres declarados en el nivel superior pertenecen a un solo espacio de nombres.
Incluye los siguientes nombres.

1. Todos los nombres de variables globales.
2. Todos los nombres de funciones y predicados, tanto integrados como definidos por el usuario.
3. Todos los nombres de tipos enumerados y nombres de casos de enumeración.
4. Todos los nombres de anotación.

Debido a que los modelos MiniZinc de archivos múltiples se componen a través de la concatenación (:ref:`spec-Model-Instance-Files`), todos los archivos comparten este espacio de nombres de nivel superior. Por lo tanto, una variable ``x`` declarado en un archivo de modelo no se pudo declarar con un tipo diferente en un archivo diferente, por ejemplo.

MiniZinc admite la sobrecarga de operaciones integradas y definidas por el usuario.

.. _spec-scopes:



Scopes
~~~~~~

Within the top-level namespace, there are several kinds of local scope that
introduce local names:

- Comprehension expressions (:ref:`spec-Set-Comprehensions`).
- Let expressions (:ref:`spec-Let-Expressions`).
- Function and predicate argument lists and bodies (:ref:`spec-preds-and-fns`).

The listed sections specify these scopes in more detail.  In each case, any
names declared in the local scope overshadow identical global names.

.. _spec-types:



Tipos y Tipos de Instanciación
------------------------------

MiniZinc proporciona:
- Cuatro tipos de escalar incorporados: Booleanos (Boolean), enteros (Integer), flotantes (Float) y cadenas (Strings);
- Tipos enumerados;
- Dos tipos integrados compuestos: conjuntos y matrices multidimensionales;
- Y el tipo de anotación extensible del usuario :mzn:`ann`.

Cada tipo tiene uno o más posibles de *instanciación*. La instanciación de una variable o valor indica si se ha fijado a un valor conocido o no.  Un emparejamiento de un tipo y creación de instancias se llama *tipo-instanciación*.

Comenzamos discutiendo algunas propiedades que se aplican a todos los tipos. Luego presentamos instancias con más detalle. Luego cubrimos cada tipo individualmente, dando: una visión general del tipo y sus posibles instancias, la sintaxis para su tipo-instanciación, si es un tipo finito (y si es así, su dominio), si es variable, el orden y operaciones de igualdad, si sus variables deben inicializarse en el momento de la instancia y si puede estar involucrado en coacciones automáticas.


Propiedades de los tipos
~~~~~~~~~~~~~~~~~~~~~~~~

La siguiente lista presenta algunas propiedades generales de los tipos MiniZinc.

- Actualmente, todos los tipos son monotipos. En el futuro, podemos permitir tipos que son polimórficos en otros tipos y también las limitaciones asociadas.
- Distinguimos tipos que son *tipos finitos*. En MiniZinc, los tipos finitos incluyen booleanos, enumeraciones (enums), tipos definidos via conjunto de expresiones tipo-instanciación como los tipos de rango (ver :ref:`spec-Set-Expression-Type-insts`), así como conjuntos y matrices, compuestos de tipos finitos. Los tipos que no son tipos finitos son enteros no restringidos, flotantes no restringidos, cadenas no restringidas y :mzn:`ann`. Los tipos finitos son relevantes para conjuntos (:mzn:`spec-Sets`) y los índices de matriz (:mzn:`spec-Arrays`). Cada tipo finito tiene un *dominio*, que es un valor establecido que contiene todos los valores posibles representados por el tipo.
- Todos los tipos de primer orden (esto excluye :mzn:`ann`) tiene incorporado un orden total y un sistema incorporado en la igualdad; ``>``, ``<``, ``==``/``=``, ``!=``, ``<=`` and ``>=`` los operadores de comparación se pueden aplicar a cualquier par de valores del mismo tipo. *Razón fundamental: Esto facilita la especificación de ruptura de simetría y de predicados y funciones polimórficos.* Tenga en cuenta que, como en la mayoría de los lenguajes, el uso de igualdad en flotantes o tipos que contienen flotantes generalmente no es confiable debido a su representación inexacta. Una implementación puede optar por advertir sobre el uso de igualdad con flotantes o tipos que contienen flotantes.

.. _spec-instantiations:


Instanciaciones
~~~~~~~~~~~~~~~

Cuando se evalúa un modelo MiniZinc, el valor de cada variable puede ser inicialmente desconocido. A medida que se ejecuta, el *dominio* de cada variable (el conjunto de valores que puede tomar) puede reducirse, posiblemente hasta un único valor.

Una *instanciación* (a veces abreviado como *inst*) describe cómo fija o no fija una variable en una tiempo-instancia. En el nivel más básico, el sistema de creación de instancias distingue dos tipos de variables:

#. *Parametros*, cuyos valores están fijados a tiempo-instancia, usualmente escrito como "fijo" (fixed).
#. *Variables de decisión* (a menudo abreviado como *variables*), cuyos valores pueden estar completamente sin fijar en tiempo-instancia, pero puede volverse fijo en tiempo de ejecución (de hecho, la fijación de variables de decisión es el objetivo total de la resolución de restricciones).

En las variables de decisión MiniZinc pueden tener los siguientes tipos: Booleanos, enteros, flotantes y conjuntos de enteros, y enumeración.
Arreglos y :mzn:`ann` puede contener variables de decisión.

.. _spec-type-inst:


Tipo-Instanciación
~~~~~~~~~~~~~~~~~~

Como cada variable tiene un tipo y una instancia, a menudo se combinan en un único *tipo-instanciación*. Tipo-instanciación son principalmente lo que tratamos cuando escribimos modelos, en lugar de tipos.

Una variable de tipo-instanciación *nunca cambia*.  Esto significa que una variable de decisión cuyo valor se vuelve fijo durante la evaluación del modelo todavía tiene su original tipo-instanciación (ejemplo :mzn:`var int`), porque ese era su tipo-instanciación a un tiempo-instancia.

Algunos tipos-instanciación puede ser coaccionado automáticamente a otro tipo-instanciación. Por ejmplo, si un :mzn:`par int` valor se usa en un contexto donde :mzn:`var int` se espera, que es forzado automáticamente a una :mzn:`var int`. Escribimos esto :mzn:`par int` |coerce| :mzn:`var int`. Además, cualquier tipo-instanciación puede considerarse coercible a sí mismo.
MiniZinc permite coerciones entre algunos tipos también.

Algunos tipo-instanciación pueden ser *varificado* (varified). Por ejemplo, hecho sin fijar en el nivel superior.
Por ejemplo, :mzn:`par int` está varificado a :mzn:`var int`.  Escribimos esto :mzn:`par int` |varify| :mzn:`var int`.

Tipo-instanciación que son varificables incluyen el tipo-instanciación de los tipos que pueden ser variables de decisión (Booleanos, enteros, flotantes, conjuntos, tipos enumerados).
La varificación es relevante para tipo-instanciación sinónimos y accesos a la matriz.
