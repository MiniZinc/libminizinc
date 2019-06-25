.. |coerce| replace:: :math:`\stackrel{c}{\rightarrow}`

.. |varify| replace:: :math:`\stackrel{v}{\rightarrow}`

.. |TyOverview| replace:: *Overview.*

.. |TyInsts| replace:: *Allowed Insts.*

.. |TySyntax| replace:: *Syntax.*

.. |TyFiniteType| replace:: *Finite?*

.. |TyVarifiable| replace:: *Varifiable?*

.. |TyOrdering| replace:: *Ordering.*

.. |TyInit| replace:: *Initialisation.*

.. |TyCoercions| replace:: *Coercions.*

.. _ch-mzn-spec:

Specification of MiniZinc
=========================

Introduction
------------

This document defines MiniZinc, a language for modelling constraint
satisfaction and optimisation problems.

MiniZinc is a high-level, typed, mostly first-order, functional, modelling
language.  It provides:

- mathematical notation-like syntax (automatic coercions, overloading,
  iteration, sets, arrays);
- expressive constraints (finite domain, set, linear arithmetic, integer);
- support for different kinds of problems (satisfaction, explicit
  optimisation);
- separation of data from model;
- high-level data structures and data encapsulation (sets,
  arrays, enumerated types, constrained type-insts);
- extensibility (user-defined functions and predicates);
- reliability (type checking, instantiation checking, assertions);
- solver-independent modelling;
- simple, declarative semantics.

MiniZinc is similar to 
OPL and moves closer to CLP languages such as ECLiPSe.

This document has the following structure.
:ref:`spec-syntax-notation` introduces the syntax notation
used throughout the specification.
:ref:`spec-Overview` provides a high-level overview of MiniZinc models.
:ref:`spec-Syntax-Overview` covers syntax basics.
:ref:`spec-High-level-Model-Structure` covers high-level structure: items,
multi-file models, namespaces, and scopes.
:ref:`spec-Types` introduces types and type-insts.
:ref:`spec-Expressions` covers expressions.
:ref:`spec-Items` describes the top-level items in detail.
:ref:`spec-Annotations` describes annotations.
:ref:`spec-Partiality` describes how partiality is handled in various
cases.
:ref:`spec-builtins` describes the language built-ins.
:ref:`spec-Grammar` gives the MiniZinc grammar.
:ref:`spec-Content-types` defines content-types used in this specification.

This document also provides some explanation of why certain design decisions
were made.  Such explanations are marked by the word *Rationale* and
written in italics, and do not constitute part of the specification as such.
*Rationale: These explanations are present because they are useful to both
the designers and the users of MiniZinc.*

Original authors.
~~~~~~~~~~~~~~~~~

The original version of this document was prepared by
Nicholas Nethercote, Kim Marriott, Reza Rafeh, Mark Wallace
and Maria Garcia de la Banda.
MiniZinc is evolving, however, and so is this document.

For a formally published paper on the MiniZinc language
and the superset Zinc language, please see:

- N. Nethercote, P.J. Stuckey, R. Becket, S. Brand, G.J. Duck, and G. Tack.
  Minizinc: Towards a standard CP modelling language.
  In C. Bessiere, editor, *Proceedings of the 13th International
  Conference on Principles and Practice of Constraint Programming*, volume 4741
  of *LNCS*, pages 529--543. Springer-Verlag, 2007.
- K. Marriott, N. Nethercote, R. Rafeh, P.J. Stuckey,
  M. Garcia de la Banda, and M. Wallace.
  The Design of the Zinc Modelling Language.
  *Constraints*, 13(3):229-267, 2008.

.. _spec-syntax-notation:

Notation
--------

The basics of the EBNF used in this specification are as follows.

- Non-terminals are written between angle brackets, :mzndef:`<item>`.
- Terminals are written in double quotes, e.g. :mzndef:`"constraint"`.
  A double quote terminal is written as a sequence of three double quotes: :mzndef:`"""`.
- Optional items are written in square brackets,
  e.g. :mzndef:`[ "var" ]`.
- Sequences of zero or more items are written with parentheses and a
  star, e.g. :mzndef:`( "," <ident> )*`.
- Sequences of one or more items are written with parentheses and a
  plus, e.g. :mzndef:`( <msg> )+`.
- Non-empty lists are written with an item, a separator/terminator
  terminal, and three dots.  For example, this:

  .. code-block:: minizincdef
    
    <expr> "," ...

  is short for this:

  .. code-block:: minizincdef
    
    <expr> ( "," <expr> )* [ "," ]

  The final terminal is always optional in non-empty lists.
- Regular expressions are used in some
  productions, e.g. :mzndef:`[-+]?[0-9]+`.

MiniZinc's grammar is presented piece-by-piece throughout this document.  It is
also available as a whole in :ref:`spec-Grammar`.
The output grammar also includes some details of the use of whitespace.
The following conventions are used:

- A newline character or CRLF sequence is written ``\n``.

.. - A sequence of space characters of length :math:`n` is written ``nSP``, e.g., ``2SP``.

.. _spec-Overview:

Overview of a Model
-------------------

Conceptually, a MiniZinc problem specification has two parts.

1. The *model*: the main part of the problem specification, which
   describes the structure of a particular class of problems.
2. The *data*: the input data for the model, which specifies one
   particular problem within this class of problems.

The pairing of a model with a particular data set is a *model
instance* (sometimes abbreviated to *instance*).

The model and data may be separated, or the data
may be "hard-wired" into the model.
:ref:`spec-Model-Instance-Files` specifies how the model and data can be
structured within files in a model instance.

There are two broad classes of problems: satisfaction and optimisation.
In satisfaction problems all solutions are considered equally good,
whereas in optimisation problems the solutions are
ordered according to an objective and
the aim is to find a solution whose objective is optimal.
:ref:`spec-Solve-Items` specifies how the class of problem is chosen.

Evaluation Phases
~~~~~~~~~~~~~~~~~

A MiniZinc model instance is evaluated in two distinct phases.

1. Instance-time: static checking of the model instance.
2. Run-time: evaluation of the instance (i.e., constraint solving).

The model instance may not compile due to a problem with the model and/or data,
detected at instance-time.
This could be caused by a syntax error, a type-inst error,
the use of an unsupported feature or operation, etc.
In this case the outcome of evaluation is a static error;
this must be reported prior to run-time.
The form of output for static errors is implementation-dependent,
although such output should be easily recognisable as a static error.

An implementation may produce warnings during all evaluation phases.
For example, an implementation may be able to determine that
unsatisfiable constraints exist prior to run-time,
and the resulting warning given to the user may be more helpful than
if the unsatisfiability is detected at run-time.

An implementation must produce a warning
if the objective for an optimisation problem is unbounded.


.. _spec-run-time-outcomes:

Run-time Outcomes
~~~~~~~~~~~~~~~~~

Assuming there are no static errors,
the output from the run-time phase has the following abstract form:

.. literalinclude:: output.mzn
  :language: minizincdef
  :start-after: % Output
  :end-before: %

If a solution occurs in the output
then it must be feasible.
For optimisation problems,
each solution must be strictly better than any preceding solution.

If there are no solutions in the output,
the outcome must indicate that there are no solutions.

If the search is complete the output may state this after the solutions.
The absence of the completness message indicates that the search is incomplete.

Any warnings produced during run-time must be summarised
after the statement of completeness.
In particular, if there were any warnings at all during run-time
then the summary must indicate this fact.

The implementation may produce text in any format after the warnings.
For example, it may print
a summary of benchmarking statistics or resources used.

.. _spec-output:

Output
~~~~~~

Implementations must be capable of producing output of content type
``application/x-zinc-output``,
which is described below and also in :ref:`spec-Content-types`.
Implementations may also produce output in alternative formats.
Any output should conform to
the abstract format from the previous section
and must have the semantics described there.

Content type ``application/x-zinc-output`` extends
the syntax from the previous section as follows:

.. literalinclude:: output.mzn
  :language: minizincdef
  :start-after: % Solutions
  :end-before: %

The solution text for each solution must be
as described in :ref:`spec-Output-Items`.
A newline must be appended if the solution text does not end with a newline.
*Rationale: This allows solutions to be extracted from output
without necessarily knowing how the solutions are formatted.*
Solutions end with a sequence of ten dashes followed by a newline.

.. literalinclude:: output.mzn
  :language: minizincdef
  :start-after: % Unsatisfiable
  :end-before: %

The completness result is printed on a separate line.
*Rationale: The strings are designed to clearly indicate
the end of the solutions.*

.. literalinclude:: output.mzn
  :language: minizincdef
  :start-after: % Complete
  :end-before: %

If the search is complete, a statement corresponding to the outcome is printed.
For an outcome of no solutions
the statement is that the model instance is unsatisfiable,
for an outcome of no more solutions
the statement is that the solution set is complete,
and for an outcome of no better solutions
the statement is that the last solution is optimal.
*Rationale: These are the logical implications of a search being complete.*

.. literalinclude:: output.mzn
  :language: minizincdef
  :start-after: % Messages
  :end-before: %

If the search is incomplete,
one or more messages describing reasons for incompleteness may be printed.
Likewise, if any warnings occurred during search
they are repeated after the completeness message.
Both kinds of message should have lines that start with ``%``
so they are recognized as comments by post-processing.
*Rationale: This allows individual messages to be easily recognised.*

For example, the following may be output for an optimisation problem:

.. code-block:: bash

    =====UNSATISFIABLE=====
    % trentin.fzn:4: warning: model inconsistency detected before search.

Note that, as in this case,
an unbounded objective is not regarded as a source of incompleteness.

.. _spec-syntax-overview:

Syntax Overview
---------------

Character Set
~~~~~~~~~~~~~

The input files to MiniZinc must be encoded as UTF-8.

MiniZinc is case sensitive.  There are no places where upper-case or
lower-case letters must be used.  

MiniZinc has no layout restrictions, i.e., any single piece of whitespace
(containing spaces, tabs and newlines) is equivalent to any other.


Comments
~~~~~~~~

A ``%`` indicates that the rest of the line is a comment.  MiniZinc
also has block comments, using symbols ``/*`` and ``*/``
to mark the beginning and end of a comment.

.. _spec-identifiers:

Identifiers
~~~~~~~~~~~

Identifiers have the following syntax:

.. code-block:: minizincdef

  <ident> ::= [A-Za-z][A-Za-z0-9_]*       % excluding keywords
            | "'" [^'\xa\xd\x0]* "'"

.. code-block:: minizinc

  my_name_2
  MyName2
  'An arbitrary identifier'

A number of keywords are reserved and cannot be used as identifiers.  The
keywords are:
:mzn:`ann`, 
:mzn:`annotation`, 
:mzn:`any`, 
:mzn:`array`, 
:mzn:`bool`, 
:mzn:`case`,
:mzn:`constraint`, 
:mzn:`diff`,
:mzn:`div`,
:mzn:`else`,
:mzn:`elseif`, 
:mzn:`endif`, 
:mzn:`enum`, 
:mzn:`false`, 
:mzn:`float`,
:mzn:`function`,
:mzn:`if`,
:mzn:`in`,
:mzn:`include`,
:mzn:`int`,
:mzn:`intersect`,
:mzn:`let`,
:mzn:`list`,
:mzn:`maximize`,
:mzn:`minimize`,
:mzn:`mod`,
:mzn:`not`,
:mzn:`of`,
:mzn:`op`,
:mzn:`opt`,
:mzn:`output`,
:mzn:`par`,
:mzn:`predicate`,
:mzn:`record`,
:mzn:`satisfy`,
:mzn:`set`,
:mzn:`solve`,
:mzn:`string`,
:mzn:`subset`,
:mzn:`superset`,
:mzn:`symdiff`,
:mzn:`test`,
:mzn:`then`,
:mzn:`true`,
:mzn:`tuple`,
:mzn:`type`,
:mzn:`union`,
:mzn:`var`,
:mzn:`where`,
:mzn:`xor`.

A number of identifiers are used for built-ins;  see :ref:`spec-builtins`
for details.

.. _spec-High-level-Model-Structure:

High-level Model Structure
--------------------------

A MiniZinc model consists of multiple *items*:

.. literalinclude:: grammar.mzn
  :language: minizincdef
  :start-after: % A MiniZinc model
  :end-before: %

Items can occur in any order; identifiers need not be declared before they are used. Items have the following top-level syntax:

.. literalinclude:: grammar.mzn
  :language: minizincdef
  :start-after: % Items
  :end-before: %

Include items provide a way of combining multiple files into a single
instance.  This allows a model to be split into multiple files
(:ref:`spec-Include-Items`).

Variable declaration items introduce new global variables and possibly
bind them to a value (:ref:`spec-Declarations`).

Assignment items bind values to global variables
(:ref:`spec-Assignments`).

Constraint items describe model constraints (:ref:`spec-Constraint-Items`).

Solve items are the "starting point" of a model, and specify exactly
what kind of solution is being looked for:  plain satisfaction, or the
minimization/maximization of an expression.  Each model must have exactly
one solve item (:ref:`spec-Solve-Items`).

Output items are used for nicely presenting the result of a model
execution (:ref:`spec-Output-Items`).

Predicate items, test items (which are just a special type of predicate)
and function items introduce new user-defined predicates and
functions which can be called in expressions (:ref:`spec-preds-and-fns`).
Predicates, functions, and built-in operators are described collectively as
*operations*.

Annotation items augment the :mzn:`ann` type, values of which can specify
non-declarative and/or solver-specific information in a model.

.. _spec-model-instance-files:

Model Instance Files
~~~~~~~~~~~~~~~~~~~~

MiniZinc models can be constructed from multiple files using
include items (see :ref:`spec-Include-Items`).  MiniZinc has no
module system as such;  all the included files are simply concatenated and
processed as a whole, exactly as if they had all been part of a single file.
*Rationale: We have not found much need for one so far.  If bigger models
become common and the single global namespace becomes a problem, this should
be reconsidered.*

Each model may be paired with one or more data files.  Data files are more
restricted than model files.  They may only contain variable assignments (see
:ref:`spec-Assignments`).

Data files may not include calls to user-defined operations.

Models do not contain the names of data files; doing so would fix the data
file used by the model and defeat the purpose of allowing separate data
files.  Instead, an implementation must allow one or more data files to be
combined with a model for evaluation via a mechanism such as the
command-line.

When checking a model with
data, all global variables with fixed type-insts must be assigned, unless
they are not used (in which case they can be removed from the model without
effect).

A data file can only be checked for static errors in conjunction with a
model, since the model contains the declarations that include the types of
the variables assigned in the data file.

A single data file may be shared between multiple models, so long as the
definitions are compatible with all the models.

.. _spec-namespaces:

Namespaces
~~~~~~~~~~

All names declared at the top-level belong to a single namespace.
It includes the following names.

1. All global variable names.
2. All function and predicate names, both built-in and user-defined.
3. All enumerated type names and enum case names.
4. All annotation names.

Because multi-file MiniZinc models are composed via
concatenation (:ref:`spec-Model-Instance-Files`), all files share
this top-level namespace.  Therefore a variable ``x`` declared in one
model file could not be declared with a different type in a different file,
for example.

MiniZinc supports overloading of built-in and user-defined operations.

.. _spec-scopes:

Scopes
~~~~~~

Within the top-level namespace, there are several kinds of local scope that
introduce local names:

- Comprehension expressions (:ref:`spec-Set-Comprehensions`).
- Let expressions (:ref:`spec-Let-Expressions`).
- Function and predicate argument lists and bodies (:ref:`spec-preds-and-fns`).

The listed sections specify these scopes in more detail.  In each case, any
names declared in the local scope overshadow identical global names.

.. _spec-types:

Types and Type-insts
--------------------

MiniZinc provides four scalar built-in types:  Booleans, integers, floats, and
strings; enumerated types; two compound built-in types:  sets and multi-dimensional arrays;
and the user extensible annotation type :mzn:`ann`.

Each type has one or more possible *instantiations*.  The
instantiation of a variable or value indicates if it is fixed to a known
value or not.  A pairing of a type and instantiation is called a
*type-inst*.

We begin by discussing some properties that apply to every type.  We then
introduce instantiations in more detail.  We then cover each type
individually, giving:  an overview of the type and its possible
instantiations, the syntax for its type-insts, whether it is a finite
type (and if so, its domain), whether it is varifiable, the ordering and
equality operations, whether its variables must be initialised at
instance-time, and whether it can be involved in automatic coercions.  

Properties of Types
~~~~~~~~~~~~~~~~~~~

The following list introduces some general properties of MiniZinc types.

- Currently all types are monotypes. In the future we may allow types which
  are polymorphic in other types and also the associated constraints.
- We distinguish types which are *finite types*. In MiniZinc, finite types
  include Booleans, enums, types defined via set expression type-insts such
  as range types (see :ref:`spec-Set-Expression-Type-insts`), as well as
  sets and arrays, composed of finite types. Types that are not finite types
  are unconstrained integers, unconstrained floats, unconstrained strings,
  and :mzn:`ann`. Finite types are relevant to sets (:mzn:`spec-Sets`) and
  array indices (:mzn:`spec-Arrays`). Every finite type has a *domain*,
  which is a set value that holds all the possible values represented by the
  type.
- Every first-order type (this excludes :mzn:`ann`) has a built-in total
  order and a built-in equality; ``>``, ``<``, ``==``/``=``, ``!=``, ``<=``
  and ``>=`` comparison operators can be applied to any pair of values of
  the same type. *Rationale: This facilitates the specification of symmetry
  breaking and of polymorphic predicates and functions.* Note that, as in
  most languages, using equality on floats or types that contain floats is
  generally not reliable due to their inexact representation. An
  implementation may choose to warn about the use of equality with floats or
  types that contain floats.

.. _spec-instantiations:

Instantiations
~~~~~~~~~~~~~~

When a MiniZinc model is evaluated, the value of each variable may initially
be unknown.  As it runs, each variable's *domain* (the set of
values it may take) may be reduced, possibly to a single value.

An *instantiation* (sometimes abbreviated to *inst*) describes
how fixed or unfixed a variable is at instance-time.  At the most basic
level, the instantiation system distinguishes between two kinds of
variables:  

#. *Parameters*, whose values are fixed at instance-time (usually written just as "fixed").
#. *Decision variables* (often abbreviated to *variables*), whose values may
   be completely unfixed at instance-time, but may become fixed at run-time
   (indeed, the fixing of decision variables is the whole aim of constraint
   solving).

In MiniZinc decision variables can have the following types: Booleans,
integers, floats, and sets of integers, and enums.
Arrays and :mzn:`ann` can contain decision variables.

.. _spec-type-inst:

Type-insts
~~~~~~~~~~

Because each variable has both a type and an inst, they are often
combined into a single *type-inst*.  Type-insts are primarily what
we deal with when writing models, rather than types.

A variable's type-inst *never changes*.  This means a decision
variable whose value becomes fixed during model evaluation still has its
original type-inst (e.g. :mzn:`var int`), because that was its
type-inst at instance-time.

Some type-insts can be automatically coerced to another type-inst.  For
example, if a :mzn:`par int` value is used in a context where a
:mzn:`var int` is expected, it is automatically coerced to a
:mzn:`var int`.  We write this :mzn:`par int` |coerce| :mzn:`var int`.
Also, any type-inst can be considered coercible to itself. 
MiniZinc allows coercions between some types as well.

Some type-insts can be *varified*, i.e., made unfixed at the top-level.
For example, :mzn:`par int` is varified to :mzn:`var int`.  We write this
:mzn:`par int` |varify| :mzn:`var int`.

Type-insts that are varifiable include the type-insts of the types that can
be decision variables (Booleans, integers, floats, sets, enumerated types).
Varification is relevant to type-inst synonyms and 
array accesses.

Type-inst expression overview
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

This section partly describes how to write type-insts in MiniZinc models.
Further details are given for each type as they are described in the
following sections.

A type-inst expression specifies a type-inst.
Type-inst expressions may include type-inst constraints.
Type-inst expressions appear in variable declarations
(:ref:`spec-Declarations`) and user-defined operation items
(:ref:`spec-preds-and-fns`)..

Type-inst expressions have this syntax:

.. literalinclude:: grammar.mzn
  :language: minizincdef
  :start-after: % Type-inst expressions
  :end-before: %

(The final alternative, for range types, uses the numeric-specific
:mzndef:`<num-expr>` non-terminal, defined in :ref:`spec-Expressions-Overview`,
rather than the :mzndef:`<expr>` non-terminal.  If this were not the case, the rule
would never match because the ``..`` operator would always be matched
by the first :mzndef:`<expr>`.)

This fully covers the type-inst expressions for scalar types.  The compound
type-inst expression syntax is covered in more detail in
:ref:`spec-Built-in-Compound-Types`.  

The :mzn:`par` and :mzn:`var` keywords (or lack of them) determine the
instantiation.  The :mzn:`par` annotation can be omitted;  the following
two type-inst expressions are equivalent:

.. code-block:: minizinc

    par int
    int

*Rationale: The use of the explicit* :mzn:`var` *keyword allows an
implementation to check that all parameters are initialised in the model or
the instance.  It also clearly documents which variables are parameters, and
allows more precise type-inst checking.*

A type-inst is fixed if it does not contain :mzn:`var`,
with the exception of :mzn:`ann`.

Note that several type-inst expressions that are syntactically expressible
represent illegal type-insts.  For example, although the grammar allows
:mzn:`var` in front of all these base type-inst expression tails, it is a
type-inst error to have :mzn:`var` in the front of a string or array
expression.

.. _spec-built-in-scalar-types:

Built-in Scalar Types and Type-insts
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Booleans
++++++++

|TyOverview|
Booleans represent truthhood or falsity.  *Rationale: Boolean values are
not represented by integers.
Booleans can be explicit converted to
integers with the* :mzn:`bool2int` *function, which makes the user's intent
clear.*

|TyInsts|
Booleans can be fixed or unfixed.

|TySyntax|
Fixed Booleans are written :mzn:`bool` or :mzn:`par bool`.  Unfixed
Booleans are written as :mzn:`var bool`.

|TyFiniteType|
Yes.  The domain of a Boolean is :mzn:`false, true`.

|TyVarifiable|
:mzn:`par bool` |varify| :mzn:`var bool`, :mzn:`var bool` |varify| :mzn:`var bool`.

|TyOrdering|
The value :mzn:`false` is considered smaller than :mzn:`true`.

|TyInit|
A fixed Boolean variable must be initialised at instance-time;  an unfixed
Boolean variable need not be.

|TyCoercions|
:mzn:`par bool` |coerce| :mzn:`var bool`.

Also Booleans can be automatically coerced to integers; see
:ref:`spec-Integers`.

.. _spec-integers:

Integers
++++++++

|TyOverview|
Integers represent integral numbers.  Integer representations are
implementation-defined.  This means that the representable range of
integers is implementation-defined.  However, an implementation should
abort at run-time if an integer operation overflows.

|TyInsts|
Integers can be fixed or unfixed.

|TySyntax|
Fixed integers are written :mzn:`int` or :mzn:`par int`.  Unfixed
integers are written as :mzn:`var int`.

|TyFiniteType|
Not unless constrained by a set expression (see :ref:`spec-Set-Expression-Type-insts`).

|TyVarifiable|
:mzn:`par int` |varify| :mzn:`var int`,
:mzn:`var int` |varify| :mzn:`var int`.

|TyOrdering|
The ordering on integers is the standard one.

|TyInit|
A fixed integer variable must be initialised at instance-time;  an unfixed
integer variable need not be.

|TyCoercions|
:mzn:`par int` |coerce| :mzn:`var int`,
:mzn:`par bool` |coerce| :mzn:`par int`,
:mzn:`par bool` |coerce| :mzn:`var int`,
:mzn:`var bool` |coerce| :mzn:`var int`.

Also, integers can be automatically coerced to floats;  see
:ref:`spec-Floats`.


.. _spec-floats:

Floats
++++++

|TyOverview|
Floats represent real numbers.  Float representations are
implementation-defined.  This means that the representable range and
precision of floats is implementation-defined.  However, an
implementation should abort at run-time on exceptional float operations
(e.g., those that produce ``NaN``, if using IEEE754 floats).

|TyInsts|
Floats can be fixed or unfixed.

|TySyntax|
Fixed floats are written :mzn:`float` or :mzn:`par float`.  Unfixed
floats are written as :mzn:`var float`.

|TyFiniteType|
Not unless constrained by a set expression (see :ref:`spec-Set-Expression-Type-insts`).

|TyVarifiable|
:mzn:`par float` |varify| :mzn:`var float`,
:mzn:`var float` |varify| :mzn:`var float`.

|TyOrdering|
The ordering on floats is the standard one.

|TyInit|
A fixed float variable must be initialised at instance-time;  an unfixed
float variable need not be.

|TyCoercions|
:mzn:`par int` |coerce| :mzn:`par float`,
:mzn:`par int` |coerce| :mzn:`var float`,
:mzn:`var int` |coerce| :mzn:`var float`,
:mzn:`par float` |coerce| :mzn:`var float`.

.. _spec-enumerated-types:

Enumerated Types
++++++++++++++++

|TyOverview|
Enumerated types (or *enums* for short) provide a set of named
alternatives. Each alternative is identified by its *case name*.
Enumerated types, like in many other languages, can be used in the place of
integer types to achieve stricter type checking.

|TyInsts|
Enums can be fixed or unfixed.

|TySyntax|
Variables of an enumerated type named ``X`` are represented by the term
:mzn:`X` or :mzn:`par X` if fixed, and :mzn:`var X`
if unfixed.

|TyFiniteType|
Yes.

The domain of an enum is the set containing all of its case names.

|TyVarifiable|
:mzn:`par X` |varify| :mzn:`var X`,
:mzn:`var X` |varify| :mzn:`var X`.

|TyOrdering|
When two enum values with different case names are compared, the value with
the case name that is declared first is considered smaller than the value
with the case name that is declared second.

|TyInit|
A fixed enum variable must be initialised at instance-time; an unfixed
enum variable need not be.

|TyCoercions|
:mzn:`par X` |coerce| :mzn:`par int`,
:mzn:`var X` |coerce| :mzn:`var int`.

.. _spec-strings:

Strings
+++++++

|TyOverview|
Strings are primitive, i.e., they are not lists of characters.

String expressions are used in assertions, output items and
annotations, and string literals are used in include items.

|TyInsts|
Strings must be fixed.

|TySyntax|
Fixed strings are written :mzn:`string` or :mzn:`par string`.

|TyFiniteType|
Not unless constrained by a set expression (see :ref:`spec-Set-Expression-Type-insts`).

|TyVarifiable|
No.

|TyOrdering|
Strings are ordered lexicographically using the underlying character codes.

|TyInit|
A string variable (which can only be fixed) must be initialised at
instance-time.

|TyCoercions|
None automatic.  However, any non-string value can be manually converted to
a string using the built-in :mzn:`show` function or using string interpolation
(see :ref:`spec-String-Interpolation-Expressions`).

.. _spec-Built-in-Compound-Types:

Built-in Compound Types and Type-insts
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. _spec-sets:

Sets
++++

|TyOverview|
A set is a collection with no duplicates.

|TyInsts|
The type-inst of a set's elements must be fixed.  *Rationale: This is because
current solvers are not powerful enough to handle sets containing decision
variables.*
Sets may contain any type, and may be fixed or unfixed.
If a set is unfixed, its elements must be finite, unless it occurs in one
of the following contexts:

- the argument of a predicate, function or annotation.
- the declaration of a variable or let local variable with an
  assigned value.

|TySyntax|
A set base type-inst expression tail has this syntax:

.. literalinclude:: grammar.mzn
  :language: minizincdef
  :start-after: % Set type-inst expressions
  :end-before: %

Some example set type-inst expressions:

.. code-block:: minizinc

  set of int
  var set of bool

|TyFiniteType|
Yes, if the set elements are finite types.  Otherwise, no.

The domain of a set type that is a finite type is the powerset of the domain
of its element type.  For example, the domain of :mzn:`set of 1..2` is
:mzn:`powerset(1..2)`, which is :mzn:`{}, {1}, {1,2}, {2}`.

|TyVarifiable|
:mzn:`par set of TI` |varify| :mzn:`var set of TI`,
:mzn:`var set of TI` |varify| :mzn:`var set of TI`.

|TyOrdering|
The pre-defined ordering on sets is a lexicographic ordering of the
*sorted set form*, where :mzn:`{1,2}` is in sorted set form, for example,
but :mzn:`{2,1}` is not.
This means, for instance, :mzn:`{} < {1,3} < {2}`.

|TyInit|
A fixed set variable must be initialised at instance-time;  an unfixed
set variable need not be.

|TyCoercions|
:mzn:`par set of TI` |coerce| :mzn:`par set of UI` and
:mzn:`par set of TI` |coerce| :mzn:`var set of UI` and
:mzn:`var set of TI` |coerce| :mzn:`var set of UI`, if
:mzn:`TI` |coerce| :mzn:`UI`.

Arrays
++++++

|TyOverview|
MiniZinc arrays are maps from fixed integers to values.  
Values can be of any type.  
The values can only have base type-insts.  
Arrays-of-arrays are not allowed.
Arrays can be multi-dimensional.

MiniZinc arrays can be declared in two different ways.

- *Explicitly-indexed* arrays have index types in the declaration
  that are finite types.  For example:

  .. code-block:: minizinc

    array[0..3] of int: a1;
    array[1..5, 1..10] of var float: a5;

  For such arrays, the index type specifies exactly the indices that will
  be in the array - the array's index set is the *domain* of the
  index type - and if the indices of the value assigned do not match then
  it is a run-time error.

  For example, the following assignments cause run-time errors:

  .. code-block:: minizinc

    a1 = [4,6,4,3,2];   % too many elements
    a5 = [];            % too few elements
- *Implicitly-indexed* arrays have index types in the declaration
  that are not finite types.  For example:

  .. code-block:: minizinc

    array[int,int] of int: a6;

  No checking of indices occurs when these variables are assigned.

In MiniZinc all index sets of an array must be contiguous ranges of
integers, or enumerated types. The expression used for initialisation of an
array must have matching index sets. An array expression with an enum index
set can be assigned to an array declared with an integer index set, but not
the other way around. The exception are array literals, which can be
assigned to arrays declared with enum index sets.

For example:

.. code-block:: minizinc

  enum X = {A,B,C};
  enum Y = {D,E,F};
  array[X] of int: x = array1d(X, [5,6,7]); % correct
  array[Y] of int: y = x;                   % index set mismatch: Y != X
  array[int] of int: z = x;                 % correct: assign X index set to int
  array[X] of int: x2 = [10,11,12];         % correct: automatic coercion for array literals

The initialisation of an array can be done in a separate assignment
statement, which may be present in the model or a separate data file.

Arrays can be accessed.  See :ref:`spec-Array-Access-Expressions` for
details.

|TyInsts|
An array's size must be fixed.  Its indices must also have
fixed type-insts.  Its elements may be fixed or unfixed.

|TySyntax|
An array base type-inst expression tail has this syntax:

.. literalinclude:: grammar.mzn
  :language: minizincdef
  :start-after: % Array type-inst expressions
  :end-before: %

Some example array type-inst expressions:

.. code-block:: minizinc

  array[1..10] of int
  list of var int

Note that :mzndef:`list of <T>` is just syntactic sugar for
:mzndef:`array[int] of <T>`.  *Rationale: Integer-indexed arrays of this form
are very common, and so worthy of special support to make things easier for
modellers.  Implementing it using syntactic sugar avoids adding an extra
type to the language, which keeps things simple for implementers.*

Because arrays must be fixed-size it is a type-inst error to precede an
array type-inst expression with :mzn:`var`.

|TyFiniteType|
Yes, if the index types and element type are all finite types.
Otherwise, no.

The domain of an array type that is a finite array is the set of all
distinct arrays whose index set equals the domain of the index type
and whose elements are of the array element type.

|TyVarifiable|
No.

|TyOrdering|
Arrays are ordered lexicographically, taking absence of a value for a given key
to be before any value for that key.  For example,
:mzn:`[1, 1]` is less than
:mzn:`[1, 2]`, which is less than :mzn:`[1, 2, 3]` and
:mzn:`array1d(2..4,[0, 0, 0])` is less than :mzn:`[1, 2, 3]`.

|TyInit|
An explicitly-indexed array variable must be initialised at instance-time
only if its elements must be initialised at instance time.
An implicitly-indexed array variable must be initialised at instance-time
so that its length and index set is known.

|TyCoercions|
:mzn:`array[TI0] of TI` |coerce| :mzn:`array[UI0] of UI` if
:mzn:`TI0` |coerce| :mzn:`UI0` and :mzn:`TI` |coerce| :mzn:`UI`.

.. _spec-option-types:

Option Types
++++++++++++

|TyOverview|
Option types defined using the :mzn:`opt` type constructor, define types
that may or may not be there. They are similar to ``Maybe`` types of
Haskell implicity adding a new value :mzn:`<>` to the type.


|TyInsts|
The argument of an option type must be one of the base types
:mzn:`bool`, :mzn:`int` or :mzn:`float`.

|TySyntax|
The option type is written :mzndef:`opt <T>` where :mzndef:`<T>` if one of
the three base types, or one of their constrained instances.

|TyFiniteType|
Yes if the underlying type is finite, otherwise no.

|TyVarifiable|
Yes.

|TyOrdering|
:mzn:`<>` is always less than any other value in the type.
But beware that overloading of operators like :mzn:`<` is different for
option types.

|TyInit|
An :mzn:`opt` type variable does not need to be initialised at
instance-time. An uninitialised :mzn:`opt` type variable is automatically
initialised to :mzn:`<>`.

|TyCoercions|
:mzn:`TI` |coerce| :mzn:`opt UI` if :mzn:`TI` |coerce| :mzn:`UI`..

.. _spec-the-annotation-type:

The Annotation Type
+++++++++++++++++++

|TyOverview|
The annotation type, :mzn:`ann`, can be used to represent arbitrary term
structures.  It is augmented by annotation items (:ref:`spec-Annotation-Items`).

|TyInsts|
:mzn:`ann` is always considered unfixed, because it may contain unfixed
elements.  It cannot be preceded by :mzn:`var`.

|TySyntax|
The annotation type is written :mzn:`ann`.

|TyFiniteType|
No.

|TyVarifiable|
No.

|TyOrdering|
N/A.  Annotation types do not have an ordering defined on them.

|TyInit|
An :mzn:`ann` variable must be initialised at instance-time.

|TyCoercions|
None.


.. _spec-constrained-type-insts:

Constrained Type-insts
~~~~~~~~~~~~~~~~~~~~~~

One powerful feature of MiniZinc is *constrained type-insts*.  A
constrained type-inst is a restricted version of a *base* type-inst,
i.e., a type-inst with fewer values in its domain.

.. _spec-set-expression-type-insts:

Set Expression Type-insts
+++++++++++++++++++++++++

Three kinds of expressions can be used in type-insts.

#. Integer ranges:  e.g. :mzn:`1..3`.
#. Set literals:  e.g. :mzn:`var {1,3,5}`.
#. Identifiers:  the name of a set parameter (which can be global,
   let-local, the argument of a predicate or function, or a generator
   value) can serve as a type-inst.

In each case the base type is that of the set's elements, and the values
within the set serve as the domain.  For example, whereas a variable with
type-inst :mzn:`var int` can take any integer value, a variable with
type-inst :mzn:`var 1..3` can only take the value 1, 2 or 3.

All set expression type-insts are finite types.  Their domain is equal to
the set itself.

Float Range Type-insts
++++++++++++++++++++++

Float ranges can be used as type-insts, e.g. :mzn:`1.0 .. 3.0`.  These are
treated similarly to integer range type-insts, although :mzn:`1.0 .. 3.0` is
not a valid expression whereas :mzn:`1 .. 3` is.

Float ranges are not finite types.

.. _spec-expressions:

Expressions
-----------

.. _spec-expressions-overview:

Expressions Overview
~~~~~~~~~~~~~~~~~~~~

Expressions represent values.  They occur in various kinds of items.  They
have the following syntax:

.. literalinclude:: grammar.mzn
  :language: minizincdef
  :start-after: % Expressions
  :end-before: %

Expressions can be composed from sub-expressions combined with operators.
All operators (binary and unary) are described in :ref:`spec-Operators`,
including the precedences of the binary operators.  All unary operators bind
more tightly than all binary operators.

Expressions can have one or more annotations.  Annotations bind
more tightly than unary and binary operator applications, but less tightly
than access operations and non-operator applications.  In some cases this
binding is non-intuitive.  For example, in the first three of the following
lines, the annotation :mzn:`a` binds to the identifier expression
:mzn:`x` rather than the operator application.  However, the fourth
line features a non-operator application (due to the single quotes around
the :mzn:`not`) and so the annotation binds to the whole application.

.. code-block:: minizinc

  not x::a;
  not (x)::a;
  not(x)::a;
  'not'(x)::a;

:ref:`spec-Annotations` has more on annotations.

Expressions can be contained within parentheses.

The array access operations
all bind more tightly than unary and binary operators and annotations.  
They are described in more detail in :ref:`spec-Array-Access-Expressions`.

The remaining kinds of expression atoms (from :mzndef:`<ident>` to
:mzndef:`<gen-call-expr>`) are described in
:ref:`spec-Identifier-Expressions-and-Quoted-Operator-Expressions` to :ref:`spec-Generator-Call-Expressions`.

We also distinguish syntactically valid numeric expressions.  This allows
range types to be parsed correctly.

.. literalinclude:: grammar.mzn
  :language: minizincdef
  :start-after: % Numeric expressions
  :end-before: %

.. _spec-operators:

Operators
~~~~~~~~~

Operators are functions that are distinguished by their syntax in one or two
ways.  First, some of them contain non-alphanumeric characters that normal
functions do not (e.g. :mzn:`+`).  Second, their application is written
in a manner different to normal functions.

We distinguish between binary operators, which can be applied in an infix
manner (e.g. :mzn:`3 + 4`), and unary operators, which can be applied in a
prefix manner without parentheses (e.g. :mzn:`not x`).  We also
distinguish between built-in operators and user-defined operators.  The
syntax is the following:

.. literalinclude:: grammar.mzn
  :language: minizincdef
  :start-after: % Built-in operators
  :end-before: %

Again, we syntactically distinguish numeric operators.

.. literalinclude:: grammar.mzn
  :language: minizincdef
  :start-after: % Built-in numeric operators
  :end-before: %

Some operators can be written using their unicode symbols, which are listed
in :numref:`bin-ops-unicode` (recall that MiniZinc input is UTF-8).

.. _bin-ops-unicode:

.. cssclass:: table-nonfluid table-bordered

.. table:: Unicode equivalents of binary operators

  ================  =======================  ==========
  Operator          Unicode symbol           UTF-8 code
  ================  =======================  ==========
  :mzn:`<->`        :math:`\leftrightarrow`  E2 86 94
  :mzn:`->`         :math:`\rightarrow`      E2 86 92
  :mzn:`<-`         :math:`\leftarrow`       E2 86 90
  :mzn:`not`        :math:`\lnot`            C2 AC
  ``\/``            :math:`\lor`             E2 88 A8
  ``/\``            :math:`\land`            E2 88 A7
  :mzn:`!=`         :math:`\neq`             E2 89 A0
  :mzn:`<=`         :math:`\leq`             E2 89 A4
  :mzn:`>=`         :math:`\geq`             E2 89 A5
  :mzn:`in`         :math:`\in`              E2 88 88
  :mzn:`subset`     :math:`\subseteq`        E2 8A 86
  :mzn:`superset`   :math:`\supseteq`        E2 8A 87
  :mzn:`union`      :math:`\cup`             E2 88 AA
  :mzn:`intersect`  :math:`\cap`             E2 88 A9
  ================  =======================  ==========

The binary operators are listed in :numref:`bin-ops`. A lower precedence
number means tighter binding; for example, :mzn:`1+2*3` is parsed as
:mzn:`1+(2*3)` because :mzn:`*` binds tighter than :mzn:`+`. Associativity
indicates how chains of operators with equal precedences are handled; for
example, :mzn:`1+2+3` is parsed as :mzn:`(1+2)+3` because :mzn:`+` is
left-associative, :mzn:`a++b++c` is parsed as :mzn:`a++(b++c)` because
:mzn:`++` is right-associative, and :mzn:`1<x<2` is a syntax error because
:mzn:`<` is non-associative.

.. _bin-ops:

.. cssclass:: table-nonfluid table-bordered

.. table:: Binary infix operators

  ===============================  ====== ======
  Symbol(s)                        Assoc. Prec. 
  ===============================  ====== ======
  :mzn:`<->`                       left   1200  

  :mzn:`->`                        left   1100  
  :mzn:`<-`                        left   1100  

  ``\/``                           left   1000  
  :mzn:`xor`                       left   1000  

  ``/\``                           left   900   

  :mzn:`<`                         none   800   
  :mzn:`>`                         none   800   
  :mzn:`<=`                        none   800   
  :mzn:`>=`                        none   800   
  :mzn:`==`,                   
  :mzn:`=`                         none   800   
  :mzn:`!=`                        none   800   
                             
  :mzn:`in`                        none   700   
  :mzn:`subset`                    none   700   
  :mzn:`superset`                  none   700   
                             
  :mzn:`union`                     left   600   
  :mzn:`diff`                      left   600   
  :mzn:`symdiff`                   left   600   
                             
  :mzn:`..`                        none   500   
                             
  :mzn:`+`                         left   400   
  :mzn:`-`                         left   400   
                             
  :mzn:`*`                         left   300   
  :mzn:`div`                       left   300   
  :mzn:`mod`                       left   300   
  :mzn:`/`                         left   300   
  :mzn:`intersect`                 left   300   
                             
  :mzn:`++`                        right  200   

  `````  :mzndef:`<ident>` `````   left   100   
  ===============================  ====== ======


A user-defined binary operator is created by backquoting a normal
identifier, for example:

.. code-block:: minizinc

  A `min2` B

This is a static error if the identifier is not the name of a binary
function or predicate.

The unary operators are: :mzn:`+`, :mzn:`-` and :mzn:`not`.
User-defined unary operators are not possible.

As :ref:`spec-Identifiers` explains, any built-in operator can be used as
a normal function identifier by quoting it, e.g: :mzn:`'+'(3, 4)` is
equivalent to :mzn:`3 + 4`.

The meaning of each operator is given in :ref:`spec-builtins`.

Expression Atoms
~~~~~~~~~~~~~~~~

.. _spec-Identifier-Expressions-and-Quoted-Operator-Expressions:

Identifier Expressions and Quoted Operator Expressions
++++++++++++++++++++++++++++++++++++++++++++++++++++++

Identifier expressions and quoted operator expressions have the following
syntax:

.. literalinclude:: grammar.mzn
  :language: minizincdef
  :start-after: % Identifiers and quoted operators
  :end-before: %

Examples of identifiers were given in :ref:`spec-Identifiers`.  The
following are examples of quoted operators:

.. code-block:: minizinc

  '+'
  'union'

In quoted operators, whitespace is not permitted between either quote
and the operator.  :ref:`spec-Operators` lists MiniZinc's built-in operators.

Syntactically, any identifier or quoted operator can serve as an expression.
However, in a valid model any identifier or quoted operator serving as an
expression must be the name of a variable.

.. _spec-Anonymous-Decision-Variables:

Anonymous Decision Variables
++++++++++++++++++++++++++++

There is a special identifier, :mzn:`_`, that represents an unfixed,
anonymous decision variable.  It can take on any type that can be a decision
variable.  It is particularly useful for initialising decision variables
within compound types.  For example, in the following array the first and
third elements are fixed to 1 and 3 respectively and the second and fourth
elements are unfixed:

.. code-block:: minizinc

  array[1..4] of var int: xs = [1, _, 3, _];

Any expression that does not contain :mzn:`_` and does not involve
decision variables is fixed.

Boolean Literals
++++++++++++++++

Boolean literals have this syntax:

.. literalinclude:: grammar.mzn
  :language: minizincdef
  :start-after: % Boolean literals
  :end-before: %

Integer and Float Literals
++++++++++++++++++++++++++

There are three forms of integer literals - decimal, hexadecimal, and
octal - with these respective forms:

.. literalinclude:: grammar.mzn
  :language: minizincdef
  :start-after: % Integer literals
  :end-before: %

For example: :mzn:`0`, :mzn:`005`, :mzn:`123`, :mzn:`0x1b7`,
:mzn:`0o777`;  but not :mzn:`-1`.

Float literals have the following form:

.. literalinclude:: grammar.mzn
  :language: minizincdef
  :start-after: % Float literals
  :end-before: %

For example: :mzn:`1.05`, :mzn:`1.3e-5`, :mzn:`1.3+e5`;  but not
:mzn:`1.`, :mzn:`.5`, :mzn:`1.e5`, :mzn:`.1e5`, :mzn:`-1.0`,
:mzn:`-1E05`.
A :mzn:`-` symbol preceding an integer or float literal is parsed as a
unary minus (regardless of intervening whitespace), not as part of the
literal.  This is because it is not possible in general to distinguish a
:mzn:`-` for a negative integer or float literal from a binary minus
when lexing.

.. _spec-String-Interpolation-Expressions:

String Literals and String Interpolation
++++++++++++++++++++++++++++++++++++++++

String literals are written as in C:

.. literalinclude:: grammar.mzn
  :language: minizincdef
  :start-after: % String literals
  :end-before: %

This includes C-style escape sequences, such as :mzn:`\"` for
double quotes, :mzn:`\\` for backslash, and
:mzn:`\n` for newline.

For example: :mzn:`"Hello, world!\n"`.

String literals must fit on a single line.  

Long string literals can be split across multiple lines using string
concatenation.  For example:

.. code-block:: minizinc

    string: s = "This is a string literal "
             ++ "split across two lines.";

A string expression can contain an arbitrary MiniZinc expression, which will
be converted to a string similar to the builtin :mzn:`show` function and
inserted into the string.

For example:

.. code-block:: minizinc

  var set of 1..10: q;
  solve satisfy;
  output [show("The value of q is \(q), and it has \(card(q)) elements.")];

.. _spec-set-literals:

Set Literals
++++++++++++

Set literals have this syntax:

.. literalinclude:: grammar.mzn
  :language: minizincdef
  :start-after: % Set literals
  :end-before: %

For example:

.. code-block:: minizinc

  { 1, 3, 5 }
  { }
  { 1, 2.0 }

The type-insts of all elements in a literal set must be the same, or
coercible to the same type-inst (as in the last example above, where the
integer :mzn:`1` will be coerced to a :mzn:`float`).


.. _spec-set-comprehensions:

Set Comprehensions
++++++++++++++++++

Set comprehensions have this syntax:

.. literalinclude:: grammar.mzn
  :language: minizincdef
  :start-after: % Set comprehensions
  :end-before: %

For example (with the literal equivalent on the right):

.. code-block:: minizinc

    { 2*i | i in 1..5 }     % { 2, 4, 6, 8, 10 }
    {  1  | i in 1..5 }     % { 1 }   (no duplicates in sets)

The expression before the :mzn:`|` is the *head expression*.  The
expression after the :mzn:`in` is a *generator expression*.
Generators can be restricted by a *where-expression*.  For example:

.. code-block:: minizinc

  { i | i in 1..10 where (i mod 2 = 0) }     % { 2, 4, 6, 8, 10 }

When multiple generators are present, the right-most generator acts as the
inner-most one.  For example:

.. code-block:: minizinc

    { 3*i+j | i in 0..2, j in {0, 1} }    % { 0, 1, 3, 4, 6, 7 }

The scope of local generator variables is given by the following rules:

- They are visible within the head expression (before the :mzn:`|`).
- They are visible within the where-expression of their own generator.
- They are visible within generator expressions and where-expressions in any subsequent generators.

The last of these rules means that the following set comprehension is allowed:

.. code-block:: minizinc

    { i+j | i in 1..3, j in 1..i }  % { 1+1, 2+1, 2+2, 3+1, 3+2, 3+3 }

Multiple where-expressions are allowed, as in the following example:

.. code-block:: minizinc

    [f(i, j) | i in A1 where p(i), j in A2 where q(i,j)]


A generator expression must be an array or a fixed set.

*Rationale: For set comprehensions, set generators would suffice, but for
array comprehensions, array generators are required for full expressivity
(e.g., to provide control over the order of the elements in the resulting
array).  Set comprehensions have array generators for consistency with array
comprehensions, which makes implementations simpler.*

The where-expression (if present) must be Boolean.  
It can be var, in which case the type of the comprehension is lifted to an optional type.

.. _spec-array-literals:

Array Literals
++++++++++++++

Array literals have this syntax:

.. literalinclude:: grammar.mzn
  :language: minizincdef
  :start-after: % Array literals
  :end-before: %

For example:

.. code-block:: minizinc

    [1, 2, 3, 4]
    []
    [1, _]


In a array literal all elements must have the same type-inst, or
be coercible to the same type-inst (as in the last example above, where the
fixed integer :mzn:`1` will be coerced to a :mzn:`var int`).

The indices of a array literal are implicitly :mzn:`1..n`, where :mzn:`n` is
the length of the literal.

.. _spec-2d-array-literals:

2d Array Literals
+++++++++++++++++

Simple 2d array literals have this syntax:

.. literalinclude:: grammar.mzn
  :language: minizincdef
  :start-after: % 2D Array literals
  :end-before: %

For example:

.. code-block:: minizinc

    [| 1, 2, 3
     | 4, 5, 6
     | 7, 8, 9 |]       % array[1..3, 1..3]
    [| x, y, z |]       % array[1..1, 1..3]
    [| 1 | _ | _ |]     % array[1..3, 1..1]

In a 2d array literal, every sub-array must have the same length.

In a 2d array literal all elements must have the same type-inst, or
be coercible to the same type-inst (as in the last example above, where the
fixed integer :mzn:`1` will be coerced to a :mzn:`var int`).

The indices of a 2d array literal are implicitly :mzn:`(1,1)..(m,n)`,
where :mzn:`m` and :mzn:`n` are determined by the shape of the literal.


.. _spec-array-comprehensions:

Array Comprehensions
++++++++++++++++++++

Array comprehensions have this syntax:

.. literalinclude:: grammar.mzn
  :language: minizincdef
  :start-after: % Array comprehensions
  :end-before: %

For example (with the literal equivalents on the right):

.. code-block:: minizinc

    [2*i | i in 1..5]       % [2, 4, 6, 8, 10]

Array comprehensions have more flexible type and inst requirements than set
comprehensions (see :ref:`spec-Set-Comprehensions`).

Array comprehensions are allowed over a variable set with finite type, the
result is an array of optional type, with length equal to the 
cardinality of the upper bound of the variable set.
For example:

.. code-block:: minizinc

    var set of 1..5: x;
    array[int] of var opt int: y = [ i * i | i in x ];

The length of array will be 5. 

Array comprehensions are allowed where the where-expression is a :mzn:`var bool`.
Again the resulting array is of optional type, and of length
equal to that given by the generator expressions.
For example:

.. code-block:: minizinc

   var int x;
   array[int] of var opt int: y = [ i | i in 1..10 where i != x ];

The length of the array will be 10.

The indices of an evaluated simple array comprehension are implicitly
:mzn:`1..n`, where :mzn:`n` is the length of the evaluated comprehension.

.. _spec-array-access-expressions:

Array Access Expressions
++++++++++++++++++++++++

Array elements are accessed using square brackets after an expression:

.. literalinclude:: grammar.mzn
  :language: minizincdef
  :start-after: % Array access
  :end-before: %

For example:

.. code-block:: minizinc

    int: x = a1[1];

If all the indices used in an array access are fixed, the type-inst of the
result is the same as the element type-inst.  However, if any
indices are not fixed, the type-inst of the result is the varified element
type-inst.  For example, if we have:

.. code-block:: minizinc

   array[1..2] of int: a2 = [1, 2];
   var int: i;

then the type-inst of :mzn:`a2[i]` is :mzn:`var int`.  If the element type-inst
is not varifiable, such an access causes a static error.

Multi dimensional arrays 
are accessed using comma separated indices.

.. code-block:: minizinc

    array[1..3,1..3] of int: a3;
    int: y = a3[1, 2];

Indices must match the index set type of the array. For example, an array
declared with an enum index set can only be accessed using indices from that
enum.

.. code-block:: minizinc

    enum X = {A,B,C};
    array[X] of int: a4 = [1,2,3];
    int: y = a4[1];                  % wrong index type
    int: z = a4[B];                  % correct

Array Slice Expressions
+++++++++++++++++++++++

Arrays can be *sliced* in order to extract individual rows, columns or blocks. The syntax is that of an array access expression (see above), but where one or more of the expressions inside the square brackets are set-valued.

For example, the following extracts row 2 from a two-dimensional array:

.. code-block:: minizinc

    array[1..n,4..8] of int: x;
    array[int] of int: row_2_of_x = x[2,4..8];

Note that the resulting array ``row_2_of_x`` will have index set ``4..8``.

A short-hand for all indices of a particular dimension is to use just dots:

.. code-block:: minizinc

    array[1..n,4..8] of int: x;
    array[int] of int: row_2_of_x = x[2,..];

You can also restrict the index set by giving a sub-set of the original index set as the slice:

.. code-block:: minizinc

    array[1..n,4..8] of int: x;
    array[int] of int: row_2_of_x = x[2,5..6];

The resulting array ``row_2_of_x`` will now have length 2 and index set ``5..6``.

The dots notation also allows for partial bounds, for example:

.. code-block:: minizinc

    array[1..n,4..8] of int: x;
    array[int] of int: row_2_of_x = x[2,..6];

The resulting array will have length 3 and index set ``4..6``. Of course ``6..`` would also be allowed and result in an array with index set ``6..8``.

Annotation Literals
+++++++++++++++++++

Literals of the :mzn:`ann` type have this syntax:

.. literalinclude:: grammar.mzn
  :language: minizincdef
  :start-after: % Annotation literals
  :end-before: %

For example:

.. code-block:: minizinc

    foo
    cons(1, cons(2, cons(3, nil)))

There is no way to inspect or deconstruct annotation literals in a MiniZinc
model;  they are intended to be inspected only by an implementation, e.g., to
direct compilation.

If-then-else Expressions
++++++++++++++++++++++++

MiniZinc provides if-then-else expressions, which provide selection from two
alternatives based on a condition.  They have this syntax:

.. literalinclude:: grammar.mzn
  :language: minizincdef
  :start-after: % If-then-else expressions
  :end-before: %

For example:

.. code-block:: minizinc

    if x <= y then x else y endif
    if x < 0 then -1 elseif x > 0 then 1 else 0 endif

The presence of the :mzn:`endif` avoids possible ambiguity when an
if-then-else expression is part of a larger expression.

The type-inst of the :mzn:`if` expression must be :mzn:`par bool` or
:mzn:`var bool`.
The :mzn:`then` and
:mzn:`else` expressions must have the same type-inst, or be coercible to the
same type-inst, which is also the type-inst of the whole expression.

If the :mzn:`if` expression is :mzn:`var bool` then the type-inst of the
:mzn:`then` and :mzn:`else` expressions must be varifiable.

If the :mzn:`if` expression is :mzn:`par bool` then 
evaluation of if-then-else expressions is lazy: the condition is evaluated,
and then only one of the :mzn:`then` and :mzn:`else` branches are evaluated,
depending on whether the condition succeeded or failed. 
This is not the case if it is :mzn:`var bool`.


.. _spec-let-expressions:

Let Expressions
+++++++++++++++

Let expressions provide a way of introducing local names for one or more
expressions and local constraints
that can be used within another expression.  They are
particularly useful in user-defined operations.

Let expressions have this syntax:

.. literalinclude:: grammar.mzn
  :language: minizincdef
  :start-after: % Let expressions
  :end-before: %

For example:

.. code-block:: minizinc

    let { int: x = 3; int: y = 4; } in x + y;
    let { var int: x; 
          constraint x >= y /\ x >= -y /\ (x = y \/ x = -y); } 
    in x 

The scope of a let local variable covers:

- The type-inst and initialisation expressions of any subsequent variables
  within the let expression (but not the variable's own initialisation
  expression).
- The expression after the :mzn:`in`, which is parsed as greedily as
  possible.

A variable can only be declared once in a let expression.

Thus in the following examples the first is acceptable but the rest are not:

.. code-block:: minizinc

    let { int: x = 3; int: y = x; } in x + y;  % ok
    let { int: y = x; int: x = 3; } in x + y;  % x not visible in y's defn.
    let { int: x = x; } in x;                  % x not visible in x's defn.
    let { int: x = 3; int: x = 4; } in x;      % x declared twice

.. 
  The type-inst expressions can include type-inst variables if the let is
  within a function or predicate body in which the same type-inst variables
  were present in the function or predicate signature.
  TODO: type-inst variables are currently not fully supported

The initialiser for a let local variable can be omitted only if the variable
is a decision variable.  For example:

.. code-block:: minizinc

    let { var int: x; } in ...;    % ok
    let {     int: x; } in ...;    % illegal

The type-inst of the entire let expression is the type-inst of the expression
after the :mzn:`in` keyword.

There is a complication involving let expressions in negative contexts.  A
let expression occurs in a negative context if it occurs in an expression
of the form :mzn:`not X`, :mzn:`X <-> Y}` or in the sub-expression
:mzn:`X` in :mzn:`X -> Y` or :mzn:`Y <- X`, or in a subexpression 
:mzn:`bool2int(X)`.

If a let expression is used in a negative context, then any let-local
decision variables must be defined only in terms of non-local variables and
parameters.  This is because local variables are implicitly existentially
quantified, and if the let expression occurred in a negative context then
the local variables would be effectively universally quantified which is not
supported by MiniZinc.

Constraints in let expressions float to the nearest enclosing Boolean
context.  For example

.. code-block:: minizinc

     constraint b -> x + let { var 0..2: y; constraint y != -1;} in y >= 4;

is equivalent to

.. code-block:: minizinc

     var 0..2: y;
     constraint b -> (x + y >= 4 /\ y != 1);

For backwards compatibility with older versions of MiniZinc, items inside the ``let`` can also be separated by commas instead of semicolons.

Call Expressions
++++++++++++++++

Call expressions are used to call predicates and functions.

Call expressions have this syntax:

.. literalinclude:: grammar.mzn
  :language: minizincdef
  :start-after: % Call expressions
  :end-before: %

For example:

.. code-block:: minizinc

    x = min(3, 5);

The type-insts of the expressions passed as arguments must match the
argument types of the called predicate/function.  The return type of the
predicate/function must also be appropriate for the calling context.

Note that a call to a function or predicate with no arguments is
syntactically indistinguishable from the use of a variable, and so must be
determined during type-inst checking.

Evaluation of the arguments in call expressions is strict: all arguments
are evaluated before the call itself is evaluated.  Note that this includes
Boolean operations such as ``/\``, ``\/``, :mzn:`->` and :mzn:`<-`
which could be lazy in one argument.  The one exception is :mzn:`assert`,
which is lazy in its third argument (:ref:`spec-Other-Operations`).

*Rationale: Boolean operations are strict because: (a) this minimises
exceptional cases;  (b) in an expression like* :mzn:`A -> B` *where*
:mzn:`A` *is not fixed and* :mzn:`B` *causes an abort, the appropriate
behaviour is unclear if laziness is present;  and (c) if a user needs
laziness, an if-then-else can be used.*

The order of argument evaluation is not specified.  *Rationale: Because MiniZinc
is declarative, there is no obvious need to specify an evaluation order, and
leaving it unspecified gives implementors some freedom.*

.. _spec-generator-call-expressions:

Generator Call Expressions
++++++++++++++++++++++++++

MiniZinc has special syntax for certain kinds of call expressions which makes
models much more readable.

Generator call expressions have this syntax:

.. literalinclude:: grammar.mzn
  :language: minizincdef
  :start-after: % Generator call expressions
  :end-before: %

A generator call expression :mzn:`P(Gs)(E)` is equivalent to the call
expression :mzn:`P([E | Gs])`.
For example, the expression:

.. code-block:: minizinc

    forall(i,j in Domain where i<j)
        (noattack(i, j, queens[i], queens[j]));

(in a model specifying the N-queens problem) is equivalent to:

.. code-block:: minizinc

    forall( [ noattack(i, j, queens[i], queens[j])
            | i,j in Domain where i<j ] );

The parentheses around the latter expression are mandatory;  this avoids
possible confusion when the generator call expression is part of a larger
expression.

The identifier must be the name of a unary predicate or function that takes
an array argument.

The generators and where-expression (if present) have the same requirements
as those in array comprehensions (:ref:`spec-Array-Comprehensions`).

.. _spec-items:

Items
-----

This section describes the top-level program items.

.. _spec-include-items:

Include Items
~~~~~~~~~~~~~

Include items allow a model to be split across multiple files.  They have
this syntax:

.. literalinclude:: grammar.mzn
  :language: minizincdef
  :start-after: % Include items
  :end-before: %

For example:

.. code-block:: minizinc

    include "foo.mzn";

includes the file ``foo.mzn``.

Include items are particularly useful for accessing libraries or breaking up
large models into small pieces.  They are not, as :ref:`spec-Model-Instance-Files`
explains, used for specifying data files.

If the given name is not a complete path then the file is searched for in an
implementation-defined set of directories.  The search directories must be
able to be altered with a command line option.

.. _spec-declarations:

Variable Declaration Items
~~~~~~~~~~~~~~~~~~~~~~~~~~

Variable declarations have this syntax:

.. literalinclude:: grammar.mzn
  :language: minizincdef
  :start-after: % Variable declaration items
  :end-before: %

For example:

.. code-block:: minizinc

    int: A = 10;

It is a type-inst error if a variable is declared and/or defined more than
once in a model.

A variable whose declaration does not include an assignment can be
initialised by a separate assignment item (:ref:`spec-Assignments`).  For
example, the above item can be separated into the following two items:

.. code-block:: minizinc

    int: A;
    ...
    A = 10;

All variables that contain a parameter component must be defined at
instance-time.

Variables can have one or more annotations.
:ref:`spec-Annotations` has more on annotations.


.. _spec-enum_items:

Enum Items
~~~~~~~~~~

Enumerated type items have this syntax:

.. literalinclude:: grammar.mzn
  :language: minizincdef
  :start-after: % Enum items
  :end-before: %

An example of an enum:

.. code-block:: minizinc

     enum country = {Australia, Canada, China, England, USA};

Each alternative is called an *enum case*.  The identifier used to name
each case (e.g. :mzn:`Australia`) is
called the *enum case name*.

Because enum case names all reside in the top-level namespace
(:ref:`spec-Namespaces`), case names in different enums must be distinct.

An enum can be declared but not defined, in which case it must be defined
elsewhere within the model, or in a data file.
For example, a model file could contain this:

.. code-block:: minizinc

    enum Workers;
    enum Shifts;

and the data file could contain this:

.. code-block:: minizinc

    Workers = { welder, driller, stamper };
    Shifts  = { idle, day, night };

Sometimes it is useful to be able to refer to one of the enum case names
within the model.  This can be achieved by using a variable.  The model
would read:

.. code-block:: minizinc

    enum Shifts;
    Shifts: idle;            % Variable representing the idle constant.

and the data file:

.. code-block:: minizinc

    enum Shifts = { idle_const, day, night };
    idle = idle_const;      % Assignment to the variable.

Although the constant :mzn:`idle_const` cannot be mentioned in the
model, the variable :mzn:`idle` can be.

All enums must be defined at instance-time.

Enum items can be annotated.
:ref:`spec-Annotations` has more details on annotations.

Each case name can be coerced automatically to the integer corresponding to its index in the type.

.. code-block:: minizinc

  int: oz = Australia;  % oz = 1

For each enumerated type :mzn:`T`, the following functions exist:

.. code-block:: minizinc

  % Return next greater enum value of x in enum type X
  function T: enum_next(set of T: X, T: x);
  function var T: enum_next(set of T: X, var T: x);
  
  % Return next smaller enum value of x in enum type X
  function T: enum_prev(set of T: X, T: x);
  function var T: enum_prev(set of T: X, var T: x);

  % Convert x to enum type X
  function T: to_enum(set of T: X, int: x);
  function var T: to_enum(set of T: X, var int: x);

.. _spec-assignments:

Assignment Items
~~~~~~~~~~~~~~~~

Assignments have this syntax:

.. literalinclude:: grammar.mzn
  :language: minizincdef
  :start-after: % Assign items
  :end-before: %

For example:

.. code-block:: minizinc

    A = 10;

.. % \pjs{Add something about automatic coercion of index sets?}

.. _spec-constraint-items:

Constraint Items
~~~~~~~~~~~~~~~~

Constraint items form the heart of a model.  Any solutions found for a model
will satisfy all of its constraints.

Constraint items have this syntax:

.. literalinclude:: grammar.mzn
  :language: minizincdef
  :start-after: % Constraint items
  :end-before: %

For example:

.. code-block:: minizinc

    constraint a*x < b;

The expression in a constraint item must have type-inst :mzn:`par bool` or
:mzn:`var bool`; note however that constraints with fixed expressions are
not very useful.

.. _spec-solve-items:

Solve Items
~~~~~~~~~~~

Every model must have exactly one or no solve item.  Solve items have the
following syntax:

.. literalinclude:: grammar.mzn
  :language: minizincdef
  :start-after: % Solve item
  :end-before: %

Example solve items:

.. code-block:: minizinc

    solve satisfy;
    solve maximize a*x + y - 3*z;

The solve item determines whether the model represents a constraint
satisfaction problem or an optimisation problem.  If there is no solve item, the model is assumed to be a satisfaction problem.
For optimisation problems, the given expression is the one to be minimized/maximized.

The expression in a ``minimize``/``maximize`` solve item can have integer or float type.

*Rationale: This is possible because all type-insts have a defined order.*
Note that having an expression with a fixed type-inst in a solve item is not
very useful as it means that the model requires no optimisation.

Solve items can be annotated.  :ref:`spec-Annotations` has more details on
annotations.

.. _spec-output-items:

Output Items
~~~~~~~~~~~~

Output items are used to present the solutions of a model instance.
They have the following syntax:

.. literalinclude:: grammar.mzn
  :language: minizincdef
  :start-after: % Output items
  :end-before: %

For example:

.. code-block:: minizinc

    output ["The value of x is ", show(x), "!\n"];

The expression must have type-inst :mzn:`array[int] of par string`. It can
be composed using the built-in operator :mzn:`++` and the built-in functions
:mzn:`show`, :mzn:`show_int`, and :mzn:`show_float` (:ref:`spec-builtins`),
as well as string interpolations
(:ref:`spec-String-Interpolation-Expressions`). The output is the
concatenation of the elements of the array. If multiple output items exist,
the output is the concatenation of all of their outputs, in the order in
which they appear in the model.

If no output item is present,
the implementation should print all the global variables and their values
in a readable format.

.. _spec-annotation-items:

Annotation Items
~~~~~~~~~~~~~~~~

Annotation items are used to augment the :mzn:`ann` type.  They have the
following syntax:

.. literalinclude:: grammar.mzn
  :language: minizincdef
  :start-after: % Annotation items
  :end-before: %

For example:

.. code-block:: minizinc

    annotation solver(int: kind);

It is a type-inst error if an annotation is declared and/or defined more
than once in a model.

The use of annotations is described in :ref:`spec-Annotations`.

.. _spec-preds-and-fns:

User-defined Operations
~~~~~~~~~~~~~~~~~~~~~~~

.. 
  % XXX: not saying if operations need to be defined.  Implementation
  % currently requires functions and tests to be defined if used, but
  % predicates can be bodyless even if used.  Should perhaps require functions
  % to be defined even if they're not used (like parameters), and make tests
  % like predicates?

MiniZinc models can contain user-defined operations.  They have this syntax:

.. literalinclude:: grammar.mzn
  :language: minizincdef
  :start-after: % Predicate, test and function items
  :end-before: %

The type-inst expressions can include type-inst variables in
the function and predicate declaration.

For example, predicate :mzn:`even` checks that its argument is an even
number.

.. code-block:: minizinc

    predicate even(var int: x) =
        x mod 2 = 0;

A predicate supported natively by the target solver can be declared as
follows:

.. code-block:: minizinc

    predicate alldifferent(array [int] of var int: xs);

Predicate declarations that are natively supported 
in MiniZinc are restricted to using FlatZinc
types (for instance, multi-dimensional and non-1-based arrays are
forbidden).
.. % \pjs{need to fix this if we allow2d arrays in FlatZinc!}

Declarations for user-defined operations can be annotated.
:ref:`spec-Annotations` has more details on annotations.

.. _spec-basic-properties:

Basic Properties
++++++++++++++++

The term "predicate" is generally used to refer to both test items and
predicate items.  When the two kinds must be distinguished, the terms
"test item" and "predicate item" can be used.

The return type-inst of a test item is implicitly :mzn:`par bool`.  The
return type-inst of a predicate item is implicitly :mzn:`var bool`.

Predicates and functions are allowed to be recursive. Termination of
a recursive function call depends solely on its fixed arguments, i.e., 
recursive functions and predicates cannot be used to define recursively 
constrained variables.
.. % \Rationale{This ensures that the satisfiability of models is decidable.}

Predicates and functions introduce their own local names, being those of the
formal arguments.  The scope of these names covers the predicate/function
body.  Argument names cannot be repeated within a predicate/function
declaration.

Ad-hoc polymorphism
+++++++++++++++++++

MiniZinc supports ad-hoc polymorphism via overloading.  Functions
and predicates (both built-in and user-defined) can be overloaded.  A name
can be overloaded as both a function and a predicate.

It is a type-inst error if a single version of an overloaded operation with
a particular type-inst signature is defined more than once
in a model.  For example:

.. code-block:: minizinc

    predicate p(1..5: x);
    predicate p(1..5: x) = false;       % ok:     first definition
    predicate p(1..5: x) = true;        % error:  repeated definition

The combination of overloading and coercions can cause problems.
Two overloadings of an operation are said to *overlap* if they could match
the same arguments.  For example, the following overloadings of :mzn:`p`
overlap, as they both match the call :mzn:`p(3)`.

.. code-block:: minizinc

    predicate p(par int: x);
    predicate p(var int: x);

However, the following two predicates do not overlap because they cannot
match the same argument:

.. code-block:: minizinc

    predicate q(int:        x);
    predicate q(set of int: x);

We avoid two potential overloading problems by placing some restrictions on
overlapping overloadings of operations.

#. The first problem is ambiguity.  Different placement of coercions in
   operation arguments may allow different choices for the overloaded function.
   For instance, if a MiniZinc function :mzn:`f` is overloaded like this:

   .. code-block:: minizinc

    function int: f(int: x, float: y) = 0;
    function int: f(float: x, int: y) = 1;

   then :mzn:`f(3,3)` could be either 0 or 1 depending on
   coercion/overloading choices.

   To avoid this problem, any overlapping overloadings of an operation must
   be semantically equivalent with respect to coercion.  For example, the
   two overloadings of the predicate :mzn:`p` above must have bodies that are
   semantically equivalent with respect to overloading.

   Currently, this requirement is not checked and the modeller must satisfy it
   manually.  In the future, we may require the sharing of bodies among
   different versions of overloaded operations, which would provide automatic
   satisfaction of this requirement.
#. The second problem is that certain combinations of overloadings could
   require a MiniZinc implementation to perform combinatorial search in
   order to explore different choices of coercions and overloading.  For
   example, if function :mzn:`g` is overloaded like this:

   .. code-block:: minizinc

       function float: g(int: t1, float: t2) = t2;
       function int  : g(float: t1, int: t2) = t1;

   then how the overloading of :mzn:`g(3,4)` is resolved depends upon its
   context:

   .. code-block:: minizinc

       float: s = g(3,4);
       int: t = g(3,4);

   In the definition of :mzn:`s` the first overloaded definition must be used
   while in the definition of :mzn:`t` the second must be used.
   
   To avoid this problem, all overlapping overloadings of an operation must be
   closed under intersection of their input type-insts.  That is, if overloaded
   versions have input type-inst :math:`(S_1,....,S_n)` and :math:`(T_1,...,T_n)` then
   there must be another overloaded version with input type-inst
   :math:`(R_1,...,R_n)` where each :math:`R_i` is the greatest lower bound (*glb*) of
   :math:`S_i` and :math:`T_i`.
   
   Also, all overlapping overloadings of an operation must be monotonic.  That
   is, if there are overloaded versions with input type-insts :math:`(S_1,....,S_n)`
   and :math:`(T_1,...,T_n)` and output type-inst :math:`S` and :math:`T`, respectively, then
   :math:`S_i \preceq T_i` for all :math:`i`, implies :math:`S \preceq T`.  At call sites, the
   matching overloading that is lowest on the type-inst lattice is always used.
   
   For :mzn:`g` above, the type-inst intersection (or *glb*) of
   :mzn:`(int,float)`  and :mzn:`(float,int)` is
   :mzn:`(int,int)`.  Thus, the overloaded versions are not closed under
   intersection and the user needs to provide another overloading for
   :mzn:`g` with input type-inst :mzn:`(int,int)`.  The natural
   definition is:

   .. code-block:: minizinc

       function int: g(int: t1, int: t2) = t1;

   Once :mzn:`g` has been augmented with the third overloading, it satisfies
   the monotonicity requirement because the output type-inst of the third
   overloading is :mzn:`int` which is less than the output
   type-inst of the original overloadings.
   
   Monotonicity and closure under type-inst conjunction ensure that whenever an
   overloaded function or predicate is reached during type-inst checking, there
   is always a unique and safe "minimal" version to choose, and so the
   complexity of type-inst checking remains linear.  Thus in our example
   :mzn:`g(3,4)` is always resolved by choosing the new overloaded
   definition.


Local Variables
+++++++++++++++

Local variables in operation bodies are introduced using let expressions.
For example, the predicate :mzn:`have_common_divisor` takes two
integer values and checks whether they have a common divisor greater than
one:

.. code-block:: minizinc

    predicate have_common_divisor(int: A, int: B) =
        let {
            var 2..min(A,B): C;
        } in
            A mod C = 0 /\
            B mod C = 0;

However, as :ref:`spec-Let-Expressions` explained, because :mzn:`C` is
not defined, this predicate cannot be called in a negative context.  The
following is a version that could be called in a negative context:

.. code-block:: minizinc

    predicate have_common_divisor(int: A, int: B) =
        exists(C in 2..min(A,B))
            (A mod C = 0 /\ B mod C = 0);

.. _spec-annotations:

Annotations
-----------

Annotations allow a modeller to specify
non-declarative and solver-specific information that is beyond the core
language.  Annotations do not change the meaning of a model, however, only
how it is solved.

Annotations can be attached to variables (on their declarations), constraints,
expressions, type-inst synonyms, enum items, solve items and on user
defined operations.
They have the following syntax:

.. literalinclude:: grammar.mzn
  :language: minizincdef
  :start-after: % Annotations

