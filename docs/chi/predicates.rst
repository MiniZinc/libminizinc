.. index::
  single: predicate
  single: function

.. _sec-predicates:

谓词和函数
========================

MiniZinc中的谓词允许我们用简洁的方法来表达模型中的复杂约束。
MiniZinc中的谓词利用预先定义好的全局约束建模，同时也让建模者
获取以及定义新的复杂约束。MiniZinc中的函数用来捕捉模型中的共同结构。
实际上，一个谓词就是一个输出类型为 :mzn:`var bool` 的函数。

.. _sec-globals:

全局约束
------------------

.. index::
  single: global constraint

MiniZinc中定义了很多可以在建模中使用的全局约束。
由于全局约束的列表一直在慢慢增加，最终确定的列表可以在发布的文档中找到。
下面我们讨论一些最重要的全局约束。


Alldifferent
~~~~~~~~~~~~

.. index::
  single: alldifferent
  single: global constraint; alldifferent

约束 :mzn:`alldifferent` 的输入为一个变量数组，它约束了这些变量取不同的值。
 :mzn:`alldifferent` 的使用有以下格式

.. code-block:: minizinc

  alldifferent(array[int] of var int: x)

即，参数是一个整型变量数组。

:mzn:`alldifferent` 是约束规划中被最多研究以及使用的全局约束之一。
它被用来定义分配子问题，人们也给出了 :mzn:`alldifferent` 的高效全局传播器。 :download:`send-more-money.mzn <examples/send-more-money.mzn>` (:numref:`ex-smm`) 和 :download:`sudoku.mzn <examples/sudoku.mzn>` (:numref:`ex-sudoku`) 是使用 :mzn:`alldifferent` 的模型例子。

Cumulative
~~~~~~~~~~

.. index::
  single: cumulative
  single: global constraint; cumulative

约束 :mzn:`cumulative` 被用来描述资源累积使用情况。

.. code-block:: minizinc

  cumulative(array[int] of var int: s, array[int] of var int: d, 
             array[int] of var int: r, var int: b)

规定对于一个起始时间为 :mzn:`s` ，持续时间为 :mzn:`d` 以及资源需求量为 :mzn:`r` 的任务集合，在任何时间对资源的需求量都不能超过一个全局资源量界限 :mzn:`b` 。

.. literalinclude:: examples/moving.mzn
  :language: minizinc
  :name: ex-moving
  :caption: 使用 ``cumulative`` 来建模搬运家具问题的模型 (:download:`moving.mzn <examples/moving.mzn>`).

.. literalinclude:: examples/moving.dzn
  :language: minizinc
  :name: ex-movingd
  :caption: 使用 ``cumulative`` 来建模搬运家具问题的数据   (:download:`moving.dzn <examples/moving.dzn>`).

:numref:`ex-moving` 中的模型为搬运家具规划一个行程表使得每一份家具在搬运的过程中都有足够的搬用工和足够的手推车可以使用。允许的时间，可以使用的搬运工以及手推车被给出，每个物体的搬运持续时间，需要的搬运工和手推车的数量等数据也被给出。使用 :numref:`ex-movingd` 中的数据，命令 

.. code-block:: bash

  $ minizinc moving.mzn moving.dzn

可能会得到如下输出

.. code-block:: none

  start = [0, 60, 60, 90, 120, 0, 15, 105]
  end = 140
  ----------
  ==========

:numref:`fig-histogram-a` and :numref:`fig-histogram-b` 
给出了这个解中搬运时每个时间点所需要的搬运工和手推车。

.. _fig-histogram-a:

.. figure:: figures/handlers.*
  
  搬运时搬运工使用量直方图

.. _fig-histogram-b:

.. figure:: figures/trolleys.*
  
  搬运时手推车使用量直方图

Table
~~~~~

.. index::
  single: table
  single: global constraint; table

约束 :mzn:`table` 强制变量元组从一个元组集合中取值。由于MiniZinc中没有元组，我们用数组来描述它。
根据元组是布尔型还是整型， :mzn:`table` 的使用有以下两种格式

