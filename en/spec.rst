Specification of MiniZinc
=========================

High-level Model Structure
--------------------------

A MiniZinc model consists of multiple *items*:

.. literalinclude:: grammar.mzn
  :language: minizincdef
  :start-after: % A MiniZinc model
  :end-before: %

Items can occur in any order; identifiers need not be declared before they are used. Items have the following top-level syntax:

.. literalinclude:: grammar.mzn
  :language: minizincdef
  :start-after: % Items
  :end-before: %

Types and Type-insts
--------------------

Type-inst expression overview
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. literalinclude:: grammar.mzn
  :language: minizincdef
  :start-after: % Type-inst expressions
  :end-before: %

Built-in Compound Types and Type-insts
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Sets
++++

.. literalinclude:: grammar.mzn
  :language: minizincdef
  :start-after: % Set type-inst expressions
  :end-before: %

Arrays
++++++

.. literalinclude:: grammar.mzn
  :language: minizincdef
  :start-after: % Array type-inst expressions
  :end-before: %


Full grammar
------------

.. literalinclude:: grammar.mzn
  :language: minizincdef