For example:

.. code-block:: minizinc

    int: x::foo;
    x = (3 + 4)::bar("a", 9)::baz("b");
    solve :: blah(4)
        minimize x;

The types of the argument expressions must match the argument types of the
declared annotation.  Like user-defined predicates and functions,
annotations can be overloaded.

Annotation signatures can contain type-inst variables.

The order and nesting of annotations do not matter.  For the expression case
it can be helpful to view the annotation connector :mzn:`::` as an
overloaded operator:

.. code-block:: minizinc

    ann: '::'(var $T: e, ann: a);       % associative
    ann: '::'(ann:    a, ann: b);       % associative + commutative

Both operators are associative, the second is commutative.  This means that
the following expressions are all equivalent:

.. code-block:: minizinc

    e :: a :: b
    e :: b :: a
    (e :: a) :: b
    (e :: b) :: a
    e :: (a :: b)
    e :: (b :: a)

This property also applies to annotations on solve items and variable
declaration items.  *Rationale: This property make things simple, as it
allows all nested combinations of annotations to be treated as if they are
flat, thus avoiding the need to determine what is the meaning of an
annotated annotation.  It also makes the MiniZinc abstract syntax tree simpler
by avoiding the need to represent nesting.*

Annotations have to be values of the :mzn:`ann` type or string literals. The 
latter are used for *naming* constraints and expressions, for example

.. code-block:: minizinc

    constraint ::"first constraint" alldifferent(x);
    constraint ::"second constraint" alldifferent(y);
    constraint forall (i in 1..n) (my_constraint(x[i],y[i])::"constraint \(i)");

Note that constraint items can *only* be annotated with string literals.

*Rationale: Allowing arbitrary annotations on constraint items makes the grammar ambiguous, and seems unneccessary since we can just as well annotate the constraint expression.*

.. _spec-partiality:

Partiality
----------

The presence of constrained type-insts in MiniZinc means that
various operations are potentially *partial*, i.e., not clearly defined
for all possible inputs.  For example, what happens if a function expecting
a positive argument is passed a negative argument?  What happens if a
variable is assigned a value that does not satisfy its type-inst constraints?
What happens if an array index is out of bounds?  This section describes
what happens in all these cases.

.. % \pjs{This is not what seems to happen in the current MiniZinc!}

In general, cases involving fixed values that do not satisfy constraints
lead to run-time aborts.
*Rationale: Our experience shows that if a fixed value fails a constraint, it
is almost certainly due to a programming error.  Furthermore, these cases
are easy for an implementation to check.*

But cases involving unfixed values vary, as we will see.
*Rationale: The best thing to do for unfixed values varies from case to case.
Also, it is difficult to check constraints on unfixed values, particularly
because during search a decision variable might become fixed and then
backtracking will cause this value to be reverted, in which case aborting is
a bad idea.*

Partial Assignments
~~~~~~~~~~~~~~~~~~~

The first operation involving partiality is assignment.  There are four
distinct cases for assignments.

- A value assigned to a fixed, constrained global variable is checked at
  run-time;  if the assigned value does not satisfy its constraints, it is
  a run-time error.  In other words, this:

  .. code-block:: minizinc

    1..5: x = 3;

  is equivalent to this:

  .. code-block:: minizinc

    int: x = 3;
    constraint assert(x in 1..5,
                      "assignment to global parameter 'x' failed")

- A value assigned to an unfixed, constrained global variable makes the
  assignment act like a constraint;  if the assigned value does not
  satisfy the variable's constraints, it causes a run-time model failure.
  In other words, this:

  .. code-block:: minizinc

    var 1..5: x = 3;

  is equivalent to this:

  .. code-block:: minizinc

    var int: x = 3;
    constraint x in 1..5;

  *Rationale: This behaviour is easy to understand and easy to implement.*

- A value assigned to a fixed, constrained let-local variable is checked at
  run-time;  if the assigned value does not satisfy its constraints, it is
  a run-time error.  In other words, this:

  .. code-block:: minizinc

    let { 1..5: x = 3; } in x+1

  is equivalent to this:

  .. code-block:: minizinc

    let { int: x = 3; } in
        assert(x in 1..5,
               "assignment to let parameter 'x' failed", x+1)

- A value assigned to an unfixed, constrained let-local variable makes the
  assignment act like a constraint;  if the constraint fails at run-time, the
  failure "bubbles up" to the nearest enclosing Boolean scope, where it
  is interpreted as :mzn:`false`.

  *Rationale: This behaviour is consistent with assignments to global
  variables.*

Note that in cases where a value is partly fixed and partly unfixed, e.g., some
arrays, the different elements are checked according to the different cases,
and fixed elements are checked before unfixed elements.  For example:

.. code-block:: minizinc

    u = [ let { var 1..5: x = 6} in x, let { par 1..5: y = 6; } in y) ];

This causes a run-time abort, because the second, fixed element is checked
before the first, unfixed element.  This ordering is true for the cases in the
following sections as well.  *Rationale: This ensures that failures cannot
mask aborts, which seems desirable.*

Partial Predicate/Function and Annotation Arguments
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The second kind of operation involving partiality is calls and annotations.

.. 
  % The behaviour for these operations is simple:  constraints on arguments are
  % ignored.
  %
  % \Rationale{This is easy to implement and easy to understand.  It is also
  % justifiable in the sense that predicate/function/annotation arguments are
  % values that are passed in from elsewhere;  if those values are to be
  % constrained, that could be done earlier.  (In comparison, when a variable
  % with a constrained type-inst is declared, any assigned value must clearly
  % respect that constraint.)}

The semantics is similar to assignments:  fixed arguments that fail their constraints
will cause aborts, and unfixed arguments that fail their constraints will
cause failure, which bubbles up to the nearest enclosing Boolean scope.


Partial Array Accesses
~~~~~~~~~~~~~~~~~~~~~~

The third kind of operation involving partiality is array access.  There
are two distinct cases.

- A fixed value used as an array index is checked at run-time;  if the
  index value is not in the index set of the array, it is a run-time
  error.

- An unfixed value used as an array index makes the access act like a
  constraint;  if the access fails at run-time, the failure "bubbles up"
  to the nearest enclosing Boolean scope, where it is interpreted as
  :mzn:`false`.  For example:

  .. code-block:: minizinc

    array[1..3] of int: a = [1,2,3];
    var int: i;
    constraint (a[i] + 3) > 10 \/ i = 99;

  Here the array access fails, so the failure bubbles up to the
  disjunction, and :mzn:`i` is constrained to be 99.
  *Rationale: Unlike predicate/function calls, modellers in practice
  sometimes do use array accesses that can fail.   In such cases, the
  "bubbling up" behaviour is a reasonable one.*

.. _spec-builtins:

Built-in Operations
-------------------

This appendix lists built-in operators, functions and
predicates.  They may be implemented as true built-ins, or in libraries that
are automatically imported for all models.  Many of them are overloaded.

Operator names are written within single quotes when used in type
signatures, e.g. :mzn:`bool: '\/'(bool, bool)`.

We use the syntax :mzn:`TI: f(TI1,...,TIn)` to represent an operation
named :mzn:`f` that takes arguments with type-insts :mzn:`TI,...,TIn`
and returns a value with type-inst :mzn:`TI`.  This is slightly more
compact than the usual MiniZinc syntax, in that it omits argument names.


Comparison Operations
~~~~~~~~~~~~~~~~~~~~~


Less than.  Other comparisons are similar:
greater than (:mzn:`>`),
less than or equal (:mzn:`<=`),
greater than or equal (:mzn:`>=`),
equality (:mzn:`==`, :mzn:`=`),
and disequality (:mzn:`!=`).

.. % \pjs{Check use of any here!}

.. code-block:: minizinc

      bool: '<'(    $T,     $T)
  var bool: '<'(var $T, var $T)





Arithmetic Operations
~~~~~~~~~~~~~~~~~~~~~


Addition.  Other numeric operations are similar:
subtraction (:mzn:`-`), and
multiplication (:mzn:`*`).

.. code-block:: minizinc

      int:   '+'(    int,       int)
  var int:   '+'(var int,   var int)
      float: '+'(    float,     float)
  var float: '+'(var float, var float)





Unary minus.  Unary plus (:mzn:`+`) is similar.

.. code-block:: minizinc

      int:   '-'(    int)
  var int:   '-'(var int)
      float: '-'(    float)
  var float: '-'(var float)





Integer and floating-point division and modulo.

.. code-block:: minizinc

      int:   'div'(    int,       int)
  var int:   'div'(var int,   var int)
      int:   'mod'(    int,       int)
  var int:   'mod'(var int,   var int)
      float: '/'  (    float,     float)
  var float: '/'  (var float, var float)



The result of the modulo operation, if non-zero, always has the same sign as
its first operand.  The integer division and modulo operations are connected
by the following identity:

.. code-block:: minizinc

  x = (x div y) * y + (x mod y)



Some illustrative examples:

.. code-block:: minizinc

   7 div  4 =  1        7 mod  4 =  3
  -7 div  4 = -1       -7 mod  4 = -3 
   7 div -4 = -1        7 mod -4 = 3
  -7 div -4 =  1       -7 mod -4 = -3





Sum multiple numbers.
Product (:mzn:`product`) is similar.  Note that the sum of an empty array
is 0, and the product of an empty array is 1.

.. code-block:: minizinc

      int:   sum(array[$T]  of     int  )
  var int:   sum(array[$T]  of var int  )
      float: sum(array[$T]  of     float)
  var float: sum(array[$T]  of var float)





Minimum of two values;  maximum (:mzn:`max`) is
similar.

.. code-block:: minizinc

  $T:     min(    $T,     $T)
  var $T: min(var $T, var $T)





Minimum of an array of values;  maximum (:mzn:`max`) is similar.
Aborts if the array is empty.

.. code-block:: minizinc

  $U:     min(array[$T]  of     $U)
  var $U: min(array[$T]  of var $U)





Minimum of a fixed set;  maximum (:mzn:`max`) is similar.
Aborts if the set is empty.

.. code-block:: minizinc

  $T:    min(set of $T)





Absolute value of a number.

.. code-block:: minizinc

      int:   abs(    int)
  var int:   abs(var int)
      float: abs(    float)
  var float: abs(var float)





Square root of a float.  Aborts if argument is negative.

.. code-block:: minizinc

      float: sqrt(    float)
  var float: sqrt(var float)





Power operator.  E.g. :mzn:`pow(2, 5)` gives :mzn:`32`.

.. code-block:: minizinc

    int: pow(int,       int)
  float: pow(float,     float)


.. 
  % We should also have:
  %  var float: pow(var float, int)


Natural exponent.

.. code-block:: minizinc

      float: exp(float)
  var float: exp(var float)





Natural logarithm.  Logarithm to base 10 (:mzn:`log10`) and logarithm to base
2 (:mzn:`log2`) are similar.

.. code-block:: minizinc

      float: ln(float)
  var float: ln(var float)





General logarithm;  the first argument is the base.

.. code-block:: minizinc

  float: log(float, float)





Sine.  Cosine (:mzn:`cos`), tangent (:mzn:`tan`), inverse sine
(:mzn:`asin`), inverse cosine (:mzn:`acos`), inverse tangent
(:mzn:`atan`), hyperbolic sine (:mzn:`sinh`), hyperbolic cosine
(:mzn:`cosh`), hyperbolic tangent (:mzn:`tanh`),
inverse hyperbolic sine (:mzn:`asinh`), inverse hyperbolic cosine
(:mzn:`acosh`) and inverse hyperbolic tangent (:mzn:`atanh`) are similar.

.. code-block:: minizinc

      float: sin(float)
  var float: sin(var float)





Logical Operations
~~~~~~~~~~~~~~~~~~


Conjunction.  Other logical operations are similar:
disjunction (``\/``)
reverse implication (:mzn:`<-`),
forward implication (:mzn:`->`),
bi-implication (:mzn:`<->`),
exclusive disjunction (:mzn:`xor`),
logical negation (:mzn:`not`).

Note that the implication operators are not written
using :mzn:`=>`, :mzn:`<=` and :mzn:`<=>` as is the case in some
languages.  This allows :mzn:`<=` to instead represent "less than or
equal".

.. code-block:: minizinc

      bool: '/\'(    bool,     bool)
  var bool: '/\'(var bool, var bool)





Universal quantification.
Existential quantification (:mzn:`exists`) is similar.  Note that, when
applied to an empty list, :mzn:`forall` returns :mzn:`true`, and
:mzn:`exists` returns :mzn:`false`.

.. code-block:: minizinc

      bool: forall(array[$T]  of     bool)
  var bool: forall(array[$T]  of var bool)





