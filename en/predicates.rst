.. index::
  single: predicate
  single: function

.. _sec-predicates:

Predicates and Functions
========================

Predicates in MiniZinc 
allow us to capture complex constraints of our model
in a succinct way.  Predicates in MiniZinc 
are used to model with both predefined global
constraints, and to capture and define new complex constraints by the
modeller.
Functions are used in MiniZinc to capture common structures of models.
Indeed a predicate is just a function with output type :mzn:`var bool`.

.. _sec-globals:

Global Constraints
------------------

.. index::
  single: global constraint

There are many global constraints defined in MiniZinc for use in modelling.
The definitive list is to be found in the documentation for the release, as
the list is slowly growing.  
Below we discuss some of the most important global constraints.


Alldifferent
~~~~~~~~~~~~

.. index::
  single: alldifferent
  single: global constraint; alldifferent

The :mzn:`alldifferent` constraint takes an array of variables and constrains them
to take different values.
A use of the :mzn:`alldifferent` has the form

.. code-block:: minizinc

  alldifferent(array[int] of var int: x)

The argument is an array of integer variables.

The :mzn:`alldifferent` constraint is one of the most studied and used global constraints in
constraint programming.  It is used to define assignment subproblems, and
efficient global propagators for :mzn:`alldifferent` exist.
The models :download:`send-more-money.mzn <examples/send-more-money.mzn>` (:numref:`ex-smm`)
and :download:`sudoku.mzn <examples/sudoku.mzn>` (:numref:`ex-sudoku`)
are examples of models using :mzn:`alldifferent`.

Cumulative
~~~~~~~~~~

.. index::
  single: cumulative
  single: global constraint; cumulative

The :mzn:`cumulative` constraint is used for describing cumulative resource
usage. 

.. code-block:: minizinc

  cumulative(array[int] of var int: s, array[int] of var int: d, 
             array[int] of var int: r, var int: b)

It requires that a set of tasks given by start times :mzn:`s`, durations :mzn:`d`, 
and resource requirements :mzn:`r`, never require more 
than a global resource bound :mzn:`b` at any one time.

.. literalinclude:: examples/moving.mzn
  :language: minizinc
  :name: ex-moving
  :caption: Model for moving furniture using ``cumulative`` (:download:`moving.mzn <examples/moving.mzn>`).

.. literalinclude:: examples/moving.dzn
  :language: minizinc
  :name: ex-movingd
  :caption: Data for moving furniture using ``cumulative`` (:download:`moving.dzn <examples/moving.dzn>`).

The model in :numref:`ex-moving` finds a schedule for moving furniture
so that each piece of furniture has enough handlers (people) and enough trolleys
available during the move. The available time, handlers 
and trolleys are given, and the data gives for each object the move
duration,
the number of handlers and the number of trolleys required. 
Using the data shown in :mzn:`ex-movingd`, the command

.. code-block:: bash

  $ minizinc moving.mzn moving.dzn

may result in the output

.. code-block:: none

  start = [0, 60, 60, 90, 120, 0, 15, 105]
  end = 140
  ----------
  ==========

:numref:`fig-histogram-a` and :numref:`fig-histogram-b` 
show the requirements for handlers and
trolleys at each time in the move for this solution.

.. _fig-histogram-a:

.. figure:: figures/handlers.*
  
  Histogram of usage of handlers in the move.

.. _fig-histogram-b:

.. figure:: figures/trolleys.*
  
  Histogram of usage of trolleys in the move.

Table
~~~~~

.. index::
  single: table
  single: global constraint; table

The :mzn:`table` constraint enforces that a tuple of variables
takes a value from a set of tuples. Since there are no tuples in MiniZinc
this is encoded using arrays. The usage of :mzn:`table`
has one of the forms

.. code-block:: minizinc

  table(array[int] of var bool: x, array[int, int] of bool: t)
  table(array[int] of var int:  x, array[int, int] of int:  t)

depending on whether the tuples are Boolean or integer.
The constraint enforces :math:`x \in t` where we consider :math:`x`
and each row in :math:`t` to be a tuple,
and :math:`t` to be a set of tuples.

.. literalinclude:: examples/meal.mzn
  :language: minizinc
  :name: ex-meal
  :caption: Model for meal planning using ``table`` constraint (:download:`meal.mzn <examples/meal.mzn>`).

