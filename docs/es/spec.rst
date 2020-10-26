.. |coerce| replace:: :math:`\stackrel{c}{\rightarrow}`

.. |varify| replace:: :math:`\stackrel{v}{\rightarrow}`

.. |TyOverview| replace:: *Visión general.*

.. |TyInsts| replace:: *Instancias permitidas.*

.. |TySyntax| replace:: *Sintaxis.*

.. |TyFiniteType| replace:: *Finito?*

.. |TyVarifiable| replace:: *Varificable?*

.. |TyOrdering| replace:: *Ordenando.*

.. |TyInit| replace:: *Inicialización.*

.. |TyCoercions| replace:: *Coerciones.*


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
:mzn:`opt`,
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



Alcances
~~~~~~~~

Dentro del espacio de nombres de nivel superior, hay varios tipos de ámbito local que
introducir nombres locales:

- Expresiones de Comprensión (:ref:`spec-Set-Comprehensions`).
- Expresiones Let (:ref:`spec-Let-Expressions`).
- Listas de funciones y argumentos de predicados y cuerpos (:ref:`spec-preds-and-fns`).

Las secciones enumeradas especifican estos ámbitos con más detalle. En cada caso, cualquier
los nombres declarados en el ámbito local eclipsan nombres globales idénticos.


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
- Distinguimos tipos que son *tipos finitos*. En MiniZinc, los tipos finitos incluyen booleanos, enumeraciones (enums), tipos definidos via conjunto de expresiones tipo-instanciación como los tipos de rango (ver :ref:`spec-Set-Expression-type-insts`), así como conjuntos y matrices, compuestos de tipos finitos. Los tipos que no son tipos finitos son enteros no restringidos, flotantes no restringidos, cadenas no restringidas y :mzn:`ann`. Los tipos finitos son relevantes para conjuntos (:mzn:`spec-Sets`) y los índices de matriz (:mzn:`spec-Arrays`). Cada tipo finito tiene un *dominio*, que es un valor establecido que contiene todos los valores posibles representados por el tipo.
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






Visión general de expresiones de Tipo-instanciación
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Esta sección describe parcialmente cómo escribir inserciones de tipo en modelos MiniZinc.
Se proporcionan más detalles para cada tipo tal como se describen en las siguientes secciones.

Una expresión tipo-instanciación especifica un tipo-instanciación.
Una expresión tipo-instanciación puede incluir un tipo-instanciación constraints.
Una expresión tipo-instanciación aparece en declaraciones de variables (:ref:`spec-Declarations`) y elementos de operación definidos por el usuario (:ref:`spec-preds-and-fns`).

Una expresión de tipo-instanciación tiene las siguientes sintaxis:

.. literalinclude:: grammar.mzn
  :language: minizincdef
  :start-after: % Type-inst expressions
  :end-before: %

(La alternativa final, para los tipos de rango, usa el numérico específico :mzndef:`<num-expr>` no terminal, definido en :ref:`spec-Expressions-Overview`, en lugar de la :mzndef:`<expr>` no terminal.  Si este no fuera el caso, la regla nunca coincidiría porque el operador ``..`` siempre se correspondería con el primero :mzndef:`<expr>`.)

Esto cubre completamente las expresiones tipo-instanciación para tipos escalares. El compuesto de las sintaxis de las expresiones de tipo-instanciación está descrita con más detalle en :ref:`spec-Built-in-Compound-Types`.

Las palabras claves :mzn:`par` y :mzn:`var` (o falta de ellos) determina la instanciación. La anotación :mzn:`par` puede ser emitida;  las siguientes dos expresiones de tipo-instanciación son equivalentes:

.. code-block:: minizinc

    par int
    int

*Razón fundamental: El uso explícito de la palabra clave* :mzn:`var` *permite que una implementación compruebe que todos los parámetros se inicializan en el modelo o la instancia. También documenta claramente qué variables son parámetros y permite una comprobación de tipo-instanciación más precisa.*

A tipo-instanciación es fijo si no contiene :mzn:`var` o :mzn:`any`, con la excepción de :mzn:`ann`.

Tenga en cuenta que varias expresoines de tipo-instanciación que son sintácticamente expresables representan una ilegal tipo-instanciación. Por ejemplo, aunque la gramática permite :mzn:`var` frente a todas estas bases de colas de expresión de tipo-instanciación, es un error de tipo-instanciación que tiene una :mzn:`var` en el frente de una expresión de cadena o matriz.

.. _spec-built-in-scalar-types:



Tipos escalares incorporados y tipos de instanciación
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Booleanos
+++++++++

|TyOverview|
Los booleanos representan la verdad o la falsedad (verdadero o falso, true o false). *Justificación: los valores booleanos no están representados por números enteros.
Los booleanos se pueden convertir explícitamente a enteros con la función * :mzn:`bool2int` *, lo que hace que la intención del usuario sea clara.*

|TyInsts|
Los booleanos pueden ser fijos o no.

|TySyntax|
Los booleanos fijos son escritos como :mzn:`bool` o :mzn:`par bool`.  Los booleanos no fijos se escriben como :mzn:`var bool`.

|TyFiniteType|
Sí. El dominio de un booleano es :mzn:`false, true`.

|TyVarifiable|
:mzn:`par bool` |varify| :mzn:`var bool`, :mzn:`var bool` |varify| :mzn:`var bool`.

|TyOrdering|
El valor :mzn:`false` se considera más pequeño que :mzn:`true`.

|TyInit|
Una variable booleana fija se debe inicializar en el momento de la instancia; una variable booleana sin fijar no tiene que ser.

|TyCoercions|
:mzn:`par bool` |coerce| :mzn:`var bool`.

También los booleanos se pueden forzar automáticamente a enteros; ver :ref:`spec-Integers`.

.. _spec-integers:


Enteros
++++++++

|TyOverview|
Los números enteros representan números enteros. Las representaciones enteras están definidas por la implementación. Esto significa que el rango representable de enteros está definido por la implementación. Sin embargo, una implementación debe abortar en tiempo de ejecución si una operación entera se desborda.

|TyInsts|
Los enteros pueden ser fijos o no.

|TySyntax|
Los enteros fijos están escritos como :mzn:`int` o :mzn:`par int`.  Los enteros no fijos se escriben como :mzn:`var int`.

|TyFiniteType|
No, a menos que esté limitado por una expresión establecida (ver :ref:`spec-Set-Expression-Type-insts`).

|TyVarifiable|
:mzn:`par int` |varify| :mzn:`var int`,
:mzn:`var int` |varify| :mzn:`var int`.

|TyOrdering|
El orden en enteros es el estándar.

|TyInit|
Una variable entera fija se debe inicializar en el momento de la instancia; una variable entera no fijada no lo necesita.

|TyCoercions|
:mzn:`par int` |coerce| :mzn:`var int`,
:mzn:`par bool` |coerce| :mzn:`par int`,
:mzn:`par bool` |coerce| :mzn:`var int`,
:mzn:`var bool` |coerce| :mzn:`var int`.

Además, los enteros se pueden forzar automáticamente a flotantes; ver :ref:`spec-Floats`.

.. _spec-floats:



Punto Flotante (Floats)
+++++++++++++++++++++++

|TyOverview|
Los números de punto flotante representan números reales. Las representaciones de números flotantes están definidas por la implementación. Esto significa que el rango y la precisión representables de los números de punto flotante están definidos por la implementación. Sin embargo, una implementación debe abortar en tiempo de ejecución en operaciones de punto flotante excepcionales

(por ejemplo, aquellos que producen ``NaN``, es decir, Not a Number: No es un número, si usas número de punto flotante IEEE754).

|TyInsts|
Los números de punto flotante puede ser arreglado o no.

|TySyntax|
Los números de punto flotante son escritos como :mzn:`float` o :mzn:`par float`. Los números de punto flotante no fijos son escritos como :mzn:`var float`.

|TyFiniteType|
No, a menos que esté limitado por una expresión establecida (ver :ref:`spec-Set-Expression-Type-insts`).

|TyVarifiable|
:mzn:`par float` |varify| :mzn:`var float`,
:mzn:`var float` |varify| :mzn:`var float`.

|TyOrdering|
El orden en los número de punto flotante es el estándar.

|TyInit|
Una variable flotante fija se debe inicializar en el momento de la instancia; una variable flotante no fija no lo necesita.

|TyCoercions|
:mzn:`par int` |coerce| :mzn:`par float`,
:mzn:`par int` |coerce| :mzn:`var float`,
:mzn:`var int` |coerce| :mzn:`var float`,
:mzn:`par float` |coerce| :mzn:`var float`.

.. _spec-enumerated-types:





Tipos Enumerados
++++++++++++++++

|TyOverview|
Los tipos enumerados (o *enumerados* para abreviar) proporcionan un conjunto de alternativas nombradas. Cada alternativa se identifica por su *nombre del caso*.
Los tipos enumerados, como en muchos otros idiomas, se pueden usar en lugar de tipos enteros para lograr una verificación de tipo más estricta.

|TyInsts|
Las enumeraciones pueden ser fijas o no.

|TySyntax|
Las variables de un tipo enumerado llamado ``X`` están representados por el término :mzn:`X` o :mzn:`par X` si es fija, y :mzn:`var X` si es no fija.

|TyFiniteType|
Sí.
El dominio de una enumeración es el conjunto que contiene todos sus nombres de casos.

|TyVarifiable|
:mzn:`par X` |varify| :mzn:`var X`,
:mzn:`var X` |varify| :mzn:`var X`.

|TyOrdering|
Cuando se comparan dos valores enumerados con diferentes nombres de casos, el valor con el nombre del caso que se declara primero se considera más pequeño que el valor con el nombre del caso que se declara en segundo lugar.

|TyInit|
Una variable de enumeración fija se debe inicializar en el momento de la instancia; una variable enumeración no fijada no lo necesita.

|TyCoercions|
:mzn:`par X` |coerce| :mzn:`par int`,
:mzn:`var X` |coerce| :mzn:`var int`.

.. _spec-strings:



Cadenas (Strings)
+++++++++++++++++

|TyOverview|
Las cadenas son primitivas, es decir, no son listas de caracteres.

Las expresiones de cadena se utilizan en aserciones, elementos de salida y anotaciones, y los literales de cadena se utilizan en los elementos de inclusión.

|TyInsts|
Las cadenas deben ser corregidas.

|TySyntax|
Las cadenas fijas están escritas como :mzn:`string` o :mzn:`par string`.

|TyFiniteType|
No, a menos que esté limitado por una expresión establecida (ver :ref:`spec-Set-Expression-Type-insts`).

|TyVarifiable|
No.

|TyOrdering|
Las cadenas se ordenan lexicográficamente utilizando los códigos de caracteres subyacentes.

|TyInit|
Una variable de cadena (que solo se puede arreglar) se debe inicializar en el momento de la instancia.

|TyCoercions|
Ninguno automático. Sin embargo, cualquier valor que no sea de cadena se puede convertir manualmente a una cadena usando la función built-in :mzn:`show` o usando la interpolación de cuerdas (ver :ref:`spec-String-Interpolation-Expressions`).

.. _spec-Built-in-Compound-Types:



Tipos de compuestos incorporados y Tipo-Instanciación
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. _spec-sets:


Conjuntos
+++++++++

|TyOverview|
Un conjunto es una colección sin duplicados.

|TyInsts|
Un tipo-instanciación de los elementos de un conjunto debe ser fija. *Justificación: Esto se debe a que los solucionadores actuales no son lo suficientemente potentes como para manejar conjuntos que contienen variables de decisión.*

Los conjuntos pueden contener cualquier tipo, y pueden ser fijos o no.
Si un conjunto no está fijado, sus elementos deben ser finitos, a menos que ocurra en uno de los siguientes contextos:

- El argumento de un predicado, función o anotación.
- La declaración de una variable o una variable local ``let`` con un valor asignado.

|TySyntax|
Una base establecida de una expresión de tipo-instanciación tiene la siguiente sintaxis:

.. literalinclude:: grammar.mzn
  :language: minizincdef
  :start-after: % Set type-inst expressions
  :end-before: %

