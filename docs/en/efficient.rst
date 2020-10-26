.. _sec-efficient:

Effective Modelling Practices in MiniZinc
=========================================

There are almost always multiple
ways to model the same problem, some of which generate models which are
efficient to solve, and some of which are not.
In general it is very hard to tell a priori which models are the most
efficient
for solving a particular problem, and indeed it may critically depend on
the underlying solver used, and search strategy.  In this chapter we
concentrate
on modelling practices that avoid inefficiency in generating models
and generated models. For solver-specific recommendations see User
Manual/Solver Backends.

Variable Bounds
---------------

.. index::
  single: variable; bound

Finite domain propagation engines, which are the principle type of solver
targeted by MiniZinc, are more effective the tighter the bounds on the
variables involved.  They can also behave badly with problems which
have subexpressions that take large integer values, since they may
implicitly limit the size of integer variables.

.. literalinclude:: examples/grocery.mzn
  :language: minizinc
  :name: ex-grocery
  :caption: A model with unbounded variables (:download:`grocery.mzn <examples/grocery.mzn>`).

The grocery problem shown in :numref:`ex-grocery` finds 4 items
whose prices in dollars add up to 7.11 and multiply up to 7.11.
The variables are declared unbounded. Running 

.. code-block:: bash

  $ minizinc --solver chuffed grocery.mzn

yields 

.. code-block:: none

  =====UNSATISFIABLE=====

This is because the 
intermediate expressions in the multiplication
are also :mzn:`var int` 
and are given default bounds in the solver
:math:`-1,000,000 \dots 1,000,000`,
and these ranges are too small to hold the
values that the intermediate expressions may need to take.

Modifying the model so that the items are declared with tight bounds

.. code-block:: minizinc

  var 1..711: item1;
  var 1..711: item2;
  var 1..711: item3;
  var 1..711: item4;

results in a better model, since now MiniZinc can infer bounds on the
intermediate expressions and use these rather than the default bounds.
With this modification, executing the model gives

.. code-block:: none

  {120,125,150,316}
  ----------
 
Note however that even the improved model may be too difficult for
some solvers.
Running 

.. code-block:: bash

  $ minizinc --solver g12lazy grocery.mzn

does not return an answer, since the solver builds a huge representation
for the intermediate product variables.

.. defblock:: Bounding variables

  .. index::
    single: variable; bound

  Always try to use bounded variables in models. 
  When using :mzn:`let`
  declarations to introduce new variables, always try to define them 
  with correct and tight bounds.  This will make your model more efficient,
  and avoid the possibility of unexpected overflows.
  One exception is when you introduce a new variable which is 
  immediately defined as equal to an expression. Usually MiniZinc will be
  able to infer effective bounds from the expression.


Effective Generators
--------------------

.. index::
  single: generator

Imagine we want to count the number of triangles (:math:`K_3` subgraphs)
appearing in a graph.  Suppose the graph is defined by
an adjacency matrix: :mzn:`adj[i,j]` is true if nodes :mzn:`i` and :mzn:`j` are
adjacent.  We might write

.. code-block:: minizinc

  int: count = sum ([ 1 | i,j,k in NODES where i < j  /\ j < k 
                         /\ adj[i,j] /\ adj[i,k] /\ adj[j,k]]);

which is certainly correct, but it examines all triples of nodes.
If the graph is sparse we can do better by realising that some
tests can be applied as soon as we select :mzn:`i` and :mzn:`j`.

.. code-block:: minizinc

  int: count = sum ([ 1 | i,j in NODES where i < j  /\ adj[i,j],
                          k in NODES where j < k /\ adj[i,k] /\ adj[j,k]]);

You can use the builitin :mzn:`trace` :index:`function <trace>` to help
determine what is happening inside generators. 

.. defblock:: Tracing

  The function :mzn:`trace(s,e)` prints the string :mzn:`s` before
  evaluating the expression :mzn:`e` and returning its value.
  It can be used in any context.  

For example, we can see how many times the test is performed in the inner
loop for both versions of the calculation.

.. literalinclude:: examples/count1.mzn
  :language: minizinc
  :lines: 8-15

Produces the output:

.. code-block:: none

  ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  ----------

indicating the inner loop is evaluated 64 times while

.. literalinclude:: examples/count2.mzn
  :language: minizinc
  :lines: 13-14

Produces the output:

.. code-block:: none

  ++++++++++++++++
  ----------

indicating the inner loop is evaluated 16 times.

Note that you can use the dependent strings in :mzn:`trace` to
understand what is happening during model creation.

.. literalinclude:: examples/count3.mzn
  :language: minizinc
  :lines: 13-15

will print out each of triangles that is found in the calculation.
It produces the output

.. code-block:: none

  (1,2,3)
  ----------

