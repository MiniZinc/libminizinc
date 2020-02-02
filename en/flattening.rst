.. _sec-flattening:

FlatZinc and Flattening
=======================

.. \pjs{Maybe show the toolset at this point?}

Constraint solvers do not directly support MiniZinc models, rather in order
to run a MiniZinc model, it is translated into a simple subset of MiniZinc
called FlatZinc. FlatZinc reflects the fact that most constraint solvers
only solve satisfaction 
problems of the form :math:`\bar{exists} c_1 \wedge \cdots \wedge c_m`
or optimization problems of the form
:math:`\text{minimize } z \text{ subject to }  c_1 \wedge \cdots \wedge c_m`,
where :math:`c_i` are primitive constraints and :math:`z` is an integer or float
expression in a restricted form.

.. index::
  single: minizinc -c

The ``minizinc`` tool includes the MiniZinc *compiler*, which
takes a MiniZinc model and data files and creates
a flattened FlatZinc model which is equivalent to the MiniZinc model with
the given data, and that appears in the restricted form discussed above.  
Normally the construction of a FlatZinc model which is sent to a solver is
hidden from the user but you can view the result of flattening a model
``model.mzn`` with
its data ``data.dzn`` as follows:

.. code-block:: bash

  minizinc -c model.mzn data.dzn

which creates a FlatZinc model called ``model.fzn``.

In this chapter we explore the process of translation from MiniZinc to FlatZinc.

Flattening Expressions
----------------------

The restrictions of the underlying solver mean that complex expressions in
MiniZinc need to be *flattened* to only use conjunctions of primitive
constraints which do not themselves contain structured terms.

Consider the following model for ensuring that two circles in a rectangular
box do not overlap:

.. literalinclude:: examples/cnonoverlap.mzn
  :language: minizinc
  :caption: Modelling non overlap of two circles (:download:`cnonoverlap.mzn <examples/cnonoverlap.mzn>`).
  :name: fig-nonoverlap


Simplification and Evaluation
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Given the data file

.. code-block:: minizinc

  width = 10.0;
  height = 8.0;
  r1 = 2.0;
  r2 = 3.0;

the translation to FlatZinc first simplifies the model by replacing all the
parameters by their values, and evaluating any fixed expression.
After this simplification the values of parameters are not longer needed.
An exception to this is large arrays of parametric values. If they are
used more than once, then the parameter is retained to avoid duplicating the
large expression.

After simplification the variable and parameter declarations parts of
the model of :numref:`fig-nonoverlap` become

.. literalinclude:: examples/cnonoverlap.fzn
  :language: minizinc
  :start-after: % Variables
  :end-before: %

.. _sec-flat-sub:

Defining Subexpressions
~~~~~~~~~~~~~~~~~~~~~~~

Now no constraint solver directly handles complex constraint expressions
like the one in :numref:`fig-nonoverlap`.
Instead, each subexpression in the expression is named, and we create a
constraint to construct the value of each expression.  Let's examine the
subexpressions of the constraint expression. :mzn:`(x1 - x2)` is a
subexpression, if we name if :mzn:`FLOAT01` we can define it as
:mzn:`constraint FLOAT01 = x1 - x2;` Notice that this expression occurs
twice in the model. We only need to construct the value once, we can then
reuse it.  This is called *common subexpression elimination*.
The subexpression :mzn:`(x1 - x2)*(x1 - x2)` can be named
:mzn:`FLOAT02`
and we can define it as
:mzn:`constraint FLOAT02 = FLOAT01 * FLOAT01;` 
We can similarly name :mzn:`constraint FLOAT03 = y1 - y2;`
and
:mzn:`constraint FLOAT04 = FLOAT03 * FLOAT03;`
and finally :mzn:`constraint FLOAT05 = FLOAT02 * FLOAT04;`.
The inequality constraint itself becomes
:mzn:`constraint FLOAT05 >= 25.0;` since :mzn:`(r1+r2)*(r1 + r2)`
is calculated as :mzn:`25.0`.
The flattened constraint is hence

.. code-block:: minizinc

  constraint FLOAT01 = x1 - x2;
  constraint FLOAT02 = FLOAT01 * FLOAT01;
  constraint FLOAT03 = y1 - y2;
  constraint FLOAT04 = FLOAT03 * FLOAT03;
  constraint FLOAT05 = FLOAT02 * FLOAT04;
  constraint FLOAT05 >= 25.0

.. _sec-flat-fzn:

FlatZinc constraint form
~~~~~~~~~~~~~~~~~~~~~~~~