Algunos ejemplos de expresiones de conjuntos de tipo-instanciación:

.. code-block:: minizinc

  set of int
  var set of bool

|TyFiniteType|
Sí, si los elementos establecidos son tipos finitos. De otra manera no.

El dominio de un tipo de conjunto que es un tipo finito es el conjunto de poder del dominio de su tipo de elemento. Por ejemplo, el dominio de :mzn:`set of 1..2` es :mzn:`powerset(1..2)`, cual es :mzn:`{}, {1}, {1,2}, {2}`.

|TyVarifiable|
:mzn:`par set of TI` |varify| :mzn:`var set of TI`,
:mzn:`var set of TI` |varify| :mzn:`var set of TI`.

|TyOrdering|
El orden predefinido en los conjuntos es un ordenamiento lexicográfico del *forma de conjunto ordenado*, donde :mzn:`{1,2}` está en forma de conjunto ordenado, por ejemplo, :mzn:`{2,1}` no es.
Esto significa, por ejemplo, :mzn:`{} < {1,3} < {2}`.

|TyInit|
Una variable de conjunto fijo se debe inicializar en el momento de la instancia; una variable set no fijada no necesita ser.

|TyCoercions|
:mzn:`par set of TI` |coerce| :mzn:`par set of UI` y
:mzn:`par set of TI` |coerce| :mzn:`var set of UI` y
:mzn:`var set of TI` |coerce| :mzn:`var set of UI`, si
:mzn:`TI` |coerce| :mzn:`UI`.




Arreglos
++++++++

|TyOverview|
Los arrays MiniZinc son mapas de enteros fijos a valores.
Los valores pueden ser de cualquier tipo.
Los valores solo pueden tener inserciones de base tipo-instanciación.
Las matrices de matrices no están permitidas.
Las matrices pueden ser multidimensionales.

Las matrices MiniZinc se pueden declarar de dos maneras diferentes.

- *Las matrices explícitamente indexadas* tienen tipos de índice en la declaración que son tipos finitos. Por ejemplo:

  .. code-block:: minizinc

    array[0..3] of int: a1;
    array[1..5, 1..10] of var float: a5;

Para tales arrays, el tipo de índice especifica exactamente los índices que estarán en la matriz - el conjunto de índices de la matriz es el *dominio* del tipo de índice - y si los índices del valor asignado no coinciden, entonces es un tiempo de ejecución error.

Por ejemplo, las siguientes asignaciones causan errores en tiempo de ejecución:

  .. code-block:: minizinc

    a1 = [4,6,4,3,2];   % Demasiados elementos.
    a5 = [];            % Muy pocos elementos.

- *Las matrices implícitamente indexadas* tienen tipos de índice en la declaración que no son tipos finitos. Por ejemplo:

  .. code-block:: minizinc

    array[int,int] of int: a6;

  No se verifican los índices cuando estas variables están asignadas.

En MiniZinc, todos los conjuntos de índices de una matriz deben ser rangos contiguos de enteros o tipos enumerados.
La expresión utilizada para la inicialización de una matriz debe tener conjuntos de índices coincidentes.
Una expresión de matriz con un conjunto de índices enum se puede asignar a una matriz declarada con un conjunto de índices enteros, pero no al revés.
La excepción son los literales de matriz, que se pueden asignar a matrices declaradas con conjuntos de índices enumerados.

Por ejemplo:

.. code-block:: minizinc

  enum X = {A,B,C};
  enum Y = {D,E,F};
  array[X] of int: x = array1d(X, [5,6,7]); % Correcto
  array[Y] of int: y = x;                   % Conjunto de índices no
                                            % coinciden : Y != X
  array[int] of int: z = x;                 % Correcto: asigne el índice X
                                            % establecido en int
  array[X] of int: x2 = [10,11,12];         % Correcto: coerción automática
                                            % para literales de matriz

La inicialización de una matriz se puede hacer en una declaración de asignación separada, que puede estar presente en el modelo o en un archivo de datos separado.

Las matrices pueden ser accedidos. Ver :ref:`spec-Array-Access-Expressions` para más detalles.

|TyInsts|
El tamaño de una matriz debe ser fijo. Sus índices también deben tener inserciones de tipo fijo. Sus elementos pueden ser fijos o no.

|TySyntax|
Un arreglo base de una expresión de tipo-instanciación tiene la siguiente sintaxis:

.. literalinclude:: grammar.mzn
  :language: minizincdef
  :start-after: % Array type-inst expressions
  :end-before: %

Algunos ejemplos de arreglos de expresiones de tipo-instanciación:

.. code-block:: minizinc

  array[1..10] of int
  list of var int

Tenga en cuenta que :mzndef:`list of <T>` es solo sintáctico para :mzndef:`array[int] of <T>`. *Justificación: Las matrices con índices enteros de este formulario son muy comunes y merecen un soporte especial para facilitar las cosas a los modeladores. Implementarlo usando azúcar sintáctico evita agregar un tipo extra al lenguaje, lo que simplifica las cosas para los implementadores.*

Como las matrices deben ser de tamaño fijo, es un error tipo-instanciación que precede a una expresión tipo-instanciación de matriz con :mzn:`var`.

|TyFiniteType|
Sí, si los tipos de índice y el tipo de elemento son todos tipos finitos.
De otra manera no.

El dominio de un tipo de matriz que es una matriz finita es el conjunto de todas las matrices distintas cuyo conjunto de índices es igual al dominio del tipo de índice y cuyos elementos son del tipo de elemento de matriz.

|TyVarifiable|
No.

|TyOrdering|
Las matrices se ordenan lexicográficamente, tomando la ausencia de un valor para una clave dada antes de cualquier valor para esa clave. Por ejemplo,
:mzn:`[1, 1]` es menor que
:mzn:`[1, 2]`, que es menor que :mzn:`[1, 2, 3]` y
:mzn:`array1d(2..4,[0, 0, 0])` que es menor que  :mzn:`[1, 2, 3]`.

|TyInit|
Una variable de matriz indexada explícitamente debe inicializarse en instancia-tiempo solo si sus elementos deben inicializarse en el momento de la instancia.
Una variable de matriz indexada implícitamente se debe inicializar en el momento de la instancia para que se conozca su longitud y conjunto de índices.

|TyCoercions|
:mzn:`array[TI0] of TI` |coerce| :mzn:`array[UI0] of UI` si
:mzn:`TI0` |coerce| :mzn:`UI0` y :mzn:`TI` |coerce| :mzn:`UI`.

.. _spec-option-types:




Tipos de opciones
+++++++++++++++++

|TyOverview|
Los tipos de opciones definidos usando el constructor de tipo :mzn:`opt`, definen tipos que pueden o no estar allí. Son similares a tipos ``Puede`` (``Maybe``) de la implicidad de Haskell agrega un nuevo valor :mzn:`<>` al tipo.


|TyInsts|
El argumento de un tipo de opción debe ser uno de los tipos base :mzn:`bool`, :mzn:`int` o :mzn:`float`.

|TySyntax|
El tipo de opción está escrito :mzndef:`opt <T>` donde :mzndef:`<T>` si es uno de los tres tipos base, o una de sus instancias restringidas.

|TyFiniteType|
Sí, si el tipo subyacente es finito, de lo contrario no.

|TyVarifiable|
Sí.

|TyOrdering|
:mzn:`<>` siempre es menor que cualquier otro valor en el tipo.
Pero ten cuidado con la sobrecarga de operadores como :mzn:`<`, que es diferente para los tipos de opciones.

|TyInit|
Una variable de tipo :mzn:`opt` no necesita ser inicializado en el momento de la instancia. Un tipo de variable sin inicializar :mzn:`opt` se inicializa automáticamente a :mzn:`<>`.

|TyCoercions|
:mzn:`TI` |coerce| :mzn:`opt UI` si :mzn:`TI` |coerce| :mzn:`UI`..

.. _spec-the-annotation-type:



El Tipo de Anotación
++++++++++++++++++++

|TyOverview|
El tipo de anotación, :mzn:`ann`, se puede usar para representar estructuras de términos arbitrarios. Se aumenta con elementos de anotación (:ref:`spec-Annotation-Items`).

|TyInsts|
:mzn:`ann` siempre se considera no fijado, ya que puede contener elementos no fijadas. No puede ser precedido por :mzn:`var`.

|TySyntax|
El tipo de anotación está escrito :mzn:`ann`.

|TyFiniteType|
No.

|TyVarifiable|
No.

|TyOrdering|
N/A.  Los tipos de anotación no tienen un orden definido en ellos.

|TyInit|
Una variable :mzn:`ann` debe inicializarse en el momento de la instancia.

|TyCoercions|
Ninguna.


.. _spec-constrained-type-insts:




Restricciones de Tipo-instanciación
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Una poderosa característica de MiniZinc es *restricciones de tipo-instanciación*. Una restricción de tipo-instanciación es una versión restringida de *base* tipo-instanciación, ejemplo, un tipo-instanciación con un menor número de valores en su dominio.

.. _spec-set-expression-type-insts:



Conjunto de expresiones de Tipo-instanciación
+++++++++++++++++++++++++++++++++++++++++++++

Se pueden usar tres tipos de expresiones en tipo-instanciación.

#. Rangos enteros:  ejemplos. :mzn:`1..3`.
#. Conjunto de literales:  ejemplos. :mzn:`var {1,3,5}`.
#. Identifiers: el nombre de un parámetro establecido (que puede ser global, let-local, el argumento de un predicado o función, o un valor de generador) puede servir como tipo-instanciación.

En cada caso, el tipo de base es el de los elementos del conjunto, y los valores dentro del conjunto sirven como el dominio. Por ejemplo, mientras que una variable con tipo-instanciación :mzn:`var int` puede tomar cualquier valor entero, una variable con tipo-instanciación :mzn:`var 1..3` solo puede tomar el valor 1, 2 o 3.

Todas las expresiones establecidas de tipo-instanciación son tipos finitos. Su dominio es igual al conjunto en sí.



Rango de los números de Tipo-instanciación
++++++++++++++++++++++++++++++++++++++++++

Los rangos de los números de punto flotantes se pueden usar como tipo-instanciación. Por ejemplo, :mzn:`1.0 .. 3.0`.
Estos se tratan de manera similar al rango entero tipo-instanciación, a pesar de que :mzn:`1.0 .. 3.0` no es una expresión válida mientras es :mzn:`1 .. 3`.

Los rangos de números de punto flotante no son tipos finitos.

.. _spec-expressions:



Expresiones
-----------

.. _spec-expressions-overview:



Resumen de expresiones
~~~~~~~~~~~~~~~~~~~~~~

Las expresiones representan valores. Ocurren en varios tipos de artículos. Tienen la siguiente sintaxis:

.. literalinclude:: grammar.mzn
  :language: minizincdef
  :start-after: % Expressions
  :end-before: %

Las expresiones se pueden componer a partir de subexpresiones combinadas con operadores.
Todos los operadores (binarios y unarios) se describen en :ref:`spec-Operators`, incluyendo las precedencias de los operadores binarios. Todos los operadores unarios se unen más estrechamente que todos los operadores binarios.

Las expresiones pueden tener una o más anotaciones. Las anotaciones se vinculan más estrechamente que las aplicaciones de operador unario y binario, pero menos estrechamente que las operaciones de acceso y las aplicaciones que no son de operador. En algunos casos, este enlace no es intuitivo. Por ejemplo, en las primeras tres de las siguientes líneas, la anotación :mzn:`a` se une a la expresión del identificador :mzn:`x` en lugar de la aplicación del operador. Sin embargo, la cuarta línea presenta una aplicación sin operador (debido a las comillas simples alrededor del :mzn:`not`) y entonces la anotación se une a toda la aplicación.

.. code-block:: minizinc

  not x::a;
  not (x)::a;
  not(x)::a;
  'not'(x)::a;

:ref:`spec-Annotations` tiene más en anotaciones.

Las expresiones se pueden contener entre paréntesis.

