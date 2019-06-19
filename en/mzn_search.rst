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

.. literalinclude:: examples/nqueens.mzn
  :language: minizinc
  :name: ex-queens
  :caption: Model for n-queens (:download:`nqueens.mzn <examples/nqueens.mzn>`).

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

.. literalinclude:: examples/nqueens.mzn
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
    single: dom_w_deg

  Example variable choice annotations are:

  - :mzn:`input_order`: choose in order from the array
  - :mzn:`first_fail`: choose the variable with the smallest domain size, and
  - :mzn:`smallest`: choose the variable with the smallest value in its domain.
  - :mzn:`dom_w_deg`: choose the variable with the smallest value of domain 
    size divided by weighted degree, which is the number of times it has been
    in a constraint that caused failure earlier in the search.

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
  
.. literalinclude:: examples/nqueens-ann.mzn
  :language: minizinc
  :name: ex-queens-ann
  :caption: Annotated model for n-queens (:download:`nqueens-ann.mzn <examples/nqueens-ann.mzn>`).

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
  search_ann = int_search(q, input_order, indomain_random, complete);

The first just tries the queens in order setting them to the
minimum value, the second tries the queens variables in order, but sets
them to their median value, the third tries the queen variable with smallest
domain and sets it to the minimum value, and the final strategy
tries the queens variable with smallest domain setting it to its median
value.

Different search strategies can make a significant difference in
how easy it is to find solutions.
A small comparison of the number of failures made to find the first solution
of the n-queens problems using the 5 different search strategies
is shown in the table below (where --- means more than 100,000 failures).
Clearly the right search strategy can make a significant difference, and variables selection is more important than value selection, except that for this
problem random value selection is very powerful.

.. cssclass:: table-nonfluid table-bordered

+-----+-----------+--------------+--------+-----------+--------------+
|  n  | input-min | input-median | ff-min | ff-median | input-random |
+=====+===========+==============+========+===========+==============+
| 10  | 22        |  2           |  5     | 0         | 6            |
+-----+-----------+--------------+--------+-----------+--------------+
| 15  | 191       |  4           |  4     | 12        | 39           |
+-----+-----------+--------------+--------+-----------+--------------+
| 20  | 20511     |  32          |  27    | 16        | 2            |
+-----+-----------+--------------+--------+-----------+--------------+
| 25  | 2212      |  345         |  51    | 25        | 2            |
+-----+-----------+--------------+--------+-----------+--------------+
| 30  | ---       |  137         |  22    | 66        | 9            |
+-----+-----------+--------------+--------+-----------+--------------+
| 35  | ---       |  1722        |  52    | 12        | 12           |
+-----+-----------+--------------+--------+-----------+--------------+
| 40  | ---       |  ---         |  16    | 44        | 2            |
+-----+-----------+--------------+--------+-----------+--------------+
| 45  | ---       |  ---         |  41    | 18        | 6            |
+-----+-----------+--------------+--------+-----------+--------------+

.. number of nodes ??
  Different search strategies can make a significant difference in
  how easy it is to find solutions.
  A small comparison of the number of nodes made to find the first solution
  of the n-queens problems using the 5 different search strategies
  is shown in the table below (where --- means more than 100,000 nodes).
  Clearly the right search strategy can make a significant difference, and 
  variables selection is more important than value selection, except that for this
  problem random value selection is very powerful.

  +-----+-----------+--------------+--------+-----------+--------------+
  |  n  | input-min | input-median | ff-min | ff-median | input-random |
  +=====+===========+==============+========+===========+==============+
  | 10  | 49        |  11          |  17    | 7         | 17           |
  +-----+-----------+--------------+--------+-----------+--------------+
  | 15  | 391       |  20          |  16    | 34        | 89           |
  +-----+-----------+--------------+--------+-----------+--------------+
  | 20  | 41033     |  80          |  65    | 46        | 19           |
  +-----+-----------+--------------+--------+-----------+--------------+
  | 25  | 4439      |  709         |  120   | 66        | 23           |
  +-----+-----------+--------------+--------+-----------+--------------+
  | 30  | ---       |  297         |  67    | 152       | 40           |
  +-----+-----------+--------------+--------+-----------+--------------+
  | 35  | ---       |  3470        |  132   | 50        | 50           |
  +-----+-----------+--------------+--------+-----------+--------------+
  | 40  | ---       |  ---         |  64    | 116       | 34           |
  +-----+-----------+--------------+--------+-----------+--------------+
  | 45  | ---       |  ---         |  120   | 71        | 47           |
  +-----+-----------+--------------+--------+-----------+--------------+
  | 50  | ---       |  ---         |  395   | 42        | 90           |
  +-----+-----------+--------------+--------+-----------+--------------+