Flattening as its final step converts the form of the constraint to a
standard FlatZinc form which is always :math:`p(a_1, \ldots, a_n)` where
:mzn:`p` is the name of the primitive constraint and :math:`a_1, \ldots, a_n` are the
arguments. FlatZinc tries to use a minimum of different constraint forms so
for example the constraint :mzn:`FLOAT01 = x1 - x2` is first rewritten to
:mzn:`FLOAT01 + x2 = x1` and then output using the :mzn:`float_plus` primitive
constraint. The resulting constraint form is as follows:

.. literalinclude:: examples/cnonoverlap.fzn
  :language: minizinc
  :start-after: % Constraints
  :end-before: %

Bounds analysis
~~~~~~~~~~~~~~~

We are still missing one thing, the
declarations for the introduced variables :mzn:`FLOAT01`, ...,
:mzn:`FLOAT05`. While these could just be declared as
:mzn:`var float`, in order to make the solver's task easier MiniZinc tries to
determine upper and lower bounds on newly introduced variables, by a simple
bounds analysis. For example since :mzn:`FLOAT01 = x1 - x2`
and :math:`2.0 \leq` :mzn:`x1` :math:`\leq 8.0` and :math:`3.0 \leq` :mzn:`x2` :math:`\leq 7.0` then we can see that
:math:`-5.0 \leq` :mzn:`FLOAT0` :math:`\leq 5.0`. Given this information we can see that
:math:`-25.0 \leq` :mzn:`FLOAT02` :math:`\leq 25.0` (although note that if we recognized that the
multiplication was in fact a squaring we could give the much more accurate
bounds :math:`0.0 \leq` :mzn:`FLOAT02` :math:`\leq 25.0`).

The alert reader may have noticed a discrepancy between the flattened form
of the constraints in :ref:`sec-flat-sub` and :ref:`sec-flat-fzn`.  In
the latter there is no inequality constraint. Since unary inequalities can
be fully represented by the bounds of a variable, the inequality forces the
lower bound of :mzn:`FLOAT05` to be :mzn:`25.0` and is then redundant.  The final
flattened form of the model of :numref:`fig-nonoverlap` is:

.. literalinclude:: examples/cnonoverlap.fzn
  :language: minizinc

Objectives
~~~~~~~~~~

MiniZinc flattens minimization or maximization 
objectives just like constraints.  The objective
expression is flattened and a variable is created for it, just as for other
expressions. In the FlatZinc output the solve item is always on a single
variable.  See :ref:`sec-let` for an example.

.. \pjs{Do we need an example here?}

Linear Expressions
------------------

One of the most important form of constraints, widely used for modelling,
are linear constraints of the form


.. math:: a_1 x_1 + \cdots + a_n x_n \begin{array}[c]{c} = \\ \leq \\ < \end{array} a_0

where :math:`a_i` are integer or floating point constants, and
:math:`x_i` are integer or floating point variables. 
They are highly expressive, and are the only class of constraint supported
by (integer) linear programming constraint solvers. 
The translator from MiniZinc to FlatZinc tries to create linear
constraints, rather than break up linear constraints into many
subexpressions.

.. \pjs{Maybe use the equation from SEND-MORE-MONEY instead?}

.. literalinclude:: examples/linear.mzn
  :language: minizinc
  :caption: A MiniZinc model to illustrate linear constraint flattening (:download:`linear.mzn <examples/linear.mzn>`).
  :name: fig-lflat

Consider the model shown in :numref:`fig-lflat`. Rather than create
variables for all the subexpressions :math:`3*x`, :math:`3*x - y`, :math:`x * z`, :math:`3*x - y + x*z`,
:math:`x + y + z`, :math:`d * (x + y + z)`, :math:`19 + d * (x + y + z)`,
and :math:`19 + d * (x + y + z) - 4*d`
translation will attempt to create a large linear constraint which captures
as much as possible of the constraint in a single FlatZinc
constraint. 

Flattening creates linear expressions as a single unit rather than building
intermediate variables for each subexpression. It also simplifies the linear
expression created.  Extracting the linear expression from the constraints
leads to 

.. code-block:: minizinc

  var 0..80: INT01;
  constraint 4*x + z + INT01 <= 23;
  constraint INT01 = x * z;

Notice how the *nonlinear expression* :math:`x \times z` is extracted as a
new subexpression and given a name, while the remaining terms are collected
together so that each variable appears exactly once (and indeed variable :math:`y`
whose terms cancel is eliminated). 

Finally each constraint is written to FlatZinc form obtaining:

.. code-block:: minizinc

  var 0..80: INT01;
  constraint int_lin_le([1,4,1],[INT01,x,z],23);
  constraint int_times(x,z,INT01);

