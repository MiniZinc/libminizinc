.. _ch-solution-checkers:

Automatic Solution Checking
===========================

MiniZinc provides support for checking the correctness of solutions of a model by using a *checker model*. This approach has two main applications:

- Instructors can provide checker models for student assignments. This provides students with immediate, detailed feedback on their modelling attempts.
- A simplified checker model can be used to verify a complex model used for solving. This can be useful when experimenting with new decompositions of constraints, or for post-processing solutions if some constraints cannot be added to the original model (e.g. in case the solver does not support certain constructs).

Running MiniZinc with solution checking is easy. On the command line, simply pass the name of the checker model in addition to the problem model:

.. code-block:: bash

  minizinc model.mzn model.mzc.mzn data.dzn

Checker models can be pre-compiled in order to obfuscate their contents (e.g. if they contain clues to students how to model the problem):

.. code-block:: bash

  minizinc --compile-solution-checker model.mzc.mzn

This will create the compiled checker model ``model.mzc``, which can be used instead of the clear text version for checking:

.. code-block:: bash

  minizinc model.mzn model.mzc data.dzn

The MiniZinc IDE has built-in support for solution checkers. Whenever the current project contains a file with the same file name as the current model but file extension ``.mzc`` or ``.mzc.mzn``, the "Run" button turns into a "Run+Check" button.

The rest of this section describes how to implement checker models.

Basic checker models
--------------------

At its core, a checker model takes each solution that a solver produces as input, and outputs whether the solution is correct or not. Let's use the simple map colouring model from the tutorial as an example. Here is the model again:

.. literalinclude:: examples/aust.mzn
  :language: minizinc
  :caption: A MiniZinc model :download:`aust.mzn <examples/aust.mzn>` for colouring the states and territories in Australia
  :name: ex-aust-2

A checker model for this model requires the values of the variables :mzn:`wa`, :mzn:`nt`, and so on, for each solution, and then has to test whether all constraints hold. The output of the checker model should contain a line starting with ``CORRECT`` if the solution passes the test, or ``INCORRECT`` if it doesn't.

Since these values will be fixed in any solution, checker models simply declare parameters with the same name as the model variables:

.. literalinclude:: examples/aust.mzc.mzn
  :language: minizinc
  :caption: A MiniZinc checker model :download:`aust.mzc.mzn <examples/aust.mzc.mzn>` for the map colouring problem
  :name: ex-aust-check

Running the model and the checker will produce output like this:

.. code-block:: none

  wa=3	 nt=2	 sa=1
  q=3	 nsw=2	 v=3
  t=3
  % Solution checker report:
  % CORRECT
  ----------

The solution checker report is embedded as comments in the original output.

Detailed feedback
-----------------

The basic checker model above only reports whether the solutions satisfy the constraints, but it doesn't provide any insights into the nature of the error if a constraint is violated.

We can use standard MiniZinc functionality to provide much more detailed feedback. The following checker model introduces a helper function :mzn:`check` which outputs a detailed error message if a constraint doesn't hold. The results of all the checks are combined, and if any of the constraints was violated, the output is ``INCORRECT``, otherwise it is ``CORRECT``.

.. literalinclude:: examples/aust-2.mzc.mzn
  :language: minizinc
  :caption: A checker model :download:`aust-2.mzc.mzn <examples/aust-2.mzc.mzn>` for the map colouring problem with more detailed error messages
  :name: ex-aust-check-2

However, the checker model will only report the first violated constraint, since the conjunction operator short-circuits the evaluation when one of its arguments is false. For example, if we remove all constraints from the original model, the output would be:

.. code-block:: none

  wa=3	 nt=3	 sa=3
  q=3	 nsw=3	 v=3
  t=3
  % Solution checker report:
  % ERROR: ERROR: wa and nt have the same colour
  % INCORRECT
  ----------

In order to get all error messages, we can force the evaluation of all checkers by creating an auxiliary array of check results:

.. literalinclude:: examples/aust-3.mzc.mzn
  :language: minizinc
  :caption: A checker model :download:`aust-3.mzc.mzn <examples/aust-3.mzc.mzn>` for the map colouring problem without short-circuit evaluation
  :name: ex-aust-check-3

Now the output contains all error messages (for the case where the model has no constraints):