.. literalinclude:: examples/meal.dzn
  :language: minizinc
  :name: ex-meald
  :caption: Data for meal planning defining the ``table`` used (:download:`meal.dzn <examples/meal.dzn>`).

The model in :numref:`ex-meal` searches for balanced meals.
Each meal item has a name (encoded as an integer), a kilojoule count,
protein in grams, salt in milligrams, and fat in grams, as well as cost
in cents.  The relationship between these items is encoded using
a :mzn:`table` constraint.
The model searches for a minimal cost meal 
which has a minimum kilojoule count
:mzn:`min_energy`, a minimum amount of protein :mzn:`min_protein`,
maximum amount of salt :mzn:`max_salt` and fat :mzn:`max_fat`.

Regular
~~~~~~~

.. index::
  single: regular
  single: global constraint; regular


The :mzn:`regular` constraint is used to enforce that a sequence of
variables takes a value defined by a finite automaton.
The usage of :mzn:`regular` has the form

.. code-block:: minizinc

  regular(array[int] of var int: x, int: Q, int: S,
          array[int,int] of int: d, int: q0, set of int: F)

It constrains that 
the sequence of values in array :mzn:`x` (which must all be in the :index:`range`
:mzn:`1..S`)
is accepted by the :index:`DFA` of :mzn:`Q` states with input :mzn:`1..S`
and transition function :mzn:`d` (which maps :mzn:`<1..Q, 1..S>` to 
:mzn:`0..Q`) and initial state
:mzn:`q0` (which must be in :mzn:`1..Q`) and accepting states :mzn:`F`
(which all must be in :mzn:`1..Q`). 
State 0 is reserved to be an always failing state. 

.. _fig-dfa:

.. figure:: figures/dfa.*
  
  A DFA determining correct rosters.

Consider a nurse rostering problem. Each nurse is scheduled for each day as
either: (d) on day shift, (n) on night shift, or (o) off.
In each four day period a nurse must have at least one day off, and
no nurse can be scheduled for 3 night shifts in a row.
This can be encoded using the incomplete DFA shown in :numref:`fig-dfa`.
We can encode this DFA as having start state :mzn:`1`, final states :mzn:`1..6`,
and transition function 

.. cssclass:: table-nonfluid table-bordered

+---+---+---+---+
|   | d | n | o |
+===+===+===+===+
| 1 | 2 | 3 | 1 |
+---+---+---+---+
| 2 | 4 | 4 | 1 |
+---+---+---+---+
| 3 | 4 | 5 | 1 |
+---+---+---+---+
| 4 | 6 | 6 | 1 |
+---+---+---+---+
| 5 | 6 | 0 | 1 |
+---+---+---+---+
| 6 | 0 | 0 | 1 |
+---+---+---+---+

Note that state 0 in the table indicates an error state.
The model shown in :numref:`ex-nurse` finds a schedule for
:mzn:`num_nurses` nurses over :mzn:`num_days` days, where we
require :mzn:`req_day` nurses on day shift each day, and 
:mzn:`req_night` nurses on night shift, and that each nurse
takes at least :mzn:`min_night` night shifts.

.. literalinclude:: examples/nurse.mzn
  :language: minizinc
  :name: ex-nurse
  :caption: Model for nurse rostering using ``regular`` constraint (:download:`nurse.mzn <examples/nurse.mzn>`)

Running the command

.. code-block:: bash

  $ minizinc nurse.mzn nurse.dzn

finds a 10 day schedule for 7 nurses, requiring 3 on each day shift
and 2 on each night shift, with a minimum 2 night shifts per nurse.
A possible output is

.. code-block:: none

  d o n o n o d n o o
  d o d n n o d d n o
  o d d n o n d n o n
  o d d d o n n o n n
  d d n o d d n o d d
  n n o d d d o d d d
  n n o d d d o d d d
  ----------

There is an alternate form of the regular constraint
:mzn:`regular_nfa` which specifies the regular
expression using an NFA (without :math:`\epsilon` arcs).
This constraint has the form

.. code-block:: minizinc

  regular_nfa(array[int] of var int: x, int: Q, int: S,
          array[int,int] of set of int: d, int: q0, set of int: F)