.. code-block:: minizinc

  table(array[int] of var bool: x, array[int, int] of bool: t)
  table(array[int] of var int:  x, array[int, int] of int:  t)

强制约束了 :math:`x \in t` ，其中 :math:`x` 和 :math:`t` 中的每一行是元组， :math:`t` 是一个元组集合。

.. literalinclude:: examples/meal.mzn
  :language: minizinc
  :name: ex-meal
  :caption: 使用 ``table`` 约束来建模食物规划问题的模型 (:download:`meal.mzn <examples/meal.mzn>`).

.. literalinclude:: examples/meal.dzn
  :language: minizinc
  :name: ex-meald
  :caption: 定义 ``table`` 的食物规划的数据 (:download:`meal.dzn <examples/meal.dzn>`).

:numref:`ex-meal` 中的模型寻找均衡的膳食。每一个食物项都有一个名字（用整数表示），卡路里数，蛋白质克数，盐毫克数，脂肪克数以及单位为分的价钱。这些个项之间的关系用一个 :mzn:`table` 约束来描述。
模型寻找拥有最小花费，最少卡路里数 :mzn:`min_energy` ，最少蛋白质量 :mzn:`min_protein` ，最大盐分 :mzn:`max_salt` 以及脂肪 :mzn:`max_fat` 的膳食。

Regular
~~~~~~~

.. index::
  single: regular
  single: global constraint; regular

约束 :mzn:`regular` 用来约束一系列的变量取有限自动机定义的值。 :mzn:`regular` 的使用有以下方式 

.. code-block:: minizinc

  regular(array[int] of var int: x, int: Q, int: S,
          array[int,int] of int: d, int: q0, set of int: F)

它约束了 :mzn:`x` 中的一列值（它们必须是在范围 :index:`range`
:mzn:`1..S` 内）被一个有 :mzn:`Q` 个状态，输入为 :mzn:`1..S` ，转换函数为 :mzn:`d` （ :mzn:`<1..Q, 1..S>` 映射到 :mzn:`0..Q` ），初始状态为 :mzn:`q0` （必须在 :mzn:`1..Q` 中）和接受状态为 :mzn:`F`（必须在 :mzn:`1..Q` 中）的 :index:`DFA` 接受。
状态0被保留为总是会失败的状态。


.. _fig-dfa:

.. figure:: figures/dfa.*
  
  判定正确排班的DFA。

我们来看下护士排班问题。每一个护士每一天被安排为以下其中一种：(d)白班(n)夜班或者(o)休息。
每四天，护士必须有至少一天的休息。每个护士都不可以被安排为连续三天夜班。这个问题可以使用
:numref:`fig-dfa` 中的不完全DFA来表示。我们可以把这个DFA表示为初始状态是 :mzn:`1` ，结束状态是 :mzn:`1..6` ，转换函数为

.. cssclass:: table-nonfluid table-bordered

+---+---+---+---+
|   | d | n | o |
+===+===+===+===+
| 1 | 2 | 3 | 1 |
+---+---+---+---+
| 2 | 4 | 4 | 1 |
+---+---+---+---+
| 3 | 4 | 5 | 1 |
+---+---+---+---+
| 4 | 6 | 6 | 1 |
+---+---+---+---+
| 5 | 6 | 0 | 1 |
+---+---+---+---+
| 6 | 0 | 0 | 1 |
+---+---+---+---+

注意状态表中的状态0代表一个错误状态。 :numref:`ex-nurse` 中给出的模型为 :mzn:`num_nurses` 个护士 :mzn:`num_days` 天寻找一个排班，其中我们要求白天有 :mzn:`req_day` 个护士值班，晚上有 :mzn:`req_night` 个护士值班，以及每个护士至少有 :mzn:`min_night` 个夜班。

.. literalinclude:: examples/nurse.mzn
  :language: minizinc
  :name: ex-nurse
  :caption: 使用 ``regular`` 约束来建模的护士排班问题模型  (:download:`nurse.mzn <examples/nurse.mzn>`)

