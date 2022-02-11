.. _ch-json-stream:

Machine-readable JSON output format
===================================

MiniZinc supports a machine-readable output format activated with the ``--json-stream`` (see :ref:`ch-cmdline-options`) option.

In this mode, MiniZinc outputs a line-break delimited stream of JSON objects (called messages).
These are output to the standard output stream. The standard error stream is not affected by this mode.
Each message contains the key ``type`` with a string value denoting the kind of message.

.. defblock:: A note on the message format

  In the following sections, messages are formatted on multiple lines for readability, however actual messages are always a single line.

Solution messages
-----------------

.. code-block:: json
  
  {
    "type": "solution",
    "time": 1000,
    "output": {
      "foo": "foo output section",
      "bar": "bar output section"
    },
    "sections": ["foo", "bar"]
  }

This message is produced whenever a solution is output (at the point where a ``----------`` would typically be produced).

The ``time`` field is optional and is only present if run with ``--output-time``. It gives the elapsed time in milliseconds.

The ``output`` field contains an object mapping output section names to their contents.
Output section contents can be either:

- A string containing the output value
- A JSON value if the section is a JSON section (the ``json`` section, or a section ending with ``_json``)

The ``sections`` field contain a list of output section names (strings) to show the order that they were produced.

Note that all sections are included in the ``output`` key object.
The resulting output for enabled sections is available in the ``raw`` section.

Checker messages
----------------

.. code-block:: json
  
  {
    "type": "checker",
    "output": {
      "default": "foo output section"
    },
    "sections": ["default"]
  }

This message gives solution checker results and is produced directly preceding the solution it is associated with.
Its format is the same as the ``solution`` message.

Status messages
---------------

.. code-block:: json
  
  {
    "type": "status",
    "status": "ALL_SOLUTIONS",
    "time": 1000
  }

This message is produced when a final status message is received from a solver (at the point where a status line such as ``==========`` would be produced).

The ``status`` field contains the name of the solver status and can take one of the follow values:

- ``"ALL_SOLUTIONS"``
- ``"OPTIMAL_SOLUTION"``
- ``"UNSATISFIABLE"``
- ``"UNBOUNDED"``
- ``"UNSAT_OR_UNBOUNDED"``
- ``"UNKNOWN"``
- ``"ERROR"``

The ``time`` field is optional and is only present if run with ``--output-time``. It gives the elapsed time in milliseconds.

Statistics messages
-------------------

.. code-block:: json
  
  {
    "type": "statistics",
    "statistics": {
      "method": "satisfy",
      "flatTime": 1000
    }
  }

This message is produced whenever a set of statistics is output (at the point where ``%%%mzn-stat-end`` would typically be produced).

The ``statistics`` field contains an object mapping statistics names to their values.

Timestamp messages
------------------

.. code-block:: json
  
  {
    "type": "time",
    "time": 1000
  }

This message is produced to indicate the current solve time in a standalone way.
For example, when running with ``--canonicalize`` and ``--output-time``, solution messages are printed at the end, however as solutions are produced, ``time`` messages are output.

The ``time`` field gives the elapsed time in milliseconds.

Comment messages
----------------

.. code-block:: json
  
  {
    "type": "comment",
    "comment": "% comment produced by solver\n"
  }

This message is produced when a solver outputs a comment.

The ``comment`` field contains the comment as a string (including the leading ``%`` and trailing newline).

Trace messages
--------------

.. code-block:: json
  
  {
    "type": "trace",
    "section": "default",
    "message": "traced message\n"
  }

This message is produced when a ``trace_stdout``, ``trace_to_section()`` or similar call is evaluated.

The ``section`` field gives the section name as a string (``default`` when using ``trace_stdout``).

The ``message`` field gives the message contents as a string, or as a JSON value if appropriate (such as when using ``trace_exp``).

Note that ``trace()`` calls output to standard error as normal.

Profiling messages
------------------

.. code-block:: json
  
  {
    "type": "profiling",
    "entries": [...]
  }

This message is produced when ``--output-detailed-timing``

The ``entries`` field contains a list of timing information entry objects.

Entries have the following format:

.. code-block:: json
  
  {
    "filename": "model.mzn",
    "line": 1,
    "time": 100
  }

Paths messages
--------------

.. code-block:: json
  
  {
    "type": "paths",
    "paths": [...]
  }

This message is produced when ``--output-paths-to-stdout`` is used.

The ``paths`` field contains a list of objects representing either a variable path or a constraint path.

Variable paths have the following format:

.. code-block:: json

  {
    "flatZincName": "X_INTRODUCED_0_",
    "niceName": "x[1]",
    "path": "model.mzn|1|27|1|27|id|x;|0|0|0|0|il|0;"
  }

Constraint paths have the following format:

.. code-block:: json

  {
    "constraintIndex": 9,
    "path": "model.mzn|3|12|3|59|ca|forall;model.mzn|3|12|3|59|ac;model.mzn|3|20|3|20|i=4;model.mzn|3|23|3|23|j=5;model.mzn|3|47|3|58|bin|'!=';model.mzn|3|47|3|58|ca|int_lin_ne;"
  }

Error messages
--------------

.. code-block:: json
  
  {
    "type": "error",
    "what": "type error",
    "location": {...},
    "message": "cannot determine coercion from type float to type var int"
  }

This message is produced when an error occurs.

The ``what`` field contains the kind of error as a string.

The ``message`` field contains the error message as a string.

Error messages can also optionally contain one or both of:

- A ``location`` field containing a location object as described in :ref:`ch-json-stream-location`.
- A ``stack`` field containing a list of stack trace objects as described in :ref:`ch-json-stream-stack-trace`.

Some kinds of error messages have additional properties (depending on the ``what`` of the error message):

- The ``syntax error`` message can optionally contain the field ``includedFrom`` giving a list of file names.
- The ``cyclic include error`` message contains the field ``cycle`` giving a list of file names.

.. _ch-json-stream-location:

Locations
~~~~~~~~~

Locations in a file are represented as follows:

.. code-block:: json
  
  {
    "filename": "model.mzn",
    "firstLine": 1,
    "firstColumn": 1,
    "lastLine": 3,
    "lastColumn": 10
  }

.. _ch-json-stream-stack-trace:

Stack traces
~~~~~~~~~~~~

Stack traces are represented as a list of objects with the following format:

.. code-block:: json

  {
    "location": {...}
    "isCompIter": false,
    "description": "variable declaration"
  }

Warning messages
----------------

.. code-block:: json
  
  {
    "type": "warning",
    "location": {...},
    "stack": [...],
    "message": "Warning message"
  }

This message is produced when a warning is emitted.

The optional ``location`` field contains the relevant location (see :ref:`ch-json-stream-location`) if present.

The optional ``stack`` field contains a stack trace (see :ref:`ch-json-stream-stack-trace`) if present.

The ``message`` field contains the warning message as a string.

Note that if ``-Werror`` is used then ``error`` messages are produced instead.
