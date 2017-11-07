.. _sec-search:

Search
======

.. index::
  single: annotation

By default in MiniZinc there is no declaration of how
we want to search for solutions. This leaves the search
completely up to the underlying solver.
But sometimes, particularly for combinatorial integer problems,
we may want to specify how the search should be undertaken.
This requires us to communicate to the solver a :index:`search` strategy.
Note that the search strategy is *not* really part
of the model.
Indeed it is not required that each solver implements all
possible search strategies.
MiniZinc uses a consistent approach to communicating extra information
to the constraint solver using *annotations*.

Finite Domain Search
--------------------

.. index::
  single: search; finite domain

Search in a finite domain solver involves examining the
remaining possible values of variables and choosing to
constrain some variables further.
The search then adds a new constraint that
restricts the remaining values
of the variable
(in effect guessing where the solution might lie),
and then applies propagation to determine what other values
are still possible in solutions.
In order to guarantee completeness, the search leaves another
choice which is the negation of the new constraint.
The search ends either when
the finite domain solver detects that all constraints are satisfied,
and hence a solution has been found, or that the constraints are
unsatisfiable.
When unsatisfiability is detected
the search must proceed down a different set of
choices.  Typically finite domain solvers use :index:`depth first search <search; depth first>`
where they undo the last choice made and then try to make a new choice.

.. literalinclude:: examples/nqueens_es.mzn
  :language: minizinc
  :name: ex-queens
  :caption: Model for n-queens (:download:`nqueens_es.mzn <examples/nqueens_es.mzn>`).

A simple example of a finite domain problem is the :math:`n` queens
problem which requires that we
place :math:`n` queens on an :math:`n \times n` chessboard so that none can
attack another.
The variable :mzn:`q[i]` records in which row the queen in column :mzn:`i`
is placed. The :mzn:`alldifferent` constraints ensure
that no two queens are on the same row, or diagonal.
A typical (partial) search tree
for :mzn:`n = 9` is illustrated in :numref:`fig-9q-a`.
We first set :mzn:`q[1] = 1`, this removes values from the domains of other
variables, so that e.g. :mzn:`q[2]` cannot take the values 1 or 2.
We then set :mzn:`q[2] = 3`, this further removes values from the domains
of other variables. We set :mzn:`q[3] = 5` (its earliest possible value).
The state of the chess board after these three decisions is shown in
:numref:`fig-9q-b` where the queens indicate the position
of the queens fixed already and
the stars indicate positions where we cannot place a queen
since it would be able to take an already placed queen.

.. _fig-9q-a:

.. figure:: figures/tree-4.*

  Partial search trees for 9 queens

.. _fig-9q-b:

.. figure:: figures/chess9x9-3.*

  The state after the addition of ``q[1] = 1``, ``q[2] = 4``, ``q[3] = 5``

.. _fig-9q-c:

.. figure:: figures/chess9x9-4.*

  The initial propagation on adding further ``q[6] = 4``

A search strategy determines which choices to make. The decisions we have
made so far follow the simple strategy of picking the
first variable which is not fixed yet, and try to set it to its least
possible value.  Following this strategy the next decision would be
:mzn:`q[4] = 7`.
An alternate strategy for variable selection is to choose the variable whose
current set of possible values (*domain*) is smallest.
Under this so called *first-fail*
variable selection strategy the next decision would be
:mzn:`q[6] = 4`.
If we make this decision, then initially propagation removes the additional
values shown in :numref:`fig-9q-c`. But this leaves only one value for
:mzn:`q[8]`, :mzn:`q[8] = 7`, so this is forced, but then this leaves only one
possible value for :mzn:`q[7]` and :mzn:`q[9]`, that is 2. Hence a constraint must be
violated. We have detected unsatisfiability, and the solver must backtrack
undoing the last decision :mzn:`q[6] = 4` and adding its negation :mzn:`q[6] != 4`
(leading us to state (c) in the tree in :numref:`fig-9q-a`)
which forces :mzn:`q[6] = 8`. This removes some values from the domain
and then we again reinvoke the search strategy to decide what to do.

Many finite domain searches are defined in this way:
choose a variable to constrain further, and then choose how to
constrain it further.

Search Annotations
------------------

