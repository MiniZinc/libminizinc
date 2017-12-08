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
A newline must be appended if the solution text does not end with a newline.
*Rationale: This allows solutions to be extracted from output without necessarily knowing how the solutions are formatted.*
Solutions end with a sequence of ten dashes followed by a newline.

.. literalinclude:: output.mzn
  :language: minizincdef
  :start-after: % Unsatisfiable
  :end-before: %

The completness result is printed on a separate line. *Rationale: The strings are designed to clearly indicate the end of the solutions.*

.. literalinclude:: output.mzn
  :language: minizincdef
  :start-after: % Complete
  :end-before: %

If the search is complete, a statement corresponding to the outcome is printed.
For an outcome of no solutions the statement is that the model instance is unsatisfiable, for an outcome of no more solutions the statement is that the solution set is complete, and for an outcome of no better solutions the statement is that the last solution is optimal.
*Rationale: These are the logical implications of a search being complete.*

.. literalinclude:: output.mzn
  :language: minizincdef
  :start-after: % Messages
  :end-before: %

If the search is incomplete, one or more messages describing reasons for incompleteness may be printed.
Likewise, if any warnings occurred during search they are repeated after the completeness message.
Both kinds of message should have lines that start with ``%`` so they are recognized as comments by post-processing.
*Rationale: This allows individual messages to be easily recognised.*

For example, the following may be output for an optimisation problem:

.. code-block:: bash

    =====UNSATISFIABLE=====
    % trentin.fzn:4: warning: model inconsistency detected before search.

Note that, as in this case, an unbounded objective is not regarded as a source of incompleteness.

.. _spec-syntax-overview:
