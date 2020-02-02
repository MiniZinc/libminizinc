.. _ch-fzn-interfacing:

Interfacing Solvers to Flatzinc
===============================

This document describes the interface between the MiniZinc system and FlatZinc solvers.

.. _ch-fzn-spec:

Specification of FlatZinc
-------------------------

This document is the specification of the FlatZinc modelling language.
It also includes a definition of the standard command line options a FlatZinc solver should support
in order to work with the ``minizinc`` driver program (and the MiniZinc IDE).

FlatZinc is the target constraint modelling language into which MiniZinc
models are translated.
It is a very simple solver independent problem specification language,
requiring minimal implementation effort to support.

Throughout this document: :math:`r_1`, :math:`r_2` denote float literals; :math:`x_1, x_2, \dots, x_k, i, j, k` denote int literals; :math:`y_1, y_2, \dots, y_k, y_i` denote literal array elements.

Comments
~~~~~~~~

Comments start with a percent sign ``%`` and extend to the end of the line. Comments can appear anywhere in a model.

Types
~~~~~

There are three varieties of types in FlatZinc.

- *Parameter* types apply to fixed values that are specified directly in the model.
- *Variable* types apply to values computed by the solver during search. Every parameter type has a corresponding variable type; the variable type being distinguished by a ``var`` keyword.
- *Annotations* and *strings*: annotations can appear on variable declarations, constraints, and on the solve goal. They provide information about how a variable or constraint should be treated by the solver (e.g., whether a variable should be output as part of the result or whether a particular constraint should implemented using domain consistency). Strings may appear as arguments to annotations, but nowhere else.

Parameter types
+++++++++++++++

Parameters are fixed quantities explicitly specified in the model
(see rule :mzndef:`<par-type>` in :numref:`ch-fzn-syntax`).

============================================ ===========================
Type                                         Values
============================================ ===========================
``bool``                                     ``true`` or ``false``
``float``                                    float
``int``                                      int
``set of int``                               subset of int
``array [1..`` :math:`n` ``] of bool``       array of bools
``array [1..`` :math:`n` ``] of float``      array of floats
``array [1..`` :math:`n` ``] of int``        array of ints
``array [1..`` :math:`n` ``] of set of int`` array of sets of ints
============================================ ===========================

A parameter may be used where a variable is expected, but not vice versa.

In predicate declarations the following additional parameter types are allowed.

=================================================================================== ===========================
Type                                                                                Values
=================================================================================== ===========================
:math:`r_a` ``..`` :math:`r_b`                                                      bounded float
:math:`x_a` ``..`` :math:`x_b`                                                      int in range
``{`` :math:`x_a, x_b, \ldots, x_k` ``}``                                           int in set
``set of`` :math:`x_a` ``..`` :math:`x_b`                                           subset of int range
``set of`` ``{`` :math:`x_a, x_b, \ldots, x_k` ``}``                                subset of int set
``array [1..`` :math:`n` ``] of`` :math:`r_a` ``..`` :math:`r_b`                    array of floats in range
``array [1..`` :math:`n` ``] of`` :math:`x_a` ``..`` :math:`x_b`                    array of ints in range
``array [1..`` :math:`n` ``] of set of`` :math:`x_a` ``..`` :math:`x_b`             array of sets of ints in range
``array [1..`` :math:`n` ``] of set of`` ``{`` :math:`x_a, x_b, \ldots, x_k` ``}``  array of subsets of set
=================================================================================== ===========================

A range :math:`x_a` ``..`` :math:`x_b` denotes a closed interval
:math:`\{x | x_a \leq x \leq x_b\}` (same for float ranges).

An array type appearing in a predicate declaration may use just
``int`` instead of ``1..`` :math:`n` for the array index range
in cases where the array argument can be of any length.

Variable types
++++++++++++++

Variables are quantities decided by the solver
(see rules :mzndef:`<basic-var-type>` and :mzndef:`<array-var-type>` in :numref:`ch-fzn-syntax`).

+-----------------------------------------------------------------------------------+
| Variable type                                                                     |
+===================================================================================+
|``var bool``                                                                       |
+-----------------------------------------------------------------------------------+
|``var float``                                                                      |
+-----------------------------------------------------------------------------------+
|``var`` :math:`r_a` ``..`` :math:`r_b`                                             |
+-----------------------------------------------------------------------------------+
|``var int``                                                                        |
+-----------------------------------------------------------------------------------+
|``var`` :math:`x_a` ``..`` :math:`x_b`                                             |
+-----------------------------------------------------------------------------------+
|``var`` ``{`` :math:`x_a, x_b, \ldots, x_k` ``}``                                  |
+-----------------------------------------------------------------------------------+
|``var set of`` :math:`x_a` ``..`` :math:`x_b`                                      |
+-----------------------------------------------------------------------------------+
|``var set of {`` :math:`x_a, x_b, \ldots, x_k` ``}``                               |
+-----------------------------------------------------------------------------------+
|``array [1..`` :math:`n` ``] of var bool``                                         |
+-----------------------------------------------------------------------------------+
|``array [1..`` :math:`n` ``] of var float``                                        |
+-----------------------------------------------------------------------------------+
|``array [1..`` :math:`n` ``] of var`` :math:`r_a` ``..`` :math:`r_b`               |
+-----------------------------------------------------------------------------------+
|``array [1..`` :math:`n` ``] of var int``                                          |
+-----------------------------------------------------------------------------------+
|``array [1..`` :math:`n` ``] of var`` :math:`x_a` ``..`` :math:`x_b`               |
+-----------------------------------------------------------------------------------+
|``array [1..`` :math:`n` ``] of var set of`` :math:`x_a` ``..`` :math:`x_b`        |
+-----------------------------------------------------------------------------------+
|``array [1..`` :math:`n` ``] of var set of {`` :math:`x_a, x_b, \ldots, x_k` ``}`` |
+-----------------------------------------------------------------------------------+