Las operaciones de acceso a la matriz se unen más estrechamente que las anotaciones y operadores unarios y binarios.
Se describen con más detalle en :ref:`spec-Array-Access-Expressions`.

Los tipos restantes de átomos de expresión (from :mzndef:`<ident>` a
:mzndef:`<gen-call-expr>`) están descritos en
:ref:`spec-Identifier-Expressions-and-Quoted-Operator-Expressions` a :ref:`spec-Generator-Call-Expressions`.

También distinguimos expresiones numéricas sintácticamente válidas. Esto permite que los tipos de rango sean analizados correctamente.

.. literalinclude:: grammar.mzn
  :language: minizincdef
  :start-after: % Numeric expressions
  :end-before: %

.. _spec-operators:



Operadores
~~~~~~~~~~

Los operadores son funciones que se distinguen por su sintaxis de una o dos formas. Primero, algunos de ellos contienen caracteres no alfanuméricos que las funciones normales no (por ejemplo, :mzn:`+`). Segundo, su aplicación está escrita de una manera diferente a las funciones normales.

Distinguimos entre operadores binarios, que se pueden aplicar de manera infija (por ejemplo, :mzn:`3 + 4`), y operadores unarios, que se pueden aplicar de manera prefija sin paréntesis (por ejemplo, :mzn:`not x`). También distinguimos entre operadores integrados y operadores definidos por el usuario. La sintaxis es la siguiente:

.. literalinclude:: grammar.mzn
  :language: minizincdef
  :start-after: % Built-in operators
  :end-before: %

Nuevamente, distinguimos sintácticamente operadores numéricos.

.. literalinclude:: grammar.mzn
  :language: minizincdef
  :start-after: % Built-in numeric operators
  :end-before: %

Algunos operadores se pueden escribir utilizando sus símbolos Unicode, que se enumeran en :numref:`bin-ops-unicode` (recuerde que la entrada de MiniZinc es UTF-8).

.. _bin-ops-unicode:

.. cssclass:: table-nonfluid table-bordered

.. table:: Unicode equivalents of binary operators


  ================  =======================  ============
  Operador          Símbolo Unicode symbol   Código UTF-8
  ================  =======================  ============
  :mzn:`<->`        :math:`\leftrightarrow`  E2 86 94
  :mzn:`->`         :math:`\rightarrow`      E2 86 92
  :mzn:`<-`         :math:`\leftarrow`       E2 86 90
  :mzn:`not`        :math:`\lnot`            C2 AC
  ``\/``            :math:`\lor`             E2 88 A8
  ``/\``            :math:`\land`            E2 88 A7
  :mzn:`!=`         :math:`\neq`             E2 89 A0
  :mzn:`<=`         :math:`\leq`             E2 89 A4
  :mzn:`>=`         :math:`\geq`             E2 89 A5
  :mzn:`in`         :math:`\in`              E2 88 88
  :mzn:`subset`     :math:`\subseteq`        E2 8A 86
  :mzn:`superset`   :math:`\supseteq`        E2 8A 87
  :mzn:`union`      :math:`\cup`             E2 88 AA
  :mzn:`intersect`  :math:`\cap`             E2 88 A9
  ================  =======================  ============

Los operadores binarios se enumeran en :numref:`bin-ops`. Un número de precedencia más bajo significa un enlace más estricto; por ejemplo, :mzn:`1+2*3` es analizado como :mzn:`1+(2*3)` porque :mzn:`*` se une más fuerte que :mzn:`+`. La asociatividad indica cómo se manejan las cadenas de operadores con igual precedencia; por ejemplo, :mzn:`1+2+3` es analizado como :mzn:`(1+2)+3` porque :mzn:`+` es de izquierda asociativo, :mzn:`a++b++c` es analizado como :mzn:`a++(b++c)` porque :mzn:`++` es asociativo por la derecha, y :mzn:`1<x<2` es un error de sintaxis porque :mzn:`<` no es asociativo.

.. _bin-ops:

.. cssclass:: table-nonfluid table-bordered

.. table:: Binary infix operators

  ===============================  ========== ===========
  Symbolo(s)                       Asociación Precedencia
  ===============================  ========== ===========
  :mzn:`<->`                       izquierda  1200

  :mzn:`->`                        izquierda  1100
  :mzn:`<-`                        izquierda  1100

  ``\/``                           izquierda  1000
  :mzn:`xor`                       izquierda  1000

  ``/\``                           izquierda  900

  :mzn:`<`                         ninguna    800
  :mzn:`>`                         ninguna    800
  :mzn:`<=`                        ninguna    800
  :mzn:`>=`                        ninguna    800
  :mzn:`==`,
  :mzn:`=`                         ninguna    800
  :mzn:`!=`                        ninguna    800

  :mzn:`in`                        ninguna    700
  :mzn:`subset`                    ninguna    700
  :mzn:`superset`                  ninguna    700

  :mzn:`union`                     izquierda  600
  :mzn:`diff`                      izquierda  600
  :mzn:`symdiff`                   izquierda  600

  :mzn:`..`                        ninguna    500

  :mzn:`+`                         izquierda  400
  :mzn:`-`                         izquierda  400

  :mzn:`*`                         izquierda  300
  :mzn:`div`                       izquierda  300
  :mzn:`mod`                       izquierda  300
  :mzn:`/`                         izquierda  300
  :mzn:`intersect`                 izquierda  300

  :mzn:`++`                        derecha    200

  `````  :mzndef:`<ident>` `````   izquierda  100
  ===============================  ========== ===========


Un operador binario definido por el usuario se crea mediante el retroceso de un identificador normal, por ejemplo:

.. code-block:: minizinc

  A `min2` B

Este es un error estático si el identificador no es el nombre de una función o predicado binario.

Los operadores unarios son: :mzn:`+`, :mzn:`-` y :mzn:`not`.
Los operadores unarios definidos por el usuario no son posibles.

Como explica:ref:`spec-Identifiers`, cualquier operador incorporado se puede usar como un identificador de función normal citándolo, por ejemplo: :mzn:`'+'(3, 4)` es equivalente a :mzn:`3 + 4`.

El significado de cada operador se da en :ref:`spec-builtins`.




Expresiones atómicas
~~~~~~~~~~~~~~~~~~~~

.. _spec-Identifier-Expressions-and-Quoted-Operator-Expressions:

Expresiones de identificador y expresiones de operador entrecomilladas
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Las expresiones de identificador y las expresiones de operador entre comillas tienen la siguiente sintaxis:

.. literalinclude:: grammar.mzn
  :language: minizincdef
  :start-after: % Identifiers and quoted operators
  :end-before: %

Se dieron ejemplos de identificadores en :ref:`spec-Identifiers`.  Los siguientes son ejemplos de operadores cotizados:

.. code-block:: minizinc

  '+'
  'union'

En los operadores citados, el espacio en blanco no está permitido entre la cotización y el operador.  :ref:`spec-Operators` enumera los operadores incorporados de MiniZinc.

Sintácticamente, cualquier identificador o operador citado puede servir como una expresión.
Sin embargo, en un modelo válido, cualquier identificador o operador citado que sirva como expresión debe ser el nombre de una variable.

.. _spec-Anonymous-Decision-Variables:



Variables de decisión anónimas
++++++++++++++++++++++++++++++

Hay un identificador especial, :mzn:`_`, que representa una variable de decisión anónima no fija. Puede tomar cualquier tipo que pueda ser una variable de decisión. Es particularmente útil para inicializar variables de decisión dentro de tipos compuestos. Por ejemplo, en la siguiente matriz, los elementos primero y tercero se fijan en 1 y 3, respectivamente, y los elementos segundo y cuarto no están fijos:

.. code-block:: minizinc

  array[1..4] of var int: xs = [1, _, 3, _];

Cualquier expresión que no contenga :mzn:`_` y no involucra variables de decisión, por lo que es fijo.




Literales booleanos
+++++++++++++++++++

Los literales booleanos tienen esta sintaxis:

.. literalinclude:: grammar.mzn
  :language: minizincdef
  :start-after: % Boolean literals
  :end-before: %



Entero y Literales de números de punto flotante
+++++++++++++++++++++++++++++++++++++++++++++++

Hay tres formas de literales enteros: decimal, hexadecimal y octal, con estas formas respectivas:

.. literalinclude:: grammar.mzn
  :language: minizincdef
  :start-after: % Integer literals
  :end-before: %

Por ejemplo: :mzn:`0`, :mzn:`005`, :mzn:`123`, :mzn:`0x1b7`, :mzn:`0o777`;  pero no :mzn:`-1`.

Los literales de punto flotante poseen la siguiente forma:

.. literalinclude:: grammar.mzn
  :language: minizincdef
  :start-after: % Float literals
  :end-before: %

Por ejemplo: :mzn:`1.05`, :mzn:`1.3e-5`, :mzn:`1.3+e5`;  pero no :mzn:`1.`, :mzn:`.5`, :mzn:`1.e5`, :mzn:`.1e5`, :mzn:`-1.0`, :mzn:`-1E05`.

Un símbolo :mzn:`-` precede a un entero o literal de número flotante se analiza como unario menos (ndependientemente del espacio en blanco intermedio), no como parte del literal.  Esto se debe a que, en general, no es posible distinguir una :mzn:`-` para un entero negativo o literal de número flotante de un binario menos cuando se lee.

.. _spec-String-Interpolation-Expressions:




Literales de cadenas e interpolación de cadenas
+++++++++++++++++++++++++++++++++++++++++++++++

Los literales de cadena se escriben como en C:

.. literalinclude:: grammar.mzn
  :language: minizincdef
  :start-after: % String literals
  :end-before: %

Esto incluye secuencias de escape estilo C, como :mzn:`\"` para comillas dobles, :mzn:`\\` para la barra invertida, y :mzn:`\n` para nueva línea.

Por ejemplo: :mzn:`"Hello, world!\n"`.

Los literales de cadena deben caber en una sola línea.

Los literales de cadena larga se pueden dividir en varias líneas mediante la concatenación de cadenas. Por ejemplo:

.. code-block:: minizinc

    string: s = "This is a string literal "
             ++ "split across two lines.";

Una expresión de cadena puede contener una expresión arbitraria de MiniZinc, que se convertirá en una cadena similar a la función builtin :mzn:`show` y se insertará en la cadena.

Por ejemplo:

.. code-block:: minizinc

  var set of 1..10: q;
  solve satisfy;
  output [show("The value of q is \(q), and it has \(card(q)) elements.")];

.. _spec-set-literals:



Establecer Literales
++++++++++++++++++++

Los literales establecidos tienen esta sintaxis:

.. literalinclude:: grammar.mzn
  :language: minizincdef
  :start-after: % Set literals
  :end-before: %

Por ejemplo:

.. code-block:: minizinc

  { 1, 3, 5 }
  { }
  { 1, 2.0 }

La instanciación de tipo de todos los elementos en un conjunto literal debe ser la misma, o coercible para la misma instancia de tipo (como en el último ejemplo anterior, donde el entero :mzn:`1` será coaccionado a un :mzn:`float`).


.. _spec-set-comprehensions:





Establecer comprensiones
++++++++++++++++++++++++

Las comprensiones establecidas tienen esta sintaxis:

.. literalinclude:: grammar.mzn
  :language: minizincdef
  :start-after: % Set comprehensions
  :end-before: %

Por ejemplo (con el equivalente literal a la derecha):

.. code-block:: minizinc

    { 2*i | i in 1..5 }     % { 2, 4, 6, 8, 10 }
    {  1  | i in 1..5 }     % { 1 }   (no duplicates in sets)

La expresión antes del :mzn:`|` es la *expresión de la cabeza*. La expresión después del :mzn:`in` es una *expresión del generador*.
Los generadores pueden ser restringidos por una expresión mientras (*where-expression*). Por ejemplo:

.. code-block:: minizinc

  { i | i in 1..10 where (i mod 2 = 0) }     % { 2, 4, 6, 8, 10 }

Cuando hay múltiples generadores presentes, el generador que está más a la derecha actúa como el más interno. Por ejemplo:

.. code-block:: minizinc

    { 3*i+j | i in 0..2, j in {0, 1} }    % { 0, 1, 3, 4, 6, 7 }

El alcance de las variables del generador local viene dado por las siguientes reglas:

- Son visibles dentro de la expresión de la cabeza (antes de :mzn:`|`).
- Son visibles dentro de la expresión mientras (where-expression).
- Son visibles dentro de las expresiones del generador en cualquier generador posterior.

La última de estas reglas significa que se permite la siguiente comprensión del conjunto:

.. code-block:: minizinc

    { i+j | i in 1..3, j in 1..i }  % { 1+1, 2+1, 2+2, 3+1, 3+2, 3+3 }

Una expresión de generador debe ser una matriz o un conjunto fijo.

*Razón fundamental: Para las comprensiones del conjunto, los generadores de conjuntos serían suficientes, pero para las comprensiones de matriz, se requieren generadores de matriz para una expresividad total (ejemplo., para proporcionar control sobre el orden de los elementos en la matriz resultante). Las comprensiones de conjuntos tienen generadores de matriz para coherencia con las comprensiones de matriz, lo que hace que las implementaciones sean más simples.*


La expresión mientras (si está presente) debe ser booleano.
Puede ser ``var``, en cuyo caso el tipo de comprensión se eleva a un tipo opcional. Solo una donde se permite la expresión por comprensión.

*Razón fundamental:
Permitir una expresión-donde por generador es otra posibilidad, y una que aparentemente podría dar como resultado una evaluación más eficiente en algunos casos. Por ejemplo, considere la siguiente comprensión:*

.. code-block:: minizinc

    [f(i, j) | i in A1, j in A2 where p(i) /\ q(i,j)]

*Si se permitieran múltiples expresiones-where, esto podría expresarse más eficientemente de la siguiente manera, lo que evita las "iteraciones de bucle interno" infructuosas para cada "iteración de bucle externo" que no satisface* :mzn:`p(i)`:

.. code-block:: minizinc

    [f(i, j) | i in A1 where p(i), j in A2 where q(i,j)]

*Sin embargo, esta eficiencia también se puede lograr con comprensiones anidadas:*

.. code-block:: minizinc

    [f(i, j) | i in [k | k in A1 where p(k)], j in A2 where q(i,j)]

*Por lo tanto, una sola expresión de dónde es todo lo que es compatible.*

.. _spec-array-literals:



Literales de matrices
+++++++++++++++++++++

Los literales de matriz tienen esta sintaxis:

.. literalinclude:: grammar.mzn
  :language: minizincdef
  :start-after: % Array literals
  :end-before: %

Por ejemplo:

.. code-block:: minizinc

    [1, 2, 3, 4]
    []
    [1, _]


En una matriz literal, todos los elementos deben tener la misma instancia de tipo, o ser coercibles para la misma instancia de tipo (como en el último ejemplo anterior, donde el entero fijo :mzn:`1` será coaccionado a un :mzn:`var int`).

Los índices de una matriz literal son implícitamente :mzn:`1..n`, donde :mzn:`n` es la longitud del literal.

.. _spec-2d-array-literals:



Arreglos literales en 2d
++++++++++++++++++++++++

Los literales simples de arreglos 2d tienen esta sintaxis:

.. literalinclude:: grammar.mzn
  :language: minizincdef
  :start-after: % 2D Array literals
  :end-before: %

Por ejemplo:

.. code-block:: minizinc

    [| 1, 2, 3
     | 4, 5, 6
     | 7, 8, 9 |]       % array[1..3, 1..3]
    [| x, y, z |]       % array[1..1, 1..3]
    [| 1 | _ | _ |]     % array[1..3, 1..1]

En un 2d array literal, cada sub-array debe tener la misma longitud.

En un 2d array literal, todos los elementos deben tener la misma instancia de tipo, o ser coercibles para el mismo tipo de instanciación (como en el último ejemplo anterior, donde el entero fijo :mzn:`1` será coaccionado a un :mzn:`var int`).

Los índices de un 2d array literal están implícitamente :mzn:`(1,1)..(m,n)`, donde :mzn:`m` y :mzn:`n` están determinados por la forma del literal.


.. _spec-array-comprehensions:




Arreglo de Comprensiones
++++++++++++++++++++++++

Arreglo de Comprensiones tienen la siguiente sintaxis:

.. literalinclude:: grammar.mzn
  :language: minizincdef
  :start-after: % Array comprehensions
  :end-before: %

Por ejemplo (con los equivalentes literales a la derecha):

.. code-block:: minizinc

    [2*i | i in 1..5]       % [2, 4, 6, 8, 10]

Arreglo de Comprensiones tener requisitos de tipo e instanciación más flexibles que las comprensiones establecidas (see :ref:`spec-Set-Comprehensions`).

Arreglo de Comprensiones se permiten sobre un conjunto de variables con tipo finito, el resultado es una matriz de tipo opcional, con una longitud igual a la cardinalidad del límite superior del conjunto de variables.

Por ejemplo:

.. code-block:: minizinc

    var set of 1..5: x;
    array[int] of var opt int: y = [ i * i | i in x ];

La longitud de la matriz será 5.

Se permiten las comprensiones de matriz donde la expresión mientras es :mzn:`var bool`.
De nuevo, la matriz resultante es de tipo opcional, y de longitud igual a la dada por las expresiones del generador.
Por ejemplo:

.. code-block:: minizinc

   var int x;
   array[int] of var opt int: y = [ i | i in 1..10 where i != x ];

La longitud de la matriz será 10.

Los índices de una comprensión de matriz simple evaluada son implícitamente
:mzn:`1..n`, donde :mzn:`n` es la duración de la comprensión evaluada.

.. _spec-array-access-expressions:



Expresiones de Matriz de Acceso
+++++++++++++++++++++++++++++++

Se accede a los elementos de matriz usando corchetes después de una expresión:

.. literalinclude:: grammar.mzn
  :language: minizincdef
  :start-after: % Array access
  :end-before: %

Por ejemplo:

.. code-block:: minizinc

    int: x = a1[1];

Si todos los índices utilizados en un arreglo de acceso son fijos, la tipo-instanciación del resultado es la misma que el elemento tipo-instanciación. Sin embargo, si los índices no son fijos, la tipo-instanciación del resultado es el elemento varificado tipo-instanciación. Por ejemplo, si tenemos:

.. code-block:: minizinc

   array[1..2] of int: a2 = [1, 2];
   var int: i;

Entonces el tipo-instanciación de :mzn:`a2[i]` es :mzn:`var int`. Si el elemento tipo-instanciación no es variable, tal acceso causa un error estático.

Se accede a matrices multidimensionales usando índices separados por comas.

.. code-block:: minizinc

    array[1..3,1..3] of int: a3;
    int: y = a3[1, 2];

Los índices deben coincidir con el tipo de conjunto de índice de la matriz. Por ejemplo, una matriz declarada con un conjunto de índices enum solo se puede acceder usando índices de esa enumeración.

.. code-block:: minizinc

    enum X = {A,B,C};
    array[X] of int: a4 = [1,2,3];
    int: y = a4[1];                  % tipo de índice incorrecto
    int: z = a4[B];                  % correcto


Literales de Anotación
++++++++++++++++++++++

Literales del tipo :mzn:`ann` tiene la siguiente sintaxis:

.. literalinclude:: grammar.mzn
  :language: minizincdef
  :start-after: % Annotation literals
  :end-before: %

Por ejemplo:

.. code-block:: minizinc

    foo
    cons(1, cons(2, cons(3, nil)))

No hay forma de inspeccionar o deconstruir literales de anotación en un modelo MiniZinc;  están destinados a ser inspeccionados solo por una implementación, por ejemplo, para dirigir la compilación.






Expresiones Si-Entonces-Sino (If-then-else)
+++++++++++++++++++++++++++++++++++++++++++

MiniZinc proporciona expresiones if-then-else, que proporcionan una selección de dos alternativas basadas en una condición. Ellos tienen esta sintaxis:

.. literalinclude:: grammar.mzn
  :language: minizincdef
  :start-after: % If-then-else expressions
  :end-before: %

Por ejemplo:

.. code-block:: minizinc

    if x <= y then x else y endif
    if x < 0 then -1 elseif x > 0 then 1 else 0 endif

La presencia del :mzn:`endif` evita la posible ambigüedad cuando una expresión if-then-else es parte de una expresión más grande.

La instanciación de tipo de la :mzn:`if` la expresión debe ser :mzn:`par bool` o :mzn:`var bool`.
El :mzn:`then` y :mzn:`else` las expresiones deben tener el mismo tipo-instanciación o ser coercibles para el mismo tipo-instanciación, que también es el tipo-instanciación de toda la expresión.

Si la expresión :mzn:`if` es :mzn:`var bool` entonces el tipo-instanciación de el :mzn:`then` y la expresión :mzn:`else` debe ser varificable.

Si la expresión :mzn:`if` es :mzn:`par bool` entonces la evaluación de las expresiones if-then-else es floja (lazy): la condición es evaluada, y luego solo uno de los :mzn:`then` y :mzn:`else` las ramas se evalúan, dependiendo de si la condición tuvo éxito o falló.
Este no es el caso si es :mzn:`var bool`.


.. _spec-let-expressions:


Expresiones Let
+++++++++++++++

Las expresiones Let proporcionan una forma de introducir nombres locales para una o más expresiones y restricciones locales que se pueden usar dentro de otra expresión. Son particularmente útiles en operaciones definidas por el usuario.

Las expresions ``let`` tienen la siguiente sintaxis:

.. literalinclude:: grammar.mzn
  :language: minizincdef
  :start-after: % Let expressions
  :end-before: %

Por ejemplo:

.. code-block:: minizinc

    let { int: x = 3, int: y = 4; } in x + y;
    let { var int: x;
          constraint x >= y /\ x >= -y /\ (x = y \/ x = -y); }
    in x

El alcance de una variable local ``let`` abarca:

- Las expresiones de tipo-instanciación e inicialización de cualquier variable posterior dentro de la expresión let (pero no la expresión de inicialización propia de la variable).
- La expresión después del :mzn:`in`, que es analizado tan codiciosamente como sea posible.

Una variable solo puede declararse una vez en una expresión let.

Por lo tanto, en los siguientes ejemplos, el primero es aceptable, pero el resto no:

.. code-block:: minizinc

    let { int: x = 3; int: y = x; } in x + y;  % ok
    let { int: y = x; int: x = 3; } in x + y;  % x not visible in y's defn.
    let { int: x = x; } in x;                  % x not visible in x's defn.
    let { int: x = 3; int: x = 4; } in x;      % x declared twice

..
  The type-inst expressions can include type-inst variables if the let is
  within a function or predicate body in which the same type-inst variables
  were present in the function or predicate signature.
  TODO: type-inst variables are currently not fully supported

El inicializador para una variable local let puede omitirse solo si la variable es una variable de decisión. Por ejemplo:

.. code-block:: minizinc

    let { var int: x; } in ...;    % ok
    let {     int: x; } in ...;    % illegal

La tipo-instanciación de toda la expresión let es la tipo-instanciación de la expresión después de la palabra clave :mzn:`in`.

Hay una complicación que involucra expresiones de let en contextos negativos.  Una expresión ``let`` ocurre en un contexto negativo si ocurre en una expresión de la forma :mzn:`not X`, :mzn:`X <-> Y}` o en la sub-expresión
:mzn:`X` en :mzn:`X -> Y` o :mzn:`Y <- X`, o en una subexpresion
:mzn:`bool2int(X)`.

Las restricciones en las expresiones de let flotan al contexto booleano circundante más cercano. Por ejemplo

.. code-block:: minizinc

     constraint b -> x + let { var 0..2: y; constraint y != -1;} in y >= 4;

Es equivalente a:

.. code-block:: minizinc

     var 0..2: y;
     constraint b -> (x + y >= 4 /\ y != 1);



Expresiones de llamada
++++++++++++++++++++++

Las expresiones de llamada se usan para llamar predicados y funciones.

Las expresiones de llamada tienen esta sintaxis:

.. literalinclude:: grammar.mzn
  :language: minizincdef
  :start-after: % Call expressions
  :end-before: %

Por ejemplo:

.. code-block:: minizinc

    x = min(3, 5);


El tipo-instanciación de las expresiones pasadas como argumentos debe coincidir con los tipos de argumento de las llamadas predicado/función. El tipo de retorno del predicado/función también debe ser apropiado para el contexto de llamada.

Tenga en cuenta que una llamada a una función o predicado sin argumentos es sintácticamente indistinguible del uso de una variable, y así debe determinarse durante la comprobación de tipo-instanciación.

La evaluación de los argumentos en las expresiones de llamada es estricta: todos los argumentos se evalúan antes de que se evalúe la llamada. Tenga en cuenta que esto incluye operaciones booleanas como ``/\``, ``\/``, :mzn:`->` y :mzn:`<-` que podría ser flojo (lazy) en un argumento. La única excepción es :mzn:`assert`, que es flojo en su tercer argumento (:ref:`spec-Other-Operations`).

*Razón fundamental: Las operaciones booleanas son estrictas porque:
(a) esto minimiza casos excepcionales;
(b) en una expresión como* :mzn:`A -> B` *donde* :mzn:`A` *no es fijo y* :mzn:`B` *provoca un aborto, el comportamiento apropiado no está claro si la pereza está presente; y
(c) si un usuario necesita laziness, un if-then-else puede ser usado.*

El orden de evaluación de los argumentos no está especificado. *Justificación: debido a que MiniZinc es declarativo, no existe una necesidad obvia de especificar una orden de evaluación, y dejarla sin especificar da a los implementadores cierta libertad.*

.. _spec-generator-call-expressions:



Expresiones de Generador de llamadas
++++++++++++++++++++++++++++++++++++

MiniZinc tiene una sintaxis especial para ciertos tipos de expresiones de llamada lo que hace que los modelos sean mucho más legibles.

Las expresiones de llamada del generador tienen esta sintaxis:

.. literalinclude:: grammar.mzn
  :language: minizincdef
  :start-after: % Generator call expressions
  :end-before: %

Una expresión de llamada del generador :mzn:`P(Gs)(E)` es equivalente a la expresión de llamada :mzn:`P([E | Gs])`.
Por ejemplo, la expresión:

.. code-block:: minizinc

    forall(i,j in Domain where i<j)
        (noattack(i, j, queens[i], queens[j]));

(en un modelo que especifica el problema N-Reinas) es equivalente a:

.. code-block:: minizinc

    forall( [ noattack(i, j, queens[i], queens[j])
            | i,j in Domain where i<j ] );

Los paréntesis alrededor de la última expresión son obligatorios; esto evita una posible confusión cuando la expresión de llamada del generador es parte de una expresión más grande.

El identificador debe ser el nombre de un predicado o función unaria que toma un argumento de matriz.

Los generadores y expresión-mientras (si está presente) tienen los mismos requisitos que aquellos en las comprensiones de matriz (:ref:`spec-Array-Comprehensions`).

.. _spec-items:




Elementos
---------

Esta sección describe los elementos del programa de nivel superior.

.. _spec-include-items:



Incluir elementos
~~~~~~~~~~~~~~~~~~~~

Incluir elementos permite dividir un modelo en varios archivos. Ellos tienen esta sintaxis:

.. literalinclude:: grammar.mzn
  :language: minizincdef
  :start-after: % Include items
  :end-before: %

Por ejemplo:

.. code-block:: minizinc

    include "foo.mzn";

Incluye el archivo ``foo.mzn``.

Incluir elementos es particularmente útil para acceder a bibliotecas o dividir modelos grandes en piezas pequeñas. Ellos no son, como :ref:`spec-Model-Instance-Files` explica, utilizado para especificar archivos de datos.

Si el nombre de pila no es completo, el archivo se busca en un conjunto de directorios definido por la implementación. Los directorios de búsqueda deben poder modificarse con una opción de línea de comando.

.. _spec-declarations:



Elementos de declaración variable
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Las declaraciones de variables tienen esta sintaxis:

.. literalinclude:: grammar.mzn
  :language: minizincdef
  :start-after: % Variable declaration items
  :end-before: %

Por ejemplo:

.. code-block:: minizinc

    int: A = 10;

Es un error de instantación de tipo si una variable se declara y/o define más de una vez en un modelo.

Una variable cuya declaración no incluye una asignación se puede inicializar mediante un elemento de asignación por separado (:ref:`spec-Assignments`).  Por ejemplo, el artículo anterior se puede separar en los siguientes dos elementos:

.. code-block:: minizinc

    int: A;
    ...
    A = 10;

Todas las variables que contienen un componente de parámetro deben definirse en instancia-tiempo.

Las variables pueden tener una o más anotaciones. :ref:`spec-Annotations` tiene más en las anotaciones.


.. _spec-enum_items:






Elementos de Enumeración
~~~~~~~~~~~~~~~~~~~~~~~~

Los elementos de tipo enumerados tienen esta sintaxis:

.. literalinclude:: grammar.mzn
  :language: minizincdef
  :start-after: % Enum items
  :end-before: %

Un ejemplo de una enumeración:

.. code-block:: minizinc

     enum country = {Australia, Canada, China, England, USA};

Cada alternativa se llama *caso de enumeración (enum case)*.  El identificador utilizado para nombrar cada caso (por ejemplo, :mzn:`Australia`) se llama el *nombre del caso enum (enum case name)*.

Debido a que los nombres de casos de enumeraciones residen en el espacio de nombres de nivel superior (:ref:`spec-Namespaces`), los nombres de los casos en diferentes enumeraciones deben ser distintos.

Una enumeración puede declararse pero no definirse, en cuyo caso debe definirse en otra parte dentro del modelo o en un archivo de datos.
Por ejemplo, un archivo de modelo podría contener esto:

.. code-block:: minizinc

    enum Workers;
    enum Shifts;

Y el archivo de datos podría contener esto:

.. code-block:: minizinc

    Workers = { welder, driller, stamper };
    Shifts  = { idle, day, night };

Algunas veces es útil poder referirse a uno de los nombres de las cajas enum dentro del modelo. Esto se puede lograr usando una variable. El modelo leería:

.. code-block:: minizinc

    enum Shifts;
    Shifts: idle;            % Variable representing the idle constant.

Y el archivo de datos:

.. code-block:: minizinc

    enum Shifts = { idle_const, day, night };
    idle = idle_const;      % Assignment to the variable.

Aunque la constante :mzn:`idle_const` no se puede mencionar en el modelo, la variable :mzn:`idle` puede ser.

Todas las enumeraciones se deben definir en el momento de la instancia.

Los elementos de enumeraciones se pueden anotar. :ref:`spec-Annotations` tiene más detalles sobre las anotaciones.

Cada nombre de caso se puede forzar automáticamente al entero correspondiente a su índice en el tipo.

.. code-block:: minizinc

  int: oz = Australia;  % oz = 1

Para cada tipo enumerado :mzn:`T`, las siguientes funciones existen:

.. code-block:: minizinc

  % Devuelve el siguiente valor de enumeración mayor de x en un tipo de enumeración X
  function T: enum_next(set of T: X, T: x);
  function var T: enum_next(set of T: X, var T: x);

  % Devuelve el siguiente valor enumeración más pequeño de x en un tipo de enumeración X
  function T: enum_prev(set of T: X, T: x);
  function var T: enum_prev(set of T: X, var T: x);

  % Convertir x a un tipo de enumeración X
  function T: to_enum(set of T: X, int: x);
  function var T: to_enum(set of T: X, var int: x);

.. _spec-assignments:




Artículos de asignación
~~~~~~~~~~~~~~~~~~~~~~~

Las asignaciones tienen esta sintaxis:

.. literalinclude:: grammar.mzn
  :language: minizincdef
  :start-after: % Assign items
  :end-before: %

Por ejemplo:

.. code-block:: minizinc

    A = 10;

.. % \pjs{Add something about automatic coercion of index sets?}

.. _spec-constraint-items:



Objetos de restricción
~~~~~~~~~~~~~~~~~~~~~~

Los elementos de restricción forman el corazón de un modelo. Cualquier solución encontrada para un modelo satisfará todas sus limitaciones.
Los elementos de restricción tienen esta sintaxis:

.. literalinclude:: grammar.mzn
  :language: minizincdef
  :start-after: % Constraint items
  :end-before: %

Por ejemplo:

.. code-block:: minizinc

    constraint a*x < b;

La expresión en un elemento de restricción debe tener instanciación de tipo :mzn:`par bool` o :mzn:`var bool`; sin embargo, tenga en cuenta que las restricciones con expresiones fijas no son muy útiles.

.. _spec-solve-items:



Elementos Solve
~~~~~~~~~~~~~~~

Cada modelo debe tener exactamente un elemento de solución. Los elementos de resolución tienen la siguiente sintaxis:

.. literalinclude:: grammar.mzn
  :language: minizincdef
  :start-after: % Solve item
  :end-before: %

Ejemplo de elmentos solve:

.. code-block:: minizinc

    solve satisfy;
    solve maximize a*x + y - 3*z;

El elemento solve determina si el modelo representa un problema de satisfacción de restricciones o un problema de optimización. En el último caso, la expresión dada es la que se debe minimizar/maximizar.

La expresión en un elemento de solve de minimizar/maximizar puede tener un tipo entero o flotante

*Rationale: This is possible because all type-insts have a defined order.*
Tenga en cuenta que tener una expresión con una instanciación de tipo fija en un elemento solve no es muy útil, ya que significa que el modelo no requiere optimización.

Elementos solve puede ser anotado. :ref:`spec-Annotations` tiene más detalles sobre las anotaciones.

.. _spec-output-items:



Elementos de salida
~~~~~~~~~~~~~~~~~~~

Los elementos de salida se utilizan para presentar las soluciones de una instancia modelo.
Tienen la siguiente sintaxis:

.. literalinclude:: grammar.mzn
  :language: minizincdef
  :start-after: % Output items
  :end-before: %

Por ejemplo:

.. code-block:: minizinc

    output ["The value of x is ", show(x), "!\n"];

La expresión debe tener un tipo-instanciación de :mzn:`array[int] of par string`. Se puede componer utilizando el operador incorporado :mzn:`++` y las funciones incorporadas :mzn:`show`, :mzn:`show_int`, y :mzn:`show_float` (:ref:`spec-builtins`), así como interpolaciones de cuerdas (:ref:`spec-String-Interpolation-Expressions`). La salida es la concatenación de los elementos de la matriz. Si existen múltiples elementos de salida, la salida es la concatenación de todas sus salidas, en el orden en que aparecen en el modelo.

Si no hay ningún elemento de salida presente, la implementación debe imprimir todas las variables globales y sus valores en un formato legible.

.. _spec-annotation-items:


Artículos de Anotación
~~~~~~~~~~~~~~~~~~~~~~

Los elementos de anotación se utilizan para aumentar el tipo :mzn:`ann`.  Tienen la siguiente sintaxis:

.. literalinclude:: grammar.mzn
  :language: minizincdef
  :start-after: % Annotation items
  :end-before: %

Por ejemplo:

.. code-block:: minizinc

    annotation solver(int: kind);

Es un error de instanciación de tipo si una anotación se declara y/o define más de una vez en un modelo.

El uso de anotaciones se describe en :ref:`spec-Annotations`.

.. _spec-preds-and-fns:






Operaciones definidas por el usuario
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

..
  % XXX: not saying if operations need to be defined.  Implementation
  % currently requires functions and tests to be defined if used, but
  % predicates can be bodyless even if used.  Should perhaps require functions
  % to be defined even if they're not used (like parameters), and make tests
  % like predicates?

Los modelos MiniZinc pueden contener operaciones definidas por el usuario. Ellos tienen esta sintaxis:

.. literalinclude:: grammar.mzn
  :language: minizincdef
  :start-after: % Predicate, test and function items
  :end-before: %

Las expresiones de instanciación de tipos pueden incluir variables type-inst en la función y declaración de predicado.