.. _sec-unroll:

Unrolling Expressions
---------------------

Most models require creating a number of constraints which is 
dependent on the input data.  MiniZinc supports these models with array
types, list and set comprehensions, and aggregation functions. 

Consider the following aggregate expression from the production scheduling
example of :numref:`ex-prod-planning`.

.. code-block:: minizinc

  int: mproducts = max (p in Products)
                       (min (r in Resources where consumption[p,r] > 0)
                                       (capacity[r] div consumption[p,r]));

Since this uses generator call syntax we can rewrite it to equivalent
form which is processed by the compiler:

.. code-block:: minizinc

  int: mproducts = max([ min [ capacity[r] div consumption[p,r]
                             | r in Resources where consumption[p,r] > 0])
                       | p in Products]);

Given the data

.. code-block:: minizinc

  nproducts = 2; 
  nresources = 5; 
  capacity = [4000, 6, 2000, 500, 500];
  consumption= [| 250, 2, 75, 100, 0,
                | 200, 0, 150, 150, 75 |];

this first builds the array for :mzn:`p = 1`

.. code-block:: minizinc

  [ capacity[r] div consumption[p,r]
                             | r in 1..5 where consumption[p,r] > 0]          

which is :mzn:`[16, 3, 26, 5]` and then calculates the minimum as 3.
It then builds the same array for :mzn:`p = 2` which is
:mzn:`[20, 13, 3, 6]` and calculates the minimum as 3. It then constructs the
array :mzn:`[3, 3]` and calculates the maximum as 3.  
There is no representation of :mzn:`mproducts` in the output FlatZinc,
this evaluation is simply used to replace :mzn:`mproducts` by the
calculated value 3.

The most common form of aggregate expression in a constraint model is
:mzn:`forall`.  Forall expressions are unrolled into multiple constraints.

Consider the MiniZinc fragment

.. code-block:: minizinc

  array[1..8] of var 0..9: v = [S,E,N,D,M,O,R,Y];
  constraint forall(i,j in 1..8 where i < j)(v[i] != v[j])

which arises from the SEND-MORE-MONEY example of :numref:`ex-smm`
using a default decomposition for :mzn:`alldifferent`.
The :mzn:`forall` expression creates a constraint for each :math:`i, j` pair which meet
the requirement :math:`i < j`, thus creating

.. code-block:: minizinc

  constraint v[1] != v[2]; % S != E
  constraint v[1] != v[3]; % S != N
  ...
  constraint v[1] != v[8]; % S != Y
  constraint v[2] != v[3]; % E != N
  ...
  constraint v[7] != v[8]; % R != Y

In FlatZinc form this is

.. code-block:: minizinc

  constraint int_neq(S,E);
  constraint int_neq(S,N);
  ...
  constraint int_neq(S,Y);
  constraint int_neq(E,N);
  ...
  constraint int_neq(R,Y);

Notice how the temporary array variables :mzn:`v[i]` are replaced by the
original variables in the output FlatZinc.
       
Arrays
------

One dimensional arrays 
in MiniZinc can have arbitrary indices as long as they are
contiguous integers.  In FlatZinc all arrays are indexed from :mzn:`1..l`
where :mzn:`l` is the length of the array.  This means that array lookups need to
be translated to the FlatZinc view of indices. 

Consider the following MiniZinc model for balancing a seesaw
of length :mzn:`2 * l2`,
with a child of weight :mzn:`cw` kg using exactly :mzn:`m` 1kg weights.

.. code-block:: minizinc

  int: cw;                               % child weight
  int: l2;                               % half seesaw length
  int: m;                                % number of 1kg weight
  array[-l2..l2] of var 0..max(m,cw): w; % weight at each point
  var -l2..l2: p;                        % position of child
  constraint sum(i in -l2..l2)(i * w[i]) = 0; % balance
  constraint sum(i in -l2..l2)(w[i]) = m + cw; % all weights used
  constraint w[p] = cw;                  % child is at position p
  solve satisfy;

Given :mzn:`cw = 2`, :mzn:`l2 = 2`, and :mzn:`m = 3` the unrolling produces the constraints

.. code-block:: minizinc

  array[-2..2] of var 0..3: w;
  var -2..2: p
  constraint -2*w[-2] + -1*w[-1] + 0*w[0] + 1*w[1] + 2*w[2] = 0;
  constraint w[-2] + w[-1] + w[0] + w[1] + w[2] = 5; 
  constraint w[p] = 2;

