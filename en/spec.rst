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

  N. Nethercote, P.J. Stuckey, R. Becket, S. Brand, G.J. Duck, and G. Tack.
  Minizinc: Towards a standard CP modelling language.
  In C. Bessiere, editor, *Proceedings of the 13th International
  Conference on Principles and Practice of Constraint Programming*, volume 4741
  of *LNCS*, pages 529--543. Springer-Verlag, 2007.


  K. Marriott, N. Nethercote, R. Rafeh, P.J. Stuckey,
  M. Garcia de la Banda, and M. Wallace.
  The Design of the Zinc Modelling Language.
  *Constraints*, 13(3):229-267, 2008.

.. _spec-syntax-notation:

Notation
--------

The basics of the EBNF used in this specification are as follows.

- Non-terminals are written between angle brackets, :mzndef:`<item>`.
- Terminals are written in double quotes, e.g. :mzndef:`"constraint"`.
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
- A sequence of space characters of length $n$ is written $n$``SP``, e.g., ``2SP``.

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

.. code-block:: minizincdef

  <output> ::= <no-solutions> [ <warnings> ] <free-text>
             | ( <solution> )* [ <complete> ] <free-text>

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

.. code-block:: minizincdef

  <solution> ::= <solution-text> [ \n ] "----------" \n

The solution text for each solution must be
as described in :ref:`spec-Output-Items`.
A newline must be appended if the solution text does not end with a newline.
*Rationale: This allows solutions to be extracted from output
without necessarily knowing how the solutions are formatted.*
Solutions end with a sequence of ten dashes followed by a newline.

.. code-block:: minizincdef

  <no-solutions> ::= "=====UNSATISFIABLE=====" \n

The completness result is printed on a separate line.
*Rationale: The strings are designed to clearly indicate
the end of the solutions.*

.. code-block:: minizincdef

  <complete> ::= "==========" \n

If the search is complete, a statement corresponding to the outcome is printed.
For an outcome of no solutions
the statement is that the model instance is unsatisfiable,
for an outcome of no more solutions
the statement is that the solution set is complete,
and for an outcome of no better solutions
the statement is that the last solution is optimal.
*Rationale: These are the logical implications of a search being complete.*

.. code-block:: minizincdef

  <warnings> ::= ( <message> )+
  
  <message>  ::= ( <line> )+
  <line>     ::= "%" [^\n]* \n

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
(:ref:`preds-and-fns`)..

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

A type-inst is fixed if it does not contain :mzn:`var` or :mzn:`any`,
with the exception of :mzn:`ann`.

Note that several type-inst expressions that are syntactically expressible
represent illegal type-insts.  For example, although the grammar allows
:mzn:`var` in front of all these base type-inst expression tails, it is a
type-inst error to have :mzn:`var` in the front of a string or array
expression.

.. _spec-built-in-scalar-types:

Built-in Scalar Types and Type-insts
------------------------------------

Booleans
~~~~~~~~

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
~~~~~~~~

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
~~~~~~

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
~~~~~~~~~~~~~~~~

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
~~~~~~~

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

Built-in Compound Types and Type-insts
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Sets
++++

.. literalinclude:: grammar.mzn
  :language: minizincdef
  :start-after: % Set type-inst expressions
  :end-before: %

Arrays
++++++

.. literalinclude:: grammar.mzn
  :language: minizincdef
  :start-after: % Array type-inst expressions
  :end-before: %


Full grammar
------------

.. literalinclude:: grammar.mzn
  :language: minizincdef