Por ejemplo, el predicado :mzn:`even` verifica que su argumento sea un número par.

.. code-block:: minizinc

    predicate even(var int: x) =
        x mod 2 = 0;

Un predicado soportado de forma nativa por el solucionador de destino se puede declarar de la siguiente manera:

.. code-block:: minizinc

    predicate alldifferent(array [int] of var int: xs);

Las declaraciones de predicados que se admiten de forma nativa en MiniZinc están restringidas al uso de tipos de FlatZinc (por ejemplo, las matrices multidimensionales y no basadas en 1 están prohibidas).

.. % \pjs{need to fix this if we allow2d arrays in FlatZinc!}

Las declaraciones para las operaciones definidas por el usuario se pueden anotar. :ref:`spec-Annotations` tiene más detalles sobre las anotaciones.

.. _spec-basic-properties:






Propiedades básicas
+++++++++++++++++++

El término "predicado" generalmente se usa para referirse tanto a elementos de prueba como a elementos predicados. Cuando se deben distinguir los dos tipos, se pueden usar los términos "elemento de prueba" y "elemento predicado".

El retorno tipo-instanciación de un elemento de prueba es implícitamente :mzn:`par bool`.  El retorno tipo-instanciación de un elemento predicado es implícitamente :mzn:`var bool`.

Los predicados y las funciones pueden ser recursivos. La terminación de una llamada a función recursiva depende únicamente de sus argumentos fijos, es decir, las funciones recursivas y los predicados no se pueden usar para definir variables restringidas recursivamente.

.. % \Rationale{This ensures that the satisfiability of models is decidable.}

Los predicados y las funciones introducen sus propios nombres locales, siendo los de los argumentos formales. El alcance de estos nombres cubre el predicado/cuerpo de la función. Los nombres de argumento no se pueden repetir dentro de una declaración de predicado/función.


Polimorfismo ad-hoc
+++++++++++++++++++

MiniZinc admite polimorfismo ad-hoc mediante sobrecarga. Las funciones y los predicados (tanto integrados como definidos por el usuario) pueden sobrecargarse. Un nombre puede estar sobrecargado como una función y un predicado.

Es un error tipo-instanciación si una sola versión de una operación sobrecargada con una firma particular tipo-instanciación se define más de una vez en un modelo. Por ejemplo:

.. code-block:: minizinc

    predicate p(1..5: x);
    predicate p(1..5: x) = false;       % correcto : primera definición
    predicate p(1..5: x) = true;        % error    : definición repetida

La combinación de sobrecarga y coacciones puede causar problemas.
Se dice que dos sobrecargas de una operación * se superponen * si pueden coincidir con los mismos argumentos. Por ejemplo, las siguientes sobrecargas de :mzn:`p` se superponen, ya que ambos coinciden con la llamada :mzn:`p(3)`.

.. code-block:: minizinc

    predicate p(par int: x);
    predicate p(var int: x);

Sin embargo, los siguientes dos predicados no se superponen porque no pueden coincidir con el mismo argumento:

.. code-block:: minizinc

    predicate q(int:        x);
    predicate q(set of int: x);

Evitamos dos posibles problemas de sobrecarga al imponer algunas restricciones a la superposición de sobrecargas de operaciones.

#. El primer problema es la ambigüedad. La colocación diferente de las coerciones en los argumentos de operación puede permitir diferentes opciones para la función sobrecargada.
Por ejemplo, si una función MiniZinc :mzn:`f` está sobrecargado así:

   .. code-block:: minizinc

    function int: f(int: x, float: y) = 0;
    function int: f(float: x, int: y) = 1;

   Entonces :mzn:`f(3,3)` podría ser 0 o 1 dependiendo de las opciones de coerción/sobrecarga.

   Para evitar este problema, cualquier sobrecarga superpuesta de una operación debe ser semánticamente equivalente con respecto a la coerción.  Por ejemplo, las dos sobrecargas del predicado :mzn:`p` anteriormente debe tener cuerpos que son semánticamente equivalentes con respecto a la sobrecarga.

   Actualmente, este requisito no se verifica y el modelador debe satisfacerlo manualmente. En el futuro, es posible que solicitemos compartir los cuerpos entre las diferentes versiones de las operaciones sobrecargadas, lo que proporcionaría la satisfacción automática de este requisito.

#. El segundo problema es que ciertas combinaciones de sobrecargas podrían requerir una implementación de MiniZinc para realizar una búsqueda combinatoria con el fin de explorar diferentes opciones de coerción y sobrecarga. Por ejemplo, si la función :mzn:`g` está sobrecargado así:

   .. code-block:: minizinc

       function float: g(int: t1, float: t2) = t2;
       function int  : g(float: t1, int: t2) = t1;

  Entonces, cómo se resuelve la sobrecarga de :mzn:`g(3,4)` depende de su contexto:

   .. code-block:: minizinc

       float: s = g(3,4);
       int: t = g(3,4);

  En la definición de :mzn:`s` la primera definición sobrecargada debe ser utilizada mientras que en la definición de :mzn:`t` el segundo debe ser usado.

Para evitar este problema, todas las sobrecargas superpuestas de una operación deben cerrarse bajo la intersección de su entrada tipo-instanciación.  Es decir, si las versiones sobrecargadas tienen entrada de tipo-instanciación :math:`(S_1,....,S_n)` y :math:`(T_1,...,T_n)` entonces debe haber otra versión sobrecargada con entrada tipo-instanciación :math:`(R_1,...,R_n)` donde cada :math:`R_i` es el límite inferior más grande (*glb*) de :math:`S_i` y :math:`T_i`.

Además, todas las sobrecargas superpuestas de una operación deben ser monótonas. Es decir, si hay versiones sobrecargadas con entrada tipo-instanciación :math:`(S_1,....,S_n)` y :math:`(T_1,...,T_n)` y salida tipo-instanciación :math:`S` y :math:`T`, respectivamente, entonces :math:`S_i \preceq T_i` para todos :math:`i`, implica :math:`S \preceq T`.  En los sitios de llamadas, siempre se utiliza la sobrecarga de coincidencia que es más baja en el enrejado tipo-instanciación.

Para :mzn:`g` posteriormente, el tipo-instanciación intersection (o *glb*) de :mzn:`(int,float)` y :mzn:`(float,int)` es :mzn:`(int,int)`.  Por lo tanto, las versiones sobrecargadas no se cierran en intersección y el usuario debe proporcionar otra sobrecarga para :mzn:`g` con entrada de tipo-instanciación :mzn:`(int,int)`.  La definición natural es:

   .. code-block:: minizinc

       function int: g(int: t1, int: t2) = t1;

Una vez :mzn:`g` se ha aumentado con la tercera sobrecarga, satisface el requisito de monotonicidad porque la salida tipo-instanciación de la tercera sobrecarga es :mzn:`int` que es menor que la salida tipo-instanciación de las sobrecargas originales.

La monotonicidad y el cierre bajo la conjunción tipo-instanciación aseguran que siempre que se alcanza una función o predicado sobrecargado durante la comprobación de tipo-instanciación, siempre hay una versión "mínima" segura y única para elegir, por lo que la complejidad de la comprobación de tipo-instanciación permanece lineal. Por lo tanto, en nuestro ejemplo :mzn:`g(3,4)` siempre se resuelve eligiendo la nueva definición sobrecargada.





Variables locales
+++++++++++++++++

Las variables locales en los cuerpos de operación se introducen usando expresiones ``let``.
Por ejemplo, el predicado :mzn:`have_common_divisor` toma dos valores enteros y comprueba si tienen un divisor común mayor que uno:

.. code-block:: minizinc

    predicate have_common_divisor(int: A, int: B) =
        let {
            var 2..min(A,B): C;
        } in
            A mod C = 0 /\
            B mod C = 0;

Sin embargo, como :ref:`spec-Let-Expressions` explicado, porque :mzn:`C` no está definido, este predicado no puede ser llamado en un contexto negativo. La siguiente es una versión que podría llamarse en un contexto negativo:

.. code-block:: minizinc

    predicate have_common_divisor(int: A, int: B) =
        exists(C in 2..min(A,B))
            (A mod C = 0 /\ B mod C = 0);

.. _spec-annotations:



Anotaciones
-----------

Anotaciones: valores del tipo :mzn:`ann`, permitir que un modelador especifique información no declarativa y específica del solucionador que está más allá del lenguaje central. Las anotaciones no cambian el significado de un modelo, sin embargo, solo cómo se resuelve.

Las anotaciones se pueden adjuntar a variables (en sus declaraciones), expresiones, sinónimos de tipo-instanciación, elementos de enumeraciones, elementos de solución y operaciones definidas por el usuario.
Tienen la siguiente sintaxis:

.. literalinclude:: grammar.mzn
  :language: minizincdef
  :start-after: % Annotations
.. #  :end-before: %

Por ejemplo:

.. code-block:: minizinc

    int: x::foo;
    x = (3 + 4)::bar("a", 9)::baz("b");
    solve :: blah(4)
        minimize x;

Los tipos de expresiones de argumento deben coincidir con los tipos de argumento de la anotación declarada. A diferencia de los predicados y funciones definidos por el usuario, las anotaciones no pueden sobrecargarse. *Justificación: no hay una razón particularmente fuerte para esto, simplemente parecía hacer las cosas más simples.*


Las firmas de anotación pueden contener variables de tipo-instanciación.

El orden y el anidamiento de las anotaciones no importan. Para el caso de expresión, puede ser útil ver el conector de anotación :mzn:`::` como un operador sobrecargado:

.. code-block:: minizinc

    ann: '::'(any $T: e, ann: a);       % associative
    ann: '::'(ann:    a, ann: b);       % associative + commutative

Ambos operadores son asociativos, el segundo es conmutativo. Esto significa que las siguientes expresiones son todas equivalentes:

.. code-block:: minizinc

    e :: a :: b
    e :: b :: a
    (e :: a) :: b
    (e :: b) :: a
    e :: (a :: b)
    e :: (b :: a)

Esta propiedad también se aplica a las anotaciones sobre elementos de solución y elementos de declaración variable. *Justificación: esta propiedad simplifica las cosas, ya que permite que todas las combinaciones anidadas de anotaciones se traten como si fueran planas, evitando así la necesidad de determinar cuál es el significado de una anotación anotada. También simplifica el árbol de sintaxis abstracta MiniZinc al evitar la necesidad de representar el anidamiento.*

.. _spec-partiality:


Parcialidad
-----------

La presencia de tipo-instanciación restringida en MiniZinc significa que varias operaciones son potencialmente *parciales*, es decir, no están claramente definidas para todas las entradas posibles. Por ejemplo, ¿qué sucede si una función que espera un argumento positivo pasa un argumento negativo? ¿Qué sucede si a una variable se le asigna un valor que no satisface sus restricciones tipo-instanciación? ¿Qué sucede si un índice de matriz está fuera de límites? Esta sección describe lo que sucede en todos estos casos.

.. % \pjs{This is not what seems to happen in the current MiniZinc!}

En general, los casos que implican valores fijos que no satisfacen las restricciones conducen a abortos en tiempo de ejecución. *Justificación: nuestra experiencia muestra que si un valor fijo falla una restricción, es casi seguro que se debe a un error de programación. Además, estos casos son fáciles de verificar para una implementación.*

Pero los casos que involucran valores no fijados varían, como veremos. *Justificación: lo mejor que se puede hacer con los valores no fijados varía de un caso a otro. Además, es difícil verificar las restricciones en los valores no fijados, sobre todo porque durante la búsqueda una variable de decisión puede ser fija y, a continuación, retroceder hará que este valor se revierta, en cuyo caso abortar es una mala idea.*





Asignaciones parciales
~~~~~~~~~~~~~~~~~~~~~~

La primera operación que implica parcialidad es la asignación. Hay cuatro casos distintos para las asignaciones.

- Un valor asignado a una variable global fija, limitada se verifica en tiempo de ejecución; si el valor asignado no satisface sus restricciones, es un error en tiempo de ejecución. En otras palabras, esto:

  .. code-block:: minizinc

    1..5: x = 3;