运行命令

.. code-block:: bash

  $ minizinc nurse.mzn nurse.dzn

找到一个给7个护士10天的排班，要求白天有3个人值班，夜晚有2个人值班，以及每个护士最少有2个夜班。
一个可能的结果是

.. code-block:: none

  o d n n o n n d o o
  d o n d o d n n o n
  o d d o d o d n n o
  d d d o n n d o n n
  d o d n n o d o d d
  n n o d d d o d d d
  n n o d d d o d d d
  ----------

另外一种regular约束是 :mzn:`regular_nfa` 。它使用NFA（没有 :mzn:`\epsilon` 弧）来定义regular表达式。此约束有以下格式

.. code-block:: minizinc

  regular_nfa(array[int] of var int: x, int: Q, int: S,
          array[int,int] of set of int: d, int: q0, set of int: F)

它约束了数组 :mzn:`x` 中的数值序列（必须在范围 :mzn:`1..S` 中）被含有 :mzn:`Q` 个状态，输入为 :mzn:`1..S` ，转换函数为 :mzn:`d` （映射 :mzn:`<1..Q, 1..S>` 到
:mzn:`1..Q` 的子集），初始状态为 :mzn:`q0` （必须在范围 :mzn:`1..Q` 中）以及接受状态为 :mzn:`F` （必须在范围 :mzn:`1..Q` 中）的 :index:`NFA` 接受。
在这里，我们没必要再给出失败状态0，因为转换函数可以映射到一个状态的空集。


定义谓词
-------------------

.. index::
  single: predicate; definition

MiniZinc的其中一个最强大的建模特征是建模者可以定义他们自己的高级约束。这就使得他们可以对模型进行抽象化和模块化。也允许了在不同的模型之间重新利用约束以及促使了用来定义标准约束和类型的特殊库应用的发展。


.. literalinclude:: examples/jobshop2.mzn
  :language: minizinc
  :name: ex-jobshop2
  :caption: 使用谓词的车间作业调度问题模型 (:download:`jobshop2.mzn <examples/jobshop2.mzn>`)

我们用一个简单的例子开始，回顾下前面章节中的车间作业调度问题。这个模型在 :numref:`ex-jobshop2` 中给出。我们感兴趣的项是 :mzn:`谓词` 项：

.. literalinclude:: examples/jobshop2.mzn
  :language: minizinc
  :lines: 12-13

它定义了一个新的约束用来约束起始时间为 :mzn:`s1` ，持续时间为 :mzn:`d1` 的任务不能和起始时间为 :mzn:`s2` ，持续时间为 :mzn:`d2` 的任务重叠。它可以在模型的任何（包含决策变量的） :index:`布尔型表达式 <expression; Boolean>` 可以出现的地方使用。

和谓词一样，建模者也可以定义只涉及到参数的新的约束。和谓词不一样的是，它们可以被用在条件表达式的测试中。它们被关键字 :mzn:`test` 定义。例如

.. code-block:: minizinc

  test even(int:x) = x mod 2 = 0;

.. \ignore{ % for capture for testing!
.. $ mzn-g12fd jobshop2.mzn jobshop.dzn
.. } % $