We have to admit that we cheated a bit here: In certain circumstances, the MiniZinc compiler is in fact able to re-order the arguments in a ``where`` clause automatically, so that they are evaluated as early as possible. In this case, adding the ``trace`` function in fact *prevented* this optimisation. In general, it is however a good idea to help the compiler get it right, by splitting the ``where`` clauses and placing them as close to the generators as possible.


Redundant Constraints
---------------------

.. index::
  single: constraint; redundant

The form of a model will affect how well the constraint solver can solve it.
In many cases adding constraints which are redundant, i.e. are logically
implied by the existing model, may improve the search for
solutions by making more information available to the solver earlier.

Consider the magic series problem from :ref:`sec-complex`.
Running this for :mzn:`n = 16` as follows:

.. code-block:: bash

  $ minizinc --solver gecode --all-solutions --statistics magic-series.mzn -D "n=16;"

might result in output

.. code-block:: none

  s = [12, 2, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0];
  ----------
  ==========

and the statistics showing 89 failures required.

We can add redundant constraints to the model. Since each number
in the sequence counts the number of occurrences of a number we know
that they sum up to :mzn:`n`. Similarly we know that the sum of
:mzn:`s[i] * i` must also add up to :mzn:`n` because the sequence is magic.
Adding these constraints 
gives the model in
:numref:`ex-magic-series2`.

.. literalinclude:: examples/magic-series2.mzn
  :language: minizinc
  :name: ex-magic-series2
  :caption: Model solving the magic series problem with redundant constraints (:download:`magic-series2.mzn <examples/magic-series2.mzn>`).

Running the same problem as before

.. code-block:: bash

  $ minizinc --solver gecode --all-solutions --statistics magic-series2.mzn -D "n=16;"

results in the same output, but with statistics showing just 14 failures
explored. The redundant constraints have allowed the solver to prune the
search much earlier.


Modelling Choices
-----------------

There are many ways to model the same problem in MiniZinc, 
although some may be more natural than others.
Different models may have very different efficiency of solving, and worse
yet, different models may be better or worse for different solving backends.
There are however some guidelines for usually producing better models:

.. defblock:: Choosing between models

  The better model is likely to have some of the following features

  - smaller number of variables, or at least those that are not
    functionally defined by other variables
  - smaller domain sizes of variables
  - more succinct, or direct, definition of the constraints of the model
  - uses global constraints as much as possible

  In reality all this has to be tempered by how effective the search is for
  the model.  Usually the effectiveness of search is hard to judge except by
  experimentation.

Consider the problem of finding permutations of :math:`n` numbers
from 1 to :math:`n` such that the differences between adjacent numbers
also form a permutation of numbers 1 to :math:`n-1`.
Note that the :mzn:`u` variables are functionally defined by
the :mzn:`x` variables so the raw search space is :math:`n^n`.
The obvious way to model this problem is shown in :numref:`ex-allint`.

.. literalinclude:: examples/allinterval.mzn
  :language: minizinc
  :name: ex-allint
  :caption: A natural model for the all interval series problem ``prob007`` in CSPlib (:download:`allinterval.mzn <examples/allinterval.mzn>`).

In this model the array :mzn:`x` represents the permutation of the :mzn:`n`
numbers and the constraints are naturally represented using :mzn:`alldifferent`.

Running the model

.. code-block:: bash

  $ minizinc --solver gecode --all-solutions --statistics allinterval.mzn -D "n=10;"

finds all solutions in 16077 nodes and 71ms

An alternate model uses array :mzn:`y` where :mzn:`y[i]` gives the
position of the number :mzn:`i` in the sequence.  
We also model the positions of the differences using variables
:mzn:`v`. :mzn:`v[i]` is the position in the sequence where the absolute difference
:mzn:`i` occurs.  If the values of :mzn:`y[i]` and :mzn:`y[j]` differ by one 
where :mzn:`j > i`, meaning the
positions are adjacent, then :mzn:`v[j-i]` is constrained to be the earliest
of these positions.
We can add two redundant constraints to this model:
since we know that a difference of :mzn:`n-1` must result, we know that
the positions of 1 and :mzn:`n` must be adjacent (:mzn:`abs( y[1] - y[n] ) = 1`),
which also tell us that the position of difference :mzn:`n-1` is
the earlier of :mzn:`y[1]` and :mzn:`y[n]`, i.e.
:mzn:`v[n-1] = min(y[1], y[n])`.
With this we can model the problem
as shown in :numref:`ex-allint2`. The output statement recreates the
original sequence :mzn:`x` from the array of positions :mzn:`y`.

.. literalinclude:: examples/allinterval2.mzn
  :language: minizinc
  :name: ex-allint2
  :caption: An inverse model for the all interval series problem ``prob007`` in CSPlib (:download:`allinterval2.mzn <examples/allinterval2.mzn>`).

