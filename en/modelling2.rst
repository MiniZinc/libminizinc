More Complex Models
===================

In the last section we introduced the basic structure of a MiniZinc
model. In this section we introduce the array and set data structures,
enumerated types and
more complex constraints.

.. _sec-arrayset:

Arrays and Sets
---------------

Almost always we are interested in building models where the number of
constraints and variables is dependent on the input data. 
In order to do so we will usually use :index:`arrays <array>`.

Consider a simple finite element model for modelling temperatures on a
rectangular sheet of metal.  We approximate the temperatures across the
sheet by breaking the sheet into a finite number of elements in a
two-dimensional matrix. 
A model is shown in :numref:`ex-laplace`. 
It declares the width ``w`` and height ``h`` 
of the finite element model.
The declaration

.. literalinclude:: examples/laplace.mzn
  :language: minizinc
  :lines: 5-9

declares four fixed sets of integers describing the dimensions of the finite
element model: ``HEIGHT`` is the whole height of the model, while ``CHEIGHT`` is
the centre of the height omitting the top and bottom,
``WIDTH`` is the whole width of the model, while
``CWIDTH`` is the centre of the width omitting the left and rightsides,
Finally a two dimensional array of float variables ``t`` 
with rows numbered
:math:`0` to :math:`h` (``HEIGHT``) and columns :math:`0` to :math:`w` (``WIDTH``), 
to represent the temperatures at each
point in the metal plate.
We can access the element of the array in the :math:`i^{th}` row and :math:`j^{th}` column 
using an expression :mzn:`t[i,j]`.


Laplace's
equation states that when the plate reaches a steady state 
the temperature at each internal point is the average of its orthogonal
neighbours. The constraint 

.. literalinclude:: examples/laplace.mzn
  :language: minizinc
  :lines: 16-18

ensures that each internal point :math:`(i,j)` is the
average of its four orthogonal neighbours.  
The constraints

.. literalinclude:: examples/laplace.mzn
  :language: minizinc
  :lines: 20-24

restrict the temperatures on each edge to be equal, and
gives these temperatures names: ``left``, ``right``, ``top``
and ``bottom``.
While the constraints

.. literalinclude:: examples/laplace.mzn
  :language: minizinc
  :lines: 26-30

ensure that the corners (which are irrelevant) are set to 0.0.
We can determine the temperatures in a plate broken into 5 :math:`\times` 5
elements with left, right and bottom temperature 0 and top temperature 100
with the model shown in :numref:`ex-laplace`.


.. literalinclude:: examples/laplace.mzn
  :language: minizinc
  :name: ex-laplace
  :caption: Finite element plate model for determining steady state temperatures (:download:`laplace.mzn <examples/laplace.mzn>`).

Running the command

.. code-block:: bash

  $ minizinc --solver cbc laplace.mzn

gives the output

.. code-block:: none

   -0.00 100.00 100.00 100.00  -0.00
   -0.00  42.86  52.68  42.86  -0.00
   -0.00  18.75  25.00  18.75  -0.00
   -0.00   7.14   9.82   7.14  -0.00
   -0.00  -0.00  -0.00  -0.00  -0.00
  ----------

.. defblock:: Sets

  .. index::
    single: set

  Set variables are declared with a declaration of the form

  .. code-block:: minizincdef
    
    set of <type-inst> : <var-name> ;

  where sets of integers, enums (see later), floats or Booleans are allowed.
  The only type allowed for decision variable sets are variable sets of
  integers or enums.
  Set literals are of the form

  .. code-block:: minizincdef
  
    { <expr-1>, ..., <expr-n> }

  or are :index:`range` expressions over either integers, enums or floats of the form

  .. code-block:: minizincdef

    <expr-1> .. <expr-2>

  The standard :index:`set operations <operator; set>`
  are provided: element membership 
  (:mzn:`in`),
  (non-strict) subset relationship (:mzn:`subset`), 
  (non-strict) superset relationship (:mzn:`superset`), union 
  (:mzn:`union`),
  intersection (:mzn:`intersect`), 
  set difference (:mzn:`diff`), 
  symmetric set difference (:mzn:`symdiff`) 
  and the number of elements in the 
  set (:mzn:`card`).

  As we have seen set variables and set literals (including ranges) can be
  used as an implicit type in variable declarations in which case the variable
  has the type of the elements in the set and the variable is implicitly
  constrained to be a member of the set.


Our cake baking problem is an example of a very simple kind of production
planning problem.  In this kind of problem we wish to determine how much of
each kind of product to make to maximise the profit where manufacturing a
product consumes varying amounts of some fixed resources.  We can generalise
the MiniZinc model in :numref:`ex-cakes2` to handle this kind of problem
with a model that is generic in the kinds of resources and products.  The
model is shown in :numref:`ex-prod-planning` and a sample data file (for
the cake baking example) is shown in :numref:`fig-prod-planning-data`.


.. literalinclude:: examples/prod-planning.mzn
  :language: minizinc
  :name: ex-prod-planning
  :caption: Model for simple production planning (:download:`prod-planning.mzn <examples/prod-planning.mzn>`).

.. literalinclude:: examples/prod-planning-data.dzn
  :language: minizinc
  :name: fig-prod-planning-data
  :caption: Example data file for the simple production planning problem (:download:`prod-planning-data.dzn <examples/prod-planning-data.dzn>`).

The new feature in this model is the use of :index:`enumerated
types <type; enumerated>`. 
These allow us to treat the choice of resources and products as parameters to
the model.
The first item in the model

.. code-block:: minizinc

  enum Products;

declares ``Products`` as an *unknown* set of products.