.. defblock:: 谓词定义

  .. index::
    single: predicate; definition

  使用以下形式的语句，我们可以定义谓词

  .. code-block:: minizincdef
  
    predicate <谓词名> ( <参数定义>, ..., <参数定义> ) = <布尔表达式>

  :mzndef:`<谓词名>` 必须是一个合法的MiniZinc标识符，每个 :mzndef:`<参数定义>` 都是一个合法的MiniZinc类型 :index:`type` 声明。

  .. \ignore{The type-insts\index{type-inst} 
  .. of arguments may include type-inst variables\index{type-inst!variable} 
  .. which are of the
  .. form \texttt{\$T} or \texttt{any \$T} with \texttt{T} an identifier. A type-inst
  .. variable \texttt{\$T}\ttindexdef{\$T} 
  .. can match any fixed type-inst, whereas a type-inst
  .. variable \texttt{any \$T} can 
  .. also match non-fixed type-insts\index{type-index!non-fixed} 
  .. (such as \texttt{var int}).
  .. Because predicate arguments\index{argument} 
  .. receive an assignment when calling the predicate, the
  .. argument type-insts may include 
  .. implicitly indexed arrays\index{array!index set!implicit}, 
  .. as well as set variables with a
  .. non-finite element type.}

  :index:`参数` 定义的一个松弛是数组的索引类型可以是 :index:`没有限制地 <array; index set; unbounded>` 写为 :mzn:`int` 。

  类似的，使用以下形式的语句，我们定义测试

  .. code-block:: minizincdef
  
    test <谓词名> ( <参数定义>, ..., <参数定义> ) = <布尔表达式>

  其中的 :mzndef:`<布尔表达式>` 必须是固定的。

  另外我们介绍一个谓词中使用到的 :mzn:`assert` 命令的新形式。 

  .. code-block:: minizincdef
  
    assert ( <布尔表达式>, <字符串表达式>, <表达式> )

  :mzn:`assert` 表达式的类型和最后一个参数的类型一样。 :mzn:`assert` 表达式检测第一个参数是否为假，如果是则输出第二个参数字符串。如果第一个参数是真，则输出第三个参数。

注意 :index:`assert表达式 <expression; assert>` 中的第三个参数是延迟的，即如果第一个参数是假，它就不会被评估。所以它可以被用来检查

.. code-block:: minizinc

  predicate lookup(array[int] of var int:x, int: i, var int: y) = 
      assert(i in index_set(x), "index out of range in lookup"
             y = x[i]
      );

此代码在 :mzn:`i` 超出数组 :mzn:`x` 的范围时不会计算 :mzn:`x[i]` 。

定义函数
------------------

.. index::
  single: function; definition

MiniZinc中的函数和谓词一样定义，但是它有一个更一般的返回类型。

下面的函数定义了一个数独矩阵中的第 :math:`a^{th}` 个子方块的第 :math:`a1^{th}` 行。

.. code-block:: minizinc

  function int: posn(int: a, int: a1) = (a-1) * S + a1;

有了这个定义之后，我们可以把 :numref:`ex-sudoku` 中的数独问题的最后一个约束替换为

.. code-block:: minizinc

  constraint forall(a, o in SubSquareRange)( 
                    alldifferent([ puzzle [ posn(a,a0), posn(o,o1) ] | 
                                           a1,o1 in SubSquareRange ] ) );

函数对于描述模型中经常用到的复杂表达式非常有用。
例如，想象下在 :math:`n \times n` 的方格的不同位置上放置数字1到 :math:`n` 使得任何两个数字 :math:`i` 和 :math:`j` 之间的曼哈顿距离比这两个数字其中最大的值减一还要大。我们的目的是最小化数组对之间的总的曼哈顿距离。曼哈顿距离函数可以表达为：

.. literalinclude:: examples/manhattan.mzn
  :language: minizinc
  :lines: 12-14

完整的模型在 :numref:`ex-manhattan` 中给出。


.. literalinclude:: examples/manhattan.mzn
  :language: minizinc
  :name: ex-manhattan
  :caption: 阐释如何使用函数的数字放置问题模型 (:download:`manhattan.mzn <examples/manhattan.mzn>`).

.. defblock:: 函数定义

  .. index::
    single: function; definition

  函数用以下格式的语句定义 
  
  .. code-block:: minizincdef

    function <返回类型> : <函数名> ( <参数定义>, ..., <参数定义> ) = <表达式>

  :mzndef:`<函数名>` 必须是一个合法的MiniZinc标识符。每一个 :mzndef:`<参数定义>` 是一个合法的MiniZinc类型声明。 :mzndef:`<返回类型>` 是函数的返回类型，它必须是 :mzndef:`<表达式>` 的类型。参数和谓词定义中的参数有一样的限制。