But FlatZinc insists that the :mzn:`w` array starts at index 1.
This means we need to rewrite all the array accesses to use the new index
value. For fixed array lookups this is easy, for variable array lookups we
may need to create a new variable.  The result for the equations above is

.. code-block:: minizinc

  array[1..5] of var 0..3: w;
  var -2..2: p
  var 1..5: INT01;
  constraint -2*w[1] + -1*w[2] + 0*w[3] + 1*w[4] + 2*w[5] = 0;
  constraint w[1] + w[2] + w[3] + w[4] + w[5] = 5; 
  constraint w[INT01] = 2;
  constraint INT01 = p + 3;

Finally we rewrite the constraints into FlatZinc form. Note how the variable
array index lookup is mapped to :mzn:`array_var_int_element`.

.. code-block:: minizinc

  array[1..5] of var 0..3: w;
  var -2..2: p
  var 1..5: INT01;
  constraint int_lin_eq([2, 1, -1, -2], [w[1], w[2], w[4], w[5]], 0);
  constraint int_lin_eq([1, 1, 1, 1, 1], [w[1],w[2],w[3],w[4],w[5]], 5);
  constraint array_var_int_element(INT01, w, 2);
  constraint int_lin_eq([-1, 1], [INT01, p], -3);

Multidimensional arrays are supported by MiniZinc, but only single
dimension arrays are supported by FlatZinc (at present). 
This means that multidimensional arrays must be mapped to single dimension
arrays, and multidimensional array access must be mapped to single dimension
array access. 
  
Consider the Laplace equation constraints defined for a finite element
plate model in :numref:`ex-laplace`:

.. literalinclude:: examples/laplace.mzn
  :language: minizinc
  :start-after: % arraydec
  :end-before: % sides


Assuming :mzn:`w = 4` and :mzn:`h = 4` this creates the constraints

.. code-block:: minizinc

  array[0..4,0..4] of var float: t; % temperature at point (i,j)
  constraint 4.0*t[1,1] = t[0,1] + t[1,0] + t[2,1] + t[1,2];
  constraint 4.0*t[1,2] = t[0,2] + t[1,1] + t[2,2] + t[1,3];
  constraint 4.0*t[1,3] = t[0,3] + t[1,2] + t[2,3] + t[1,4];
  constraint 4.0*t[2,1] = t[1,1] + t[2,0] + t[3,1] + t[2,2];
  constraint 4.0*t[2,2] = t[1,2] + t[2,1] + t[3,2] + t[2,3];
  constraint 4.0*t[2,3] = t[1,3] + t[2,2] + t[3,3] + t[2,4];
  constraint 4.0*t[3,1] = t[2,1] + t[3,0] + t[4,1] + t[3,2];
  constraint 4.0*t[3,2] = t[2,2] + t[3,1] + t[4,2] + t[3,3];
  constraint 4.0*t[3,3] = t[2,3] + t[3,2] + t[4,3] + t[3,4];

The 2 dimensional array of 25 elements is converted to a one dimensional
array and the indices are changed accordingly: so index :mzn:`[i,j]` becomes
:mzn:`[i * 5 + j + 1]`. 

.. code-block:: minizinc

  array [1..25] of var float: t;
  constraint 4.0*t[7] = t[2] + t[6] + t[12] + t[8];
  constraint 4.0*t[8] = t[3] + t[7] + t[13] + t[9];
  constraint 4.0*t[9] = t[4] + t[8] + t[14] + t[10];
  constraint 4.0*t[12] = t[7] + t[11] + t[17] + t[13];
  constraint 4.0*t[13] = t[8] + t[12] + t[18] + t[14];
  constraint 4.0*t[14] = t[9] + t[13] + t[19] + t[15];
  constraint 4.0*t[17] = t[12] + t[16] + t[22] + t[18];
  constraint 4.0*t[18] = t[13] + t[17] + t[23] + t[19];
  constraint 4.0*t[19] = t[14] + t[18] + t[24] + t[20];


Reification
-----------

.. index::
  single: reification

FlatZinc models involve only variables and parameter declarations
and a series of primitive constraints.  Hence when we model in MiniZinc
with Boolean connectives other than conjunction, something has to be done. 
The core approach to handling complex formulae that use 
connectives other than conjunction is by
*reification*. 
Reifying a constraint :math:`c` creates a new constraint equivalent to :math:`b \leftrightarrow c`
where the Boolean variable :math:`b` is :mzn:`true`
if the constraint holds and :mzn:`false` if it doesn't hold. 

Once we have the capability to *reify* constraints the treatment of
complex formulae is not different from arithmetic expressions. We create a
name for each subexpression and a flat constraint to constrain the name to
take the value of its subexpression.