.. defblock:: Enumerated Types

  .. index::
    single: enumerated type
    single enum

  Enumerated types, which we shall refer to as ``enums``, 
  are declared with a declaration of the form

  .. code-block:: minizincdef

    enum <var-name> ;

  An enumerated type is defined by an assignment of the form


  .. code-block:: minizincdef

    enum <var-name> = { <var-name-1>, ..., <var-name-n> } ;
    
  where :mzndef:`<var-name-1>`, ..., :mzndef:`<var-name-n>` are the elements of
  the enumerated type, with name :mzndef:`<var-name>`. 
  Each of the elements of the enumerated type is also effectively declared by
  this definition as a new constant of that type.
  The declaration and definition can be combined into one line as usual.

The second item declares an array of integers:

.. code-block:: minizinc

  array[Products] of int: profit;

The :index:`index set <array; index set>`
of the array ``profit`` is ``Products``.
This means that only elements of the set ``Products``
can be used to index the array.

The elements of an enumerated type of :math:`n` elements 
act very similar to the integers :math:`1\dots n`. They can be compared, they are
ordered, by the order they appear in the enumerated type definition,
they can be interated over, they can appear as indices of arrays, in fact
they can appear anywhere an integer can appear.

In the example data file we have initialized the array using a list of 
integers

.. code-block:: minizinc

  Products = { BananaCake, ChocolateCake };  
  profit = [400,450];

meaning the profit for a banana cake is 400, while for a chocolate cake it
is 450.  Internally ``BananaCake`` will be treated like the integer 1,
while ``ChocolateCake`` will be treated like the integer 2.
While MiniZinc does not provide an explicit list type, one-dimensional
arrays with an index set :mzn:`1..n` behave like lists, and we will sometimes
refer to them as :index:`lists <list>`.

In a similar fashion, in the next two items we declare a set of resources
``Resources``, and an array ``capacity`` which gives the amount of
each resource that is available.

More interestingly, the item

.. code-block:: minizinc

  array[Products, Resources] of int: consumption;

declares a 2-D array ``consumption``. The value of
:mzn:`consumption[p,r]` is the amount of resource :mzn:`r` required to
produce one unit of product :mzn:`p`. Note that the first index is the row
and the second is the column.

The data file contains an example initialization of a 2-D array:

.. code-block:: minizinc

  consumption= [| 250, 2, 75,  100, 0,
                | 200, 0, 150, 150, 75 |];
            
Notice how the delimiter ``|`` is used to separate rows.

.. defblock:: Arrays

  .. index:
    single: array

  Thus, MiniZinc provides one- and multi-dimensional arrays
  which are declared using the type:
  
  .. code-block:: minizincdef
  
    array [ <index-set-1>, ..., <index-set-n> ] of <type-inst>
  
  MiniZinc requires that the array declaration contains the index set of each
  dimension and that the index set is either an integer range, a set
  variable initialised to an integer range, or an :index:`enumeration type <enumerated type>`.
  Arrays can contain any of the base
  types: integers, enums, Booleans, floats or strings.  These can be fixed or unfixed
  except for strings which can only be parameters. Arrays can also contain
  sets but they cannot contain arrays.

  :index:`One-dimensional array literals <array; literal; 1D>` are of form
  
  .. code-block:: minizincdef
  
    [ <expr-1>, ..., <expr-n> ]
  
  while :index:`two-dimensional array literals <array; literal; 2D>` are of form

  .. code-block:: minizincdef
  
    [| <expr-1-1>, ..., <expr-1-n> |
       ...                         |
       <expr-m-1>, ..., <expr-m-n> |]

  where the array has ``m`` rows and ``n`` columns.

  The family of built-in functions :mzn:`array1d`,
  :mzn:`array2d`, etc,
  can be used to initialise an array of any dimension from a list (or more
  exactly a one-dimensional array). The call:

  .. code-block:: minizincdef

    array<n>d(<index-set-1>, ..., <index-set-n>, <list>)

  returns an ``n`` dimensional array with index sets given by the first ``n``
  arguments and the last argument contains the elements of the array. For
  instance, :mzn:`array2d(1..3, 1..2, [1, 2, 3, 4, 5, 6])` is equivalent to
  :mzn:`[|1, 2 |3, 4 |5, 6|]`.

  Array elements are :index:`accessed <array; access>` in the usual way: :mzn:`a[i,j]` gives the
  element in the :math:`i^{th}` row and :math:`j^{th}` column.

  .. \pjs{New array functions!}


  The concatenation operator ``++``
  can be used to concatenate two
  one-dimensional arrays together. The result is a list, i.e. a
  one-dimensional array whose elements are indexed from 1.  For instance
  :mzn:`[4000, 6] ++ [2000, 500, 500]` evaluates to :mzn:`[4000, 6, 2000, 500, 500]`.
  The built-in function 
  :mzn:`length` returns the length
  of a one-dimensional array.

The next item in the model defines the parameter :mzn:`mproducts`. This is
set to an upper-bound on the number of products of any type that can be
produced. This is quite a complex example of nested
array comprehensions and aggregation operators.  We shall introduce these
before we try to understand this item and the rest of the model.

First, MiniZinc provides list comprehensions similar to those provided in
many functional programming languages, or Python. For example, the list comprehension
:mzn:`[i + j | i, j in 1..3 where j < i]` evaluates to :mzn:`[1 + 2, 1 + 3, 2 + 3]`
which is :mzn:`[3, 4, 5]`. Of course :mzn:`[3, 4, 5]` is
simply an array with index set :mzn:`1..3`.

MiniZinc also provides set comprehensions which have a similar syntax: for
instance, :mzn:`{i + j | i, j in 1..3 where j < i}` evaluates to the set
:mzn:`{3, 4, 5}`.