Es equivalente a esto:

.. code-block:: minizinc

    int: x = 3;
    constraint assert(x in 1..5,
                      "assignment to global parameter 'x' failed")

- Un valor asignado a una variable global restringida no fija hace que la asignación actúe como una restricción. Si el valor asignado no satisface las restricciones de la variable, causa un error en el modelo en tiempo de ejecución.

En otras palabras, esto:

.. code-block:: minizinc

    var 1..5: x = 3;

Es equivalente a esto:

.. code-block:: minizinc

    var int: x = 3;
    constraint x in 1..5;

*Justificación: este comportamiento es fácil de entender y fácil de implementar.*

- Un valor asignado a una variable fija, limitada y localizada se verifica en tiempo de ejecución; si el valor asignado no satisface sus restricciones, es un error en tiempo de ejecución. En otras palabras, esto:

.. code-block:: minizinc

    let { 1..5: x = 3; } in x+1

Es equivalente a esto:

.. code-block:: minizinc

    let { int: x = 3; } in
        assert(x in 1..5,
               "assignment to let parameter 'x' failed", x+1)

- Un valor asignado a una variable localmente limitada y no fijada hace que la asignación actúe como una restricción; si la restricción falla en el tiempo de ejecución, la falla "burbujea" hacia el alcance booleano circundante más cercano, donde se interpreta como :mzn:`false`.

*Justificación: este comportamiento es coherente con las asignaciones a variables globales.*

Tenga en cuenta que en los casos en que un valor es parcialmente fijo y parcialmente no fijo, por ejemplo, algunas matrices, los diferentes elementos se verifican según los diferentes casos, y los elementos fijos se comprueban antes de los elementos no fijos. Por ejemplo:

.. code-block:: minizinc

    u = [ let { var 1..5: x = 6} in x, let { par 1..5: y = 6; } in y) ];

Esto provoca un aborto en tiempo de ejecución, porque el segundo elemento fijo se comprueba antes del primer elemento no fijo. Este orden también es cierto para los casos en las siguientes secciones. *Justificación: esto asegura que las fallas no pueden enmascarar interrupciones, lo que parece deseable.*



Predicados/Funciones parciales y Argumentos de Anotaciones
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

El segundo tipo de operación que implica parcialidad es llamadas y anotaciones.

..
  % The behaviour for these operations is simple:  constraints on arguments are
  % ignored.
  %
  % \Rationale{This is easy to implement and easy to understand.  It is also
  % justifiable in the sense that predicate/function/annotation arguments are
  % values that are passed in from elsewhere;  if those values are to be
  % constrained, that could be done earlier.  (In comparison, when a variable
  % with a constrained type-inst is declared, any assigned value must clearly
  % respect that constraint.)}

La semántica es similar a las asignaciones: los argumentos fijos que fallan sus restricciones provocarán abortos, y los argumentos no fijados que fallan sus restricciones causarán fallas, que se disparan hasta el alcance booleano circundante más cercano.


Accesos parciales de matriz
~~~~~~~~~~~~~~~~~~~~~~~~~~~

El tercer tipo de operación que implica parcialidad es el acceso a la matriz. Hay dos casos distintos.

- Un valor fijo utilizado como un índice de matriz se verifica en tiempo de ejecución; si el valor del índice no está en el conjunto de índice de la matriz, es un error en tiempo de ejecución.

- Un valor no fijo utilizado como un índice de matriz hace que el acceso actúe como una restricción; si el acceso falla en el tiempo de ejecución, la falla "burbujea" hacia el alcance booleano circundante más cercano, donde se interpreta como :mzn:`false`. Por ejemplo:


  .. code-block:: minizinc

    array[1..3] of int: a = [1,2,3];
    var int: i;
    constraint (a[i] + 3) > 10 \/ i = 99;

Aquí el acceso a la matriz falla, por lo que la falla sube hasta la disyunción, y :mzn:`i` está obligado a ser 99. *Justificación: a diferencia de las llamadas al predicado/función, los modeladores en la práctica a veces usan accesos de matriz que pueden fallar. En tales casos, el comportamiento de "burbujeo" es razonable.*

.. _spec-builtins:




Operaciones incorporadas
------------------------

Este apéndice enumera los operadores integrados, las funciones y los predicados. Pueden implementarse como integradas verdaderas, o en bibliotecas que se importan automáticamente para todos los modelos. Muchos de ellos están sobrecargados.

Los nombres de los operadores están escritos dentro de comillas simples cuando se usan en firmas de tipo, por ejemplo :mzn:`bool: '\/'(bool, bool)`.

Usamos la sintaxis :mzn:`TI: f(TI1,...,TIn)` para representar una operación llamada :mzn:`f` eso toma argumentos con un tipo-instanciación :mzn:`TI,...,TIn` y devuelve un valor con tipo-instanciación :mzn:`TI`.  Esto es un poco más compacto que la sintaxis MiniZinc habitual, ya que omite los nombres de los argumentos.



Operaciones de comparación
~~~~~~~~~~~~~~~~~~~~~~~~~~


Menos que. Otras comparaciones son similares:
mayor que (:mzn:`>`),
menor o igual (:mzn:`<=`),
mayor que o igual (:mzn:`>=`),
igual (:mzn:`==`, :mzn:`=`),
y desigualdad (:mzn:`!=`).

.. % \pjs{Check use of any here!}

.. code-block:: minizinc

      bool: '<'(    $T,     $T)
  var bool: '<'(var $T, var $T)





Operaciones aritmeticas
~~~~~~~~~~~~~~~~~~~~~~~


Adición. Otras operaciones numéricas son similares: resta (:mzn:`-`), y multiplicación (:mzn:`*`).

.. code-block:: minizinc

      int:   '+'(    int,       int)
  var int:   '+'(var int,   var int)
      float: '+'(    float,     float)
  var float: '+'(var float, var float)





Menos unaria (:mzn:`-`).  Suma unaria (:mzn:`+`) es similar.

.. code-block:: minizinc

      int:   '-'(    int)
  var int:   '-'(var int)
      float: '-'(    float)
  var float: '-'(var float)





Entero y división de punto flotante y módulo.

.. code-block:: minizinc

      int:   'div'(    int,       int)
  var int:   'div'(var int,   var int)
      int:   'mod'(    int,       int)
  var int:   'mod'(var int,   var int)
      float: '/'  (    float,     float)
  var float: '/'  (var float, var float)



El resultado de la operación de módulo, si no es cero, siempre tiene el mismo signo que su primer operando. Las operaciones de división entera y módulo están conectadas por la siguiente identidad:


.. code-block:: minizinc

  x = (x div y) * y + (x mod y)



Algunos ejemplos ilustrativos:

.. code-block:: minizinc

   7 div  4 =  1        7 mod  4 =  3
  -7 div  4 = -1       -7 mod  4 = -3
   7 div -4 = -1        7 mod -4 = 3
  -7 div -4 =  1       -7 mod -4 = -3





Suma de multiples números.
Producto (:mzn:`product`) es similar. Tenga en cuenta que la suma de una matriz vacía es 0, y el producto de una matriz vacía es 1.

.. code-block:: minizinc

      int:   sum(array[$T]  of     int  )
  var int:   sum(array[$T]  of var int  )
      float: sum(array[$T]  of     float)
  var float: sum(array[$T]  of var float)





Mínimo de dos valores; máximo (:mzn:`max`) es similar.

.. code-block:: minizinc

  any $T:    min(any $T,    any $T )





Mínimo de una matriz de valores; máximo (:mzn:`max`) es similar.
Aborta si el argumento el arreglo esta vacio.

.. code-block:: minizinc

  any $U:    min(array[$T]  of any $U)





Mínimo de un conjunto fijo; máximo (:mzn:`max`) es similar.
Aborta si el argumento el conjunto esta vacio.

.. code-block:: minizinc

  $T:    min(set of $T)





Valor absoluto de un número.

.. code-block:: minizinc

      int:   abs(    int)
  var int:   abs(var int)
      float: abs(    float)
  var float: abs(var float)





Raíz cuadrada de un número en punto flotante (``float``). Aborta si el argumento es negativo.

.. code-block:: minizinc

      float: sqrt(    float)
  var float: sqrt(var float)



Operador de energía.  Por ejemplo, :mzn:`pow(2, 5)` da como resultado :mzn:`32`.

.. code-block:: minizinc

    int: pow(int,       int)
  float: pow(float,     float)


..
  % We should also have:
  %  var float: pow(var float, int)


Natural exponent.

.. code-block:: minizinc

      float: exp(float)
  var float: exp(var float)





Logaritmo natural. Logaritmo en base 10 (:mzn:`log10`) y logaritmo en base
2 (:mzn:`log2`) son similares.

.. code-block:: minizinc

      float: ln(float)
  var float: ln(var float)



Logaritmo general; el primer argumento es la base.

.. code-block:: minizinc

  float: log(float, float)


Seno (:mzn:`sin`), Coseno (:mzn:`cos`), tangente (:mzn:`tan`), seno inverso
(:mzn:`asin`), coseno inverso (:mzn:`acos`), tangente inversa
(:mzn:`atan`), seno hiperbólico (:mzn:`sinh`), coseno hiperbólico
(:mzn:`cosh`), tangente hiperbólica (:mzn:`tanh`),
seno hiperbólico inverso (:mzn:`asinh`), coseno hiperbólico inverso
(:mzn:`acosh`) y tangente hiperbólica inversa (:mzn:`atanh`) son similares.

.. code-block:: minizinc

      float: sin(float)
  var float: sin(var float)


Operaciones Lógicas
~~~~~~~~~~~~~~~~~~~

Conjunción. Otras operaciones lógicas son similares:
disyunción (``\/``)
implicación inversa (:mzn:`<-`),
implicación directa (:mzn:`->`),
bi-implicación (:mzn:`<->`),
exclusive disjunction (:mzn:`xor`),
negación lógica (:mzn:`not`).

Tenga en cuenta que los operadores de implicación no están escritos usando :mzn:`=>`, :mzn:`<=` y :mzn:`<=>` como es el caso en algunos idiomas.  Esto permite :mzn:`<=` para representar en su lugar "menor que o igual".

.. code-block:: minizinc

      bool: '/\'(    bool,     bool)
  var bool: '/\'(var bool, var bool)



Cuantificación universal.
Cuantificación existencial (:mzn:`exists`) es similar.  Tenga en cuenta que, cuando se aplica a una lista vacía, :mzn:`forall` retorna :mzn:`true`, y :mzn:`exists` retorna :mzn:`false`.

  .. code-block:: minizinc

        bool: forall(array[$T]  of     bool)
    var bool: forall(array[$T]  of var bool)





N-aria disyunción exclusiva.
N-aria bi-implicación (:mzn:`iffall`) es similar, con :mzn:`true` en lugar de :mzn:`false`.

  .. code-block:: minizinc

        bool: xorall(array[$T]  of     bool: bs) = foldl('xor', false, bs)
    var bool: xorall(array[$T]  of var bool: bs) = foldl('xor', false, bs)


Operaciones de Matriz
~~~~~~~~~~~~~~~~~~~~~

Longitud de una matriz.

.. code-block:: minizinc

int: length(array[$T] of any $U)


Concatenación de lista Devuelve la lista (matriz indexada entera) que contiene todos los elementos del primer argumento seguidos por todos los elementos del segundo argumento, con elementos que ocurren en el mismo orden que en los argumentos. Los índices resultantes están en el rango :mzn:`1..n`, donde :mzn:`n` es la suma de las longitudes de los argumentos. *Justificación: Esto permite que las matrices en forma de listas se concatenen naturalmente y evita problemas con índices superpuestos. Los índices resultantes son consistentes con los de literales indexados implícitamente.*

Tenga en cuenta que :mzn:`'++'` también realiza la concatenación de cadenas.

.. code-block:: minizinc

  array[int] of any $T: '++'(array[int] of any $T, array[int] of any $T)