In predicate declarations the following additional variable types are allowed.

+-----------------------------------------------------------------------------------+
| Variable type                                                                     |
+===================================================================================+
|``var set of int``                                                                 |
+-----------------------------------------------------------------------------------+
|``array [1..`` :math:`n` ``] of var set of int``                                   |
+-----------------------------------------------------------------------------------+

An array type appearing in a predicate declaration may use just
``int`` instead of ``1..`` :math:`n` for the array index range
in cases where the array argument can be of any length.

The string type
+++++++++++++++

String literals and literal arrays of string literals can appear as
annotation arguments, but not elsewhere.
Strings have the same syntax as in C programs (namely, they are
delimited by double quotes and the backslash character is used for
escape sequences).

Examples:

.. code-block:: minizinc

  ""                              % The empty string.
  "Hello."
  "Hello,\nWorld\t\"quoted!\""    % A string with an embedded newline, tab and quotes.

Values and expressions
~~~~~~~~~~~~~~~~~~~~~~

(See rule :mzndef:`<expr>` in :numref:`ch-fzn-syntax`)

Examples of literal values:


Type Literals
``bool`` ``true``, ``false``
``float`` ``2.718``, ``-1.0``, ``3.0e8``
``int`` ``-42``, ``0``, ``69``
``set of int`` ``{}``, ``{2, 3, 5}``, ``1..10``
``arrays`` ``[]``, ``[`` :math:`y_a, \ldots, y_k` ``]``

where each array element :math:`y_i` is either: a non-array literal, or the
name of a non-array parameter or variable, ``v``. For example:


.. code-block:: minizinc

  [1, 2, 3]             % Just literals
  [x, y, z]             % x, y, and z are variables or parameters.
  [x, 3]                % Mix of identifiers and literals

:numref:`ch-fzn-syntax` gives the regular expressions specifying the
syntax for float and int literals.

FlatZinc models
~~~~~~~~~~~~~~~

A FlatZinc model consists of:

#. zero or more external predicate declarations (i.e., a non-standard predicate that is supported directly by the target solver);
#. zero or more parameter declarations;
#. zero or more variable declarations;
#. zero or more constraints;
#. a solve goal

in that order.

FlatZinc uses the UTF-8 character set. Non-ASCII characters can only appear in string literals.

FlatZinc syntax is case sensitive (``foo`` and ``Foo`` are different
names).
Identifiers start with a letter (``[A-Za-z]``) and are followed by
any sequence of letters, digits, or underscores (``[A-Za-z0-9_]``).
Additionally, identifiers of variable or parameter names may start with an underscore.
Identifiers that correspond to the names of predicates, predicate parameters
and annotations cannot have leading underscores.

The following keywords are reserved and cannot be used as identifiers:
``annotation``, 
``any``, 
``array``, 
``bool``, 
``case``,
``constraint``, 
``diff``,
``div``,
``else``,
``elseif``, 
``endif``, 
``enum``, 
``false``, 
``float``,
``function``,
``if``,
``in``,
``include``,
``int``,
``intersect``,
``let``,
``list``,
``maximize``,
``minimize``,
``mod``,
``not``,
``of``,
``satisfy``,
``subset``,
``superset``,
``output``,
``par``,
``predicate``,
``record``,
``set``,
``solve``,
``string``,
``symdiff``,
``test``,
``then``,
``true``,
``tuple``,
``union``,
``type``,
``var``,
``where``,
``xor``.
Note that some of these keywords are not used in FlatZinc.
They are reserved because they are keywords in Zinc and MiniZinc.

FlatZinc syntax is insensitive to whitespace.

Predicate declarations
~~~~~~~~~~~~~~~~~~~~~~

(See rule :mzndef:`<predicate-item>` in :numref:`ch-fzn-syntax`)

Predicates used in the model that are not standard FlatZinc must be
declared at the top of a FlatZinc model, before any other lexical items.
Predicate declarations take the form

.. literalinclude:: fzn-grammar.mzn
  :language: minizincdef
  :start-after: % Predicate items
  :end-before: %

Annotations are not permitted anywhere in predicate declarations.

It is illegal to supply more than one predicate declaration for a given
:mzndef:`<identifier>`.

Examples:

.. code-block:: minizinc

      % m is the median value of {x, y, z}.
      %
  predicate median_of_3(var int: x, var int: y, var int: z, var int: m);
  
      % all_different([x1, .., xn]) iff
      % for all i, j in 1..n: xi != xj.
      %
  predicate all_different(array [int] of var int: xs);
  
      % exactly_one([x1, .., xn]) iff
      % there exists an i in 1..n: xi = true
      % and for all j in 1..n: j != i -> xj = false.
      %
  predicate exactly_one(array [int] of var bool: xs);


Parameter declarations
~~~~~~~~~~~~~~~~~~~~~~

(See rule ``param_decl`` in :numref:`ch-fzn-syntax`)

Parameters have fixed values and must be assigned values:

.. literalinclude:: fzn-grammar.mzn
  :language: minizincdef
  :start-after: % Parameter declarations
  :end-before: %

where :mzndef:`<par-type>` is a parameter type, :mzndef:`<var-par-identifier>` is an identifier,
and :mzndef:`<par-expr>` is a literal value (either a basic integer, float or bool literal, or a set or array of such literals).

