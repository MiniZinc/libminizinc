.. _sec-sat:

Boolean Satisfiability Modelling in MiniZinc
============================================

MiniZinc can be used to model Boolean satisfiability
problems where the variables are restricted to be Boolean (:mzn:`bool`).
MiniZinc can be used with efficient Boolean satisfiability 
solvers to solve the resulting models efficiently.

Modelling Integers
------------------

Many times although we wish to use a Boolean satisfiability solver we may
need to model some integer parts of our problem. 

There are three common ways of modelling an integer variables
:math:`I` in the range :math:`0 \dots m` where :math:`m = 2^{k}-1`
using Boolean
variables. 

- Binary: :math:`I` is represented by :math:`k` binary variables
  :math:`i_0, \ldots, i_{k-1}` where
  :math:`I = 2^{k-1} i_{k-1} + 2^{k-2} i_{k-2} + \cdots + 2 i_1 + i_0`.
  This can be represented in MiniZinc as

  .. code-block:: minizinc

    array[0..k-1]  of var bool: i;
    var 0..pow(2,k)-1: I = sum(j in 0..k-1)(bool2int(i[j])*pow(2,j));

- Unary: where :math:`I` is represented by :math:`m` binary variables
  :math:`i_1, \ldots, i_m`
  and :math:`i = \sum_{j=1}^m \mathtt{bool2int}(i_j)`.  Since there is massive redundancy in
  the unary representation we usually require that
  :math:`i_j \rightarrow i_{j-1}, 1 < j \leq m`. This can be represented in MiniZinc as

  .. code-block:: minizinc

    array[1..m]  of var bool: i;
    constraint forall(j in 2..m)(i[j] -> i[j-1]);
    var 0..m: I = sum(j in 1..m)(bool2int(i[j]);

- Value: where :math:`I` is represented by :math:`m+1` binary variables
  :math:`i_0, \ldots, i_m` where
  :math:`I = k \Leftrightarrow i_k`, and at most one of :math:`i_0, \ldots, i_m` is true. 
  This can be represented in MiniZinc as

  .. code-block:: minizinc

    array[0..m]  of var bool: i;
    constraint sum(j in 0..m)(bool2int(i[j]) == 1;
    var 0..m: I;
    constraint forall(j in 0..m)(I == j <-> i[j]);

There are advantages and disadvantages to each representation.  It depends
on what operations on integers are to required in the model as to which is
preferable.

Modelling Disequality
---------------------

Let us consider modelling a latin squares problem. A latin square
is an :math:`n \times n` grid of numbers from :math:`1..n` such that 
each number appears exactly once in every row and column. 
An integer model for latin squares is shown in :numref:`ex-latin`.

.. literalinclude:: examples/latin.mzn
  :language: minizinc
  :name: ex-latin
  :caption: Integer model for Latin Squares (:download:`latin.mzn <examples/latin.mzn>`).

The only constraint on the integers is in fact disequality, which is encoded
in the :mzn:`alldifferent` constraint. 
The value representation is the best way of representing disequality.
A Boolean only model for latin squares is shown in
:numref:`ex-latinbool`.
Note each integer array element :mzn:`a[i,j]` is replaced by an array of
Booleans.
We use the :mzn:`exactlyone` predicate to encode that each value is used
exactly once in every row and every column, as well as to encode that exactly
one of the Booleans corresponding to integer array element :mzn:`a[i,j]` is true.

.. literalinclude:: examples/latinbool.mzn
  :language: minizinc
  :name: ex-latinbool
  :caption: Boolean model for Latin Squares (:download:`latinbool.mzn <examples/latinbool.mzn>`).

Modelling Cardinality
---------------------

Let us consider modelling the Light Up puzzle. The puzzle consists of a 
rectangular grid of squares which are blank, or filled. Every filled square
may contain a number from 1 to 4, or may have no number. The aim is to place
lights
in the blank squares so that

- Each blank square is "illuminated", that is can see a light through an
  uninterrupted line of blank squares
- No two lights can see each other
- The number of lights adjacent to a numbered filled square
  is exactly the number in the filled square.

An example of a Light Up puzzle is shown in :numref:`fig-lightup`
with its solution in :numref:`fig-lightup-sol`.

.. _fig-lightup:

.. figure:: figures/lightup.*
  
  An example of a Light Up puzzle

.. _fig-lightup-sol:

.. figure:: figures/lightup2.*
  
  The completed solution of the Light Up puzzle

It is natural to model this problem
using Boolean variables to determine which
squares contain a light and which do not, but there is some integer
arithmetic to consider for the filled squares.

.. literalinclude:: examples/lightup.mzn
  :language: minizinc
  :name: ex-lightup
  :caption: SAT Model for the Light Up puzzle (:download:`lightup.mzn <examples/lightup.mzn>`).

A model for the problem is given in :numref:`ex-lightup`.
A data file for the problem shown in :numref:`fig-lightup`
is shown in :numref:`fig-lightupdzn`.

.. literalinclude:: examples/lightup.dzn
  :language: minizinc
  :name: fig-lightupdzn
  :caption: Datafile for the Light Up puzzle instance shown in :numref:`fig-lightup`.

The model makes use of a Boolean sum predicate

.. code-block:: minizinc

  predicate bool_sum_eq(array[int] of var bool:x, int:s);

which requires that the sum of an array of Boolean equals some fixed
integer. There are a number of ways of modelling such
*cardinality* constraints using Booleans.

- Adder networks: we can use a network of adders to
  build a binary Boolean representation of the sum of the Booleans
- Sorting networks: we can use a sorting network to sort
  the array of Booleans to create a unary representation of the sum
  of the Booleans
- Binary decision diagrams: we can create a binary decision diagram
  (BDD) that encodes the cardinality constraint.


.. literalinclude:: examples/bboolsum.mzn
  :language: minizinc
  :name: ex-bboolsum
  :caption: Cardinality constraints by binary adder networks (:download:`bboolsum.mzn <examples/bboolsum.mzn>`).

.. literalinclude:: examples/binarysum.mzn
  :language: minizinc
  :name: ex-binarysum
  :caption: Code for building binary addition networks (:download:`binarysum.mzn <examples/binarysum.mzn>`).

We can implement :mzn:`bool_sum_eq` using binary adder networks
using the code shown in :numref:`ex-bboolsum`.
The predicate :mzn:`binary_sum`
defined in :numref:`ex-binarysum`
creates a binary representation
of the sum of :mzn:`x` by splitting the list into two,
summing up each half to create a binary representation
and then summing these two binary numbers using :mzn:`binary_add`.
If the list :mzn:`x` is odd the last bit is saved to use as a carry in
to the binary addition.

.. \pjs{Add a picture of an adding network}

.. literalinclude:: examples/uboolsum.mzn
  :language: minizinc
  :name: ex-uboolsum
  :caption: Cardinality constraints by sorting networks (:download:`uboolsum.mzn <examples/uboolsum.mzn>`).

.. literalinclude:: examples/oesort.mzn
  :language: minizinc
  :name: ex-oesort
  :caption: Odd-even merge sorting networks (:download:`oesort.mzn <examples/oesort.mzn>`).

We can implement :mzn:`bool_sum_eq` using unary sorting networks
using the code shown in :numref:`ex-uboolsum`.
The cardinality constraint is defined by expanding the input
:mzn:`x` to have length a power of 2, and sorting the resulting bits
using an odd-even merge sorting network. 
The odd-even merge sorter shown in :mzn:`ex-oesort` works 
recursively by splitting
the input list in 2, sorting each list and merging the two
sorted lists.  

.. \pjs{Add much more stuff on sorting networks}



.. \pjs{Add a picture of an adding network}

.. literalinclude:: examples/bddsum.mzn
  :language: minizinc
  :name: ex-bddsum
  :caption: Cardinality constraints by binary decision diagrams (:download:`bddsum.mzn <examples/bddsum.mzn>`).

We can implement :mzn:`bool_sum_eq` using binary decision diagrams
using the code shown in :mzn:`ex:bddsum`.
The cardinality constraint is broken into two cases:
either the first element :mzn:`x[1]` is :mzn:`true`, 
and the sum of the remaining bits
is :mzn:`s-1`, or :mzn:`x[1]` is :mzn:`false` and the sum of the remaining bits
is :mzn:`s`. For efficiency this relies on common subexpression elimination
to avoid creating many equivalent constraints.


.. \pjs{Add a picture of a bdd network network}


