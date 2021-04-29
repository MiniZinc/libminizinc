.. _sec-optiontypes:

Option Types
============

.. index::
  single: option types

Option types are a powerful abstraction that allows for concise modelling. An
option type decision variable represents a decision that has another
possibility :math:`\top`, represented in MiniZinc as :mzn:`<>`
indicating the variable is *absent*.
Option type decisions are useful for modelling problems where a decision is
not meaningful unless other decisions are made first.

Declaring and Using Option Types
--------------------------------

.. defblock:: Option type Variables

  .. index::
    single: variable; option type

  An option type variable is declared as:
  
  .. code-block:: minizincdef

    var opt <type> : <var-name>:

  where :mzndef:`<type>` is one of :mzn:`int`, :mzn:`float` or :mzn:`bool` or
  a fixed range expression.
  Option type variables can be parameters.

  An option type variable can take the additional value
  :mzn:`<>`
  indicating *absent*.

An option type variable behaves like a normal, non-optional variable, as long as it is not absent, i.e., as long as it is not equal to :mzn:`<>`.
An absent option type variable should behave as if the object it represents does not exist. This means different things for different functions and relations.

For example, when adding :mzn:`<>` to another variable :mzn:`x`, the result is just :mzn:`x`. Similarly, :mzn:`all_different([x,y,z])` should behave exactly like
:mzn:`all_different([x,y])` in case :mzn:`z=<>`, i.e., :mzn:`z` is absent.

Option type variables can be used like their non-optional versions with most operators on Booleans, integers and floats. Many functions and predicates in the MiniZinc library are also defined for option type variables. Here are a few examples of how different functions and operators treat option types:

.. code-block:: minizinc

  % Expression:               Equivalent to:          Simplified:
  <> + a                  =   0 + a                =  a            
  a * <>                  =   a * 1                =  a
  sum([x1,<>,x2])         =   sum([x1,x2])      
  sum([<>,<>])            =   sum([])              = 0
  product([x1,x2,<>])     =   product([x1,x2])
  product([<>,<>,<>])     =   product([])          = 1
  exists([b1,<>])         =   exists([b1])         = b1
  exists([<>,<>])         =   exists([])           = false
  forall([<>,b1,b2])      =   forall([b1,b2])
  forall([<>,<>])         =   forall([])           = true
  all_different([x,<>,y]) =   all_different([x,y])

Comparison operators return :mzn:`true` if any of their arguments is absent. For instance, :mzn:`3 <= <>` is :mzn:`true`, as is :mzn:`<> <= 3`.
However, note that equality between option type expressions is only :mzn:`true` if both expressions have the same optionality: :mzn:`<> = <>` is :mzn:`true`,
but :mzn:`3 = <>` is :mzn:`false`. If you need the "weaker" version of equality, MiniZinc provides the :mzn:`~=` operator: :mzn:`3 ~= <>` is :mzn:`true`. The :mzn:`~!=` operator is the weak version of disequality, it is true is either side is absent or they are not equal. Note that :mzn:`a ~!= b` is different from :mzn:`not (a ~= b)`, because :mzn:`<> ~!= <>` is true, while :mzn:`not (<> ~= <>)` is false.

Similarly, it can sometimes be useful to have "weak" versions of the arithmetic operators that return :mzn:`<>` if any of their arguments is absent. MiniZinc provides the :mzn:`~+`, :mzn:`~-`, :mzn:`~*`, :mzn:`~/` and :mzn:`~div` operators for this purpose (e.g., :mzn:`3 + <> =3`, but :mzn:`3 ~+ <> = <>`).

.. defblock:: Operations on option type variables

  Two builtin functions and a binary operator are provided for option type variables:
  :mzn:`absent(v)` returns :mzn:`true` iff option type variable :mzn:`v` takes the value
  :mzn:`<>`,
  :mzn:`occurs(v)` returns :mzn:`true` iff option type variable :mzn:`v` does *not* take the value
  :mzn:`<>`,
  and
  :mzn:`x default y` returns :mzn:`x` if :mzn:`x` occurs, and :mzn:`y` otherwise.

  In addition, the function
  :mzn:`deopt(v)` returns the normal value of :mzn:`v` or fails if it takes the
  value :mzn:`<>`. This function should not be used in normal models, but is required
  to implement predicates over option type variables.

Option Types in Scheduling Problems
-----------------------------------

A common use of option types is for optional tasks in scheduling.
In the flexible job shop scheduling problem we have :mzn:`n` tasks to perform
on :mzn:`k` machines, and the time to complete each task on each machine
may be different. The aim is to minimize the completion time of all tasks.
A model using option types to encode the problem is given in
:numref:`ex-flexible-js`. We model the problem using :math:`n \times k` optional
tasks representing the possibility of each task run on each machine.
We require that start time of the task and its duration spans the optional
tasks that make it up, and require only one actually runs using the
:mzn:`alternative` global constraint.
We require that at most one task runs on any machine using the
:mzn:`disjunctive` global constraint extended to optional tasks.
Finally we constrain that at most :mzn:`k` tasks run at any time, a redundant
constraint that holds on the actual (not optional) tasks.

.. literalinclude:: examples/flexible-js.mzn
  :language: minizinc
  :name: ex-flexible-js
  :caption: Model for flexible job shop scheduling using option types (:download:`flexible-js.mzn <examples/flexible-js.mzn>`).
  
.. \pjs{Finish the damn section!}


Hidden Option Types
-------------------

Option type variable arise implicitly when list comprehensions are
constructed with iteration over variable sets, or where the expressions in
:mzn:`where` clauses are not fixed.

For example the model fragment

.. code-block:: minizinc

  var set of 1..n: x;
  constraint sum(i in x)(i) <= limit;

is syntactic sugar for

.. code-block:: minizinc

  var set of 1..n: x;
  constraint sum(i in 1..n)(if i in x then i else <> endif) <= limit;

The :mzn:`sum` builtin function actually operates on a list of
type-inst :mzn:`var opt int`. Since the :mzn:`<>` acts as the identity
0 for + this gives the expected results.

Similarly the model fragment

.. code-block:: minizinc

  array[1..n] of var int: x;  
  constraint forall(i in 1..n where x[i] >= 0)(x[i] <= limit);

is syntactic sugar for

.. code-block:: minizinc

  array[1..n] of var int: x;  
  constraint forall(i in 1..n)(if x[i] >= 0 then x[i] <= limit else <> endif);

Again the :mzn:`forall` function actually operates on a list
of type-inst :mzn:`var opt bool`. Since :mzn:`<>` acts
as identity :mzn:`true` for :mzn:`/\ ` this gives the expected results.

The hidden uses can lead to unexpected behaviour though so care is
warranted. Consider

.. code-block:: minizinc

  var set of 1..9: x;
  constraint card(x) <= 4;
  constraint length([ i | i in x]) > 5;
  solve satisfy;

which would appear to be unsatisfiable.  It returns :mzn:`x = {1,2,3,4}` as
example answer. This is correct since the second constraint is equivalent to

.. code-block:: minizinc

  constraint length([ if i in x then i else <> endif | i in 1..9 ]) > 5;

and the length of the list of optional integers is always 9 so the
constraint always holds!

One can avoid hidden option types by not constructing iteration over
variables sets or using unfixed :mzn:`where` clauses.
For example the above two examples could be rewritten without option types as

.. code-block:: minizinc

  var set of 1..n: x;
  constraint sum(i in 1..n)(bool2int(i in x)*i) <= limit;

and

.. code-block:: minizinc
  
  array[1..n] of var int: x;  
  constraint forall(i in 1..n)(x[i] >= 0 -> x[i] <= limit);


Option Type Parameters
----------------------

Option type variables of fixed parameter type can be used to define optional
model parameters. For example, a variable defined like this:

.. code-block:: minizinc

  opt bool: enable_feature;

could be used to enable or disable a certain optional feature of a model.
In contrast to other parameter variables, these optional parameters do not
have to be assigned a value (e.g., the data file may omit an assignment to
:mzn:`enable_feature`). In that case, the variable is automatically assigned
the value :mzn:`<>`. In the model, such an optional parameter could be used
like this:

.. code-block:: minizinc

  bool: enable_feature_enabled = enable_feature default false;

  constraint if enable_feature_enabled then ... endif;