The inverse model has the same size as the original model, in terms of
number of variables and domain sizes.  But the inverse model has a much more
indirect way of modelling the relationship between the :mzn:`y` and :mzn:`v` variables
as opposed to the relationship between :mzn:`x` and :mzn:`u` variables.
Hence we might expect the original model to be better.

The command

.. code-block:: bash

  $ minizinc --solver gecode --all-solutions --statistics allinterval2.mzn -D "n=10;"

finds all the solutions in  98343 nodes and 640 ms.
So the more direct modelling of the constraints is clearly paying off.

Note that the inverse model prints out the answers using the same :mzn:`x` view of 
the solution.  The way this is managed is using :mzn:`output_only` annotations.
The array :mzn:`x` is defined as a fixed array and annotated as :mzn:`output_only`.
This means it will only be evaluated, and can only be used in output statements.
Once a solution for :mzn:`y` is discovered the value of :mzn:`x` is calculated
during output processing, and hence can be displayed in the output.

.. defblock:: Output_only annotation

   .. index::
	single: output_only

   The :mzndef:`output_only` annotation can be applied to variable definitions.
   The variable defined must not be a :mzn:`var` type, it can only be :mzn:`par`.
   The variable must also have a right hand side definition giving its value.
   This right hand side definition can make use of :mzn:`fix` functions to access
   the values of decision variables, since it is evaluated at solution processing
   time


.. _sec-multiple-modelling-and-channels:

Multiple Modelling and Channels
-------------------------------

When we have two models for the same problem it may be 
useful to use both models together by tying the variables in the two models
together, since each can give different information to the solver.

.. literalinclude:: examples/allinterval3.mzn
  :language: minizinc
  :name: ex-allint3
  :caption: A dual model for the all interval series problem ``prob007`` in CSPlib (:download:`allinterval3.mzn <examples/allinterval3.mzn>`).

:numref:`ex-allint3` gives a dual model combining features of 
:download:`allinterval.mzn <examples/allinterval.mzn>` and :download:`allinterval2.mzn <examples/allinterval2.mzn>`.
The beginning of the model is taken from :download:`allinterval.mzn <examples/allinterval.mzn>`.
We then introduce the :mzn:`y` and :mzn:`v` variables from :download:`allinterval2.mzn <examples/allinterval2.mzn>`.
We tie the variables together using the 
global 
:mzn:`inverse` constraint:
:mzn:`inverse(x,y)` holds if :mzn:`y` is the inverse function of :mzn:`x` (and vice versa),
that is :mzn:`x[i] = j <-> y[j] = i`. A definition
is shown in :numref:`ex-inverse`.
The model does not include the constraints relating the 
:mzn:`y` and :mzn:`v` variables, they are redundant (and indeed propagation
redundant) 
so they do not add information for a
propagation solver. The :mzn:`alldifferent` constraints are also missing since
they are made redundant (and propagation redundant) by the inverse
constraints.
The only constraints are the relationships of the :mzn:`x` and :mzn:`u` variables
and the redundant constraints on :mzn:`y` and :mzn:`v`.

.. literalinclude:: examples/inverse.mzn
  :language: minizinc
  :name: ex-inverse
  :caption: A definition of the ``inverse`` global constraint (:download:`inverse.mzn <examples/inverse.mzn>`).

One of the benefits of the dual model is that there is more scope for
defining different search strategies.
Running the dual model, 

.. code-block:: bash

  $ minizinc --solver g12fd --all-solutions --statistics allinterval3.mzn -D "n=10;"

which uses the search strategy of
the inverse model, labelling the :mzn:`y` variables, 
finds all solutions in 1714 choice points and 0.5s.
Note that running the same model with labelling on the :mzn:`x` variables
requires 13142 choice points and 1.5s.

Symmetry
--------

Symmetry is very common in constraint satisfaction and optimisation problems. To illustrate this, let us look again at the n-queens problem from :numref:`ex-queens`. The top left chess board in :numref:`fig-queens-sym` shows a solution to the 8-queens problems (labeled "original"). The remaining chess boards show seven symmetric variants of the same solution: rotated by 90, 180 and 270 degrees, and flipped vertically.

.. _fig-queens-sym:

.. figure:: figures/queens_symm.*
  
  Symmetric variants of an 8-queens solution


If we wanted to enumerate *all* solutions to the 8-queens problem, we could obviously save the solver some work by only enumerating *non-symmetric* solutions, and then generating the symmetric variants ourselves. This is one reason why we want to get rid of symmetry in constraint models. The other, much more important reason, is that the solver may also **explore symmetric variants of non-solution states!**