It constrains that 
the sequence of values in array :mzn:`x` (which must all be in the range
:mzn:`1..S`)
is accepted by the :index:`NFA` of :mzn:`Q` states with input :mzn:`1..S`
and transition function :mzn:`d` (which maps :mzn:`<1..Q, 1..S>` to 
subsets of :mzn:`1..Q`) and initial state
:mzn:`q0` (which must be in :mzn:`1..Q`) and accepting states :mzn:`F` 
(which all must be in :mzn:`1..Q`). 
There is no need for a failing state 0, since the transition function can
map to an empty set of states.


Defining Predicates
-------------------

.. index::
  single: predicate; definition

One of the most powerful modelling features 
of MiniZinc is the ability for
the modeller to define their own high-level constraints. This allows them to
abstract and modularise their model. It also allows re-use of constraints in
different models and allows the development of application specific
libraries defining the standard constraints and types.


.. literalinclude:: examples/jobshop2.mzn
  :language: minizinc
  :name: ex-jobshop2
  :caption: Model for job shop scheduling using predicates (:download:`jobshop2.mzn <examples/jobshop2.mzn>`)

We start with a simple example, revisiting the job shop scheduling problem
from the previous section.  The model is shown in
:numref:`ex-jobshop2`. The item of interest is the 
:mzn:`predicate`
item:

.. literalinclude:: examples/jobshop2.mzn
  :language: minizinc
  :lines: 12-13

This defines a new constraint that enforces that a task with start time
:mzn:`s1` and duration :mzn:`d1` does not overlap with a task with start
time :mzn:`s2` and duration :mzn:`d2`.  This can now be used inside the
model anywhere any other :index:`Boolean expression <expression; Boolean>`
(involving decision variables)
can be used.

As well as predicates the modeller can define new constraints that only
involve parameters. These are useful to write fixed tests for a
conditional expression. These are defined using the keyword :mzn:`test`.
For example

.. code-block:: minizinc

  test even(int:x) = x mod 2 = 0;

.. \ignore{ % for capture for testing!
.. $ mzn-g12fd jobshop2.mzn jobshop.dzn
.. } % $

.. defblock:: Predicate definitions

  .. index::
    single: predicate; definition

  Predicates are defined by a statement of the form

  .. code-block:: minizincdef
  
    predicate <pred-name> ( <arg-def>, ..., <arg-def> ) = <bool-exp>

  The :mzndef:`<pred-name>` must be a valid MiniZinc identifier, and
  each :mzndef:`<arg-def>` is a valid MiniZinc :index:`type` declaration.

  .. \ignore{The type-insts\index{type-inst} 
  .. of arguments may include type-inst variables\index{type-inst!variable} 
  .. which are of the
  .. form \texttt{\$T} or \texttt{any \$T} with \texttt{T} an identifier. A type-inst
  .. variable \texttt{\$T}\ttindexdef{\$T} 
  .. can match any fixed type-inst, whereas a type-inst
  .. variable \texttt{any \$T} can 
  .. also match non-fixed type-insts\index{type-index!non-fixed} 
  .. (such as \texttt{var int}).
  .. Because predicate arguments\index{argument} 
  .. receive an assignment when calling the predicate, the
  .. argument type-insts may include 
  .. implicitly indexed arrays\index{array!index set!implicit}, 
  .. as well as set variables with a
  .. non-finite element type.}

  One relaxation of :index:`argument`
  definitions is that the index types for arrays
  can be :index:`unbounded <array; index set; unbounded>`, written :mzn:`int`.

  .. code-block:: minizincdef
  
    test <pred-name> ( <arg-def>, ..., <arg-def> ) = <bool-exp>

  The :mzndef:`<bool-exp>` of the body must be fixed.

  We also introduce a new form of the :mzn:`assert` command for use in
  predicates. 

  .. code-block:: minizincdef
  
    assert ( <bool-exp>, <string-exp>, <exp> )

  The type of the :mzn:`assert`
  :index:`expression <expression; assert>`
  is the same as the type of the
  last argument. 
  The :mzn:`assert` expression checks whether the first argument is false,
  and if so prints the second argument string. If the first argument is true
  it returns the third argument.

Note that :index:`assert expressions <expression; assert>` 
are lazy in the third argument, that is if the
first argument is false they will not be evaluated.
Hence, they can be used for checking:

.. code-block:: minizinc

  predicate lookup(array[int] of var int:x, int: i, var int: y) = 
      assert(i in index_set(x), "index out of range in lookup"
             y = x[i]
      );

This code will not evaluate :mzn:`x[i]` if :mzn:`i` is out of the range of the array
:mzn:`x`.

