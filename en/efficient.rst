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
and generated models.

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

  $ mzn-g12fd grocery.mzn

yields 

::

  =====UNSATISFIABLE=====
  % grocery.fzn:11: warning: model inconsistency detected before search.

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

::

  {120,125,150,316}
  ----------
 
Note however that even the improved model may be too difficult for
some solvers.
Running 

.. code-block:: bash

  $ mzn-g12lazy grocery.mzn

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

Unconstrained Variables
-----------------------

.. index::
  single: variable; unconstrained

Sometimes when modelling it is easier to introduce more variables than
actually required to model the problem.

.. literalinclude:: examples/golomb.mzn
  :language: minizinc
  :name: ex-unc
  :caption: A model for Golomb rulers with unconstrained variables (:download:`golomb.mzn <examples/golomb.mzn>`).

Consider the model for Golomb rulers shown in :numref:`ex-unc`.
A Golomb ruler of :mzn:`n` marks is one where the absolute differences
between any two marks are different. 
It creates a two dimensional array of difference variables, but 
only uses those of the form :mzn:`diff[i,j]` where :mzn:`i > j`.
Running the model as 

.. code-block:: bash

  $ mzn-g12fd golomb.mzn -D "n = 4; m = 6;"

results in output

::

  mark = [0, 1, 4, 6];
  diffs = [0, 0, 0, 0, 1, 0, 0, 0, 4, 3, 0, 0, 6, 5, 2, 0];
  ----------

and everything seems fine with the model.
But if we ask for all solutions using

.. code-block:: bash

  $ mzn-g12fd -a golomb.mzn -D "n = 4; m = 6;"

we are presented with a never ending list of the same solution!

What is going on?  In order for the finite domain solver to finish
it needs to fix all variables, including the variables :mzn:`diff[i,j]`
where :mzn:`i <= j`, which means there are countless ways of generating a
solution, simply by changing these variables to take arbitrary values.

We can avoid problems with unconstrained variables, by modifying
the model so that they are fixed to some value. For example replacing
the lines marked :mzn:`% (diff}` in :numref:`ex-unc`
to 

.. code-block:: minizinc

  constraint forall(i,j in 1..n) 
                   (diffs[i,j] = if (i > j) then mark[i] - mark[j]
                                 else 0 endif);

ensures that the extra variables are all fixed to 0. With this change
the solver returns just one solution.

MiniZinc will automatically remove variables which are unconstrained
and not used in the output.  An alternate solution to the above problem is
simply to remove the output of the :mzn:`diffs` array by changing the
output statement to

.. code-block:: minizinc

  output ["mark = \(mark);\n"];

With this change running 

.. code-block:: bash

  $ mzn-g12fd -a golomb.mzn -D "n = 4; m = 6;"

simply results in

::

  mark = [0, 1, 4, 6];
  ----------
  ==========

illustrating the unique solution.


.. defblock:: Unconstrained Variables

  .. index::
    single: variable; unconstrained

  Models should never have unconstrained variables. Sometimes it is
  difficult to model without unnecessary variables. 
  If this is the case add
  constraints to fix the unnecessary variables, 
  so they cannot influence the
  solving.


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

  int: count = sum( i,j in NODES where i < j /\ adj[i,j])
       (sum([1 | k in NODES where j < k /\ adj[i,k] /\ adj[j,k]]));

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

::

  ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  ----------

indicating the inner loop is evaluated 64 times while

.. literalinclude:: examples/count2.mzn
  :language: minizinc
  :lines: 13-14

Produces the output:

::

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

::

  (1,2,3)
  ----------

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

  $ mzn-g12fd --all-solutions --statistics magic-series.mzn -D "n=16;"

might result in output

::

  s = [12, 2, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0];
  ----------
  ==========

and the statistics showing 174 choice points required.

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

  $ mzn-g12fd --all-solutions --statistics magic-series2.mzn -D "n=16;"

results in the same output, but with statistics showing just 13 choicepoints
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

  $ mzn-g12fd -all-solutions --statistics allinterval.mzn -D "n=10;"

finds all solutions in 84598 choice points and 3s.

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

  $ mzn-g12fd --all-solutions --statistics allinterval2.mzn -D "n=10;"

finds all the solutions in  75536 choice points and 18s.
Interestingly, although the model is not as succinct here, the search on the
:mzn:`y` variables is better than searching on the :mzn:`x` variables. 
The lack of succinctness means that even though the search requires
less choice it is substantially slower.

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

  $ mzn-g12fd -all-solutions --statistics allinterval3.mzn -D "n=10;"

which uses the search strategy of
the inverse model, labelling the :mzn:`y` variables, 
finds all solutions in 1714 choice points and 0.5s.
Note that running the same model with labelling on the :mzn:`x` variables
requires 13142 choice points and 1.5s.