For example, a typical constraint solver may try to place the queen in column 1 into row 1 (which is fine), and then try to put the column 2 queen into row 3, which, at first sight, does not violate any of the constraints. However, this configuration cannot be completed to a full solution (which the solver finds out after a little search). :numref:`fig-queens-sym-unsat` shows this configuration on the top left chess board. Now nothing prevents the solver from trying, e.g., the second configuration from the left in the bottom row of :numref:`fig-queens-sym-unsat`, where the queen in column 1 is still in row 1, and the queen in column 3 is placed in row 2. Therefore, even when only searching for a single solution, the solver may explore many symmetric states that it has already seen and proven unsatisfiable before!

.. _fig-queens-sym-unsat:

.. figure:: figures/queens_symm_unsat.*
  
  Symmetric variants of an 8-queens unsatisfiable partial assignment

Static Symmetry Breaking
~~~~~~~~~~~~~~~~~~~~~~~~

The modelling technique for dealing with symmetry is called *symmetry breaking*, and in its simplest form, involves adding constraints to the model that rule out all symmetric variants of a (partial) assignment to the variables except one. These constraints are called *static symmetry breaking constraints*.

The basic idea behind symmetry breaking is to impose an *order*. For example, to rule out any vertical flips of the chess board, we could simply add the constraint that the queen in the first column must be in the top half of the board:

.. code-block:: minizinc

  constraint q[1] <= n div 2;

Convince yourself that this would remove exactly half of the symmetric variants in :numref:`fig-queens-sym`. In order to remove *all* symmetry, we need to work a bit harder.

Whenever we can express all symmetries as permutations of the array of variables, a set of *lexicographic ordering constraints* can be used to break all symmetry. For example, if the array of variables is called :mzn:`x`, and reversing the array is a symmetry of the problem, then the following constraint will break that symmetry:

.. code-block:: minizinc

  constraint lex_lesseq(x, reverse(x));

How about two-dimensional arrays? Lexicographic ordering works just the same, we only have to coerce the arrays into one dimension. For example, the following breaks the symmetry of flipping the array along one of the diagonals (note the swapped indices in the second comprehension):

.. code-block:: minizinc

  array[1..n,1..n] of var int: x;
  constraint lex_lesseq([ x[i,j] | i,j in 1..n ], [ x[j,i] | i,j in 1..n ]);

The great thing about using lexicographic ordering constraints is that we can add multiple ones (to break several symmetries simultaneously), without them interfering with each other, as long as we keep the order in the first argument the same.

For the n-queens problem, unfortunately this technique does not immediately apply, because some of its symmetries cannot be described as permutations of the :mzn:`q` array. The trick to overcome this is to express the n-queens problem in terms of Boolean variables that model, for each field of the board, whether it contains a queen or not. Now all the symmetries can be modeled as permutations of this array. Since the main constraints of the n-queens problem are much easier to express with the integer :mzn:`q` array, we simply use both models together and add channeling constraints between them, as explained in :ref:`sec-multiple-modelling-and-channels`.

The full model, with added Boolean variables, channeling constraints and symmetry breaking constraints is shown in :numref:`ex-queens-sym`. We can conduct a little experiment to check whether it successfully breaks all the symmetry. Try running the model with increasing values for :mzn:`n`, e.g. from 1 to 10, counting the number of solutions (e.g., by using the ``-s`` flag with the Gecode solver, or selecting "Print all solutions" as well as "Statistics for solving" in the IDE). You should get the following sequence of numbers of solutions: 1, 0, 0, 1, 2, 1, 6, 12, 46, 92. To verify the sequence, you can search for it in the *On-Line Encyclopedia of Integer Sequences* (http://oeis.org).

.. literalinclude:: examples/nqueens_sym.mzn
  :language: minizinc
  :name: ex-queens-sym
  :start-after: % Alternative
  :end-before: % search
  :caption: Partial model for n-queens with symmetry breaking (full model: :download:`nqueens_sym.mzn <examples/nqueens_sym.mzn>`).


Other Examples of Symmetry
~~~~~~~~~~~~~~~~~~~~~~~~~~

Many other problems have inherent symmetries, and breaking these can often make a significant difference in solving performance. Here is a list of some common cases:

- Bin packing: when trying to pack items into bins, any two bins that have 
  the same capacity are symmetric.
- Graph colouring: When trying to assign colours to nodes in a graph such 
  that adjacent nodes must have different colours, we typically model 
  colours as integer numbers. However, any permutation of colours is again a 
  valid graph colouring.
- Vehicle routing: if the task is to assign customers to certain vehicles, 
  any two vehicles with the same capacity may be symmetric (this is similar 
  to the bin packing example).
- Rostering/time tabling: two staff members with the same skill set may be 
  interchangeable, just like two rooms with the same capacity or technical 
  equipment.