Defining Functions
------------------

.. index::
  single: function; definition

Functions are defined in MiniZinc 
similarly to predicates, but with a more
general return type.

The function below defines the row in a Sudoku matrix
of the :math:`a1^{th}` row of the :math:`a^{th}` of subsquares.

.. code-block:: minizinc

  function int: posn(int: a, int: a1) = (a-1) * S + a1;

With this definition we can replace the last constraint in the
Sudoku problem shown in :numref:`ex-sudoku` by

.. code-block:: minizinc

  constraint forall(a, o in SubSquareRange)( 
                    alldifferent([ puzzle [ posn(a,a0), posn(o,o1) ] | 
                                           a1,o1 in SubSquareRange ] ) );

Functions are useful for encoding complex expressions that
are used frequently in the model.  For example, imagine
placing the numbers 1 to :math:`n` in different positions
in an :math:`n \times n` grid such that
the Manhattan distance between any two numbers :math:`i` and :math:`j` 
is greater than the maximum of the two numbers minus 1.
The aim is to minimize the total of the Manhattan distances
between the pairs.  The Manhattan distance function
can be expressed as:

.. literalinclude:: examples/manhattan.mzn
  :language: minizinc
  :lines: 12-14

The complete model is shown in :numref:`ex-manhattan`.


.. literalinclude:: examples/manhattan.mzn
  :language: minizinc
  :name: ex-manhattan
  :caption: Model for a number placement problem illustrating the use of functions (:download:`manhattan.mzn <examples/manhattan.mzn>`).

.. defblock:: Function definitions

  .. index::
    single: function; definition

  Functions are defined by a statement of the form
  
  .. code-block:: minizincdef

    function <ret-type> : <func-name> ( <arg-def>, ..., <arg-def> ) = <exp>

  The :mzndef:`<func-name>` must be a valid MiniZinc identifier, and
  each :mzndef:`<arg-def>` is a valid MiniZinc type declaration.
  The :mzndef:`<ret-type>` is the return type of the function which must be
  the type of :mzndef:`<exp>`. Arguments have the same restrictions as in
  predicate definitions.

Functions in MiniZinc can have any return type, not just fixed
return types.
Functions are useful for defining and documenting complex expressions that
are used mulitple times in a model.


Reflection Functions
--------------------

To help write generic tests and predicates, various reflection functions
return information about array index sets, var set domains and decision
variable ranges. Those for index sets are
:mzndef:`index_set(<1-D array>)`,
:mzndef:`index_set_1of2(<2-D array>)`,
:mzndef:`index_set_2of2(<2-D array>)`,
and so on for higher
dimensional arrays.

A better model of the job shop conjoins all the non-overlap constraints for a
single machine into a single disjunctive constraint.
An advantage of this approach is that while we may initially model this 
simply as a conjunction of :mzn:`non-overlap` constraints, if the underlying solver has a
better approach to solving disjunctive constraints we can use that instead,
with minimal changes to our model. The model is shown in
:numref:`ex-jobshop3`. 


.. literalinclude:: examples/jobshop3.mzn
  :language: minizinc
  :name: ex-jobshop3
  :caption: Model for job shop scheduling using ``disjunctive`` predicate (:download:`jobshop3.mzn <examples/jobshop3.mzn>`).

.. index::
  single: global constraint; disjunctive

The :mzn:`disjunctive`
constraint takes an array of start times for each
task and an array of their durations and makes sure that only one task is
active at any one
time. We define the disjunctive constraint as a :index:`predicate <predicate; definition>` with
signature

.. code-block:: minizinc

  predicate disjunctive(array[int] of var int:s, array[int] of int:d);

We can use the disjunctive constraint to define the non-overlap of tasks as
shown in :numref:`ex-jobshop3`. 
We assume a definition for the :mzn:`disjunctive` predicate is given
by the file :download:`disjunctive.mzn <examples/disjunctive.mzn>` which is included in the model.
If the underlying system
supports :mzn:`disjunctive` directly, it will include a file
:download:`disjunctive.mzn <examples/disjunctive.mzn>` in its globals directory (with contents
just the signature definition above).
If the system we are using does not support disjunctive directly
we can give our own definition by creating the file
:download:`disjunctive.mzn <examples/disjunctive.mzn>`.
The simplest implementation simply makes use of the :mzn:`no_overlap`
predicate defined above. 
A better implementation is to make use of a global :mzn:`cumulative`
constraint assuming it is supported by the underlying solver.
:numref:`ex-disj` shows an implementation of :mzn:`disjunctive`.
Note how we use the :mzn:`index_set` reflection function to
(a) check that the arguments to :mzn:`disjunctive` make sense,
and (b) construct the array of resource utilisations of the appropriate
size for :mzn:`cumulative`.
Note also that we use a ternary version of :mzn:`assert` here.

