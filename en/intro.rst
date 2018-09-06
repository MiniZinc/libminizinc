Introduction
============

MiniZinc is a language for specifying constrained optimization and
decision problems over integers and real numbers.  A MiniZinc model does not
dictate *how* to solve the problem - the MiniZinc compiler can translate it into different forms suitable for a wide range of *solvers*, such as Constraint Programming (CP), Mixed Integer Linear Programming (MIP) or Boolean Satisfiability (SAT) solvers.

The MiniZinc language lets users write models in a way that is close to a mathematical formulation of the problem, using familiar notation such as existential and universal quantifiers, sums over index sets, or logical connectives like implications and if-then-else statements. Furthermore, MiniZinc supports defining predicates and functions that let users structure their models (similar to procedures and functions in regular programming languages).

MiniZinc models are usually *parametric*, i.e., they describe a whole *class* of problems rather than an individual problem instance. That way, a model of, say, a vehicle routing problem could be reused to generate weekly plans, by instantiating it with the updated customer demands for the upcoming week.

MiniZinc is designed to interface easily to different backend solvers.  It
does this by transforming an input MiniZinc model and data file into a 
FlatZinc model. FlatZinc models consist of variable declarations
and constraint definitions as well as a definition of the objective function
if the problem is an optimization problem. 
The translation from MiniZinc to FlatZinc makes use of a library of function and predicate definitions for the particular target solver, which allows the MiniZinc compiler to produce specialised FlatZinc that only contains the types of variables and constraints that are supported by the target.
In particular, MiniZinc allows the specification of *global constraints* by
*decomposition*.  Furthermore, *annotations* of the model let the user fine tune the behaviour of the solver, independent of the declarative meaning of the model.

Structure
---------

This documentation consists of four parts. :ref:`The first part <part-introduction>` includes this introduction and then describes how to download and install MiniZinc and how to make your first steps using the MiniZinc IDE and the command line tools. :ref:`The second part <part-tutorial>` is a tutorial introduction to modelling with MiniZinc, from basic syntax and simple modelling techniques to more advanced topics. It also explains how MiniZinc is compiled to FlatZinc. :ref:`The third part <part-manual>` is a user manual for the tools that make up the MiniZinc tool chain. Finally, :ref:`The fourth part <part-reference>` contains the reference documentation for MiniZinc, including a definition of the MiniZinc language, documentation on how to interface a solver to FlatZinc, and an annotated list of all predicates and functions in the MiniZinc standard library.

How to Read This
----------------

If you are new to MiniZinc, follow the installation instructions and the introduction to the IDE and then work your way through the tutorial. Most example code can be downloaded, but it is sometimes more useful to type it in yourself to get the language into your muscle memory! If you need help, visit the MiniZinc web site at http://www.minizinc.org where you find a disucssion forum.

.. only:: builder_html

  Some of the code examples are shown in boxes like the one below. If a code 
  box has a heading, it usually also contains a link to the source code (you 
  can click on the filename).

.. only:: builder_latex

  Some of the code examples are shown in boxes like the one below. If a code 
  box has a heading, it usually lists the name of a file that can be
  downloaded from http://minizinc.org/doc-latest/en/downloads/index.html.

.. literalinclude:: examples/dummy.mzn
  :language: minizinc
  :name: ex-ex
  :caption: A code example (:download:`dummy.mzn <examples/dummy.mzn>`)

Throughout the documentation, some concepts are defined slightly more formally in special sections like this one.

.. defblock:: More details

  These sections can be skipped over if you just want to work through the 
  tutorial for the first time, but they contain important information for 
  any serious MiniZinc user!

Finally, if you find a mistake in this documentation, please report it through our GitHub issue tracker.


