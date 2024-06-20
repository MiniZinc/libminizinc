Mzn-Analyse
===========

``mzn-analyse`` is an informal collection of experimental MiniZinc
compiler "passes" that analyse and manipulate MiniZinc models.  The passes
pipeline structure allows relatively complex tasks to be performed on
MiniZinc models by combining multiple passes together.

The tool provides the ``get-diversity-anns`` pass used by the
``diverse_solutions()`` solution diversity features of the MiniZinc
python interface.


Basic Usage
-----------

``mzn-analyse input.mzn [passes]``

where ``[passes]`` is the list of passes and arguments for those passes
(``cmd:arg1,arg2,...`` with no spaces unless quoted)

An implicit ``out:-`` command (or ``out_fzn:-`` if reading FlatZinc)
is appended at the end of the pipeline if no ``out`` commands occur in
the sequence.  An explicit final ``out`` must be added to the end if
you wish to output models throughout the pipeline.

Typically, each pass takes a MiniZinc model, and outputs a MiniZinc model
to the next pass.  Optionally, a pass may store some information in a
global json store which will be dumped to stdout by default. The contents
of the json output can be inspected at any point by using the ``json_out``
pass.  The json store can be cleared by using the ``json_clear`` pass.

Passes: read/write models & json
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

These passes control the reading and writing of MiniZinc models and the
json store.  In most cases, if no argument is passed to one of these
passes, they will read/write to stdout.

  ``in:in.mzn``         Read input file (no support for stdout)
  
  ``out:out.mzn``       Write model to out.mzn (- for stdout)
  
  ``out_fzn:out.fzn``   Write model to out.fzn (- for stdout)
  
  ``no_out``            Disable automatic output insertion
  
  ``json_out:out.json`` Write collected json output to out.json (- for stdout)
  
  ``json_clear``        Clear collected json output
  
  ``no_json``           Disable automatic json output


Passes: Manipulate Models
^^^^^^^^^^^^^^^^^^^^^^^^^

These passes are for making modifications to models or isolating/removing parts of models.

  ``inline-includes`` Inline non-library includes (this allows a model to be relocated without needing to move models it includes)
  
  ``inline-all-includes`` Inline all includes
  
  ``remove-anns:name1,[name2,...]`` Remove Id and Call annotations matching names
  
  ``remove-includes:name1,[name2,...]``  Remove includes matching names
  
  ``output-all``    Add 'add_to_output' annotation to all VarDecls
  
  ``remove-stdlib``    Remove stdlib includes
  
  ``get-items:idx1,[idx2,...]``    Narrow to items indexed by idx1,...
  
  ``filter-items:iid1,[iid2,...]``     Only keep items matching iids
  
  ``remove-items:iid1,[iid2,...]``     Remove items matching iids
  
  ``filter-typeinst:{var|par}``     Just show var/par parts of model
  
  ``replace-with-newvar:location1,location2``  Replace expressions with 'let' expressions
  
  ``repeat-model:k`` Produce ``k`` copies of the model (with unique variable names)
  
  ``get-solve-anns`` Get information about the solving annotations used (useful if rewriting the original objective function)

Passes: Special Passes
^^^^^^^^^^^^^^^^^^^^^^

These passes are needed to support other tools that are developed for MiniZinc.
They are more specific to the projects where they are used.

  ``get-diversity-anns`` Extract diversity information from the model
  
  ``annotate-data-deps``     Annotate expressions with their data dependencies
  
  ``get-term-types:out.terms``  Write .terms file with types of objective terms
  
  ``get-data-deps:out.cons`` (FlatZinc only) Write .cons file with data dependencies of FlatZinc constraints
  
  ``get-exprs:location1,location2`` Extract list of expressions occurring inside location location = path.mzn|sl|sc|el|ec
  
  ``get-ast:location1,location2``  Build JSON representation of AST for whole model or just for expression matching location1 or location2, place in json_store

Examples
--------

The following are a few examples of the usage of the ``mzn-analyse`` tool.

1. Remove the solve and output items from the model and write the model to solveless.mzn. It then inlines the local includes and outputs to stdout as "FlatZinc" (no line breaks while printing an item).

  ``mzn-analyse in.mzn remove-items:solve,output out:solveless.mzn inline-includes out_fzn``

2. Remove all items except constraint items, picks out the 50th constraint, remove any annotations, then output to stdout.
The implicit output will default to `out_fzn` since the input was fzn.

  ``mzn-analyse in.fzn filter-items:constraint get-items:50 remove-anns``

3. The following requests the data-deps information for constraints 60, 61, and 62 from a FlatZinc file.
The `no_out` command disables the automatic insertion of `out_fzn`.

  ``mzn-analyse rcpsp-wet-r0.annotated.fzn filter-items:constraint get-items:60,61,62 get-data-deps no_out``

Gives the following output:

.. code-block:: json

    {"constraint_info": [
      [
        ["in", "i", "Tasks"],
        ["in", "j", "suc[i]"],
        ["assign", "j", "24"],
        ["assign", "i", "2"],
        ["eq", "suc[i]", "23..24"],
        ["eq", "Tasks", "1..32"]],
      [
        ["in", "i", "Tasks"],
        ["in", "j", "suc[i]"],
        ["assign", "j", "5"],
        ["assign", "i", "3"],
        ["eq", "suc[i]", "{5,6,17}"],
        ["eq", "Tasks", "1..32"]],
      [
        ["in", "i", "Tasks"],
        ["in", "j", "suc[i]"],
        ["assign", "j", "6"],
        ["assign", "i", "3"],
        ["eq", "suc[i]", "{5,6,17}"],
        ["eq", "Tasks", "1..32"]]]}


Limitations / Future work
-------------------------

``mzn-analyse`` is an experimental tool.  The passes were implemented for
very specific use cases and may not behave as expected in more general
cases.  The code in the repo is likely to make strong assumptions about
the underlying models that will not always hold.  The passes should be
thought of more as proof-of-concept/AST manipulation sample code to be
used as a starting point for implementing transformations.