.. literalinclude:: examples/disjunctive.mzn
  :language: minizinc
  :name: ex-disj
  :caption: Defining a ``disjunctive`` predicate using ``cumulative`` (:download:`disjunctive.mzn <examples/disjunctive.mzn>`).

.. \ignore{ % for capture for testing!
.. $ mzn-g12fd jobshop3.mzn jobshop.dzn
.. } % $



Local Variables
---------------

.. index::
  single: variable; local
  single: let

It is often useful to introduce *local variables* in a predicate,
function
or test. 
The :mzn:`let` expression allows you to do so. 
It can be used to introduce 
both decision :index:`variables <variable>`
and
:index:`parameters <parameter>`, 
but parameters must be initialised. For example: 

.. code-block:: minizinc

  var s..e: x;
  let {int: l = s div 2; int: u = e div 2; var l .. u: y;} in x = 2*y

introduces parameters :mzn:`l` and :mzn:`u` and variable :mzn:`y`. 
While most useful in :index:`predicate`, :index:`function`
and test definitions, 
:mzn:`let` expressions can also be used in other expressions, for example
for eliminating common subexpressions:

.. code-block:: minizinc

  constraint let { var int: s = x1 + x2 + x3 + x4 } in
             l <= s /\ s <= u;

Local variables can be used anywhere and can be quite useful
for simplifying complex expressions.
:numref:`ex-wedding2`
gives a revised version of the wedding model, using local variables to
define the :index:`objective` function, 
rather than adding lots of variables
to the model explicitly.

.. literalinclude:: examples/wedding2.mzn
  :language: minizinc
  :name: ex-wedding2
  :caption: Using local variables to define a complex objective function (:download:`wedding2.mzn <examples/wedding2.mzn>`).


Context
-------

.. index::
  single: context
  single: context; negative
  single: predicate
  single: function

One limitation is that predicates and functions 
containing decision variables that are not
initialised in the declaration cannot be used inside a negative
context.
The following is illegal:

.. code-block:: minizinc

  predicate even(var int:x) = 
            let { var int: y } in x = 2 * y;

  constraint not even(z);

The reason for this is that solvers only solve existentially constrained
problems, and if we introduce a local variable in a negative context, then
the variable is *universally quantified* and hence out of scope of the
underlying solvers. For example the :math:`\neg \mathit{even}(z)` is equivalent to
:math:`\neg \exists y. z = 2y` which is equivalent to
:math:`\forall y. z \neq 2y`.

If local variables are given values, then they can be used in negative 
contexts. The following is legal

.. code-block:: minizinc

  predicate even(var int:x) = 
            let { var int: y = x div 2; } in x = 2 * y;

  constraint not even(z);

Note that the meaning of :mzn:`even` is correct, since if :mzn:`x` is even
then :math:`x = 2 * (x ~\mbox{div}~ 2)`. Note that for this definition
:math:`\neg \mathit{even}(z)` is equivalent to
:math:`\neg \exists y. y = z ~\mbox{div}~ 2 \wedge z = 2y` which is equivalent to
:math:`\exists y. y = z ~\mbox{div}~ 2 \wedge \neg z \neq 2y`, because :math:`y` is
functionally defined by :math:`z`.


Every expression in MiniZinc appears in one of the four
*contexts*: :index:`root <context; !root>`, :index:`positive <context; !positive>`,
:index:`negative <context; !negative>`,
or :index:`mixed <context; !mixed>`.
The context of a non-Boolean expression is simply the context of its nearest
enclosing Boolean expression. The one exception is that the objective
expression appears in a root context (since it has no enclosing Boolean expression).

For the purposes of defining contexts we assume implication expressions
:mzn:`e1 -> e2` are rewritten equivalently as :mzn:`not e1 \/ e2`,
and similarly :mzn:`e1 <- e2` is rewritten as  :mzn:`e1 \/ not e2`.

The context for a Boolean expression is given by:

root
  root context is the context for any expression :mzn:`e` appearing as
  the argument of :mzn:`constraint` or as an
  :index:`assignment` item, or appearing as a sub expression :mzn:`e1`
  or :mzn:`e2` in an expression :mzn:`e1 /\ e2` occuring in a root context.

  Root context Boolean expressions must hold in any model of the problem.

positive
  positive context is the context for any expression
  appearing as a sub expression :mzn:`e1`
  or :mzn:`e2` in an expression :mzn:`e1 \/ e2` occuring in a root
  or positive context,
  appearing as a sub expression :mzn:`e1`
  or :mzn:`e2` in an expression :mzn:`e1 /\ e2` occuring in a positive context,
  or appearing as a sub expression :mzn:`e` in an expression
  :mzn:`not e` appearing in a negative context.

  Positive context Boolean expressions need not hold in a model, but
  making them hold will only increase the possibility that the enclosing
  constraint holds. A positive context expression has an even number of
  negations in the path from the enclosing root context to the expression.

negative
  negative context is the context for any expression appearing as a sub expression :mzn:`e1`
  or :mzn:`e2` in an expression :mzn:`e1 \/ e2` or :mzn:`e1 /\ e2` occuring in a negative context,
  or appearing as a sub expression :mzn:`e` in an expression
  :mzn:`not e` appearing in a positive context.

  Negative context Boolean expressions need not hold in a model, but
  making them false will increase the possibility that the enclosing
  constraint holds. A negative context expression has an odd number of
  negations in the path from the enclosing root context to the expression.

mixed
  mixed context is the context for any Boolean expression appearing
  as a subexpression :mzn:`e1` or :mzn:`e2` in :mzn:`e1 <-> e2`, :mzn:`e1 = e2`, or :mzn:`bool2int(e)`.

  Mixed context expression are effectively both positive and
  negative. This can be seen from the fact that :mzn:`e1 <-> e2` is equivalent
  to :mzn:`(e1 /\ e2) \/ (not e1 /\ not e2)` and
  :mzn:`x = bool2int(e)` is equivalent to :mzn:`(e /\ x=1) \/ (not e /\ x=0)`.

Consider the code fragment

.. code-block:: minizinc

  constraint x > 0 /\ (i <= 4 -> x + bool2int(x > i) = 5);

then :mzn:`x > 0` is in the root context, :mzn:`i <= 4}` is in a negative
context,
:mzn:`x + bool2int(x > i) = 5`
is in a positive context, and :mzn:`x > i` is in a mixed context.



Local Constraints
-----------------

.. index::
  single: constraint; local

Let expressions can also be used to include local constraints,
usually to constrain the behaviour of local variables.
For example, consider defining a square root function making use
of only multiplication:

.. code-block:: minizinc

  function var float: mysqrt(var float:x) = 
           let { var float: y;
                 constraint y >= 0;
                 constraint x = y * y; } in y;

The local constraints ensure
:mzn:`y` takes the correct value; which is then returned
by the function.    

Local constraints can be used in any let expression, 
though the most common
usage is in defining functions.


.. defblock:: Let expressions

  .. index::
    single: expression; let

  :index:`Local variables <variable;local>` 
  can be introduced in any expression with a *let expression*
  of the form:
  
  .. code-block:: minizincdef
  
    let { <dec>; ... <dec> ; } in <exp>
  
  The declarations :mzndef:`<dec>` 
  can be declarations of decision variables and
  parameters (which must be initialised) or constraint items.
  No declaration can make use of a newly declared variable 
  before it is introduced. 

  Note that local variables and constraints 
  cannot occur in tests.
  Local variables cannot occur in predicates 
  or functions that appear in a
  :index:`negative <context; negative>` or :index:`mixed <context; mixed>` context,
  unless the variable is defined by an expression.


Domain Reflection Functions
---------------------------

.. index::
  single: domain; reflection

Other important reflection functions are those that allow us to access
the domains of variables. The expression :mzn:`lb(x)`
returns
a value that is lower than or equal to any value that :mzn:`x` may take in
a solution of the problem. Usually it will just be the
declared lower :index:`bound <variable; bound>` of :mzn:`x`.
If :mzn:`x` is declared as a non-finite type, e.g. 
simply :mzn:`var int`
then it is an error. 
Similarly the expression :mzn:`dom(x)`
returns a (non-strict) 
superset of the possible values of :mzn:`x` in any solution of the problem.
Again it is usually the declared values, and again if it is not
declared as finite then there is an error.

