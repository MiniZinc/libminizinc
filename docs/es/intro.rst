Introducción
============

MiniZinc es un lenguaje diseñado para especificar problemas de optimización con restricciones y problemas de decisión sobre enteros y números reales.

Un modelo MiniZinc no dicta *cómo* resolver el problema - el compilador de MiniZinc puede traducirlo en diferentes maneras de una forma adecuada para una amplia gama de *solvers*, tales como Programación de Restricciones (CP), Programación Lineal de Múltiple Entero (MIP) o Boolean Satisfactoría (SAT).

El lenguaje MiniZinc permite a los usuarios escribir modelos de una manera que está cerca de una formulación matemática del problema, utilizando la notación familiar como cuantificadores existenciales y universales, sumas sobre conjuntos de índices o conexiones lógicas como implicaciones e instrucciones *if-then-else*. Además, MiniZinc soporta definir predicados y funciones que permiten a los usuarios estructurar sus modelos (similar a procedimientos y funciones en lenguajes de programación regulares).

Los modelos MiniZinc son usualmente paramétricos, es decir, describen una *clase* entera de problemas en lugar de una instancia de problema individual. Por ejemplo, un modelo de un problema de enrutamiento de vehículos podría reutilizarse para generar planes semanales, instanciándolo con las demandas actualizadas de los clientes para la próxima semana.

MiniZinc está diseñado para interactuar fácilmente con diferentes solucionadores de fondo. Esto lo hace tomando un modelo MiniZinc de entrada y un archivo de datos transformandolo a un modelo FlatZinc. Los modelos FlatZinc consisten en declaraciones de variables y definiciones de restricciones, así como una definición de la función objetivo en el caso de que el problema sea un problema de optimización.

La traducción desde MiniZinc a FlatZinc es especializable para los solucionadores de backend individuales, por lo que pueden controlar en qué terminan las restricciones de forma. En particular, MiniZinc permite la especificación de restricciones globales por descomposición.

La traducción de MiniZinc a FlatZinc hace uso de una biblioteca de funciones y definiciones de predicado para el *solver* objetivo en particular, lo que permite al compilador MiniZinc producir FlatZinc especializado que sólo contiene los tipos de variables y restricciones soportados por el destino.

En particular, MiniZinc permite la especificación de *restricciones globales* por *descomposición*. Además, las *anotaciones* del modelo permiten al usuario ajustar el comportamiento del *solver*, independientemente del significado declarativo del modelo.


Estructura
----------

Esta documentación consta de tres partes.
:ref:`La primera parte <part-reference>` incluye la introducción y luego describe cómo descargar e instalar MiniZinc. Cómo realizar sus primeros pasos con el IDE de MiniZinc y las herramientas de línea de comandos.

:ref:`La segunda parte <part-tutorial>` es una introducción al modelado con MiniZinc, desde la sintaxis básica y técnicas de modelado simples, hasta temas más avanzados. También explica cómo MiniZinc se compila a FlatZinc.

:ref:`La tercera parte <part-reference>` contiene la documentación de referencia para MiniZinc. Incluyendo una definición del lenguaje MiniZinc, documentación sobre cómo interconectar un *solver* a FlatZinc, y una lista de todos los predicados y funciones en la biblioteca estándar de MiniZinc.



Cómo leer esto
--------------

Si eres nuevo en MiniZinc, sigue las instrucciones de instalación, la introducción al IDE y luego explora el tutorial. La mayoría del código de ejemplo se puede descargar, pero a veces es más útil escribir por ti mismo para obtener el idioma en su memoria muscular!.
Si necesita ayuda, visite el sitio web de MiniZinc en http://www.minizinc.org donde encontrará un foro de discusión.

.. only:: builder_html

  Algunos de los ejemplos de código se muestran en cuadros como el siguiente. Si un cuadro de código tiene un encabezado, normalmente también contiene un vínculo al código fuente (puede hacer clic en el nombre del archivo).

.. only:: builder_latex

  Algunos de los ejemplos de código se muestran en cuadros como el siguiente. Si un cuadro de código tiene un encabezado, suele enumerar el nombre de un archivo que se puede descargar desde http://minizinc.org/tutorial/examples.

.. literalinclude:: examples/dummy_es.mzn
  :language: minizinc
  :name: ex-ex
  :caption: Un código de ejemplo (:download:`dummy_es.mzn <examples/dummy_es.mzn>`)

A lo largo de la documentación, algunos conceptos se definen un poco más formalmente en secciones especiales como ésta.

.. defblock:: Más detalles

  Estas secciones se pueden omitir si sólo desea trabajar con el tutorial por primera vez, pero contienen información importante para cualquier usuario de MiniZinc!

Finalmente, si encuentra algún error en esta documentación, infórmelo a través de nuestro GitHub issue tracker.