.. defblock:: List and Set Comprehensions

  .. index:
    single: comprehension
    single: comprehension; list

  The generic form of a list comprehension is
  
  .. code-block:: minizincdef

    [ <expr> | <generator-exp> ]
  
  The expression :mzndef:`<expr>` specifies how to construct elements in the
  output list from the elements generated by :mzndef:`<generator-exp>`. 
  The generator :mzndef:`<generator-exp>` consists of a comma separated sequence of
  generator expressions optionally followed by a Boolean expression. The two forms are
  
  .. code-block:: minizincdef
  
    <generator>
    <generator> where <bool-exp>
  
  The optional :mzndef:`<bool-exp>` in the second form acts as a filter on
  the generator expression: only elements satisfying the Boolean expression
  are used to construct elements in the output list.  A :index:`generator <comprehension; generator>`
  :mzndef:`<generator>`
  has the form
  
  .. code-block:: minizincdef
  
    <identifier>, ..., <identifier> in <array-exp>
  
  Each identifier is an *iterator*
  which takes the values
  of the array expression in turn, with the last identifier varying most rapidly.
 
  The generators of a list comprehension and :mzndef:`<bool-exp>`
  usually do not involve decision variables.
  If they do involve decision variables
  then the list produced is a list of :mzndef:`var opt <T>` where :mzndef:`<T>` is the type
  of the :mzndef:`<expr>`. See the discussion of :index:`option types <option type>`
  in :ref:`sec-optiontypes` for more details.

  :index:`Set comprehensions <comprehension; set>`
  are almost identical to list comprehensions: the only
  difference is the use of ``{`` and ``}`` to enclose the
  expression rather than ``[`` and ``]``.
  The elements generated by a set comprehension must be
  :index:`fixed`, i.e. free of decision variables.
  Similarly the generators and optional :mzndef:`<bool-exp>`
  for set comprehensions must be fixed.


.. index::
  single: forall

Second, MiniZinc provides a number of built-in functions that take a
one-dimensional array and aggregate the elements. Probably the most useful
of these is :mzn:``forall``.
This takes an array of Boolean expressions
(that is, constraints) and returns a single Boolean expression which is the
logical conjunction of the Boolean expressions in the array.

For example, consider the expression 

.. code-block:: minizinc

  forall( [a[i] != a[j] | i,j in 1..3 where i < j])

where ``a`` is an arithmetic array with index set ``1..3``. This
constrains the elements in ``a`` to be pairwise different. The list
comprehension evaluates to :mzn:`[ a[1] != a[2], a[1] != a[3], a[2] != a[3] ]`
and so the :mzn:`forall` function returns the logical
conjunction :mzn:`a[1] != a[2] /\ a[1] != a[3] /\ a[2] != a[3]`.

.. defblock:: Aggregation functions
  
  .. index::
    single: aggregation function
    single: sum
    single: product
    single: min
    single: max
    single: forall
    single: exists
    single: xorall
    single: iffall
    single: aggregation function; sum
    single: aggregation function; product
    single: aggregation function; min
    single: aggregation function; max
    single: aggregation function; forall
    single: aggregation function; exists
    single: aggregation function; xorall
    single: aggregation function; iffall

  The *aggregation functions* for arithmetic arrays are: 
  :mzn:`sum` which adds the elements, :mzn:`product` which multiplies them together,
  and :mzn:`min` and :mzn:`max` which respectively return the least and
  greatest element in the array. When applied to an empty array, :mzn:`min` and
  :mzn:`max` give a run-time error, :mzn:`sum` returns 0 and :mzn:`product`
  returns 1.

  MiniZinc provides four aggregation functions for arrays containing Boolean
  expressions. As we have seen, the first of these, 
  :mzn:`forall`, returns
  a single constraint which is the logical conjunction of the
  constraints.
  The second function, :mzn:`exists`, 
  returns the logical
  disjunction of the constraints. Thus, :mzn:`forall` enforces that all
  constraints in the array hold, while :mzn:`exists` ensures that at least
  one of the constraints holds.
  The third function, :mzn:`xorall`,
  ensures that an odd number of constraints hold.
  The fourth function, :mzn:`iffall`,
  ensures that an even number of constraints holds.

The third, and final, piece in the puzzle is that MiniZinc allows a special
syntax for aggregation functions when used with an array
comprehension. Instead of writing

.. code-block:: minizinc

  forall( [a[i] != a[j] | i,j in 1..3 where i < j])

the modeller can instead write the more mathematical looking

.. code-block:: minizinc

  forall (i,j in 1..3 where i < j) (a[i] != a[j])

The two expressions are completely equivalent: the modeller is free to use
whichever they feel looks most natural.

.. defblock:: Generator call expressions

  .. index::
    single: generator call
    single: expression; generator call

  A *generator call expression* has form

  .. code-block:: minizincdef
  
    <agg-func> ( <generator-exp> ) ( <exp> )
  
  The round brackets around the generator expression
  :mzndef:`<generator-exp>` and the constructor expression
  :mzndef:`<exp>` are not optional: they must be there.  This is
  equivalent to writing

  .. code-block:: minizincdef
  
    <agg-func> ( [ <exp> | <generator-exp> ] )

  The :index:`aggregation function` :mzndef:`<agg-func>` is any MiniZinc
  function expecting a single array as argument.


We are now in a position to understand the rest of the simple production
planning model shown in :numref:`ex-prod-planning`. For the moment ignore
the item defining :mzn:`mproducts`.  The item afterwards:

.. code-block:: minizinc

  array[Products] of var 0..mproducts: produce;