.. \ignore{ % for capture for testing!
.. $ mzn-g12fd reflection.mzn
.. } % $


.. literalinclude:: examples/reflection.mzn
  :language: minizinc
  :name: ex-reflect
  :caption: Using reflection predicates (:download:`reflection.mzn <examples/reflection.mzn>`).

For example, the model show in :numref:`ex-reflect`
may output

.. code-block:: none

  y = -10
  D = -10..10
  ----------

or

.. code-block:: none

  y = 0
  D = {0, 1, 2, 3, 4}
  ----------

or any answer with 
:math:`-10 \leq y \leq 0` and 
:math:`\{0, \ldots, 4\} \subseteq D \subseteq \{-10, \ldots, 10\}`.

Variable domain reflection expressions should be used in a manner where they are
correct for any safe approximations, but note this is not checked!
For example the additional code

.. code-block:: minizinc

  var -10..10: z;
  constraint z <= y;

is not a safe usage of the domain information. 
Since using the tighter (correct) approximation leads to 
more solutions than the weaker initial approximation.

.. TODO: this sounds wrong!

.. defblock:: Domain reflection

  .. index::
    single: domain; reflection

  There are reflection functions to interrogate 
  the possible values of expressions containing variables:
  
  - :mzndef:`dom(<exp>)`
    returns a safe approximation to the possible values of the expression. 
  - :mzndef:`lb(<exp>)`
    returns a safe approximation to the lower bound value of the expression. 
  - :mzndef:`ub(<exp>)`
    returns a safe approximation to the upper bound value of the expression. 

  The expressions for :mzn:`lb` and :mzn:`ub`
  can only be of types :mzn:`int`, :mzn:`bool`,
  :mzn:`float` or :mzn:`set of int`.
  For :mzn:`dom` the type cannot be :mzn:`float`.
  If one of the variables appearing in :mzndef:`<exp>` has a 
  :index:`non-finite declared type <type; non-finite>`
  (e.g. :mzn:`var int` or :mzn:`var float`) 
  then an error can occur.

  There are also versions that work directly on arrays of expressions (with
  similar restrictions):

  - :mzndef:`dom_array(<array-exp>)`:
    returns a safe approximation to the union of all 
    possible values of the expressions appearing in the array. 
  - :mzndef:`lb_array(<array-exp>)`
    returns a safe approximation to the lower bound of all expressions appearing
    in the array.
  - :mzndef:`ub_array(<array-exp>)`
    returns a safe approximation to the upper bound of all expressions appearing
    in the array.

The combinations of predicates, local variables and domain reflection
allows the definition of complex global constraints by decomposition.
We can define the time based decomposition 
of the :mzn:`cumulative`
constraint using the code shown in :numref:`ex-cumul`.

.. literalinclude:: examples/cumulative.mzn
  :language: minizinc
  :name: ex-cumul
  :caption: Defining a ``cumulative`` predicate by decomposition (:download:`cumulative.mzn <examples/cumulative.mzn>`).

The decomposition uses :mzn:`lb` and :mzn:`ub` to determine
the set of times :mzn:`times` over which tasks could range.
It then asserts for each time :mzn:`t` in :mzn:`times` that the
sum of resources for the active tasks at time :mzn:`t` is less than
the bound :mzn:`b`.

Scope
-----

.. index::
  single: scope

It is worth briefly mentioning the scope of declarations in MiniZinc.
MiniZinc has a single namespace, so all variables appearing 
in declarations are visible in every expression in the model.
MiniZinc introduces locally scoped
variables in a number of ways:

- as :index:`iterator <variable; iterator>`
  variables in :index:`comprehension` expressions
- using :mzn:`let` expressions
- as predicate and function :index:`arguments <argument>`

Any local scoped variable overshadows the outer scoped variables
of the same name.

.. literalinclude:: examples/scope.mzn
  :language: minizinc
  :name: ex-scope
  :caption: A model for illustrating scopes of variables (:download:`scope.mzn <examples/scope.mzn>`).

For example, in the model shown in :numref:`ex-scope` 
the :mzn:`x` in :mzn:`-x <= y` is the global :mzn:`x`,
the :mzn:`x` in
:mzn:`smallx(x)` is the iterator :mzn:`x in 1..u`,
while the :mzn:`y` in the disjunction is the second
argument of the predicate.
