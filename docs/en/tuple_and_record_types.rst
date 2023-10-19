.. _sec-tuple_and_record_types:

Tuple and record types
======================

In MiniZinc models we often deal with multiple points of data and multiple
decisions that all concern the same thing, an “object”. There are multiple ways
to modelling this. It is common practice in MiniZinc to have different arrays
for different types of data points or decisions. These arrays will then share a
common index mapping to which object each data point belongs.

In this section we present an alternative, “object-oriented”, approach in
MiniZinc in the form of tuple and record types. Using these types, different
types of data points and decisions can be combined into collections for the
modeller's convenience.

Declaring and using tuples and records
--------------------------------------

.. defblock:: Tuple types and literals

  A tuple type variable is declared as:
  
  .. code-block:: minizincdef

    <var-par> tuple(<ti-expr>, ...): <var-name>

  where one or more type instantiations, :mzndef:`<ti-expr>`, are placed between
  the parentheses to declare the types contained in the tuple variable. 

  For convenience the :mzn:`var` keyword can be used to varify all the member
  types of the tuple (i.e., :mzn:`var tuple(int, bool)` is the same type as
  :mzn:`tuple(var int, var bool)`).

  A tuple literal is created using the following syntax:
  
  .. code-block:: minizincdef

    (<expr>, [ <expr>, ... ])

  For example, :mzn:`(1, true)` is a tuple literal of type :mzn:`tuple(int,
  bool)`. Note that when a tuple contains multiple members, then adding a
  trailing comma is optional, but when it contains a single member it is
  **required** to distinguish the literal from a normal parenthesized expression.

Tuples provide a very simple way to create a collection that contains values of
different types. In a tuple variable, the values contained in the tuple can be
accessed using a number representing the place in the tuple. For example, in the
tuple :mzn:`any: x = (1, true, 2.0)` the first member, :mzn:`1`, can be
retrieved using :mzn:`x.1`, and the final member, :mzn:`2.0`, can be retrieved
using :mzn:`x.3`. Note that the MiniZinc compiler will raise a **Type Error**,
when an integer is used that is lower than one or higher than the number of
members in the tuple.

Although tuple types can be useful, it can often be confusing which member
represents what. Record types improve on this by associating a name with each
member of the type.

.. defblock:: Record types and literals

  A record type variable is declared as:
  
  .. code-block:: minizincdef

    <var-par> record(<ti-expr-and-id>, ...): <var-name>

  where one or more type instantiations with a corresponding identifier,
  :mzndef:`<ti-expr-and-id>`, are placed between the parentheses to declare the
  types contained in the record variable. 

  For convenience the :mzn:`var` keyword can be used to varify all the member
  types of the record (i.e., :mzn:`var record(int: i, bool: b)` is the same type
  as :mzn:`record(var int: i, var bool: b)`).

  A record literal is created using the following syntax:
  
  .. code-block:: minizincdef

    (<ident>: <expr>[, ... ])

  For example, :mzn:`(i: 1, b: true)` is a record literal of type
  :mzn:`record(int: i, bool: b)`. Different from tuples, record literals with
  only a single member do not require a trailing comma.

The syntax for accessing a member of a record is very similar to accessing a
member of a tuple. The difference is that instead of a number, we use the name
given to the member. For example, given the record :mzn:`any: x = (i: 1, b: true)`,
the integer value :mzn:`1` that is named by identifier :mzn:`i` is retrieved
using :mzn:`x.i`. Using identifiers that do not name a member (or any number)
will once more result in a **Type Error**.

Using type-inst synonyms
------------------------

When using records and tuples, writing the types in many places can quickly
become tedious and confusing. Additionally, it might often make sense to give a
name to such a type-inst to describe its meaning. For this purpose, you can use
type-inst synonyms in MiniZinc.

.. defblock:: Type-inst synonyms

  A type-inst synonym is declared as:
  
  .. code-block:: minizincdef

    type <ident> <annotations> = <ti-expr>;

  where the identifier :mzndef:`<ident>` can be used instead of the type-inst
  :mzndef:`<ti-expr>` where required.

For example, in the following MiniZinc fragment we declare two synonyms,
:mzn:`Coord` and :mzn:`Number`

.. code-block:: minizinc

  type Coord = var record(int: x, int: y, int: z);
  type Number = int;

In a model that contains these definitions, we can now declare a variable
:mzn:`array[1..10] of Coord: placement;` or a function :mzn:`function Number:
add(Number: x, Number: y) = x + y;`

Similar to record and tuple types, the :mzn:`var` keyword can be used before the
identifier of a type-inst synonym to varify the type-inst. For instance, given
the synonym :mzn:`type OnOff = bool;` and the variable declaration
:mzn:`var OnOff: choice;`, the type-inst of :mzndef:`choice` would be :mzn:`var
bool`. Different from records and tuple, the reverse is also possible using the
:mzn:`par` keyword. For instance, the given the synonym :mzn:`type Choice = var
bool;` and the variable declaration :mzn:`par Choice: check = fix(choice);` the
type-inst of :mzndef:`check` would be :mzn:`bool`.