Annotations are not permitted anywhere in parameter declarations.

Examples:

.. code-block:: minizinc

  float: pi = 3.141;
  array [1..7] of int: fib = [1, 1, 2, 3, 5, 8, 13];
  bool: beer_is_good = true;

Variable declarations
~~~~~~~~~~~~~~~~~~~~~

(See rule ``var_decl`` in :numref:`ch-fzn-syntax`)

Variables have variable types and can be declared with optional assignments.
The assignment can fix a variable to a literal value, or create an alias to another
variable. Arrays of variables always have an assignment, defining them in terms of an array literal
that can contain identifiers of variables or constant literals.
Variables may be declared with zero or more annotations.

.. literalinclude:: fzn-grammar.mzn
  :language: minizincdef
  :start-after: % Variable declarations
  :end-before: %

where :mzndef:`<basic-var-type>` and :mzndef:`<array-var-type>` are variable types, :mzndef:`<var-par-identifier>` is an identifier,
:mzndef:`<annotations>` is a (possibly empty) set of annotations, :mzndef:`<basic-expr>` is an identifier or a literal, and :mzndef:`<array-literal>` is a literal array
value.

Examples:

.. code-block:: minizinc

  var 0..9: digit;
  var bool: b;
  var set of 1..3: s;
  var 0.0..1.0: x;
  var int: y :: mip;        % 'mip' annotation: y should be a MIP variable.
  array [1..3] of var 1..10: b = [y, 3, digit];


Constraints
~~~~~~~~~~~

(See rule :mzndef:`<constraint-item>` in :numref:`ch-fzn-syntax`)

Constraints take the following form and may include zero or more annotations:


.. literalinclude:: fzn-grammar.mzn
  :language: minizincdef
  :start-after: % Constraint items
  :end-before: %

The arguments expressions (:mzndef:`<expr>`) can be literal values or identifiers.

Examples:

.. code-block:: minizinc

  constraint int_le(0, x);    % 0 <= x
  constraint int_lt(x, y);    % x <  y
  constraint int_le(y, 10);   % y <= 10
      % 'domain': use domain consistency for this constraint:
      % 2x + 3y = 10
  constraint int_lin_eq([2, 3], [x, y], 10) :: domain;

Solve item
~~~~~~~~~~

(See rule :mzndef:`<solve-item>` in :numref:`ch-fzn-syntax`)

A model finishes with a solve item, taking one of the following forms:

.. literalinclude:: fzn-grammar.mzn
  :language: minizincdef
  :start-after: % Solve item
  :end-before: %

The first alternative searches for any satisfying assignment, the second one searches for an assignment minimizing the given expression, and the third one for an assignment maximizing the expression. The :mzndef:`<basic-expr>` can be either a variable identifier or a literal value (if the objective function is constant).

A solution consists of a complete assignment where all variables in the
model have been given a fixed value.

Examples:


.. code-block:: minizinc

  solve satisfy;      % Find any solution using the default strategy.
  
  solve minimize w;   % Find a solution minimizing w, using the default strategy.
  
      % First label the variables in xs in the order x[1], x[2], ...
      % trying values in ascending order.
  solve :: int_search(xs, input_order, indomain_min, complete)
      satisfy;    % Find any solution.
  
      % First use first-fail on these variables, splitting domains
      % at each choice point.
  solve :: int_search([x, y, z], first_fail, indomain_split, complete)
      maximize x; % Find a solution maximizing x.

Annotations
~~~~~~~~~~~

Annotations are optional suggestions to the solver concerning how
individual variables and constraints should be handled (e.g., a
particular solver may have multiple representations for int variables)
and how search should proceed.
An implementation is free to ignore any annotations it does not
recognise, although it should print a warning on the standard error
stream if it does so.
Annotations are unordered and idempotent: annotations can be reordered
and duplicates can be removed without changing the meaning of the
annotations.


An annotation is prefixed by ``::``, and either just an identifier or an expression that looks like a predicate call:

.. literalinclude:: fzn-grammar.mzn
  :language: minizincdef
  :start-after: % Annotations
  :end-before: %

The arguments of the second alternative can be any expression or other annotations (without the leading ``::``).

Search annotations
++++++++++++++++++

While an implementation is free to ignore any or all annotations in a
model, it is recommended that implementations at least recognise the
following standard annotations for solve items.

.. code-block:: minizincdef

  seq_search([<searchannotation>, ...])

allows more than one search annotation to be specified in a particular
order (otherwise annotations can be handled in any order).

A :mzndef:`<searchannotation>` is one of the following:

.. code-block:: minizincdef

  int_search(<vars>, <varchoiceannotation>, <assignmentannotation>, <strategyannotation>)

  bool_search(<vars>, <varchoiceannotation>, <assignmentannotation>, <strategyannotation>)

  set_search(<vars>, <varchoiceannotation>, <assignmentannotation>, <strategyannotation>)

where :mzndef:`<vars>` is the identifier of an array variable or an array literal specifying
the variables to be assigned (ints, bools, or sets respectively). Note that these arrays may contain literal values.

:mzndef:`<varchoiceannotation>` specifies how the next variable to be assigned is
chosen at each choice point.
Possible choices are as follows (it is recommended that implementations
support the starred options):