Consider the following constraint expression that occurs in the jobshop
scheduling example of :numref:`ex-jobshop`.

.. code-block:: minizinc

  constraint %% ensure no overlap of tasks 
      forall(j in 1..tasks) (
          forall(i,k in 1..jobs where i < k) ( 
              s[i,j] + d[i,j] <= s[k,j] \/ 
              s[k,j] + d[k,j] <= s[i,j]
      ) );

Given the data file  

.. code-block:: minizinc

  jobs = 2;
  tasks = 3;
  d = [| 5, 3, 4 | 2, 6, 3 |]

then the unrolling creates 

.. code-block:: minizinc

  constraint s[1,1] + 5 <= s[2,1] \/ s[2,1] + 2 <= s[1,1];
  constraint s[1,2] + 3 <= s[2,2] \/ s[2,2] + 6 <= s[1,2];
  constraint s[1,3] + 4 <= s[2,3] \/ s[2,3] + 3 <= s[1,3];

Reification of the constraints that appear in the disjunction
creates new Boolean variables to define the values of each expression.

.. code-block:: minizinc

  array[1..2,1..3] of var 0..23: s;
  constraint BOOL01 <-> s[1,1] + 5 <= s[2,1];
  constraint BOOL02 <-> s[2,1] + 2 <= s[1,1];
  constraint BOOL03 <-> s[1,2] + 3 <= s[2,2];
  constraint BOOL04 <-> s[2,2] + 6 <= s[1,2];
  constraint BOOL05 <-> s[1,3] + 4 <= s[2,3];
  constraint BOOL06 <-> s[2,3] + 3 <= s[1,3];
  constraint BOOL01 \/ BOOL02;
  constraint BOOL03 \/ BOOL04;
  constraint BOOL05 \/ BOOL06;

Each primitive constraint can now be mapped to the FlatZinc form.
Note how the two dimensional array :mzn:`s` is mapped to a one dimensional form.

.. code-block:: minizinc

  array[1..6] of var 0..23: s;
  constraint int_lin_le_reif([1, -1], [s[1], s[4]], -5, BOOL01);
  constraint int_lin_le_reif([-1, 1], [s[1], s[4]], -2, BOOL02);
  constraint int_lin_le_reif([1, -1], [s[2], s[5]], -3, BOOL03);
  constraint int_lin_le_reif([-1, 1], [s[2], s[5]], -6, BOOL04);
  constraint int_lin_le_reif([1, -1], [s[3], s[6]], -4, BOOL05);
  constraint int_lin_le_reif([-1, 1], [s[3], s[6]], -3, BOOL06);
  constraint array_bool_or([BOOL01, BOOL02], true);
  constraint array_bool_or([BOOL03, BOOL04], true);
  constraint array_bool_or([BOOL05, BOOL06], true);

The :mzn:`int_lin_le_reif` is the reified form of the linear constraint
:mzn:`int_lin_le`.

Most FlatZinc primitive constraints :math:`p(\bar{x})` have a reified form
:math:`\mathit{p\_reif}(\bar{x},b)` which takes an additional final argument :math:`b`
and defines the constraint :math:`b \leftrightarrow p(\bar{x})`. 
FlatZinc primitive constraints which define functional relationships,
like :mzn:`int_plus` and :mzn:`int_plus`, do
not need to support reification. Instead, the equality with the result of the function is reified.

Another important use of reification arises when we use the coercion
function :mzn:`bool2int` (either explicitly, or implicitly by using a Boolean
expression as an integer expression). Flattening creates a Boolean
variable to hold the value of the Boolean expression argument, as well as an
integer variable (restricted to :mzn:`0..1`) to hold this value.

Consider the magic series problem of :numref:`ex-magic-series`.

.. literalinclude:: examples/magic-series.mzn
  :language: minizinc
  :end-before: solve satisfy

Given :mzn:`n = 2` the unrolling creates

.. code-block:: minizinc

  constraint s[0] = bool2int(s[0] = 0) + bool2int(s[1] = 0);
  constraint s[1] = bool2int(s[0] = 1) + bool2int(s[1] = 1);

and flattening creates

.. code-block:: minizinc

  constraint BOOL01 <-> s[0] = 0;
  constraint BOOL03 <-> s[1] = 0;
  constraint BOOL05 <-> s[0] = 1;
  constraint BOOL07 <-> s[1] = 1;
  constraint INT02 = bool2int(BOOL01);
  constraint INT04 = bool2int(BOOL03);
  constraint INT06 = bool2int(BOOL05);
  constraint INT08 = bool2int(BOOL07);
  constraint s[0] = INT02 + INT04;
  constraint s[1] = INT06 + INT08;