Restart
-------

.. index::
  single: search; restart

Any kind of depth first search for solving optimization problems 
suffers from the problem that wrong decisions made at the top of 
the search tree can take an exponential amount of search to undo.
One common way to ameliorate this problem is to restart the search 
from the top thus having a chance to make different decisions.

MiniZinc includes annotations to control restart behaviour. These 
annotations, like other search annotations, are attached to the 
solve item of the model. 


.. defblock:: Restart search annotations

  .. index::
	single: restart_luby
	single: restart_geometric
	single: restart_linear
	single: restart_constant
	single: restart_none

  The different restart annotations control how frequently a restart occurs. 
  Restarts occur when a limit in nodes is reached, where search returns to the
  top of the search tree and begins again. The possibilities are

  - :mzndef:`restart_constant(<scale>)` where :mzndef:`<scale>` is an integer 
    defining after how many nodes to restart. 
  - :mzndef:`restart_linear(<scale>)` where :mzndef:`<scale>` is an integer
    defining the initial number of nodes before the first restart. The second
    restart gets twice as many nodes, the third gets three times, etc.
  - :mzndef:`restart_geometric(<base>,<scale>)` where :mzndef:`<base>` is a 
    float and :mzndef:`<scale>` is an integer. The :mzn:`k` th restart has a
    node limit of :mzn:`<scale> * <base>^k`.
  - :mzndef:`restart_luby(<scale>)` where :mzndef:`<scale>` is an integer. 
    The :mzn:`k` th restart gets :mzn:`<scale>*L[k]` where :mzn`L[k]` is the
    :mzn:`k` th number in the Luby sequence. The Luby sequence looks like 
    1 1 2 1 1 2 4 1 1 2 1 1 2 4 8 ..., that is it repeats two copies of the 
    sequence ending in :mzn:`2^i` before adding the number :mzn:`2^{i+1}`.
  - :mzndef:`restart_none` dont apply any restart
    (useful for setting a :mzn:`ann` parameter that controls restart).


  Solvers behaviour where two or more restart annotations are used is
  undefined.

Restart search is much more robust in finding solutions, since it can avoid
getting stuck in a non-productive area of the search.  Note that restart
search does not make much sense if the underlying search strategy does
not do something different the next time it starts at the top.
For example the search annotation

.. code-block:: minizinc

  solve :: int_search(q, input_order, indomain_min, complete);
        :: restart_linear(1000)
        satisfy

does not very much sense since the underlying search is deterministic and
each restart will just redo the same search as the previous search.
Some solvers record the parts of the search tree that have already been 
searched and avoid them. This will mean deterministic restarts will simply
effectively continue the search from the previous position. This gives
no benefit to restarts, whose aim is to change decisions high in the search
tree.

The simplest way to ensure that something is different in each restart
is to use some randomization, either in variable choice or value choice.
Alternatively some variable selection strategies make use of information
gathered from earlier search and hence will give different behaviour, for
example :mzn:`dom_w_deg`. 

To see the effectiveness of restart lets examine the n-queens problem again
with the underlying search strategy

.. code-block:: minizinc

  int_search(q, first_fail, indomain_random, complete);

with one of four restart strategies

.. code-block:: minizinc

  r1 = restart_constant(100);
  r2 = restart_linear(100);
  r3 = restart_geometric(1.5,100);
  r4 = restart_luby(100);
  r5 = restart_none;

.. cssclass:: table-nonfluid table-bordered

+-----+-----------+--------------+-----------+-----------+------+
|  n  | constant  | linear       | geometric | luby      | none |
+=====+===========+==============+===========+===========+======+
| 10  | 35        |  35          |  35       | 35        | 14   |
+-----+-----------+--------------+-----------+-----------+------+
| 15  | 36        |  36          |  36       | 36        | 22   |
+-----+-----------+--------------+-----------+-----------+------+
| 20  | 15        |  15          |  15       | 16        |      |
+-----+-----------+--------------+-----------+-----------+------+
| 25  | 2212      |  345         |  51       | 25        |      |
+-----+-----------+--------------+-----------+-----------+------+
| 30  | ---       |  137         |  22       | 66        |      |
+-----+-----------+--------------+-----------+-----------+------+
| 35  | ---       |  1722        |  52       | 12        |      |
+-----+-----------+--------------+-----------+-----------+------+
| 40  | 148       |  148         |  194      | 148       | 15   |
+-----+-----------+--------------+-----------+-----------+------+
| 100 | 183       |  183         |  183      | 183       | 103  |
+-----+-----------+--------------+-----------+-----------+------+
| 500 | 1480      |  1480        |  1480     | 1480      | 1434 |
+-----+-----------+--------------+-----------+-----------+------+
| 1000| 994       |  994         |  994      | 994       | 994  |
+-----+-----------+--------------+-----------+-----------+------+