N-ary exclusive disjunction.
N-ary bi-implication (:mzn:`iffall`) is similar, with :mzn:`true` instead
of :mzn:`false`.

.. code-block:: minizinc

      bool: xorall(array[$T]  of     bool: bs) = foldl('xor', false, bs)
  var bool: xorall(array[$T]  of var bool: bs) = foldl('xor', false, bs)





Set Operations
~~~~~~~~~~~~~~


Set membership.

.. code-block:: minizinc

      bool: 'in'(     $T,       set of $T )
  var bool: 'in'(var int,   var set of int)





Non-strict subset.  Non-strict superset (:mzn:`superset`) is similar.

.. code-block:: minizinc

      bool: 'subset'(    set of $T ,     set of $T )
  var bool: 'subset'(var set of int, var set of int)





Set union.  Other set operations are similar:
intersection (:mzn:`intersect`),
difference (:mzn:`diff`),
symmetric difference (:mzn:`symdiff`).

.. code-block:: minizinc

      set of  $T: 'union'(    set of  $T,     set of  $T )
  var set of int: 'union'(var set of int, var set of int )





Set range.  If the first argument is larger than the second
(e.g. :mzn:`1..0`), it returns the empty set.

.. code-block:: minizinc

  set of int: '..'(int, int)





Cardinality of a set.

.. code-block:: minizinc

      int: card(    set of  $T)
  var int: card(var set of int)





Union of an array of sets.
Intersection of multiple sets (:mzn:`array_intersect`) is similar.

.. code-block:: minizinc

      set of  $U:    array_union(array[$T]  of     set of  $U)
  var set of int:    array_union(array[$T]  of var set of int)


Array Operations
~~~~~~~~~~~~~~~~


Length of an array.

.. code-block:: minizinc

  int: length(array[$T] of $U)





List concatenation.  Returns the list (integer-indexed array) containing
all elements of the first argument followed by all elements of the
second argument, with elements occurring in the same order as
in the arguments.  The resulting indices are in the range :mzn:`1..n`,
where :mzn:`n` is the sum of the lengths of the arguments.
*Rationale: This allows list-like arrays to be concatenated naturally
and avoids problems with overlapping indices.  The resulting indices
are consistent with those of implicitly indexed array literals.*
Note that :mzn:`'++'` also performs string concatenation.

.. code-block:: minizinc

  array[int] of $T: '++'(array[int] of $T, array[int] of $T)

Index sets of arrays.  If the argument is a literal, returns :mzn:`1..n`
where :mzn:`n` is the (sub-)array length.  Otherwise, returns the declared
or inferred index set.  This list is only partial, it extends in the obvious
way, for arrays of higher dimensions.

.. code-block:: minizinc

  set of $T:  index_set     (array[$T]      of $V)
  set of $T:  index_set_1of2(array[$T, $U]  of $V)
  set of $U:  index_set_2of2(array[$T, $U]  of $V)
  ...

Replace the indices of the array given by the last argument with the
Cartesian product of the sets given by the previous arguments.  Similar
versions exist for arrays up to 6 dimensions.

.. code-block:: minizinc

  array[$T1] of $V: array1d(set of $T1, array[$U] of $V)
  array[$T1,$T2] of $V:
      array2d(set of $T1, set of $T2, array[$U] of $V)
  array[$T1,$T2,$T3] of $V:
      array3d(set of $T1, set of $T2, set of $T3, array[$U] of $V)


Coercion Operations
~~~~~~~~~~~~~~~~~~~


Round a float towards :math:`+\infty`, :math:`-\infty`, and the nearest integer,
respectively.

.. code-block:: minizinc

  int: ceil (float)
  int: floor(float)
  int: round(float)


Explicit casts from one type-inst to another.

.. code-block:: minizinc

      int:          bool2int(    bool)
  var int:          bool2int(var bool)
      float:        int2float(    int)
  var float:        int2float(var int)
  array[int] of $T: set2array(set of $T)


String Operations
~~~~~~~~~~~~~~~~~


To-string conversion.  Converts any value to a string for output purposes.
The exact form of the resulting string is implementation-dependent.

.. code-block:: minizinc

  string: show($T)


Formatted to-string conversion for integers.
Converts the integer given by the second argument into a string right justified
by the number of characters given by the first argument, or left justified
if that argument is negative.
If the second argument is not fixed, the form of the string is
implementation-dependent.

.. code-block:: minizinc

  string: show_int(int, var int);


Formatted to-string conversion for floats.
Converts the float given by the third argument into a string right
justified by the number of characters given by the first argument, or left
justified if that argument is negative.
The number of digits to appear after the decimal point is given by
the second argument.
It is a run-time error for the second argument to be negative.
If the third argument is not fixed, the form of the string is
implemenation-dependent.

.. code-block:: minizinc

  string: show_float(int, int, var float)


String concatenation.  Note that :mzn:`'++'` also performs array
concatenation.

.. code-block:: minizinc

  string: '++'(string, string)


Concatenate an array of strings.
Equivalent to folding :mzn:`'++'` over the array, but may be implemented more
efficiently.

.. code-block:: minizinc

   string: concat(array[$T] of string)


Concatenate an array of strings, putting a seperator beween adjacent strings.
Returns the the empty string if the array is empty.

.. code-block:: minizinc

   string: join(string, array[$T] of string)


Bound and Domain Operations
~~~~~~~~~~~~~~~~~~~~~~~~~~~

The bound operations :mzn:`lb` and :mzn:`ub` return fixed, correct lower/upper bounds
to the expression.
For numeric types, they return a lower/upper bound value,
e.g. the lowest/highest value the expression can take.
For set types, they return a subset/superset,
e.g. the intersection/union of all possible values of the set expression.

The bound operations abort on expressions that have no corresponding finite bound.
For example, this would be the case for a variable declared without bounds
in an implementation that does not assign default bounds.
(Set expressions always have a finite lower bound of course,
namely :mzn:`{}`, the empty set.)

Numeric lower/upper bound:

.. code-block:: minizinc

  int:   lb(var int)
  float: lb(var float)
  int:   ub(var int)
  float: ub(var float)


Set lower/upper bound:

.. code-block:: minizinc

  set of int: lb(var set of int)
  set of int: ub(var set of int)

Versions of the bound operations that operate on arrays are also available,
they return a safe lower bound or upper bound for all members of the array
- they abort if the array is empty:

.. code-block:: minizinc

  int:        lb_array(array[$T] of var int)
  float:      lb_array(array[$T] of var float)
  set of int: lb_array(array[$T] of var set of int)
  int:        ub_array(array[$T] of var int)
  float:      ub_array(array[$T] of var float)
  set of int: ub_array(array[$T] of var set of int)





Integer domain:

.. code-block:: minizinc

  set of int: dom(var int)



The domain operation :mzn:`dom` returns a fixed superset
of the possible values of the expression.


Integer array domain, returns a superset of all possible values that may
appear in the array - this aborts if the array is empty:

.. code-block:: minizinc

  set of int: dom_array(array[$T] of var int)

Domain size for integers:

.. code-block:: minizinc

  int: dom_size(var int)

The domain size operation :mzn:`dom_size` is equivalent
to :mzn:`card(dom(x))`.

Note that these operations can produce different results depending on when
they are evaluated and what form the argument takes.  For example, consider
the numeric lower bound operation.

- If the argument is a fixed expression, the result is the argument's
  value.

- If the argument is a decision variable, then the result depends on
  the context.

  - If the implementation can determine a lower bound for the variable,
    the result is that lower bound.
    The lower bound may be from the variable's declaration,
    or higher than that due to preprocessing,
    or lower than that if an implementation-defined lower bound is applied
    (e.g. if the variable was declared with no lower bound,
    but the implementation imposes a lowest possible bound).

  - If the implementation cannot determine a lower bound for the variable,
    the operation aborts.

- If the argument is any other kind of unfixed expression, the
  lower bound depends on the bounds of unfixed subexpressions
  and the connecting operators.

.. _spec-option-type-operations:

Option Type Operations
~~~~~~~~~~~~~~~~~~~~~~~

The option type value (:math:`\top`) is written

.. code-block:: minizinc

  opt $T:  '<>';


One can determine if an option type variable actually occurs or not using
:mzn:`occurs` and :mzn:`absent`

.. code-block:: minizinc

  par bool: occurs(par opt $T);
  var bool: occurs(var opt $T);
  par bool: absent(par opt $T);
  var bool: absent(var opt $T);


One can return the non-optional value of an option type variable using the
function :mzn:`deopt`

.. code-block:: minizinc

  par $T: deopt{par opt $T);
  var $T: deopt(var opt $T);


.. 
  % Note that this is not really a function only a pseudo function placeholder
  % used in the translation of option types to non-option types.
  % \pjs{Explain better}

.. _spec-other-operations:

Other Operations
~~~~~~~~~~~~~~~~~~~~~~~

Check a Boolean expression is true, and abort if not, printing the second
argument as the error message.  The first one returns the third argument, and
is particularly useful for sanity-checking arguments to predicates and
functions;  importantly, its third argument is lazy, i.e. it is only evaluated
if the condition succeeds.  The second one returns :mzn:`true` and is useful
for global sanity-checks (e.g. of instance data) in constraint items.

.. code-block:: minizinc

  $T:   assert(bool, string, s$T)
  par bool: assert(bool, string)


Abort evaluation, printing the given string.

.. code-block:: minizinc

  $T: abort(string)

Return true. As a side-effect, an implementation may print the first argument.

.. code-block:: minizinc

  bool: trace(string)


Return the second argument.
As a side-effect, an implementation may print the first argument.

.. code-block:: minizinc

  $T: trace(string, $T)


Check if the argument's value is fixed at this point in evaluation.  If not,
abort; if so, return its value.  This is most useful in output items when
decision variables should be fixed: it allows them to be used in places
where a fixed value is needed, such as if-then-else conditions.

.. code-block:: minizinc

  $T: fix(var $T)


As above, but return :mzn:`false` if the argument's value is not fixed.

.. code-block:: minizinc

  par bool: is_fixed(var $T)


.. _spec-content-types:

Content-types
-------------

The content-type ``application/x-zinc-output`` defines
a text output format for Zinc.
The format extends the abstract syntax and semantics
given in :ref:`spec-Run-time-Outcomes`,
and is discussed in detail in :ref:`spec-Output`.

The full syntax is as follows:

.. literalinclude:: output.mzn
  :language: minizincdef

The solution text for each solution must be
as described in :ref:`spec-Output-Items`.
A newline must be appended if the solution text does not end with a newline.

.. _spec-json:

JSON support
------------

MiniZinc can support reading input parameters and providing output formatted
as JSON objects. A JSON input file needs to have the following structure:

- Consist of a single top-level object

- The members of the object (the key-value pairs) represent model parameters

- Each member key must be a valid MiniZinc identifier (and it supplies the value for the corresponding parameter of the model)

- Each member value can be one of the following:

  - A string (assigned to a MiniZinc string parameter)

  - A number (assigned to a MiniZinc int or float parameter)

  - The values ``true`` or ``false`` (assigned to a MiniZinc bool parameter)

  - An array of values. Arrays of arrays are supported only if all inner arrays are of the same length, so that they can be mapped to multi-dimensional MiniZinc arrays.

  - A set of values encoded as an object with a single member with key ``"set"`` and a list of values (the elements of the set).

This is an example of a JSON parameter file using all of the above features:

.. code-block:: json

    {
      "n" : 3,
      "distances" : [ [1,2,3],
                      [4,5,6]],
      "patterns"  : [ {"set" : [1,3,5]}, {"set" : [2,4,6]} ]
    }


The first parameter declares a simple integer ``n``. The
``distances`` parameter is a two-dimensional array; note that all inner
arrays must be of the same size in order to map to a (rectangular) MiniZinc
two-dimensional array. The third parameter is an array of sets of integers.

**Note**: The JSON input and output currently does not support enumerated types. This will be added in a future release.

.. _spec-grammar:

Full grammar
------------

Items
~~~~~

.. literalinclude:: grammar.mzn
  :language: minizincdef
  :end-before: % Type-inst expressions

Type-Inst Expressions
~~~~~~~~~~~~~~~~~~~~~

.. literalinclude:: grammar.mzn
  :language: minizincdef
  :start-after: % Type-inst expressions
  :end-before: % Expressions

Expressions
~~~~~~~~~~~

.. literalinclude:: grammar.mzn
  :language: minizincdef
  :start-after: % Expressions
  :end-before: % Miscellaneous

Miscellaneous Elements
~~~~~~~~~~~~~~~~~~~~~~

.. literalinclude:: grammar.mzn
  :language: minizincdef
  :start-after: % Miscellaneous