The final FlatZinc form is

.. code-block:: minizinc

  var bool: BOOL01;
  var bool: BOOL03;
  var bool: BOOL05;
  var bool: BOOL07;
  var 0..1: INT02;
  var 0..1: INT04;
  var 0..1: INT06;
  var 0..1: INT08;
  array [1..2] of var 0..2: s;
  constraint int_eq_reif(s[1], 0, BOOL01);
  constraint int_eq_reif(s[2], 0, BOOL03);
  constraint int_eq_reif(s[1], 1, BOOL05);
  constraint int_eq_reif(s[2], 1, BOOL07);
  constraint bool2int(BOOL01, INT02);
  constraint bool2int(BOOL03, INT04);
  constraint bool2int(BOOL05, INT06);
  constraint bool2int(BOOL07, INT08);
  constraint int_lin_eq([-1, -1, 1], [INT02, INT04, s[1]], 0);
  constraint int_lin_eq([-1, -1, 1], [INT06, INT08, s[2]], 0);
  solve satisfy;

Predicates
----------

An important factor in the support for MiniZinc by many different solvers
is that global constraints (and indeed FlatZinc constraints) can be
specialized for the particular solver. 

Each solver will either specify a predicate without a definition, or with a
definition. For example a solver that has a builtin global :mzn:`alldifferent`
predicate, will include the definition

.. code-block:: minizinc

  predicate alldifferent(array[int] of var int:x);

in its globals library, while a solver using the default decomposition will
have the definition

.. code-block:: minizinc

  predicate alldifferent(array[int] of var int:x) =
      forall(i,j in index_set(x) where i < j)(x[i] != x[j]);

Predicate calls :math:`p(\bar{t})`
are flattened by first constructing variables :math:`v_i` for
each argument terms :math:`t_i`.
If the predicate has no definition we simply use a call to the predicate
with the constructed arguments: :math:`p(\bar{v})`.
If the predicate has a definition :math:`p(\bar{x}) = \phi(\bar{x})`
then we replace the predicate call :math:`p(\bar{t})`
with the body of the predicate with the formal arguments replaced by the
argument variables, that is :math:`\phi(\bar{v})`. 
Note that if a predicate call  :math:`p(\bar{t})`
appears in a reified position and it has no definition, we check for the
existence of a reified version of the predicate :math:`\mathit{p\_reif}(\bar{x},b)` in which
case we use that.

Consider the :mzn:`alldifferent` constraint in the 
SEND-MORE-MONEY example of :numref:`ex-smm`

.. code-block:: minizinc

  constraint alldifferent([S,E,N,D,M,O,R,Y]);

If the solver has a builtin :mzn:`alldifferent` we simply construct a new variable
for the argument, and replace it in the call.

.. code-block:: minizinc

  array[1..8] of var 0..9: v = [S,E,N,D,M,O,R,Y];
  constraint alldifferent(v);

Notice that bounds analysis attempts to find tight bounds on the new array
variable.   The reason for constructing the array argument is if we use the
same array twice the FlatZinc solver does not have to construct it twice.
In this case since it is not used twice a later stage of the translation
will replace :mzn:`v` by its definition.

What if the solver uses the default definition of :mzn:`alldifferent`? 
Then the variable :mzn:`v` is defined as usual, and the predicate call is
replaced by a renamed copy where :mzn:`v` replaces the formal argument :mzn:`x`.
The resulting code is 

.. code-block:: minizinc

  array[1..8] of var 0..9: v = [S,E,N,D,M,O,R,Y];
  constraint forall(i,j in 1..8 where i < j)(v[i] != v[j])

which we examined in :ref:`sec-unroll`.

Consider the following constraint, where :mzn:`alldifferent` appears in a reified
position. 

.. code-block:: minizinc

  constraint alldifferent([A,B,C]) \/ alldifferent([B,C,D]);

If the solver has a reified form of :mzn:`alldifferent` this will be flattened to

.. code-block:: minizinc

  constraint alldifferent_reif([A,B,C],BOOL01);
  constraint alldifferent_reif([B,C,D],BOOL02);
  constraint array_bool_or([BOOL01,BOOL02],true);

Using the default decomposition, the predicate replacement will first create

.. code-block:: minizinc

  array[1..3] of var int: v1 = [A,B,C];
  array[1..3] of var int: v2 = [B,C,D];
  constraint forall(i,j in 1..3 where i<j)(v1[i] != v1[j]) \/
             forall(i,j in 1..3 where i<j)(v2[i] != v2[j]);

