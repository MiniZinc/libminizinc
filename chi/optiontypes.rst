.. _sec-optiontypes:

选项类型
============

.. index::
  single: option types

选项类型是一个强大的抽象，使得简洁建模成为可能。一个选项类型决策变量代表了一个有其他可能 :math:`\top` 的变量，在MiniZinc中表达为 :mzn:`<>` ，代表了这个变量是 *缺失的* 。选项类型变量在建模一个包含在其他变量没做决定之前不会有意义的变量的问题时是很有用的。

声明和使用选项类型
--------------------------------

.. defblock:: 选项类型变量

  .. index::
    single: variable; option type

  一个选项类型变量被声明为： 
  
  .. code-block:: minizincdef

    var opt <类型> : <变量名>:

  其中 :mzndef:`<类型>` 是 :mzn:`int` ， :mzn:`float` 或 :mzn:`bool` 中的一个，或者是一个固定范围的表达式。选项类型变量可以是参数，但是这个不常用。 

  一个选项类型变量可以有附加值 :mzn:`<>` 表明它是 *缺失的* 。

  三个内建函数被提供给选项类型变量： :mzn:`absent(v)` 只有在选项类型变量 :mzn:`v` 取值 :mzn:`<>` 时返回 :mzn:`true` ， :mzn:`occurs(v)` 只有在选项类型变量 :mzn:`v` *不* 取值 :mzn:`<>` 时返回 :mzn:`true` ， 以及 :mzn:`<>` 返回 :mzn:`v` 的正常值或者当它取值 :mzn:`<>` 时返回失败。

选项类型最常被用到的地方是调度中的可选择任务。在灵活的车间作业调度问题中，我们有 :mzn:`n` 个在 :mzn:`k` 个机器上执行的任务，其中完成每个机器上每个任务的时间可能是不一样的。我们的目标是最小化所有任务的总完成时间。一个使用选项类型来描述问题的模型在 :numref:`ex-flexible-js` 中给出。在建模这个问题的时候，我们使用 :math:`n \times k` 个可选择的任务来代表每个机器上每个任务的可能性。
我们使用 :mzn:`alternative` 全局约束来要求任务的起始时间和它的持续时间跨越了组成它的可选择任务的时间，同时要求只有一个会实际运行。
我们使用 :mzn:`disjunctive` 全局变量在每个机器上最多有一个任务在运行，这里我们延伸到可选择的任务。最后我们约束任何时候最多有$k$个任务在运行，利用一个在实际（不是可选择的）任务上作用的冗余约束。


.. literalinclude:: examples/flexible-js.mzn
  :language: minizinc
  :name: ex-flexible-js
  :caption: 使用选项类型的灵活车间作业调度模型 (:download:`flexible-js.mzn <examples/flexible-js.mzn>`).
  
.. \pjs{Finish the damn section!}

隐藏选项类型
-------------------

当列表推导式是从在变量集合迭代上创建而来，或者 :mzn:`where` 从句中的表达式还没有固定时，选项类型变量会隐式地出现。

例如，模型片段

.. code-block:: minizinc

  var set of 1..n: x;
  constraint sum(i in x)(i) <= limit;

是以下的语法糖

.. code-block:: minizinc

  var set of 1..n: x;
  constraint sum(i in 1..n)(if i in x then i else <> endif) <= limit;

内建函数 :mzn:`sum` 实际上在一列类型-实例化 :mzn:`var opt int` 上操作。由于 :mzn:`<>` 在+中表现为标识0，我们会得到期望的结果。

类似地，模型片段

.. code-block:: minizinc

  array[1..n] of var int: x;  
  constraint forall(i in 1..n where x[i] >= 0)(x[i] <= limit);

是以下的语法糖

.. code-block:: minizinc

  array[1..n] of var int: x;  
  constraint forall(i in 1..n)(if x[i] >= 0 then x[i] <= limit else <> endif);

同样地，函数 :mzn:`forall` 实际上在一列类型-实例化 :mzn:`var opt bool` 上操作。由于 :mzn:`<>` 在 :mzn:`/\ ` 上表现为标识 :mzn:`true` ，我们可以得到期望的结果。

尽管我们已经很小心了，隐式的使用可能会导致意外的行为。观察

.. code-block:: minizinc

  var set of 1..9: x;
  constraint card(x) <= 4;
  constraint length([ i | i in x]) > 5;
  solve satisfy;

它本应该是一个不可满足的问题。它返回 :mzn:`x = {1,2,3,4}` 作为一个解例子。这个是正确的因为第二个约束等于

.. code-block:: minizinc

  constraint length([ if i in x then i else <> endif | i in 1..9 ]) > 5;

而可选择整数列表的长度总会是9，所以这个约束总是会满足。

我们可以通过不在变量集合上创建迭代或者使用不固定的 :mzn:`where` 从句来避免隐式的选项类型。
例如，上面的两个例子可以不使用选项类型重写为

.. code-block:: minizinc

  var set of 1..n: x;
  constraint sum(i in 1..n)(bool2int(i in x)*i) <= limit;

和

.. code-block:: minizinc
  
  array[1..n] of var int: x;  
  constraint forall(i in 1..n)(x[i] >= 0 -> x[i] <= limit);