.. index::
  single: search; annotation
  single: solve

Search annotations in MiniZinc
specify how to search in order to find a solution to the
problem. The annotation is attached to the solve item, after the keyword
:mzn:`solve`.
The search annotation

.. literalinclude:: examples/nqueens_es.mzn
  :language: minizinc
  :lines: 11-12

appears on the solve item. Annotations are attached to parts of
the model using the connector :mzn:`::`.
This search annotation means that we should search by selecting from
the array of integer variables :mzn:`q`, the variable with the smallest
current domain (this is the :mzn:`first_fail` rule), and try setting
it to its smallest possible value
(:mzn:`indomain_min`
value selection), looking across the entire search tree
(:mzn:`complete` search).



.. % \begin{tabular}{|c|c|c|c|c|c|c|c|c|}
.. % \hline
.. % Q & . & . & . & . & . & . & . & . \\ \hline
.. % . & . & . &   &   & . &   &   &   \\ \hline
.. % . & Q & . & . & . & . & . & . & . \\ \hline
.. % . & . & . & . &   &   &   &   &   \\ \hline
.. % . & . & Q & . & . & . & . & . & . \\ \hline
.. % . & . & . & . & . & . &   &   &   \\ \hline
.. % . & . & . &   & . & . & . &   &   \\ \hline
.. % . & . & . &   &   & . & . & . &   \\ \hline
.. % . & . & . &   &   &   & . & . & . \\ \hline
.. % \end{tabular}

.. defblock:: Basic search annotations

  .. index::
    single: int_search
    single: bool_search
    single: set_search

  There are three basic search annotations corresponding to different
  basic variable types:

  - :mzndef:`int_search( <variables>, <varchoice>, <constrainchoice>, <strategy> )`
    where :mzndef:`<variables>` is a one dimensional array of :mzn:`var int`,
    :mzndef:`<varchoice>` is a variable choice annotation discussed below,
    :mzndef:`<constrainchoice>` is a choice of how to constrain a variable, discussed
    below, and :mzndef:`<strategy>` is a search strategy which we will assume for now
    is :mzn:`complete`.
  - :mzndef:`bool_search( <variables>, <varchoice>, <constrainchoice>, <strategy> )`
    where :mzndef:`<variables>` is a one dimensional array of :mzn:`var bool`
    and the rest are as above.
  - :mzndef:`set_search( <variables>, <varchoice>, <constrainchoice>, <strategy> )`
    where :mzndef:`<variables>` is a one dimensional array of :mzn:`var set of int`
    and the rest are as above.
  - :mzndef:`float_search( <variables>, <precision>, <varchoice>, <constrainchoice>, <strategy> )`
    where :mzndef:`<variables>` is a one dimensional array of :mzn:`var float`,
    :mzndef:`<precision>` is a fixed float specifying the :math:`\epsilon` below which
    two float values are considered equal,
    and the rest are as above.

  .. index::
    single: search; variable choice
    single: input_order
    single: first_fail
    single: smallest

  Example variable choice annotations are:

  - :mzn:`input_order`: choose in order from the array
  - :mzn:`first_fail`: choose the variable with the smallest domain size, and
  - :mzn:`smallest`: choose the variable with the smallest value in its domain.

  .. index::
    single: search; constrain choice
    single: indomain_min
    single: indomain_median
    single: indomain_random
    single: indomain_split

  Example ways to constrain a variable are:

  - :mzn:`indomain_min`: assign the variable its smallest domain value,
  - :mzn:`indomain_median`: assign the variable its median domain value,
  - :mzn:`indomain_random`: assign the variable a random value from its domain, and
  - :mzn:`indomain_split` bisect the variables domain excluding the upper half.

  The :mzndef:`<strategy>` is almost always :mzn:`complete` for complete search.
  For a complete list of variable and constraint choice annotations
  see the FlatZinc specification in the MiniZinc reference
  documentation.

We can construct more complex search strategies using search
constructor annotations. There is only one such annotation at present:

.. index::
  single: search; sequential
  single: seq_search

.. code-block:: minizinc

  seq_search([ <search-ann>, ..., <search-ann> ])

The sequential search constructor first undertakes the search given
by the first annotation in its list, when all variables in this annotation
are fixed it undertakes the second search annotation, etc. until all
search annotations are complete.