which will eventually be flattened to the FlatZinc form

.. code-block:: minizinc

  constraint int_neq_reif(A,B,BOOL01);
  constraint int_neq_reif(A,C,BOOL02);
  constraint int_neq_reif(B,C,BOOL03);
  constraint array_bool_and([BOOL01,BOOL02,BOOL03],BOOL04);
  constraint int_neq_reif(B,D,BOOL05);
  constraint int_neq_reif(C,D,BOOL06);
  constraint array_bool_and([BOOL03,BOOL05,BOOL06],BOOL07);
  constraint array_bool_or([BOOL04,BOOL07],true);

Note how common subexpression elimination reuses the 
reified inequality :mzn:`B != C` (although there is a better translation which
lifts the common constraint to the top level conjunction).

.. _sec-let:

Let Expressions
---------------

Let expressions are a powerful facility of MiniZinc to introduce new
variables. This is useful for creating common sub expressions, and for
defining local variables for predicates.  
During flattening let expressions are translated to variable and constraint
declarations. The relational semantics of MiniZinc means that these
constraints must appear as if conjoined 
in the first enclosing Boolean expression. 

A key feature of let expressions is that each time they are used they
create new variables. 

Consider the flattening of the code

.. code-block:: minizinc

  constraint even(u) \/ even(v);
  predicate even(var int: x) = 
            let { var int: y } in x = 2 * y;

First the predicate calls are replaced by their definition.

.. code-block:: minizinc

  constraint (let { var int: y} in u = 2 * y) \/
             (let { var int: y} in v = 2 * y);   

Next let variables are renamed apart

.. code-block:: minizinc

  constraint (let { var int: y1} in u = 2 * y1) \/
             (let { var int: y2} in v = 2 * y2);   

Finally variable declarations are extracted to the top level

.. code-block:: minizinc

  var int: y1;
  var int: y2;
  constraint u = 2 * y1 \/ v = 2 * y2;   

Once the let expression is removed we can flatten as usual.        

Remember that let expressions can define values for newly introduced
variables (and indeed must do so for parameters).
These implicitly define constraints that must also be flattened.

Consider the complex objective function for wedding seating problem of
:numref:`ex-wedding2`.

.. code-block:: minizinc

  solve maximize sum(h in Hatreds)(
        let {  var Seats: p1 = pos[h1[h]],
               var Seats: p2 = pos[h2[h]],
               var 0..1: same = bool2int(p1 <= 6 <-> p2 <= 6) } in   
        same * abs(p1 - p2) + (1-same) * (abs(13 - p1 - p2) + 1));

For conciseness we assume only the first two Hatreds, so

.. code-block:: minizinc

  set of int: Hatreds = 1..2;
  array[Hatreds] of Guests: h1 = [groom, carol];
  array[Hatreds] of Guests: h2 = [clara, bestman];

The first step of flattening is to unroll the :mzn:`sum` expression, giving
(we keep the guest names and parameter :mzn:`Seats` for clarity only, in
reality they would be replaced by their definition):

.. code-block:: minizinc

  solve maximize 
        (let { var Seats: p1 = pos[groom],
               var Seats: p2 = pos[clara],
               var 0..1: same = bool2int(p1 <= 6 <-> p2 <= 6) } in   
         same * abs(p1 - p2) + (1-same) * (abs(13 - p1 - p2) + 1)) 
        +
        (let { var Seats: p1 = pos[carol],
               var Seats: p2 = pos[bestman],
               var 0..1: same = bool2int(p1 <= 6 <-> p2 <= 6) } in   
         same * abs(p1 - p2) + (1-same) * (abs(13 - p1 - p2) + 1)); 

Next each new variable in a let expression is renamed to be distinct

.. code-block:: minizinc

  solve maximize 
        (let { var Seats: p11 = pos[groom],
               var Seats: p21 = pos[clara],
               var 0..1: same1 = bool2int(p11 <= 6 <-> p21 <= 6) } in   
         same1 * abs(p11 - p21) + (1-same1) * (abs(13 - p11 - p21) + 1)) 
        +
        (let { var Seats: p12 = pos[carol],
               var Seats: p22 = pos[bestman],
               var 0..1: same2 = bool2int(p12 <= 6 <-> p22 <= 6) } in   
         same2 * abs(p12 - p22) + (1-same2) * (abs(13 - p12 - p22) + 1)); 

Variables in the let expression 
are extracted to the top level and 
defining constraints are extracted to the correct
level (which in this case is also the top level).