defines a one-dimensional array :mzn:`produce` of decision variables.  The
value of :mzn:`produce[p]` will be set to the amount of product :mzn:`p`
in the optimal solution.
The next item

.. code-block:: minizinc

  array[Resources] of var 0..max(capacity): used;

defines a set of auxiliary variables that record how much of each resource
is used. 
The next two constraints

.. code-block:: minizinc

  constraint forall (r in Resources)      
             (used[r] = sum (p in Products) (consumption[p, r] * produce[p]));
  constraint forall (r in Resources)(used[r] <= capacity[r] );

compute in :mzn:`used[r]`  the total consumption of the
resource :mzn:`r` and ensure it is less than the available amount.
Finally, the item

.. code-block:: minizinc

  solve maximize sum (p in Products) (profit[p]*produce[p]);

indicates that this is a maximisation problem and that the objective to be maximised is the total profit.


We now return to the definition of :mzn:`mproducts`. For each product
:mzn:`p` the expression

.. code-block:: minizinc

  (min (r in Resources where consumption[p,r] > 0) 
       (capacity[r] div consumption[p,r]))

determines the maximum amount of :mzn:`p` that can be produced taking into
account the amount of each resource :mzn:`r` and how much of :mzn:`r` is
required to produce the product. Notice the use of the filter
:mzn:`where consumption[p,r] > 0` to ensure that only resources required to make the
product are considered so as to avoid a division by zero error.  Thus, the
complete expression

.. code-block:: minizinc

  int: mproducts = max (p in Products) 
                       (min (r in Resources where consumption[p,r] > 0) 
                            (capacity[r] div consumption[p,r]));

computes the maximum amount of *any* product that can be produced, and
so this can be used as an upper bound on the domain of the decision
variables in :mzn:`produce`.

Finally notice the output item is more complex, 
and uses :index:`list comprehensions <comprehension; list>`
to create an understandable output. Running

.. code-block:: bash

  $ minizinc --solver gecode prod-planning.mzn prod-planning-data.dzn

results in the output

.. code-block:: none

  BananaCake = 2;
  ChocolateCake = 2;
  Flour = 900;
  Banana = 4;
  Sugar = 450;
  Butter = 500;
  Cocoa = 150;
  ----------
  ==========


Global Constraints
------------------

.. \index{constraint!global|see{global constraint}}
.. \index{global constraint}

MiniZinc includes a library of global constraints which can also be used
to define models. An example is the :mzn:`alldifferent`
constraint which requires
all the variables appearing in its argument to be pairwise different.


.. literalinclude:: examples/send-more-money.mzn
  :language: minizinc
  :name: ex-smm
  :caption: Model for the cryptarithmetic problem SEND+MORE=MONEY (:download:`send-more-money.mzn <examples/send-more-money.mzn>`)

The SEND+MORE=MONEY problem requires assigning a different
digit to each letter so that the arithmetic constraint holds.
The model shown in :numref:`ex-smm` uses the constraint expression
:mzn:`alldifferent([S,E,N,D,M,O,R,Y])`
to ensure that each letter takes a different digit value.
The global constraint is made available
in the model using include item

.. code-block:: minizinc

  include "alldifferent.mzn";

which makes 
the global constraint :mzn:`alldifferent` usable by the model.
One could replace this line by

.. code-block:: minizinc

  include "globals.mzn";

which includes all globals.

A list of all the global constraints 
defined for MiniZinc is included in the
release documentation. See :ref:`sec-globals` for
a description of some important global constraints. 


Conditional Expressions
-----------------------

.. \index{expression!conditional}

MiniZinc provides a conditional *if-then-else-endif*
expression.
An example of its use is

.. code-block:: minizinc

  int: r = if y != 0 then x div y else 0 endif;

which sets :mzn:`r` to :mzn:`x` divided by :mzn:`y` unless :mzn:`y` is zero in which case
it sets it to zero.

.. defblock:: Conditional expressions

  .. index::
    single: expression; conditional

  The form of a conditional expression is

  .. code-block:: minizincdef

    if <bool-exp> then <exp-1> else <exp-2> endif

  It is a true expression rather than a control flow statement and so can be used in other expressions.
  It evaluates to :mzndef:`<exp-1>` if the Boolean expression :mzndef:`<bool-exp>` is true and 
  :mzndef:`<exp-2>` otherwise. The type of the conditional expression is that of 
  :mzndef:`<exp-1>` and :mzndef:`<exp-2>` which must have the same
  type.

  If the :mzndef:`<bool-exp>` contains decision variables, then the 
  type-inst
  of the expression is :mzndef:`var <T>` where :mzndef:`<T>` is the type of
  :mzndef:`<exp-1>` and :mzndef:`<exp-2>` even if both
  expressions are fixed.

.. literalinclude:: examples/sudoku.mzn
  :language: minizinc
  :name: ex-sudoku
  :caption: Model for generalized Sudoku problem (:download:`sudoku.mzn <examples/sudoku.mzn>`)


.. literalinclude:: examples/sudoku.dzn
  :language: minizinc
  :name: ex-sudokud
  :caption: Example data file for generalised Sudoku problem (:download:`sudoku.dzn <examples/sudoku.dzn>`)

.. _fig-sudoku:

.. figure:: figures/sudoku.*
  
  The problem represented by data file :download:`sudoku.dzn <examples/sudoku.dzn>`

Conditional expressions are very useful in building complex models, or
complex output. Consider the model of Sudoku problems shown in
:numref:`ex-sudoku`. The initial board positions are given by the
:mzn:`start` parameter where 0 represents an empty
board position. This is converted to constraints on the decision variables
:mzn:`puzzle` using the conditional expression