MiniZinc中的函数可以有任何返回类型，而不只是固定的返回类型。
在定义和记录多次出现在模型中的复杂表达式时，函数是非常有用的。

反射函数
--------------------

为了方便写出一般性的测试和谓词，各种反射函数会返回数组的下标集合，var集合的定义域以及决策变量范围的信息。关于下标集合的有以下反射函数
:mzndef:`index_set(<1-D array>)`,
:mzndef:`index_set_1of2(<2-D array>)`,
:mzndef:`index_set_2of2(<2-D array>)`,
以及关于更高维数组的反射函数。

车间作业问题的一个更好的模型是把所有的对于同一个机器上的不重叠约束结合为一个单个的析取约束。
这个方法的一个优点是虽然我们只是初始地把它建模成一个 :mzn:`non-overlap` 约束的连接，但是如果下层的求解器对于解决析取约束有一个更好的方法，在对我们的模型最小改变的情况下，我们可以直接使用它。这个模型在 :numref:`ex-jobshop3` 中给出。

.. literalinclude:: examples/jobshop3.mzn
  :language: minizinc
  :name: ex-jobshop3
  :caption: 使用 ``disjunctive`` 谓词的车间作业调度问题模型 (:download:`jobshop3.mzn <examples/jobshop3.mzn>`).

.. index::
  single: global constraint; disjunctive

约束 :mzn:`disjunctive` 获取每个任务的开始时间数组以及它们的持续时间数组，确保每次只有一个任务是被激活的。
我们定义析取约束为一个有以下特征的 :index:`谓词 <predicate; definition>`

.. code-block:: minizinc

  predicate disjunctive(array[int] of var int:s, array[int] of int:d);

在 :numref:`ex-jobshop3` 中，我们可以用这个析取约束定义任务之间不重叠。
我们假设 :mzn:`disjunctive` 谓词的定义已经在模型中引用的文件 :download:`disjunctive.mzn <examples/disjunctive.mzn>` 中给出。

如果下层的系统直接支持 :mzn:`disjunctive` ，则会在它的全局目录下包含一个 :download:`disjunctive.mzn <examples/disjunctive.mzn>` 文件（拥有上述特征定义内容）。
如果我们使用的系统不直接支持析取，通过创建文件 :download:`disjunctive.mzn <examples/disjunctive.mzn>` ，我们可以给出我们自己的定义。最简单的实现是单单使用上面定义的 :mzn:`no_overlap` 谓词。
一个更好的实现是利用全局约束 :mzn:`cumulative` ，假如下层求解器支持它的话。 :numref:`ex-disj` 给出了一个 :mzn:`disjunctive` 的实现。
注意我们使用 :mzn:`index_set` 反射函数来（a）检查 :mzn:`disjunctive` 的参数是有意义的，以及（b）构建 :mzn:`cumulative` 的合适大小的资源利用数组。另外注意这里我们使用了 :mzn:`assert` 的三元组版本。

.. literalinclude:: examples/disjunctive.mzn
  :language: minizinc
  :name: ex-disj
  :caption: 使用 ``cumulative`` 来定义一个 ``disjunctive`` 谓词  (:download:`disjunctive.mzn <examples/disjunctive.mzn>`).

.. \ignore{ % for capture for testing!
.. $ mzn-g12fd jobshop3.mzn jobshop.dzn
.. } % $



局部变量
---------------

.. index::
  single: variable; local
  single: let

在谓词，函数或者测试中，引进 *局部变量* 总是非常有用的。
表达式 :mzn:`let` 允许你去这样做。
它可以被用来引进决策变量 :index:`决策变量 <variable>` 和 :index:`参数 <parameter>` ，但是参数必须被初始化。
例如：

.. code-block:: minizinc

  var s..e: x;
  let {int: l = s div 2; int: u = e div 2; var l .. u: y;} in x = 2*y

引进了参数 :mzn:`l` 和 :mzn:`u` 以及变量 :mzn:`y` 。
:mzn:`let` 表达式虽然在 :index:`谓词` ， :index:`函数` 和测试定义中最有用，它也可以被用在其他的表达式中。例如，来消除共同的子表达式：