+-----------------------+---------------+------------------------------------------------------------------------------------------------------+
| ``input_order``       | :math:`\star` | Choose variables in the order they appear in :mzndef:`vars`.                                         |
+-----------------------+---------------+------------------------------------------------------------------------------------------------------+
| ``first_fail``        | :math:`\star` | Choose the variable with the smallest domain.                                                        |
+-----------------------+---------------+------------------------------------------------------------------------------------------------------+
| ``anti_first_fail``   |               | Choose the variable with the largest domain.                                                         |
+-----------------------+---------------+------------------------------------------------------------------------------------------------------+
| ``smallest``          |               | Choose the variable with the smallest value in its domain.                                           |
+-----------------------+---------------+------------------------------------------------------------------------------------------------------+
| ``largest``           |               | Choose the variable with the largest value in its domain.                                            |
+-----------------------+---------------+------------------------------------------------------------------------------------------------------+
| ``occurrence``        |               | Choose the variable with the largest number of attached constraints.                                 |
+-----------------------+---------------+------------------------------------------------------------------------------------------------------+
| ``most_constrained``  |               | Choose the variable with the smallest domain, breaking ties using the number of constraints.         |
+-----------------------+---------------+------------------------------------------------------------------------------------------------------+
| ``max_regret``        |               | Choose the variable with the largest difference between the two smallest values in its domain.       |
+-----------------------+---------------+------------------------------------------------------------------------------------------------------+
| ``dom_w_deg``         |               | Choose the variable with the smallest value of domain size divided by weighted degree,               |
|                       |               | where the weighted degree is the number of times the variables been in a constraint which failed     |
+-----------------------+---------------+------------------------------------------------------------------------------------------------------+

:mzndef:`<assignmentannotation>` specifies how the chosen variable should be
constrained. 
Possible choices are as follows (it is recommended that implementations
support at least the starred options):

+---------------------------+---------------+------------------------------------------------------------------------------------------------------+
| ``indomain_min``          | :math:`\star` | Assign the smallest value in the variable's domain.                                                  |
+---------------------------+---------------+------------------------------------------------------------------------------------------------------+
| ``indomain_max``          | :math:`\star` | Assign the largest value in the variable's domain.                                                   |
+---------------------------+---------------+------------------------------------------------------------------------------------------------------+
| ``indomain_middle``       |               | Assign the value in the variable's domain closest to the mean of its current bounds.                 |
+---------------------------+---------------+------------------------------------------------------------------------------------------------------+
| ``indomain_median``       |               | Assign the middle value in the variable's domain.                                                    |
+---------------------------+---------------+------------------------------------------------------------------------------------------------------+
| ``indomain``              |               | Nondeterministically assign values to the variable in ascending order.                               |
+---------------------------+---------------+------------------------------------------------------------------------------------------------------+
| ``indomain_random``       |               | Assign a random value from the variable's domain.                                                    |
+---------------------------+---------------+------------------------------------------------------------------------------------------------------+
| ``indomain_split``        |               | Bisect the variable's domain, excluding the upper half first.                                        |
+---------------------------+---------------+------------------------------------------------------------------------------------------------------+
| ``indomain_reverse_split``|               | Bisect the variable's domain, excluding the lower half first.                                        |
+---------------------------+---------------+------------------------------------------------------------------------------------------------------+
| ``indomain_interval``     |               | If the variable's domain consists of several contiguous intervals,                                   |
|                           |               | reduce the domain to the first interval. Otherwise just split the variable's domain.                 |
+---------------------------+---------------+------------------------------------------------------------------------------------------------------+

Of course, not all assignment strategies make sense for all search
annotations (e.g., ``bool_search`` and ``indomain_split``).

Finally, :mzndef:`<strategyannotation>` specifies a search strategy;
implementations should at least support ``complete`` (i.e., exhaustive
search).

Output annotations
++++++++++++++++++

Model output is specified through variable annotations.
Non-array output variables are annotated with
``output_var``.
Array output variables are annotated with
``output_array([`` :math:`x_1` ``..`` :math:`x_2` ``, ... ])``
where :math:`x_1` ``..`` :math:`x_2` ``, ...`` are the index set ranges of the
original MiniZinc array (which
may have had multiple dimensions and/or index sets that do not start at
1). See :numref:`ch-fzn-output` for details on the output format.

Variable definition annotations
+++++++++++++++++++++++++++++++

To support solvers capable of exploiting functional relationships, a
variable defined as a function of other variables may be annotated thus:

.. code-block:: minizinc

  var int: x :: is_defined_var;

  ...

  constraint int_plus(y, z, x) :: defines_var(x);

(The ``defines_var`` annotation should appear on exactly one
constraint.)
This allows a solver to represent ``x`` internally as a representation
of ``y+z`` rather than as a separate constrained variable.
The ``is_defined_var`` annotation on the declaration of ``x``
provides "early warning" to the solver that such an option is
available.

Intermediate variables
++++++++++++++++++++++

Intermediate variables introduced during conversion of a MiniZinc
model to FlatZinc may be annotated thus:

.. code-block:: minizinc

  var int: X_INTRODUCED_3 :: var_is_introduced;

This information is potentially useful to the solver's search strategy.

Constraint annotations
++++++++++++++++++++++

Annotations can be placed on constraints advising the solver how the
constraint should be implemented.
Here are some constraint annotations supported by some solvers:


+---------------------------+----------------------------------------------------------------------------+
| ``bounds`` or ``boundsZ`` | Use integer bounds propagation.                                            |
+---------------------------+----------------------------------------------------------------------------+
| ``boundsR``               | Use real bounds propagation.                                               |
+---------------------------+----------------------------------------------------------------------------+
| ``boundsD``               | A tighter version of ``boundsZ`` where support for the bounds must exist.  |
+---------------------------+----------------------------------------------------------------------------+
| ``domain``                | Use domain propagation.                                                    |
+---------------------------+----------------------------------------------------------------------------+
|``priority(k)``            | where ``k`` is an integer constant indicating propagator priority.         |
+---------------------------+----------------------------------------------------------------------------+