.. code-block:: minizinc

  constraint forall(i,j in PuzzleRange)(
       if start[i,j] > 0 then puzzle[i,j] = start[i,j] else true endif );

Conditional expressions are also very useful for 
defining complex :index:`output`.
In the Sudoku model of :numref:`ex-sudoku` the expression

.. code-block:: minizinc

  if j mod S == 0 then " " else "" endif 

inserts an extra space between groups of size :mzn:`S`.
The output expression also uses conditional expressions to
add blank lines
after each :mzn:`S` lines. The resulting output is highly readable.

The remaining constraints ensure that the numbers appearing in each
row and column and :math:`S \times S` subsquare are all different.

.. index::
  single: runtime flag; -a
  single: runtime flag; --all-solutions
  single: solution; all

One can use MiniZinc to search 
for all solutions to a satisfaction problem (:mzn:`solve satisfy`). In the MiniZinc IDE, this can be achieved by setting the number of solutions to zero in the "User-defined behavior" part of the solver configuration pane (see :numref:`ide_solver_config`). On the command line, one can use the flag ``-a``
or ``--all-solutions``. Running

.. code-block:: bash

  $ minizinc --solver gecode --all-solutions sudoku.mzn sudoku.dzn

results in

.. code-block:: none

   5 9 3  7 6 2  8 1 4 
   2 6 8  4 3 1  5 7 9 
   7 1 4  9 8 5  2 3 6 

   3 2 6  8 5 9  1 4 7
   1 8 7  3 2 4  9 6 5
   4 5 9  1 7 6  3 2 8

   9 4 2  6 1 8  7 5 3
   8 3 5  2 4 7  6 9 1
   6 7 1  5 9 3  4 8 2
  ----------
  ==========

The line ``==========``
is output when the system has output all possible
solutions, here verifying that there is exactly one.

.. _sec-enum:

Enumerated Types
----------------

.. index::
  single: type; enumerated

Enumerated types allows us to build models that depend on a set of objects
which are part of the data, or are named in the model, and hence make models
easier to understand and debug.
We have introduce enumerated types or enums briefly, in this subsection we
will explore how we can use them more fully, and show some of the built in
functions for dealing with enumerated types.

Let's revisit the problem of coloring the graph of Australia from :ref:`sec-modelling`.


.. literalinclude:: examples/aust-enum.mzn
  :language: minizinc
  :name: ex-aust-enum
  :caption: Model for coloring Australia using enumerated types (:download:`aust-enum.mzn <examples/aust-enum.mzn>`).

The model shown in :numref:`ex-aust-enum` declares an enumerated type
:mzn:`Color` which must be defined in the data file.  Each of the state
variables is declared to take a value from this enumerated type.
Running this program using 

.. code-block:: bash
  
  $ minizinc --solver gecode -D"Color = { red, yellow, blue };" aust-enum.mzn

might result in output

.. code-block:: none

  wa = blue;
  nt = yellow;
  sa = red;
  q = blue;
  nsw = yellow;
  v = blue;
  t = red;


.. defblock:: Enumerated Type Variable Declarations

  .. index::
    single: variable; declaration; enum

  An enumerated type parameter is declared as either:
  
  .. code-block:: minizincdef
  
    <enum-name> : <var-name>
    <l>..<u> : <var-name>
  
  where :mzndef:`<enum-name>` is the name of a enumerated type, and
  :mzndef:`<l>` and :mzndef:`<u>` are fixed enumerated type expressions of the same
  enumerated type.

  An enumerated type decision variable is declared
  as either:

  .. code-block:: minizincdef

    var <enum-name> : <var-name>
    var <l>..<u> : <var-name>

  where :mzndef:`<enum-name>` is the name of a enumerated type, and
  :mzndef:`<l>` and :mzndef:`<u>` are fixed enumerated type expressions of the same
  enumerated type.

A key behaviour of enumerated types is that they are automatically coerced
to integers when they are used in a position expecting an integer.
For example, this allows us to use global constraints defined on integers,
such as

.. code-block:: minizinc

  global_cardinality_low_up([wa,nt,sa,q,nsw,v,t],
                            [red,yellow,blue],[2,2,2],[2,2,3]);

This requires at least two states to be colored each color and three to be
colored blue.


.. defblock:: Enumerated Type Operations

  There are a number of built in operations on enumerated types:

  - :mzn:`enum_next(X,x)`: returns the next value in after :mzn:`x` in the
    enumerated type :mzn:`X`. This is a partial function, if :mzn:`x` is the last value in
    the enumerated type :mzn:`X` then the function returns :math:`\bot` causing the Boolean
    expression containing the expression to evaluate to :mzn:`false`.
  - :mzn:`enum_prev(X,x)`:
    returns the previous value before :mzn:`x` in the
    enumerated type :mzn:`X`. Similarly :mzn:`enum_prev` is a partial function. 
  - :mzn:`to_enum(X,i)`: maps an integer expression :mzn:`i` to an
    enumerated type value in type :mzn:`X` or evaluates to :math:`\bot` if :mzn:`i` is less
    than or equal to 0 or greater than the number of elements in :mzn:`X`.

  Note also that a number of standard functions are applicable to enumerated
  types:

  - :mzn:`card(X)`: returns the cardinality of an enumerated type :mzn:`X`.
  - :mzn:`min(X)`: returns the minimum element of of an enumerated type :mzn:`X`.
  - :mzn:`max(X)`: returns the maximum element of of an enumerated type :mzn:`X`.


.. _sec-complex:

Complex Constraints
-------------------

.. index::
  single: constraint; complex