THE CURRENT EXPERIMENT IS USELESS!

.. _sec_warm_starts:

Warm Starts
-----------

In many cases when solving an optimization or satisfaction
problem we may have solved a
previous version of the problem which is very similar.  In this case it
can be advantageous to use the previous solution found when searching for
solution to the new problem. This is currently supported by some MIP backends.

The warm start annotations are attached to the solve item, just like other 
search annotations.


.. defblock:: Warm start search annotations

  .. index::
        single: warm_start

  The different restart annotations control how frequently a restart occurs.
  Restarts occur when a limit in nodes is reached, where search returns to the
  top of the search tree and begins again. The possibilities are

  - :mzndef:`warm_start(<vars>,<vals>)` where :mzndef:`<vars>` is a one 
    dimensional array of integer variables, and :mzndef:`<vals>` is a 
    one dimensional array of integer of the same length giving the warm start values
    for each integer variable in :mzn:`<vars>`.
  - :mzndef:`warm_start(<vars>,<vals>)` where :mzndef:`<vars>` is a one 
    dimensional array of float variables, and :mzndef:`<vals>` is a 
    one dimensional array of floats of the same length giving the warm start values
    for each float variable in :mzn:`<vars>`.
  - :mzndef:`warm_start(<vars>,<vals>)` where :mzndef:`<vars>` is a one 
    dimensional array of Boolean variables, and :mzndef:`<vals>` is a 
    one dimensional array of Booleans of the same length giving the warm start values
    for each Boolean variable in :mzn:`<vars>`.
  - :mzndef:`warm_start(<vars>,<vals>)` where :mzndef:`<vars>` is a one 
    dimensional array of set variables, and :mzndef:`<vals>` is a 
    one dimensional array of sets of integers of the same length giving the warm start values
    for each set variable in :mzn:`<vars>`.


The warm start annotation can be used by the solver as part of value selection. For example, if the selected
variable :mzn:`v[i]` has in its current domain the warm start value :mzn:`w[i]` then this is
the value selected for the variable.  If not the solver uses the existing value selection rule
applicable to that variable.
The order of warm_starts, relative to other search annotations, can be
important (especially for CP), so they all might need to be put into a ``seq_search`` as below:

.. code-block:: minizinc

    array[1..3] of var 0..10: x;
    array[1..3] of var 0.0..10.5: xf;
    var bool: b;
    array[1..3] of var set of 5..9: xs;
    constraint b+sum(x)==1;
    constraint b+sum(xf)==2.4;
    constraint 5==sum( [ card(xs[i]) | i in index_set(xs) ] );
    solve
      :: warm_start_array( [                     %%% Can be on the upper level
        warm_start( x, [<>,8,4] ),               %%% Use <> for missing values
        warm_start( xf, array1d(-5..-3, [5.6,<>,4.7] ) ),
        warm_start( xs, array1d( -3..-2, [ 6..8, 5..7 ] ) )
      ] )
      :: seq_search( [
        warm_start_array( [                      %%% Now included in seq_search to keep order
          warm_start( x, [<>,5,2] ),             %%% Repeated warm_starts allowed but not specified
          warm_start( xf, array1d(-5..-3, [5.6,<>,4.7] ) ),
          warm_start( xs, array1d( -3..-2, [ 6..8, 5..7 ] ) )
        ] ),
        warm_start( [b], [true] ),
        int_search(x, first_fail, indomain_min, complete)
      ] )
      minimize x[1] + b + xf[2] + card( xs[1] intersect xs[3] );

If you'd like to provide a most complete warmstart information, please provide values for all
variables which are output when there is no output item or when compiled with ``--output-mode dzn``.
.. Still, this excludes auxiliary variables introduced by ``let`` expressions. To capture them, you can customize
the output item, or try the FlatZinc level, see below.

..
  Using Warm Starts At The FlatZinc Level
  +++++++++++++++++++++++++++++++++++++++

  You can insert warm start information in the FlatZinc in the same way for all non-fixed variables.
  Just make sure the fzn interpreter outputs their values by annotating them as ``output_var(_array)``
  and capture the fzn output by, e.g., piping to ``solns2out --output-raw <file_raw.dzn>``.
  You can also insert high-level output into FZN warm start. When compiling the initial model, add
  empty warm start annotations for all important variables - they will be kept in FZN. In the next solve,
  fill the values. To fix the order of annotations, put them into a ``warm_start_array``.

  AUTOMATE, e.g., adding a solution in dzn format as a warm start during parsing?

  A MORE REALISTIC EXAMPLE OF THEIR USE (jobshop???)