Conjuntos de índices de matrices. Si el argumento es un literal, retorna :mzn:`1..n` en donde :mzn:`n` es el largo de la sub-matriz.  De lo contrario, devuelve el conjunto de índices declarado o inferido. Esta lista es solo parcial, se extiende de manera obvia, para matrices de mayores dimensiones.

.. code-block:: minizinc

  set of $T:  index_set     (array[$T]      of any $V)
  set of $T:  index_set_1of2(array[$T, $U]  of any $V)
  set of $U:  index_set_2of2(array[$T, $U]  of any $V)
  ...

Reemplace los índices de la matriz dada por el último argumento con el
Producto cartesiano de los conjuntos dados por los argumentos anteriores. Existen versiones similares para matrices de hasta 6 dimensiones.

.. code-block:: minizinc

  array[$T1] of any $V: array1d(set of $T1, array[$U] of any $V)
  array[$T1,$T2] of any $V:
      array2d(set of $T1, set of $T2, array[$U] of any $V)
  array[$T1,$T2,$T3] of any $V:
      array3d(set of $T1, set of $T2, set of $T3, array[$U] of any $V)




Operaciones de coerción
~~~~~~~~~~~~~~~~~~~~~~~

Redondea un número de punto flotante (``float``) hacia :math:`+\infty`, :math:`-\infty`, y el entero más cercano, respectivamente.

.. code-block:: minizinc

  int: ceil (float)
  int: floor(float)
  int: round(float)


Conversiones explícitas de una instancia de tipo-instanciación.

.. code-block:: minizinc

      int:          bool2int(    bool)
  var int:          bool2int(var bool)
      float:        int2float(    int)
  var float:        int2float(var int)
  array[int] of $T: set2array(set of $T)




Operaciones de cadena
~~~~~~~~~~~~~~~~~~~~~


Conversión de cadena. Convierte cualquier valor en una cadena para fines de salida. La forma exacta de la cadena resultante depende de la implementación.

.. code-block:: minizinc

  string: show(any $T)


Conversión de cadena formateada para enteros.
Convierte el entero dado por el segundo argumento en una cadena a la derecha justificada por el número de caracteres dados por el primer argumento, o justificado a la izquierda si ese argumento es negativo.
Si el segundo argumento no es fijo, la forma de la cadena depende de la implementación.

.. code-block:: minizinc

  string: show_int(int, var int);


Conversión de cadena formateada para flotadores.
Convierte la flotación dada por el tercer argumento en una cadena a la derecha justificada por el número de caracteres dados por el primer argumento, o justificada a la izquierda si ese argumento es negativo.
La cantidad de dígitos que aparecen después del punto decimal viene dada por el segundo argumento.
Es un error en tiempo de ejecución para que el segundo argumento sea negativo.
Si el tercer argumento no es fijo, la forma de la cadena depende de la implementación.

.. code-block:: minizinc

  string: show_float(int, int, var float)


Concatenación de cadenas Tenga en cuenta que :mzn:`'++'` también realiza concatenación de matriz.

.. code-block:: minizinc

  string: '++'(string, string)


Concatenar una matriz de cadenas.
Equivalente a :mzn:`'++'` sobre la matriz, pero puede implementarse de manera más eficiente.

.. code-block:: minizinc

   string: concat(array[$T] of string)


Concatenar una matriz de cadenas, poniendo un separador entre cadenas adyacentes.
Devuelve la cadena vacía si la matriz está vacía.

.. code-block:: minizinc

   string: join(string, array[$T] of string)




Operaciones vinculadas y de dominio
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Las operaciones de límite :mzn:`lb` y :mzn:`ub` devuelve los límites fijos inferiores/superiores corregidos a la expresión.

Para los tipos numéricos, devuelven un valor límite inferior/superior, por ejemplo, el valor más-bajo/más-alto que la expresión puede tomar.

Para los tipos de conjuntos, devuelven un subconjunto/superconjunto, por ejemplo, la intersección/unión de todos los valores posibles de la expresión establecida.

Las operaciones enlazadas abortan en expresiones que no tienen un límite finito correspondiente.
Por ejemplo, este sería el caso de una variable declarada sin límites en una implementación que no asigna límites predeterminados. Las expresiones set siempre tienen un límite inferior finito de curso, es decir :mzn:`{}`, el conjunto vacío.

Numérico de límite inferior/superior:

.. code-block:: minizinc

  int:   lb(var int)
  float: lb(var float)
  int:   ub(var int)
  float: ub(var float)


Establecer límite inferior/superior:

.. code-block:: minizinc

  set of int: lb(var set of int)
  set of int: ub(var set of int)

Las versiones de las operaciones vinculadas que operan en matrices también están disponibles, devuelven un límite inferior seguro o límite superior para todos los miembros de la matriz - abortan si la matriz está vacía:

.. code-block:: minizinc

  int:        lb_array(array[$T] of var int)
  float:      lb_array(array[$T] of var float)
  set of int: lb_array(array[$T] of var set of int)
  int:        ub_array(array[$T] of var int)
  float:      ub_array(array[$T] of var float)
  set of int: ub_array(array[$T] of var set of int)




Dominio Entero:

.. code-block:: minizinc

  set of int: dom(var int)


La operación de dominio :mzn:`dom` devuelve un superconjunto fijo de los posibles valores de la expresión.

Entero de dominio de matriz, devuelve un superconjunto de todos los valores posibles que pueden aparecer en la matriz; esto se cancela si la matriz está vacía:

.. code-block:: minizinc

  set of int: dom_array(array[$T] of var int)

Tamaño de dominio para enteros:

.. code-block:: minizinc

  int: dom_size(var int)

La operación de tamaño de dominio :mzn:`dom_size` es equivalente a :mzn:`card(dom(x))`.

Tenga en cuenta que estas operaciones pueden producir resultados diferentes dependiendo de cuándo se evalúan y qué forma toma el argumento. Por ejemplo, considere la operación de límite inferior numérico.


- Si el argumento es una expresión fija, el resultado es el valor del argumento.

- Si el argumento es una variable de decisión, el resultado depende del contexto.

- Si la implementación puede determinar un límite inferior para la variable, el resultado es ese límite inferior. El límite inferior puede ser de la declaración de la variable, o más alto que debido al preprocesamiento, o menor que si se aplica un límite inferior definido por la implementación (por ejemplo, si la variable se declaró sin límite inferior, pero la implementación impone un límite mínimo ligado).

- Si la implementación no puede determinar un límite inferior para la variable, la operación aborta.

- Si el argumento es cualquier otro tipo de expresión no fijada, el límite inferior depende de los límites de las subexpresiones no fijadas y de los operadores de conexión.


.. _spec-option-type-operations:



Operaciones de Tipo de Opción
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

El valor del tipo de opción (:math:`\top`) es escrito

.. code-block:: minizinc

  opt $T:  '<>';


Uno puede determinar si una variable de tipo de opción realmente ocurre o no usando :mzn:`occurs` y :mzn:`absent`

.. code-block:: minizinc

  par bool: occurs(par opt $T);
  var bool: occurs(var opt $T);
  par bool: absent(par opt $T);
  var bool: absent(var opt $T);


Uno puede devolver el valor no opcional de una variable de tipo de opción usando la función :mzn:`deopt`

.. code-block:: minizinc

  par $T: deopt{par opt $T);
  var $T: deopt(var opt $T);


..
  % Note that this is not really a function only a pseudo function placeholder
  % used in the translation of option types to non-option types.
  % \pjs{Explain better}

.. _spec-other-operations:



Otras operaciones
~~~~~~~~~~~~~~~~~~~~~~~

Verifique que una expresión booleana sea verdadera, y aborte si no, imprimiendo el segundo argumento como el mensaje de error. El primero devuelve el tercer argumento, y es particularmente útil para los argumentos de comprobación de formalidad para los predicados y las funciones; importante, su tercer argumento es flojo, es decir, solo se evalúa si la condición tiene éxito. El segundo devuelve :mzn:`true` y es útil para verificaciones de cordura globales (por ejemplo, de datos de instancia) en elementos de restricción.


.. code-block:: minizinc

  any $T:   assert(bool, string, any $T)
  par bool: assert(bool, string)


Cancelar la evaluación, imprimiendo la cadena dada.

.. code-block:: minizinc

  any $T: abort(string)

Devuelve verdadero. Como efecto secundario, una implementación puede imprimir el primer argumento.

.. code-block:: minizinc

  bool: trace(string)


Devuelve el segundo argumento.
Como efecto secundario, una implementación puede imprimir el primer argumento.

.. code-block:: minizinc

  any $T: trace(string, any $T)

Verifique si el valor del argumento está fijo en este punto de la evaluación. Si no, aborta; si es así, devuelve su valor. Esto es más útil en los elementos de salida cuando las variables de decisión deben ser corregidas: permite que se utilicen en lugares donde se necesita un valor fijo, como las condiciones if-then-else.


.. code-block:: minizinc

  $T: fix(any $T)


Como arriba, pero retorna :mzn:`false` si el valor del argumento no es fijo.

.. code-block:: minizinc

  par bool: is_fixed(any $T)


.. _spec-content-types:



Tipos de Contenido
------------------

El tipo de contenido ``application/x-zinc-output`` define un formato de salida de texto para Zinc.
El formato amplía la sintaxis abstracta y la semántica dadas en


 :ref:`spec-Run-time-Outcomes`, and is discussed in detail in :ref:`spec-Output`.

La sintaxis completa es la siguiente:

.. literalinclude:: output.mzn
  :language: minizincdef

El texto de la solución para cada solución debe ser como se describe en :ref:`spec-Output-Items`.
Se debe agregar una nueva línea si el texto de la solución no finaliza con una nueva línea.

.. _spec-json:



Soporte de JSON
-----------------------------

MiniZinc puede admitir la lectura de parámetros de entrada y proporcionar resultados formateados como objetos JSON. Un archivo de entrada JSON debe tener la siguiente estructura:

- Consiste en un solo objeto de nivel superior

- Los miembros del objeto (pares clave-valor) representan parámetros del modelo

- Cada clave miembro debe ser un identificador MiniZinc válido (y proporciona el valor para el parámetro correspondiente del modelo)

- Cada valor de miembro puede ser uno de los siguientes:

  - Una cadena (asignado a un parámetro de cadena MiniZinc)

  - Un número (asignado a un parámetro MiniZinc int o float)

  - Los valores ``true`` o ``false`` (asignado a un parámetro bool MiniZinc)

  - Una matriz de valores. Las matrices de matrices solo se admiten si todas las matrices internas tienen la misma longitud, por lo que se pueden asignar a matrices multidimensionales MiniZinc.

  - Un conjunto de valores codificados como un objeto con un único miembro con clave ``"set"`` y una lista de valores (los elementos del conjunto).

Este es un ejemplo de un archivo de parámetros JSON que utiliza todas las características anteriores:

.. code-block:: json

    {
      "n" : 3,
      "distances" : [ [1,2,3],
                      [4,5,6]],
      "patterns"  : [ {"set" : [1,3,5]}, {"set" : [2,4,6]} ]
    }


El primer parámetro declara un entero simple ``n``. El parámetro ``distances`` es una matriz bidimensional; tenga en cuenta que todas las matrices internas deben ser del mismo tamaño para mapearse en una matriz bidimensional (rectangular) MiniZinc. El tercer parámetro es una matriz de conjuntos de enteros.

.. _spec-grammar:



Gramática completa
------------------

Elementos
~~~~~~~~~

.. literalinclude:: grammar.mzn
  :language: minizincdef
  :end-before: % Type-inst expressions

Expresiones de Tipo-Instanciación
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. literalinclude:: grammar.mzn
  :language: minizincdef
  :start-after: % Type-inst expressions
  :end-before: % Expressions

Expresiones
~~~~~~~~~~~

.. literalinclude:: grammar.mzn
  :language: minizincdef
  :start-after: % Expressions
  :end-before: % Miscellaneous

Elementos Misceláneos
~~~~~~~~~~~~~~~~~~~~~~~

.. literalinclude:: grammar.mzn
  :language: minizincdef
  :start-after: % Miscellaneous