.. _ch-fzn-output:

Output
------

An implementation can produce three types of output: solutions, statistics, and errors.

Solution output
~~~~~~~~~~~~~~~

An implementation must output values for all and only the variables
annotated with ``output_var`` or ``output_array`` (output
annotations must not appear on parameters). Output must be printed to
the standard output stream.

For example:

.. code-block:: minizinc

  var 1..10: x :: output_var;
  var 1..10: y;       % y is not output.
      % Output zs as a "flat" representation of a 2D array:
  array [1..4] of var int: zs :: output_array([1..2, 1..2]);

All non-error output must be sent to the standard output stream.

Output must take the following form:

.. code-block:: minizincdef

  <var-par-identifier> = <basic-literal-expr> ;

or, for array variables,

.. code-block:: minizincdef

  <var-par-identifier> = array<N>d(<a>..<b>, ..., [<y1>, <y2>, ... <yk>]);

where :mzndef:`<N>` is the number of index sets specified in the
corresponding :mzndef:`output_array` annotation,
:mzndef:`<a>..<b>, ...` are the index set ranges,
and :mzndef:`<y1>, <y2>, ... <yk>` are literals of the element type.

Using this format, the output of a FlatZinc model solution is
suitable for input to a MiniZinc model as a data file (this is why
parameters are not included in the output).

Implementations must ensure that *all* model variables (not
just the output variables) have satisfying assignments before printing a
solution.

The output for a solution must be terminated with ten consecutive
minus signs on a separate line: ``----------``.

Multiple solutions may be output, one after the other, as search
proceeds. How many solutions should be output depends on the mode the solver is run in as controlled by the ``-a`` command line flag (see :numref:`fzn-cmdline-options`).

If at least one solution has been found and search then terminates
having explored the whole search space, then ten
consecutive equals signs should be printed on a separate line:
``==========``.

If no solutions have been found and search terminates having explored
the whole search space, then ``=====UNSATISFIABLE=====`` should be
printed on a separate line.

If the objective of an optimization problem is unbounded, then
``=====UNBOUNDED=====`` should be printed on a separate line.

If no solutions have been found and search terminates having
*not* explored the whole search space, then
``=====UNKNOWN=====`` should be printed on a separate line.

Implementations may output further information about the solution(s),
or lack thereof, in the form of FlatZinc comments.

Examples:

Asking for a single solution to this model:

.. code-block:: minizinc

  var 1..3: x :: output_var;
  solve satisfy

might produce this output:

.. code-block:: minizinc

  x = 1;
  ----------

Asking for all solutions to this model:

.. code-block:: minizinc

  array [1..2] of var 1..3: xs :: output_array([1..2]);
  constraint int_lt(xs[1], xs[2]);    % x[1] < x[2].
  solve satisfy

might produce this output:

.. code-block:: minizinc

  xs = array1d(1..2, [1, 2]);
  ----------
  xs = array1d(1..2, [1, 3]);
  ----------
  xs = array1d(1..2, [2, 3]);
  ----------
  ==========


Asking for a single solution to this model:

.. code-block:: minizinc

  var 1..10: x :: output_var;
  solve maximize x;

should produce this output:

.. code-block:: minizinc

  x = 10;
  ----------
  ==========

The row of equals signs indicates that a complete search was performed
and that the last result printed is the optimal solution.

Running a solver on this model with some termination condition (such as a very short time-out):

.. code-block:: minizinc

  var 1..10: x :: output_var;
  solve maximize x;

might produce this output:

.. code-block:: minizinc

  x = 1;
  ----------
  x = 2;
  ----------
  x = 3;
  ----------

Because the output does not finish with ``==========``, search did not
finish, hence these results must be interpreted as approximate solutions
to the optimization problem.

Asking for a solution to this model:

.. code-block:: minizinc

  var 1..3: x :: output_var;
  var 4..6: y :: output_var;
  constraint int_lt(y, x);    % y < x.
  solve satisfy;

should produce this output:

.. code-block:: minizinc

  =====UNSATISFIABLE=====

indicating that a complete search was performed and no solutions were
found (i.e., the problem is unsatisfiable).

Statistics output
~~~~~~~~~~~~~~~~~

FlatZinc solvers can output statistics in a standard format so that it can be read by scripts,
for example, in order to run experiments and automatically aggregate the results.
Statistics should be printed to the standard output stream in the form of FlatZinc comments that follow a specific format.
Statistics can be output at any time during the solving, i.e., before the first solution, between solutions,
and after the search has finished. Statistics output corresponding to a solution should be the last one
before its '----------' separator.

Each value should be output on a line of its own in the following format:

.. code-block:: minizincdef

  %%%mzn-stat: <name>=<value>

Each block of statistics is terminated by a line of its own with the following format:

.. code-block:: minizincdef

  %%%mzn-stat-end

**Example**

.. code-block:: minizincdef

  %%%mzn-stat: objective=1e+308
  %%%mzn-stat: objectiveBound=0
  %%%mzn-stat: nodes=0
  %%%mzn-stat: solveTime=2.3567
  %%%mzn-stat-end

  (no feasible solution found yet but something can be printed...)

  %%%mzn-stat: objective=12345
  %%%mzn-stat: objectiveBound=122
  %%%mzn-stat: nodes=35
  %%%mzn-stat: solveTime=78.5799
  %%%mzn-stat-end

  (the corresponding feasible solution with value 12345 goes here
     or before its statistics but above the separator)
  ----------               (<- the solution separator)

  %%%mzn-stat: objective=379
  %%%mzn-stat: objectiveBound=379
  %%%mzn-stat: nodes=4725
  %%%mzn-stat: solveTime=178.5799
  %%%mzn-stat-end

  (the corr. optimal solution with value 379 goes here)
  ----------
  ==========               (<- the 'search complete' marker)

  %%%mzn-stat: objective=379      (<- this is the concluding output)
  %%%mzn-stat: objectiveBound=379
  %%%mzn-stat: nodes=13456
  %%%mzn-stat: solveTime=2378.5799
  %%%mzn-stat-end