.. code-block:: minizinc

  constraint let { var int: s = x1 + x2 + x3 + x4 } in
             l <= s /\ s <= u;

局部变量可以被用在任何地方，在简化复杂表达式时也很有用。
通过使用局部变量来定义目标 :index:`objective` 函数而不是显式地加入很多个变量， :numref:`ex-wedding2` 给出了稳定婚姻模型的一个改进版本。


.. literalinclude:: examples/wedding2.mzn
  :language: minizinc
  :name: ex-wedding2
  :caption: 使用局部变量来定义一个复杂的目标函数 (:download:`wedding2.mzn <examples/wedding2.mzn>`).


语境
-------

.. index::
  single: context
  single: context; negative
  single: predicate
  single: function

有一个局限，即含有决策变量并且在声明时没有初始化的谓词和函数不可以被用在一个否定语境下。下面例子是非法的

.. code-block:: minizinc

  predicate even(var int:x) = 
            let { var int: y } in x = 2 * y;

  constraint not even(z);

原因是求解器只解决存在约束的问题。如果我们在否定语境下引入了一个局部变量，则此变量是 *普遍地量化* 了，因此超出下层求解器的解决范围。例如， :math:`\neg \mathit{even}(z)` 等价于 :math:`\neg \exists y. z = 2y` ，然后等价于 :math:`\forall y. z \neq 2y` 。

如果局部变量被赋了值，则它们可以被用在否定语境中。下面的例子是合法的

.. code-block:: minizinc

  predicate even(var int:x) = 
            let { var int: y = x div 2; } in x = 2 * y;

  constraint not even(z);

注意，现在 :mzn:`even` 的意思是正确的，因为如果 :mzn:`x` 是偶数，则 :math:`x = 2 * (x ~\mbox{div}~ 2)` 。
由于 math:`y` 被 :math:`z` 功能性定义了， :math:`\neg \mathit{even}(z)` 等价于 :math:`\neg \exists y. y = z ~\mbox{div}~ 2 \wedge z = 2y` ，同时等价于 :math:`\exists y. y = z ~\mbox{div}~ 2 \wedge \neg z \neq 2y`。

MiniZinc中的任意表达式都出现在以下四种 *语境* 中的一种中： :index:`根 <context; !root>` ， :index:`肯定 <context; !positive>` ， :index:`否定 <context; !negative>` ，或者 :index:`混合 <context; !mixed>` 。
非布尔型表达式的语境直接地为包含其最近的布尔型表达式的语境。唯一的例外是目标表达式出现在一个根语境下（由于它没有包含其的布尔型表达式）。

为了方便定义语境，我们把蕴含表达式 :mzn:`e1 -> e2` 等价地写为 :mzn:`not e1 \/ e2` ， :mzn:`e1 <- e2` 等价地写为 :mzn:`e1 \/ not e2` 。

一个布尔型表达式的语境可能有：

根
  根语境是任何作为 :mzn:`constraint` 的参数或者作为一个赋值项 :index:`assignment` 出现的表达式 :math:`e` 的语境，或者作为一个出现在根语境中的 :mzn:`e1 /\ e2` 的子表达式 :mzn:`e1` 或 :mzn:`e2` 的语境。

  根语境下的布尔型表达式必须在问题的任何模型中都满足。 

肯定
  肯定语境是任何作为一个出现在根语境或者肯定语境中的 :mzn:`e1 \/ e2` 的子表达式 :mzn:`e1` 或 :mzn:`e2` 的语境，或者是作为一个出现在肯定语境中的 :mzn:`e1 /\ e2` 的子表达式 :mzn:`e1` 或 :mzn:`e2`的语境，或者是作为一个出现在否定语境中的 :mzn:`not e` 的子表达式 :mzn:`e` 的语境。

  肯定语境下的布尔型表达式不是必须要在模型中满足，但是满足它们会增加包含其的约束被满足的可能性。对于一个肯定语境下的表达式，从包含其的根语境到此表达式有偶数个否定。

