.. _ch-assumptions:

Assumptions (experimental)
==========================

.. note::

  The assumption interface is **experimental**.
  Its modelling syntax, the ``fzn_assume`` solver interface and the ``%%%mzn-core`` output described below may change or be removed in future versions of MiniZinc.

Some solvers can solve a model *under assumptions*: a set of Boolean conditions that are assumed to hold during the solving process.
If the model is unsatisfiable under those assumptions, such a solver can report an *unsatisfiable core*: a subset of the assumptions that is sufficient to cause the unsatisfiability.
This is useful for explanation and debugging: rather than just learning that a model is unsatisfiable, you learn *which* of your assumptions are in conflict.

MiniZinc exposes this through the ``assume`` predicate.
The assumptions you provide are recorded, and any core reported by the solver is translated back into the original MiniZinc expressions you wrote.

Only some solvers support this interface.
If the selected solver does not, the assumptions are posted as ordinary hard constraints.
The model is still solved correctly, but no unsatisfiable core is reported.
We also issue a warning to that effect.

Using ``assume``
----------------

The predicate lives in the experimental library in ``experimental/assume.mzn`` and must be included explicitly.
Give it the conditions that you want to assume as a single root-context constraint:

.. code-block:: minizinc

  include "experimental/assume.mzn";

  var 1..10: x;
  var 1..10: y;

  constraint assume([x + y < 5, x > 8]);

  solve satisfy;

Each element of the array is assumed to be ``true`` while solving.
``assume`` may only be used in a root context (i.e. as a top-level ``constraint``); using it in a reified or negated position is an error, because assumptions cannot be conditionally imposed.

An assumption that is fixed to ``false`` makes the model trivially unsatisfiable.
If this is detected when compiling the ``assume`` call then the offending assumption is immediately reported.
This detection is best effort: an assumption that is only fixed to ``false`` by *other* parts of the model might instead be reported by the solver as a simple core containing only ``false``.

Interpreting the core
---------------------

When the solver reports a core, MiniZinc prints it in terms of the original assumption expressions.
In the default output this is a comment line:

.. code-block:: none

  %%%mzn-core: [x + y < 5, x > 8]
  =====UNSATISFIABLE=====

When solving an optimisation problem, a solver may also report a core to justify that the solution it found is optimal; the same ``%%%mzn-core`` line is used in that case.

Expressions that are not individual array elements are reported as an indexed reference into the argument.
For example, if the assumptions come from a function call ``asmp(x)`` returning an array, the core is reported as ``asmp(x)[1]``, ``asmp(x)[2]``, and so on.

With the :ref:`machine-readable JSON output <ch-json-stream>` (``--json-stream``) the core is reported as a ``core`` message instead (see :ref:`ch-json-stream`).