Constraints are the core of the MiniZinc model. 
We have seen simple relational expressions but constraints can be considerably more powerful than this.
A constraint is allowed to be any Boolean expression. 
Imagine a scheduling problem in which we have 
two tasks that cannot overlap in time.
If :mzn:`s1` and :mzn:`s2` are the corresponding start times
and :mzn:`d1` and :mzn:`d2` are the corresponding
durations
we can express this as:

.. code-block:: minizinc

  constraint s1 + d1 <= s2  \/ s2 + d2 <= s1;

which ensures that the tasks do not overlap.


.. defblock:: Booleans

  .. index::
    single: Boolean
    single: expression; Boolean
    single: true
    single: false
    single: operator; Boolean
    single: bool2int

  Boolean expressions in MiniZinc can be written using a standard mathematical syntax.
  The Boolean literals are :mzn:`true` and 
  :mzn:`false`
  and the Boolean 
  operators
  are 
  conjunction, i.e. and  (``/\``), 
  disjunction, i.e. or  (``\/``),  
  only-if (:mzn:`<-`), 
  implies (:mzn:`->`), 
  if-and-only-if (:mzn:`<->`) and 
  negation (:mzn:`not`).
  Booleans can be automatically coerced to integers, but to make this
  coercion explicit the 
  built-in function :mzn:`bool2int` can be used: it 
  returns 1 if its argument is true and 0 otherwise.

.. literalinclude:: examples/jobshop.mzn
  :language: minizinc
  :name: ex-jobshop
  :caption: Model for job-shop scheduling problems (:download:`jobshop.mzn <examples/jobshop.mzn>`).

.. literalinclude:: examples/jdata.dzn
  :language: minizinc
  :name: ex-jdata
  :caption: Data for job-shop scheduling problems (:download:`jdata.dzn <examples/jdata.dzn>`).

The job shop scheduling model given in :numref:`ex-jobshop`
gives a realistic example of the use of this disjunctive modelling
capability. In job shop scheduling we have a set of jobs, each consisting
of a sequence of tasks on separate machines: so task :mzn:`[i,j]` is the
task in the :math:`i^{th}` job performed on the :math:`j^{th}` machine.  
Each sequence of tasks must be completed in order,
and no two tasks on the same machine can overlap in time. 
Even for small instances of this problem it can be quite challenging to find
optimal solutions.

The command

.. code-block:: bash
  
  $ minizinc --solver gecode --all-solutions jobshop.mzn jdata.dzn

solves a small job shop scheduling problem, and illustrates the behaviour of 
``--all-solutions`` for optimisation problems (note that when running this in the MiniZinc IDE, ``--all-solutions`` is the default behaviour for optimisation problems).  Here the solver outputs increasingly better solutions as it finds them, rather than all possible optimal
solutions. The output from this command is:

.. code-block:: none

  end = 39
   5  9 13 22 30 
   6 13 18 25 36 
   0  4  8 12 16 
   4  8 12 16 22 
   9 16 25 27 38 
  ----------
  end = 37
   4  8 12 17 20 
   5 13 18 26 34 
   0  4  8 12 16 
   8 12 17 20 26 
   9 16 25 27 36 
  ----------
  end = 34
   0  1  5 10 13 
   6 10 15 23 31 
   2  6 11 19 27 
   1  5 10 13 19 
   9 16 22 24 33 
  ----------
  end = 30
   5  9 13 18 21 
   6 13 18 25 27 
   1  5  9 13 17 
   0  1  2  3  9 
   9 16 25 27 29 
  ----------
  ==========

indicating an optimal solution with end time 30 is finally found,
and proved optimal.
We can generate all *optimal solutions*
by adding a constraint that
:mzn:`end = 30` and changing the solve item to :mzn:`solve satisfy`
and then executing 

.. code-block:: bash

  $ minizinc --solver gecode --all-solutions jobshop.mzn jobshop.dzn

For this problem there are 3,444,375 optimal solutions. In the MiniZinc IDE, you would have to select "User-defined behavior" in the configuration pane and set the number of solutions to zero in order to display all solutions of this satisfaction problem.

.. literalinclude:: examples/stable-marriage.mzn
  :language: minizinc
  :name: ex-stable-marriage
  :caption: Model for the stable marriage problem (:download:`stable-marriage.mzn <examples/stable-marriage.mzn>`).

.. literalinclude:: examples/stable-marriage.dzn
  :language: minizinc
  :name: ex-sm-data
  :caption: Example data for the stable marriage problem (:download:`stable-marriage.dzn <examples/stable-marriage.dzn>`).

Another powerful modelling feature in MiniZinc is 
that decision variables
can be used for :index:`array access <array; access>`.  
As an example, consider the 
(old-fashioned) *stable marriage problem*. We have :mzn:`n` (straight) women and :mzn:`n` (straight)
men. Each man has a ranked list of women and vice versa. We want to find a
husband/wife for each women/man so that all marriages are *stable* in
the sense that:

- whenever :mzn:`m` prefers another women :mzn:`o` to his wife :mzn:`w`, :mzn:`o` prefers her husband to :mzn:`m`, and
- whenever :mzn:`w` prefers another man :mzn:`o` to her husband :mzn:`m`, :mzn:`o` prefers his wife to :mzn:`w`.

This can be elegantly modelled in 
MiniZinc. 
The model and sample data is shown in :numref:`ex-stable-marriage` and :numref:`ex-sm-data`. 

The first three items in the model declare the number of men/women and the
set of men and women. Here we introduce the use of *anonymous enumerated types*.
Both :mzn:`Men` and :mzn:`Women` are sets of size
:mzn:`n`, but we do not wish to mix them up so we use an anonymous enumerated
type. This allows MiniZinc to detect modelling errors where we use
:mzn:`Men` for :mzn:`Women` or vice versa.