.. code-block:: minizinc

  var Seats: p11;
  var Seats: p21;
  var 0..1: same1;
  constraint p12 = pos[clara];
  constraint p11 = pos[groom];
  constraint same1 = bool2int(p11 <= 6 <-> p21 <= 6);
  var Seats p12; 
  var Seats p22;
  var 0..1: same2;
  constraint p12 = pos[carol];
  constraint p22 = pos[bestman];
  constraint same2 = bool2int(p12 <= 6 <-> p22 <= 6) } in   
  solve maximize 
        same1 * abs(p11 - p21) + (1-same1) * (abs(13 - p11 - p21) + 1)) 
        +
        same2 * abs(p12 - p22) + (1-same2) * (abs(13 - p12 - p22) + 1)); 

Now we have constructed equivalent MiniZinc code without the use of let
expressions and the flattening can proceed as usual.

As an illustration of let expressions that do not appear at the top level
consider the following model

.. code-block:: minizinc

  var 0..9: x;
  constraint x >= 1 -> let { var 2..9: y = x - 1 } in 
                       y + (let { var int: z = x * y } in z * z) < 14;

We extract the variable definitions to the top level and the constraints to
the first enclosing Boolean context, which here is the right hand side of
the implication.

.. code-block:: minizinc

  var 0..9: x;
  var 2..9: y;
  var int: z;
  constraint x >= 1 -> (y = x - 1 /\ z = x * y /\ y + z * z < 14);

Note that if we know that the equation defining a  variable definition
cannot fail we can extract it to the top level. This will usually make
solving substantially faster. 

For the example above the constraint :mzn:`y = x - 1` can fail since the domain
of :mzn:`y` is not big enough for all possible values of :mzn:`x - 1`. But the
constraint :mzn:`z = x * y` cannot (indeed bounds analysis will give :mzn:`z`
bounds big enough to hold all possible values of :mzn:`x * y`).
A better flattening will give

.. code-block:: minizinc

  var 0..9: x;
  var 2..9: y;
  var int: z;
  constraint z = x * y;
  constraint x >= 1 -> (y = x - 1 /\ y + z * z < 14);

Currently the MiniZinc compiler does this by always defining the declared 
bounds of an
introduced variable to be big enough for its defining equation to always 
hold and then adding bounds constraints in the correct context for the let
expression.   On the example above this results in

.. code-block:: minizinc

  var 0..9: x;
  var -1..8: y;
  var -9..72: z;
  constraint y = x - 1;
  constraint z = x * y;
  constraint x >= 1 -> (y >= 2 /\ y + z * z < 14);

This translation leads to more efficient solving since the possibly 
complex calculation of the let variable is not reified.

Another reason for this approach is that it also works when introduced variables
appear in negative contexts (as long as they have a definition).
Consider the following example similar to the previous one

.. code-block:: minizinc

  var 0..9: x;
  constraint (let { var 2..9: y = x - 1 } in 
             y + (let { var int: z = x * y } in z * z) > 14) -> x >= 5;

The let expressions appear in a negated context, but each introduced
variable is defined. The flattened code is

.. code-block:: minizinc

  var 0..9: x;
  var -1..8: y;
  var -9..72: z;
  constraint y = x - 1;
  constraint z = x * y;
  constraint (y >= 2 /\ y + z * z > 14) -> x >= 5;

Note the analog to the simple approach to let elimination
does not give a correct translation:

.. code-block:: minizinc

  var 0..9: x;
  var 2..9: y;
  var int: z;
  constraint (y = x - 1 /\ z = x * y /\ y + z * z > 14) -> x >= 5;

gives answers for all possible values of :mzn:`x`, whereas the original
constraint removes the possibility that :mzn:`x = 4`.

The treatment of *constraint items* in let expressions is analogous
to defined variables. One can think of a constraint as equivalent to
defining a new Boolean variable. The definitions of the new Boolean variables
are extracted to the top level, and the Boolean remains in the correct
context. 

.. code-block:: minizinc

  constraint z > 1 -> let { var int: y,
                            constraint (x >= 0) -> y = x,
                            constraint (x < 0)  -> y = -x 
                      } in y * (y - 2) >= z;

is treated like

.. code-block:: minizinc

  constraint z > 1 -> let { var int: y,
                            var bool: b1 = ((x >= 0) -> y = x),
                            var bool: b2 = ((x < 0)  -> y = -x),
                            constraint b1 /\ b2
                      } in y * (y - 2) >= z;

and flattens to

.. code-block:: minizinc

  constraint b1 = ((x >= 0) -> y = x);
  constraint b2 = ((x < 0)  -> y = -x);
  constraint z > 1 -> (b1 /\ b2 /\ y * (y - 2) >= z);