The :mzndef:`<name>` describes the kind of statistics gathered, and the :mzndef:`<value>` can be any value of a MiniZinc type.
The following names are considered standard statistics:

======================== ====== ================================================
Name                     Type   Explanation
======================== ====== ================================================
``nodes``                int    Number of search nodes
``openNodes``            int    Number of open search nodes
``objective``            float  Current objective value
``objectiveBound``       float  Dual bound on the objective value
``failures``             int    Number of leaf nodes that were failed
``restarts``             int    Number of times the solver restarted the search
``variables``            int    Number of variables
``intVariables``         int    Number of integer variables created
``boolVariables``        int    Number of bool variables created
``floatVariables``       int    Number of float variables created
``setVariables``         int    Number of set variables created
``propagators``          int    Number of propagators created
``propagations``         int    Number of propagator invocations
``peakDepth``            int    Peak depth of search tree
``nogoods``              int    Number of nogoods created
``backjumps``            int    Number of backjumps
``peakMem``              float  Peak memory (in Mbytes)
``initTime``             float  Initialisation time (in seconds)
``solveTime``            float  Solving time (in seconds)
======================== ====== ================================================

Error and warning output
~~~~~~~~~~~~~~~~~~~~~~~~

Errors and warnings must be output to the standard error stream. When an error occurs, the implementation should exit with a non-zero exit code, signaling failure.


.. _ch-solver-specific-libraries:

Solver-specific Libraries
-------------------------

Constraints in FlatZinc can call standard predicates as well as solver-specific predicates. Standard predicates are the ones that the MiniZinc compiler assumes to be present in all solvers. Without further customisation, the compiler will try to compile the entire model into a set of these standard predicates.

Solvers can use custom predicates and *redefine* standard predicates by supplying a *solver specific library* of predicate declarations. Examples of such libraries can be found in the binary distribution of MiniZinc, inside the ``share/minizinc/gecode`` and ``share/minizinc/chuffed`` directories.

The solver-specific library needs to be made available to the MiniZinc compiler by specifying its location in the solver's configuration file, see :numref:`sec-cmdline-conffiles`.

Standard predicates
~~~~~~~~~~~~~~~~~~~

FlatZinc solvers need to support the predicates listed as ``FlatZinc builtins`` in the library reference documentation, see :numref:`ch-lib-flatzinc`.

Any standard predicate that is not supported by a solver needs to be *redefined*. This can be achieved by placing a file called ``redefinitions.mzn`` in the solver's MiniZinc library, which can contain alternative definitions of predicates, or define them as unsupported using the ``abort`` predicate.

Example for a ``redefinitions.mzn``:

.. code-block:: minizinc

  % Redefine float_sinh function in terms of exp
  predicate float_sinh(var float: a, var float: b) =
      b == (exp(a)-exp(-a))/2.0;
  
  % Mark float_tanh as unsupported
  predicate float_tanh(var float: a, var float: b) =
      abort("The builtin float_tanh is not supported by this solver.");

The redefinition can use the full MiniZinc language. Note, however, that redefining builtin predicates in terms of MiniZinc expressions can lead to problems if the MiniZinc compiler translates the high-level expression back to the redefined builtin.

The reference documentation (:numref:`ch-lib-flatzinc`) also contains sections on builtins that were added in later versions of MiniZinc. In order to maintain backwards compatibility with solvers that don't support these, they are organised in redefinition files with a version number attached, such as ``redefinitions-2.0.mzn``. In order to declare support for these builtins, the solver-specific library must contain the corresponding redefinitions file, with the predicates either redefined in terms of other predicates, or declared as supported natively by the solver by providing a predicate declaration without a body.

Example for a ``redefinitions-2.0.mzn`` that declares native support for the predicates added in MiniZinc 2.0:

.. code-block:: minizinc

  predicate bool_clause_reif(array[int] of var bool: as,
                             array[int] of var bool: bs,
                             var bool: b);
  predicate array_int_maximum(var int: m, array[int] of var int: x);
  predicate array_float_maximum(var float: m, array[int] of var float: x);
  predicate array_int_minimum(var int: m, array[int] of var int: x);
  predicate array_float_minimum(var float: m, array[int] of var float: x);


Solver-specific predicates
~~~~~~~~~~~~~~~~~~~~~~~~~~

Many solvers have built-in support for some of the constraints in the MiniZinc standard library. But without declaring which constraints they support, MiniZinc will assume that they don't support any except for the standard FlatZinc builtins mentioned in the section above.

A solver can declare that it supports a non-standard constraint by overriding one of the files of the standard library in its own solver-specific library. For example, assume that a solver supports the ``all_different`` constraint on integer variables. In the standard library, this constraint is defined in the file ``fzn_all_different_int.mzn``, with the following implementation:

.. code-block:: minizinc

  predicate fzn_all_different_int(array[int] of var int: x) =
    forall(i,j in index_set(x) where i < j) ( x[i] != x[j] );

A solver, let's call it *OptiSolve*, that supports this constraint natively can place a file with the same name, ``fzn_all_different_int.mzn``, in its library, and redefine it as follows:

.. code-block:: minizinc

  predicate optisolve_alldifferent(array[int] of var int: x);

  predicate fzn_all_different_int(array[int] of var int: x) =
    optisolve_alldifferent(x);

When a MiniZinc model that contains the ``all_different`` constraint is now compiled with the *OptiSolve* library, the generated FlatZinc will contain calls to the newly defined predicate ``optisolve_alldifferent``.

**Note:** The solver-specific library has been reorganised for MiniZinc version 2.3.0. Previously, a solver library would contain e.g. the file ``bin_packing.mzn`` in order to override the :mzn:`bin_packing` constraint. With version 2.3.0, this is still possible (in order to maintain backwards compatibility). However, the predicate :mzn:`bin_packing` from file ``bin_packing.mzn`` now delegates to the predicate :mzn:`fzn_bin_packing` in ``fzn_bin_packing.mzn``. This enables the :mzn:`bin_packing` predicate to check that the arguments are correct using assertions, before delegating to the solver-specific predicate. If your solver still uses the old library layout (i.e., overriding ``bin_packing.mzn`` instead of ``fzn_bin_packing.mzn``), you should consider updating it to the new standard.

.. _fzn-half-reif:

Reified and half-reified predicates
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

A reified constraint is a constraint that is not simply enforced, but whose truth value is bound to a Boolean variable. For example, a MiniZinc expression :mzn:`var bool: b = all_different(x);` would constrain :mzn:`b` to be true if and only if the variables :mzn:`x` take pairwise different values.

If a predicate is called in such a reified context, the MiniZinc compiler will try to find a version of the predicate with :mzn:`_reif` added to its identifier and an additional :mzn:`var bool` argument. For the above example, the compiler will try to generate the following FlatZinc code:

.. code-block:: minizinc
  
  var bool: b;
  constraint all_different_reif(x, b);

If the :mzn:`_reif` predicate does not exist, the compiler will try to use the definition of the original predicate. However, this may not be ideal: the original definition may make use of free variables in a :mzn:`let` expression (which is not allowed in reified contexts), or it may lead to inefficient solving.

Solver libraries should therefore provide reified versions of constraints whenever possible. The library contains files ``fzn_<constraintname>_reif.mzn`` for this purpose.

When a reified constraint is used in a *positive context* (see :numref:`pred-context`), the MiniZinc compiler can use a special version, called a half-reified predicate and identified by an :mzn:`_imp` suffix, instead of the :mzn:`_reif` predicate. Half-reified predicates essentially represent constraints that are *implied* by a Boolean variable rather than being equivalent to one. This typically leads to simpler translations or more efficient propagation (e.g., a half-reified :mzn:`all_different` only needs to *check* whether it is false, but it never has to implement the negation of the actual constraint).

For example, :mzn:`constraint y=0 \/ all_different(x)` might be translated as follows:

.. code-block:: minizinc

  var bool: X_INTRODUCED_1;
  var bool: X_INTRODUCED_2;
  constraint int_eq_imp(y,0,X_INTRODUCED_1);
  constraint all_different_imp(x, X_INTRODUCED_2);
  constraint array_bool_or([X_INTRODUCED_1,X_INTRODUCED_2]);

MiniZinc will decide whether to use half-reification case by case based on the availability of the :mzn:`_imp` predicate. As for reified constraints, it may be benefitial to provide specialised half-reified versions if the solver supports them. 

.. _fzn-cmdline-options:

Command-Line Interface and Standard Options
-------------------------------------------

In order to work with the ``minizinc`` command line driver, a FlatZinc solver must be an executable (which can include e.g. shell scripts) that can be invoked as follows:

.. code-block:: bash

  $ <executable-name> [options] model.fzn

where ``<executable-name>`` is the name of the executable. Solvers may support the following standard options:

.. option:: -a

  Instructs the solver to report *all* solutions in the case of satisfaction
  problems, or print *intermediate* solutions of increasing quality in the case
  of optimisation problems.

.. option:: -n <i>

  Instructs the solver to stop after reporting ``i`` solutions (only used with
  satisfaction problems).

.. option:: -f

  Instructs the solver to conduct a "free search", i.e., ignore any search 
  annotations. The solver is not *required* to ignore the annotations, but it
  is *allowed* to do so.

.. option:: -s

  Print statistics during and/or after the search for solutions. Statistics
  should be printed as FlatZinc comments to the standard output stream.
  See below for a standard format for statistics.

.. option:: -v

  Print log messages (verbose solving) to the standard error stream. If solvers
  choose to print to standard output instead, all messages must be valid
  comments (i.e., start with a ``%`` character).

.. option:: -p <i>

  Run with ``i`` parallel threads (for multi-threaded solvers).

.. option:: -r <i>

  Use ``i`` as the random seed (for any random number generators the solver
  may be using).

.. option:: -t <ms>

  Wall time limit ``ms`` milliseconds.

.. _sec-cmdline-conffiles:

Solver Configuration Files
--------------------------

In order for a solver to be available to MiniZinc, it has to be described in a *solver configuration file*. This is a simple file, in JSON or ``.dzn`` format, that contains some basic information such as the solver's name, version, where its library of global constraints can be found, and a path to its executable.
Examples are given in section Solver Backends in User Manual.

A solver configuration file must have file extension ``.msc`` (for MiniZinc Solver Configuration), and can be placed in any of the following locations:

- In the ``minizinc/solvers/`` directory of the MiniZinc installation. If you install MiniZinc from the binary distribution, this directory can be found at ``/usr/share/minizinc/solvers`` on Linux systems, inside the MiniZincIDE application on macOS system, and in the ``Program Files\MiniZinc IDE (bundled)`` folder on Windows.
- In the directory ``$HOME/.minizinc/solvers`` on Linux and macOS systems, and the Application Data directory on Windows systems.
- In any directory listed on the ``MZN_SOLVER_PATH`` environment variable (directories are separated by ``:`` on Linux and macOS, and by ``;`` on Windows systems).
- In any directory listed in the ``mzn_solver_path`` option of the global or user-specific configuration file (see :numref:`ch-user-config`)
- Alternatively, you can use the MiniZinc IDE to create solver configuration files, see :numref:`sec-ide-add-solvers` for details.

Solver configuration files must be valid JSON or ``.dzn`` files. As a JSON file, it must be an object with certain fields. As a ``.dzn`` file, it must consist of assignment items.

For example, a simple solver configuration in JSON format could look like this:

.. code-block:: json

  {
    "name" : "My Solver",
    "version": "1.0",
    "id": "org.myorg.my_solver",
    "executable": "fzn-mysolver"
  }


The same configuration in ``.dzn`` format would look like this:

.. code-block:: minizinc

  name = "My Solver";
  version = "1.0";
  id = "org.myorg.my_solver";
  executable = "fzn-mysolver";

Here is a list of all configuration options recognised by the configuration file parser. Any valid configuration file must at least contain the fields ``name``, ``version``, ``id``, and ``executable``.

- ``name`` (string, required): The name of the solver (displayed, together with the version, when you call ``minizinc --solvers``, and in the MiniZinc IDE).
- ``version`` (string, required): The version of the solver.
- ``id`` (string, required): A unique identifier for the solver, "reverse domain name" notation.
- ``executable`` (string, required): The executable for this solver that can run FlatZinc files. This can be just a file name (in which case the solver has to be on the current PATH), or an absolute path to the executable, or a relative path (which is interpreted relative to the location of the configuration file).
- ``mznlib`` (string, default ``""``): The solver-specific library of global constraints and redefinitions. This should be the name of a directory (either an absolute path or a relative path, interpreted relative to the location of the configuration file). For solvers whose libraries are installed in the same location as the MiniZinc standard library, this can also take the form ``-G<solverlib>``, e.g., ``-Ggecode`` (this is mostly the case for solvers that ship with the MiniZinc binary distribution).
- ``tags`` (list of strings, default empty): Each solver can have one or more tags that describe its features in an abstract way. Tags can be used for selecting a solver using the ``--solver`` option. There is no fixed list of tags, however we recommend using the following tags if they match the solver's behaviour:

  - ``"cp"``: for Constraint Programming solvers
  - ``"mip"``: for Mixed Integer Programming solvers
  - ``"float"``: for solvers that support float variables
  - ``"api"``: for solvers that use the internal C++ API

- ``stdFlags`` (list of strings, default empty): Which of the standard solver command line flags are supported by this solver. The standard flags are ``-a``, ``-n``, ``-s``, ``-v``, ``-p``, ``-r``, ``-f``, ``-t``.
- ``extraFlags`` (list of list of strings, default empty): Extra command line flags supported by the solver. Each entry must be a list of four strings. The first string is the name of the option (e.g. ``"--special-algorithm"``). The second string is a description that can be used to generate help output (e.g. ``"which special algorithm to use"``). The third string specifies the type of the argument (``"int"``, ``"bool"``, ``"float"``, ``"string"`` or ``"opt"``). The fourth string is the default value. The following types have an additional extended syntax:

  - ``"int:n:m"`` where ``n`` and ``m`` are integers, gives lower and upper bounds for the supported values
  - ``"float:n:m"`` where ``n`` and ``m`` are floating point numbers, gives lower and upper bounds for the supported values
  - ``"bool:onstring:offstring"`` specifies strings to add to the command line flag to turn it on (``onstring``) and off (``offstring``). E.g., ``["-interrupt","whether to catch Ctrl-C","bool:false:true","true"]`` specifies a command line option that can be called as ``-interrupt true`` or ``-interrupt false``. The standard behaviour (just ``"bool"``) means that the option is either added to the command line or not.
  - ``"opt:first option:second option:...:last option"`` specifies a list of possible values for the option

- ``supportsMzn`` (bool, default ``false``): Whether the solver can run MiniZinc directly (i.e., it implements its own compilation or interpretation of the model).
- ``supportsFzn`` (bool, default ``true``): Whether the solver can run FlatZinc. This should be the case for most solvers
- ``needsSolns2Out`` (bool, default ``true``): Whether the output of the solver needs to be passed through the MiniZinc output processor.
- ``needsMznExecutable`` (bool, default ``false``): Whether the solver needs to know the location of the MiniZinc executable. If true, it will be passed to the solver using the ``mzn-executable`` option.
- ``needsStdlibDir`` (bool, default ``false``): Whether the solver needs to know the location of the MiniZinc standard library directory. If true, it will be passed to the solver using the ``stdlib-dir`` option.
- ``isGUIApplication`` (bool, default ``false``): Whether the solver has its own graphical user interface, which means that MiniZinc will detach from the process and not wait for it to finish or to produce any output.

.. _ch-fzn-syntax:

Grammar
-------

This is the full grammar for FlatZinc. It is a proper subset of the MiniZinc grammar (see :numref:`spec-grammar`). However, instead of specifying all the cases in the MiniZinc grammar that do *not* apply to FlatZinc, the BNF syntax below contains only the relevant syntactic constructs. It uses the same notation as in :numref:`spec-syntax-notation`.

.. literalinclude:: fzn-grammar.mzn
  :language: minizincdef