.. code-block:: none

  wa=3	 nt=3	 sa=3
  q=3	 nsw=3	 v=3
  t=3
  % Solution checker report:
  % ERROR: ERROR: nsw and v have the same colour
  % ERROR: ERROR: q and nsw have the same colour
  % ERROR: ERROR: sa and v have the same colour
  % ERROR: ERROR: sa and nsw have the same colour
  % ERROR: ERROR: sa and q have the same colour
  % ERROR: ERROR: nt and q have the same colour
  % ERROR: ERROR: nt and sa have the same colour
  % ERROR: ERROR: wa and sa have the same colour
  % ERROR: ERROR: wa and nt have the same colour
  % INCORRECT
  ----------

Instance data in checker models
-------------------------------

The map colouring example was quite simple because the model did not contain any parameter declarations. For a model that is parameterised, the checker model simply contains the same parameter declarations. MiniZinc will then pass the actual parameters of the instance being solved to the checker model.

For example, the following checker model could be used for the *n*-Queens problem.

.. literalinclude:: examples/nqueens.mzc.mzn
  :language: minizinc
  :caption: A checker model :download:`nqueens.mzc.mzn <examples/nqueens.mzc.mzn>` for the n-Queens problem
  :name: ex-nqueens-check

Note how it first checks whether the solution has the right dimensions (correct array index set, and each variable is assigned a value in the correct domain), and then uses standard MiniZinc constructs to check each constraint.

Checking optimisation problems
------------------------------

Optimisation problems pose a difficulty for automatic checking. When a solver claims to prove optimality, we cannot easily verify this claim without solving the problem again (using a different model that is known to be correct). At the moment, solution checking for optimisation problems is restricted to checking that the objective has been computed correctly. The example in the next section illustrates how to check the value of the objective.

Hidden variables
----------------

For many models, the decision variables which describe the solution may not be the variables that are natural to describe the constraints. There may be other internal variables which are functionally defined by the decision variables, and which are likely to be used in the model for building constraints and/or are much more natural for describing the correctness of a candidate solution.

Consider the problem of lining up *n* people numbered from 1 to *n* for a photo
in a single line. We want to make sure that there are no more than two people of
the same gender (male, female or other) in sequence in the line. We want to
minimize the total distance between the pairs of people who are numbered
consecutively. The decisions are an array :mzn:`pos` which for each person
gives their position in the line. A correct model for this is given in :numref:`ex-photo`.

.. literalinclude:: examples/photo.mzn
  :language: minizinc
  :caption: A model :download:`photo.mzn <examples/photo.mzn>` for the photo lineup problem
  :name: ex-photo

Note that a critical part of learning how to model this problem is to realise
that it is worth introducing the inverse of the :mzn:`pos` variables, called :mzn:`who` in this model. The :mzn:`who` array makes it much easier to specify that no more than two people of the same gender can be
adjacent. The checker model **should not** include the :mzn:`who` variables, because that would effectively give away the key to the solution.

The use of hidden variables (such as :mzn:`who`) makes checking harder in two
ways. First, we may need to define these in the checker model as decision variables (rather than parameters), and MiniZinc will then need to invoke a solver to generate the values of these variables in order to check a given solution. Second, given an incorrect candidate solution, there may be
no possible value for these hidden variables, hence we have to guard the
checking to ensure we do not assume that there is some value for the hidden
variables.

Consider this data file for the photo problem:

.. code-block:: minizinc

  n = 9;
  g = [M,M,M,M,F,F,F,M,M];

and assume that an incorrect model returns the erroneous candidate solution
:mzn:`pos = [1,2,3,4,5,6,7,2,9]; _objective=25;`. If we simply assert :mzn:`inverse(pos,who)` in the checker model, then this constraint will
fail since there is no inverse of this position array, because persons 2 and
8 are both at position 2.  Hence the checker must guard the inverse
constraint, and only when that succeeds use  the computed values of the 
:mzn:`who` variables for checking the solution. A checking model for the photo problem might look like this:

.. literalinclude:: examples/photo.mzc.mzn
  :language: minizinc
  :caption: A checker model :download:`photo.mzc.mzn <examples/photo.mzc.mzn>` for the photo lineup problem
  :name: ex-photo-checker

The checker model first tests whether the given :mzn:`pos` array satisfies the :mzn:`alldifferent` property (using a custom :mzn:`test` for :mzn:`alldifferent` on a par array). If it passes the test, the :mzn:`inverse` constraint is applied. Otherwise, the :mzn:`who` array is simply fixed to a list of ones.

Note how the check for the :mzn:`alldifferent` constraint tries to give a  detailed description of the error.