The matrices :mzn:`rankWomen` and :mzn:`rankMen`, 
respectively, give the women's ranking  of the men and the men's ranking of the women.
Thus, the entry  :mzn:`rankWomen[w,m]`
gives the ranking by woman :mzn:`w` of man :mzn:`m`. The lower the number in the ranking, the more the man or women is preferred. 

There are two arrays of decision variables: :mzn:`wife` and
:mzn:`husband`. These, respectively, contain the wife of each man and the
husband of each women.

The first two constraints

.. literalinclude:: examples/stable-marriage.mzn
  :language: minizinc
  :lines: 13-14
  
ensure that the assignment of husbands and wives is consistent: :mzn:`w` is the
wife of :mzn:`m` implies :mzn:`m` is the husband of :mzn:`w` and vice versa. Notice how in
:mzn:`husband[wife[m]]` the index expression :mzn:`wife[m]` is a
decision variable, not a parameter.

The next two constraints are a  direct encoding of the stability condition:

.. literalinclude:: examples/stable-marriage.mzn
  :language: minizinc
  :lines: 16-22

This natural modelling of the stable marriage problem is made possible by
the ability to use decision variables as array indices and to construct
constraints using the standard Boolean connectives.  
The alert reader may
be wondering at this stage, what happens if the array index variable takes a
value that is outside the index set of the array. MiniZinc treats this as
failure: an array access :mzn:`a[e]` implicitly adds the constraint
:mzn:`e in index_set(a)`
to the closest surrounding Boolean context where
:mzn:`index_set(a)`
gives the index set of :mzn:`a`.

.. defblock:: Anonymous Enumerated Types

  .. index::
    single: type; enumerated; anonymous

  An *anonymous enumerated type*
  is of the form :mzndef:`anon_enum(<n>)` where :mzndef:`<n>` is a fixed integer
  expression defining the size of the enumerated type.

  An anonymous enumerated type is just like any other enumerated type except
  that we have no names for its elements. When printed out, they are given
  unique names based on the enumerated type name.

Thus for example, consider the variable declarations

.. code-block:: minizinc

  array[1..2] of int: a= [2,3];
  var 0..2: x;
  var 2..3: y;

The constraint :mzn:`a[x] = y`
will succeed with :math:`x=1 \wedge y=2` and :math:`x=2 \wedge y=3`.
And the constraint :mzn:`not a[x] = y` will succeed with 
:math:`x=0 \wedge y=2`, :math:`x=0 \wedge y=3`, :math:`x=1 \wedge y=3` and :math:`x=2 \wedge y=2`.

In the case of invalid array accesses by a parameter, the formal semantics
of MiniZinc treats this as failure so as to ensure that the treatment of
parameters and decision variables is consistent, but a warning is issued
since it is almost always an error.

.. literalinclude:: examples/magic-series.mzn
  :language: minizinc
  :name: ex-magic-series
  :caption: Model solving the magic series problem (:download:`magic-series.mzn <examples/magic-series.mzn>`).

.. index::
  single: bool2int
  single: constraint; higher order

The coercion function 
:mzn:`bool2int`
can be called with any Boolean
expression. This allows the MiniZinc 
modeller to use so called *higher order constraints*.  
As a simple example consider the *magic series problem*:
find a list of numbers :math:`s= [s_0,\ldots,s_{n-1}]`
such that :math:`s_i` is the number
of occurrences of :math:`i` in :math:`s`. An example is :math:`s = [1,2,1,0]`.

A MiniZinc 
model for this problem
is shown in :numref:`ex-magic-series`. The use of
:mzn:`bool2int` allows us to sum up the number of times the constraint
:mzn:`s[j]=i` is satisfied.
Executing the command

.. code-block:: bash

  $ minizinc --solver gecode --all-solutions magic-series.mzn -D "n=4;"

leads to the output

.. code-block:: none

  s = [1, 2, 1, 0];
  ----------
  s = [2, 0, 2, 0];
  ----------
  ==========

indicating exactly two solutions to the problem (the effect of ``--all-solutions`` can be achieved in the MiniZinc IDE using the "User-defined behavior" option in the solver configuration pane).

Note that MiniZinc will automatically coerce Booleans
to integers and integers to floats when required. 
We could replace the the constraint item in :numref:`ex-magic-series`
with

.. code-block:: minizinc

  constraint forall(i in 0..n-1) (
     s[i] = (sum(j in 0..n-1)(s[j]=i)));

and get identical results, since the Boolean expression
:mzn:`s[j] = i` will be automatically coerced to an
integer, effectively by the MiniZinc system automatically adding the
missing :mzn:`bool2int`.

.. defblock:: Coercion

  .. index::
    single: coercion; automatic
    single: coercion; bool2int
    single: coercion; int2float

  In MiniZinc one can *coerce* a Boolean value to
  an integer value using the :mzn:`bool2int` function.
  Similarly one can coerce an integer value to a float value using
  :mzn:`int2float`. 
  The instantiation of the coerced value is the same as the argument,
  e.g. :mzn:`par bool` is coerced to :mzn:`par int`, while
  :mzn:`var bool` is coerced to :mzn:`var int`. 

  MiniZinc automatically coerces Boolean expressions to integer
  expressions and integer expressions to float expressions,
  by inserting :mzn:`bool2int` and :mzn:`int2float` in the model
  appropriately.
  Note that it will also coerce Booleans to floats using two steps.


Set Constraints
---------------

.. index::
  single: constraint; set

Another powerful modelling feature of MiniZinc 
is that it allows sets 
containing integers to be decision variables:
this means that when the model is evaluated the solver will find which elements are in the set.