否定
  否定语境是任何作为一个出现在根语境或者否定语境中的 :mzn:`e1 \/ e2` 或 :mzn:`e1 /\ e2` 的子表达式 :mzn:`e1` 或 :mzn:`e2` ，或者是作为一个出现在肯定语境中的 :mzn:`not e` 的子表达式 :mzn:`e` 的语境。

  否定语境下的布尔型表达式不是必须要满足，但是让它们成假会增加包含其的约束被满足的可能性。对于一个否定语境下的表达式，从包含其的根语境到此表达式有奇数个否定。

混合
  混合语境是任何作为一个出现在 :mzn:`e1 <-> e2` , :mzn:`e1 = e2`  或者 :mzn:`bool2int(e)` 中的子表达式 :mzn:`e1` 或 :mzn:`e2` 的语境。

  混合语境下的表达式实际上既是肯定也是否定的。通过以下可以看出：:mzn:`e1 <-> e2` 等价于 :mzn:`(e1 /\ e2) \/ (not e1 /\ not e2)` 以及 :mzn:`x = bool2int(e)` 等价于 :mzn:`(e /\ x=1) \/ (not e /\ x=0)` 。

观察以下代码段

.. code-block:: minizinc

  constraint x > 0 /\ (i <= 4 -> x + bool2int(x > i) = 5);

其中 :mzn:`x > 0` 在根语境中， :mzn:`i <= 4}` 在否定语境中，
:mzn:`x + bool2int(x > i) = 5` 在肯定语境中， :mzn:`x > i` 在混合语境中。

局部约束
-----------------

.. index::
  single: constraint; local

Let表达式也可以被用来引入局部约束，通常用来约束局部变量的行为。
例如，考虑只利用乘法来定义开根号函数：

.. code-block:: minizinc

  function var float: mysqrt(var float:x) = 
           let { var float: y;
                 constraint y >= 0;
                 constraint x = y * y; } in y;

局部约束确保了 :mzn:`y` 取正确的值；而此值则会被函数返回。


局部约束可以在let表达式中使用，尽管最普遍的应用是在定义函数时。


.. defblock:: Let表达式

  .. index::
    single: expression; let

  :index:`局部变量 <variable;local>` 可以在任何以下格式的\emph{let表达式}中引入：
  
  .. code-block:: minizincdef
  
    let { <声明>; ... <声明> ; } in <表达式>
  
  声明 :mzndef:`<dec>` 可以是决策变量或者参数（此时必须被初始化）或者约束项的声明。
  任何声明都不能在一个新的声明变量还没有引进时使用它。

  注意局部变量和约束不可以出现在测试中。局部变量不可以出现在 :index:`否定 <context; negative>` 或者 :index:`混合 <context; mixed>` 语境下的谓词和函数中，除非这个变量是用表达式定义的。 

定义域反射函数
---------------------------

.. index::
  single: domain; reflection

其他重要的反射函数有允许我们对变量定义域进行访问的函数。表达式 :mzn:`lb(x)` 返回一个小于等于 :mzn:`x` 在一个问题的解中可能取的值的数值。
通常它会是 :mzn:`x` 声明的下 :index:`限 <variable; bound>` 。如果 :mzn:`x` 被声明为一个非有限类型，例如，
只是 :mzn:`var int` ，则它是错误的。
类似地，表达式 :mzn:`dom(x)` 返回一个$x$在问题的任何解中的可能值的（非严格）超集。
再次，它通常是声明的值，如果它不是被声明为有限则会出现错误。

.. \ignore{ % for capture for testing!
.. $ mzn-g12fd reflection.mzn
.. } % $


.. literalinclude:: examples/reflection.mzn
  :language: minizinc
  :name: ex-reflect
  :caption: 使用反射谓词 (:download:`reflection.mzn <examples/reflection.mzn>`).

例如， :numref:`ex-reflect` 中的模型或者输出

.. code-block:: none

  y = -10
  D = -10..10
  ----------

或