Types with both var and par members
-----------------------------------

Tuples and records can be used to collect both data and decisions about an
object, and it can be natural to mix data and decisions that concern the same
object. However, when data and decisions are contained within the same tuple or
record type, initialization of these types can be complex. To avoid the usage of
anonymous variables or let-expressions and allow the assignment from data files,
it is recommended to split data and decisions into separate types.

For example, in a rostering problem it would be natural to combine information
about the employees, both data and decisions. It might thus be natural to define
the following synonyms and variable declarations.

.. code-block:: minizinc

  enum EmpId;
  type Employee = record(
    string: name,
    array[Timespan] of bool: available,
    set of Capability: capacities,
    array[Timespan] of var Shift: shifts,
    var 0..infinity: hours,
  );

  array[EmpId] of Employee: employee;

However, it is not possible to assign :mzn:`employees` from a data file.
Instead, the data and the decisions could be split as follows.

.. code-block:: minizinc

  enum EmpId;
  type EmployeeData = record(
    string: name,
    array[Timespan] of bool: available,
    set of Capability: capacities,
  );
  type EmployeeVar = record(
    array[Timespan] of var Shift: shifts,
    var 0..infinity: hours,
  );

  array[EmpId] of EmployeeData: employee_data;
  array[EmpId] of EmployeeVar: employee_var;

Now it is possible to initialize :mzn:`employee_data` from a data file.
Depending on the model, it might be easiest to use these synonyms and variable
declarations separately, but it might be useful to combine them again. For
instance, when your model contains functions that should operate on the complete
record. The :mzn:`++` operator can be used to easily combine these synonyms and
values, as shown in the following fragment.

.. code-block:: minizinc

  type Employee = EmployeeData ++ EmployeeVar;

  array[EmpId] of Employee: employee = [employee_data[id] ++ employee_var[id] | id in EmpId];

.. defblock:: ++ operator for tuples and records

  The `++` operator can be used to combine expressions and type-inst expressions
  that both have tuple types or both have record types.
  
  .. code-block:: minizincdef

    <expr> ++ <expr>
    <ti-expr> ++ <ti-expr>

  When both (type-inst) expressions have a tuple type, this operation is very
  similar to array concatenation. The result of evaluating the expression will
  be a new tuple that contains all members of left-hand side followed by all the
  members of the right-hand side.

  When both (type-inst) expressions have a record type, then the result of
  evaluating the expression is a record that merges the fields in both records.
  Note that if both records contain a field with the same name, then the
  compiler will reject this as a Type Error.

Example: rectangle packing using records
----------------------------------------

Consider a packing problem where a collection of rectangular objects have to be
packed into a rectangular space. Importantly, none of the rectangles being
packed can overlap. To keep the model simple, we don't allow the rectangles to
be turned. When we create a model to decide whether the packing is possible, we
can use records to help us make the model easier to read.

The data for the model comes in two parts: the total area of the space and the
sizes of the rectangles being packed. We can represent both using a “dimensions”
record that includes the width and the height of a rectangle. We therefore
define the following type-inst synonym.

.. literalinclude:: examples/rect_packing/rect_packing.mzn
  :language: minizinc
  :name: ex-rect-pack-dim
  :lines: 2

To decide whether the problem is satisfiable or not, we will have to find a
placement for the rectangles being packed. We can thus use the coordinates of
their left bottom corner as our decision variables, for which a type-inst
synonym can be defined as follows.

.. literalinclude:: examples/rect_packing/rect_packing.mzn
  :language: minizinc
  :name: ex-rect-pack-coord
  :lines: 3

To enforce that no rectangles overlap, it can be easy to start by writing a
predicate that ensures that two rectangles do not overlap. This predicate will
require both the information about the dimensions of the rectangle and the
coordinate decision variable. As such, it can be valuable to first combine these
records into a new record type. The following fragment shows the additional
type-inst synonym, and a definition for the described predicate.

.. literalinclude:: examples/rect_packing/rect_packing.mzn
  :language: minizinc
  :name: ex-rect-pack-pred
  :lines: 4-11

Using these definitions the remainder of the model is now easy to define as
shown below.

.. literalinclude:: examples/rect_packing/rect_packing.mzn
  :language: minizinc
  :name: ex-rect-pack-rem
  :lines: 12-
  :caption: Partial rectangle packing model using record types (full model: :download:`rect_packing.mzn <examples/rect_packing/rect_packing.mzn>` example data: :download:`rect_packing.json <examples/rect_packing/rect_packing.json>`). :playground:`rect_packing`