Consider the jobshop scheduling model shown in :numref:`ex-jobshop3`.
We could replace the solve item with

.. code-block:: minizinc

  solve :: seq_search([
               int_search(s, smallest, indomain_min, complete),
               int_search([end], input_order, indomain_min, complete)])
        minimize end

which tries to set start times :mzn:`s` by choosing the job that can start
earliest and setting it to that time. When all start times are complete
the end time :mzn:`end` may not be fixed. Hence we set it to
its minimal possible value.

Annotations
-----------

.. index::
  single: annotation

Annotations are a first class object in MiniZinc. We
can declare new annotations in a model, and declare and assign
to annotation variables.

.. defblock:: Annotations

  .. index::
    single: ann

  Annotations have a type :mzn:`ann`.
  You can declare an annotation
  :index:`parameter` (with optional assignment):

  .. code-block:: minizincdef

    ann : <ident>;
    ann : <ident> = <ann-expr> ;

  and assign to an annotation variable just as any other parameter.

  :index:`Expressions <expression>`, :index:`variable declarations <variable; declaration>`,
  and :mzn:`solve` items can all
  be annotated using the :mzn:`::` operator.

  We can declare a new :index:`annotation`
  using the :mzn:`annotation` :index:`item <item; annotation>`:

  .. code-block:: minizincdef

    annotation <annotation-name> ( <arg-def>, ..., <arg-def> ) ;

.. literalinclude:: examples/nqueens-ann_es.mzn
  :language: minizinc
  :name: ex-queens-ann
  :caption: Annotated model for n-queens (:download:`nqueens-ann_es.mzn <examples/nqueens-ann_es.mzn>`).

The program in :numref:`ex-queens-ann` illustrates the use of annotation
declarations, annotations and annotation variables.
We declare a new annotation :mzn:`bitdomain` which is meant to tell
the solver that variables domains should be represented via bit arrays
of size :mzn:`nwords`.
The annotation is attached to the declarations of the variables :mzn:`q`.
Each of the :mzn:`alldifferent` constraints is annotated with
the built in annotation :mzn:`domain`
which instructs the solver to use
the domain propagating version of :mzn:`alldifferent` if it has one.
An annotation variable :mzn:`search_ann` is declared and used
to define the search strategy.  We can give the value to the search
strategy in a separate data file.

Example search annotations might be the following (where
we imagine each line is in a separate data file)

.. code-block:: minizinc

  search_ann = int_search(q, input_order, indomain_min, complete);
  search_ann = int_search(q, input_order, indomain_median, complete);
  search_ann = int_search(q, first_fail, indomain_min, complete);
  search_ann = int_search(q, first_fail, indomain_median, complete);

The first just tries the queens in order setting them to the
minimum value, the second tries the queens variables in order, but sets
them to their median value, the third tries the queen variable with smallest
domain and sets it to the minimum value, and the final strategy
tries the queens variable with smallest domain setting it to its median
value.

Different search strategies can make a significant difference in
how easy it is to find solutions.
A small comparison of the number of choices made to find the first solution
of the n-queens problems using the 4 different search strategies
is shown in the table below (where --- means more than 100,000 choices).
Clearly the right search strategy can make a significant difference.

.. cssclass:: table-nonfluid table-bordered

+-----+-----------+--------------+--------+-----------+
|  n  | input-min | input-median | ff-min | ff-median |
+=====+===========+==============+========+===========+
| 10  | 28        |  15          |  16    | 20        |
+-----+-----------+--------------+--------+-----------+
| 15  | 248       |  34          |  23    | 15        |
+-----+-----------+--------------+--------+-----------+
| 20  | 37330     |  97          |  114   | 43        |
+-----+-----------+--------------+--------+-----------+
| 25  | 7271      |  846         |  2637  | 80        |
+-----+-----------+--------------+--------+-----------+
| 30  | ---       |  385         |  1095  | 639       |
+-----+-----------+--------------+--------+-----------+
| 35  | ---       |  4831        |  ---   | 240       |
+-----+-----------+--------------+--------+-----------+
| 40  | ---       |  ---         |  ---   | 236       |
+-----+-----------+--------------+--------+-----------+