.. code-block:: none

  y = 0
  D = {0, 1, 2, 3, 4}
  ----------

或任何满足
:math:`-10 \leq y \leq 0` 和 
:math:`\{0, \ldots, 4\} \subseteq D \subseteq \{-10, \ldots, 10\}` 的答案。

变量定义域反射表达式应该以在任何安全近似下都正确的的方式使用。但是注意这个是没有被检查的！例如加入额外的代码

.. code-block:: minizinc

  var -10..10: z;
  constraint z <= y;

不是一个定义域信息的正确应用。因为使用更紧密（正确的）近似会比使用更弱的初始近似产生更多的解。

.. TODO: this sounds wrong!

.. defblock:: 定义域反射

  .. index::
    single: domain; reflection

  我们有查询包含变量的表达式的可能值的反射函数：
  
  - :mzndef:`dom(<表达式>)`
    返回 :mzndef:`<表达式>` 所有可能值的安全近似。
  - :mzndef:`lb(<表达式>)`
    返回 :mzndef:`<表达式>` 下限值的安全近似。
  - :mzndef:`ub(<表达式>)`
    返回 :mzndef:`<表达式>` 上限值的安全近似。

  :mzn:`lb` 和 :mzn:`ub` 的表达式必须是 :mzn:`int` ， :mzn:`bool` ，
  :mzn:`float` 或者 :mzn:`set of int` 类型。
  :mzn:`dom` 中表达式的类型不能是 :mzn:`float` 。
  如果 :mzndef:`<表达式>` 中的一个变量有一个 :index:`非有限声明类型 <type; non-finite>` （例如， :mzn:`var int` 或 :mzn:`var float` 类型），则会出现一个错误。

  我们也有直接作用于表达式数组的版本（有类似的限制）：

  - :mzndef:`dom_array(<数组表达式>)`:
    返回数组中出现的表达式的所有可能值的并集的一个安全近似。 
  - :mzndef:`lb_array(<数组表达式>)`
    返回数组中出现的所有表达式的下限的安全近似。
  - :mzndef:`ub_array(<数组表达式>)`
    返回数组中出现的所有表达式的下限的安全近似。

谓词，局部变量和定义域反射的结合使得复杂全局约束通过分解定义变为可能。
利用 :numref:`ex-cumul` 中的代码，我们可以定义 :mzn:`cumulative` 约束的根据时间的分解。

.. literalinclude:: examples/cumulative.mzn
  :language: minizinc
  :name: ex-cumul
  :caption: 利用分解来定义一个 ``谓词`` (:download:`cumulative.mzn <examples/cumulative.mzn>`).

这个分解利用 :mzn:`lb` 和 :mzn:`ub` 来决定任务可以执行的时间范围集合。
接下来，它对 :mzn:`times` 中的每个时间 :mzn:`times` 都断言在此时间 :mzn:`t`  激活的所有任务所需要的资源量总和小于界限 :mzn:`b` 。

作用域
---------------------------

.. index::
  single: scope

MiniZinc中声明的作用域值得我们简单地介绍下。
MiniZinc只有一个作用域，所以出现在声明中的所有变量都可以在模型中的每个表达式中可见。
用以下几个方式，MiniZinc引进局部作用域变量：

- :index:`推导式表达式 <variable; iterator>` 中的 :index:`迭代器` 
- 使用 :mzn:`let` 表达式 
- 谓词和函数中的 :index:`参数 <argument>`

任何局部作用域变量都会覆盖同名称的外部作用域变量。

.. literalinclude:: examples/scope.mzn
  :language: minizinc
  :name: ex-scope
  :caption: 阐述变量作用域的模型 (:download:`scope.mzn <examples/scope.mzn>`).

例如，在 :numref:`ex-scope` 中给出的模型中， :mzn:`-x <= y` 中的 :mzn:`x` 是全局 :mzn:`x` ， :mzn:`smallx(x)` 中的 :mzn:`x` 是迭代器 :mzn:`x in 1..u` ，而析取中的 :mzn:`y` 是谓词的第二个参数。