As a simple example, consider the *0/1 knapsack problem*. This is a
restricted form of the knapsack problem in which we can either choose to
place the item in the knapsack or not. Each item has a weight and a profit
and we want to find which choice of items leads to the maximum profit
subject to the knapsack not being too full.

It is natural to model this in MiniZinc with a single decision variable: :mzn:`var set of ITEM: knapsack`
where :mzn:`ITEM` is the set of possible items. If the arrays
:mzn:`weight[i]` and :mzn:`profit[i]` respectively give the weight and
profit of item :mzn:`i`, and the maximum weight the knapsack can carry is
given by :mzn:`capacity` then a natural model is given in
:numref:`ex-knapsack-binary`.

.. literalinclude:: examples/knapsack.mzn
  :language: minizinc
  :name: ex-knapsack-binary
  :caption: Model for the 0/1 knapsack problem (:download:`knapsack.mzn <examples/knapsack.mzn>`).

Notice that the :mzn:`var`
keyword comes before the :mzn:`set`
declaration indicating that the
set itself is the decision variable. 
This contrasts with an array in which the :mzn:`var` keyword
qualifies the elements in the array rather than the array itself since the
basic structure of the array is fixed, i.e. its index set.

.. literalinclude:: examples/social-golfers.mzn
  :language: minizinc
  :name: ex-social-golfers
  :caption: Model for the social golfers problems (:download:`social-golfers.mzn <examples/social-golfers.mzn>`).


As a more complex example of set constraint consider the social golfers
problem shown in :numref:`ex-social-golfers`. 
The aim is to schedule a golf tournament over :mzn:`weeks`
using :mzn:`groups` :math:`\times` :mzn:`size` golfers. Each week we have to
schedule :mzn:`groups` different groups each of size :mzn:`size`.
No two pairs of golfers should ever play in two groups.

The variables in the model are sets of golfers :mzn:`Sched[i,j]`
for the :math:`i^{th}` week and :mzn:`j^{th}` group.

The constraints shown in lines 11-32
first enforces an ordering on the first
set in each week to remove symmetry in swapping weeks. Next they
enforce an ordering on the sets in each week, and make each set have a
cardinality of :mzn:`size`.
They then ensure that each week is a partition of the set of golfers
using the global constraint 
:mzn:`partition_set`. 
Finally the last constraint ensures that no two players play in two
groups together (since the cardinality of the intersection of any two groups
is at most 1).

.. index::
  single: symmetry; breaking

There are also symmetry breaking
initialisation constraints shown in lines 34-46: 
the first week
is fixed to have all players in order; the second week is made up of the
first players of each of the first groups in the first week; finally the
model forces the first :mzn:`size` players to appear in their corresponding 
group number for the remaining weeks.

Executing the command

.. code-block:: bash

  $ minizinc --solver gecode social-golfers.mzn social-golfers.dzn

where the data file defines a problem with 4 weeks, with 4 groups
of size 3 leads to the output

.. code-block:: none

  1..3 4..6 7..9 10..12 
  {1,4,7} {2,9,12} {3,5,10} {6,8,11}
  {1,6,9} {2,8,10} {3,4,11} {5,7,12}
  {1,5,8} {2,7,11} {3,6,12} {4,9,10}
  ----------

Notice hows sets which are ranges may be output in range format.


Putting it all together
-----------------------

We finish this section with a complex example illustrating most
of the features introduced in this chapter including 
enumerated types, complex constraints, global constraints, 
and complex output.

.. literalinclude:: examples/wedding.mzn
  :language: minizinc
  :name: ex-wedding
  :caption: Planning wedding seating using enumerated types (:download:`wedding.mzn <examples/wedding.mzn>`).

The model of :numref:`ex-wedding` arranges seats at the wedding table.
The table has 12 numbered seats in order around the table, 6 on each side.
Males must sit in odd numbered seats, and females in even. 
Ed cannot sit at the end of the table because of a phobia, 
and the bride and groom must
sit next to each other. The aim is to maximize the distance between known
hatreds. The distance between seats is the difference in seat number
if on the same side, otherwise its the distance to the opposite seat
+ 1. 

Note that in the output statement we consider each seat :mzn:`s` and search for a
guest :mzn:`g` who is assigned to that seat. We make use of the built in function
:mzn:`fix` which checks if a decision variable is fixed and returns its
fixed value, and otherwise aborts. 
This is always safe to use in output statements, since by the
time the output statement is run all decision variables should be fixed.


Running

.. code-block:: bash

  $ minizinc --solver gecode wedding.mzn

Results in the output

.. code-block:: none

  ted bride groom rona ed carol ron alice bob bridesmaid bestman clara 
  ----------
  ==========

The resulting table placement is illustrated in :numref:`fig-wedding`
where the lines indicate hatreds. The total distance is 22.

.. _fig-wedding:

.. figure:: figures/wedding.*
  
  Seating arrangement at the wedding table


.. \pjs{Move the fix definition elsewhere!}

.. defblock:: Fix

  .. index::
    single: fix
    single: fixed

  In output items the built-in function :mzn:`fix` checks that
  the value of a decision variable is fixed
  and coerces the instantiation from
  decision variable to parameter.

.. % oil-blending
.. %arrays floats sum forall
.. %more complex datafile
.. 
.. %suduko
.. %2-D array
.. %complex transformation from data file
.. 
.. %jobshop
.. %disjunction,
.. 
.. %talk about other complex constraints--IC example?
.. 
.. %magic sequence
.. %reification
.. 
.. %warehouse placement
.. %reification more complex example
.. 
.. %0/1 knapsack
.. %set constraint
.. 
.. %social golfers
.. %more complex set constraint
.. 
.. %finish with larger example from Mark

