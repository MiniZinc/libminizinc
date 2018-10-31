.. _sec-efficient:

MiniZinc中的有效建模实践
=========================================

对同一个问题，几乎总是存在多种方式来建模。其中一些产生的模型可以很有效地求解，另外一些则不是。通常情况下，我们很难提前判断哪个模型是对解决一个特定的问题最有效的。事实上，这或许十分依赖于我们使用的底层求解器。在这一章中，我们专注于建模实践，来避免产生模型的过程和产生的模型低效。

变量界限
---------------

.. index::
  single: variable; bound

有限域传播器，是MiniZinc所针对的求解器中的核心类型。在当其所涉及到的变量的界限越紧凑时，此传播器越有效。
它也会当问题含有会取很大整型数值的子表达式时表现得很差，因为它们可能会隐式地限制整型变量的大小。


.. literalinclude:: examples/grocery.mzn
  :language: minizinc
  :name: ex-grocery
  :caption: 没有无界整数的模型 (:download:`grocery.mzn <examples/grocery.mzn>`).

在 :numref:`ex-grocery` 中的中的杂货店问题要找寻4个物品使得它们的价格加起来有7.11元并且乘起来也有7.11元。变量被声明为无界限。运行

.. code-block:: bash

  $ minizinc --solver g12fd grocery.mzn

得到

.. code-block:: none

  =====UNSATISFIABLE=====
  % /tmp/mznfile85EWzj.fzn:11: warning: model inconsistency detected before search.

这是因为乘法中的中间表达式的类型也会是 :mzn:`var int` ，也会被求解器给一个默认的界限 :math:`-1,000,000 \dots 1,000,000` 。但是这个范围太小了以至于不能承载住中间表达式所可能取的值。

更改模型使得初始变量都被声明为拥有更紧致的界限

.. code-block:: minizinc

  var 1..711: item1;
  var 1..711: item2;
  var 1..711: item3;
  var 1..711: item4;

我们得到一个更好的模型，因为现在MiniZinc可以推断出中间表达式的界限，并且使用此界限而不是默认的界限。在做此更改后，求解模型我们得到

.. code-block:: none

  {120,125,150,316}
  ----------
 
注意，就算是改善的模型也可能对于某些求解器来说会很难解决。
运行

.. code-block:: bash

  $ minizinc --solver g12lazy grocery.mzn

不能得到任何结果，因为求解器给中间产生的变量创建了巨大的表示。

.. defblock:: 给变量加界限

  .. index::
    single: variable; bound

  在模型中要尽量使用有界限的变量。当使用 :mzn:`let` 声明来引进新的变量时，始终尽量给它们定义正确的和紧凑的界限。这会使得你的模型更有效率，避免出现意外溢出的可能性。
  一个例外是当你引进一个新的变量然后立刻定义它等于一个表达式，通常MiniZinc都可以从此表达式推断出此变量有效的界限。

有效的生成元
--------------------

.. index::
  single: generator

想象下我们想要计算在一个图中出现的三角形的个数（ :math:`K_3` 子图）。
假设此图由一个邻接矩阵定义：如果点$i$和$j$邻接，则 :mzn:`adj[i,j]` 为真。
我们或许可以写成

.. code-block:: minizinc

  int: count = sum ([ 1 | i,j,k in NODES where i < j  /\ j < k 
                         /\ adj[i,j] /\ adj[i,k] /\ adj[j,k]]);

这当然是对的，但是它检查了所有点可能组成的三元组。
如果此图是稀疏的，在意识到一旦我们选择了 :mzn:`i` 和 :mzn:`j` ，就可以进行一些测试之后，我们可以做得更好。

.. code-block:: minizinc

  int: count = sum ([ 1 | i,j in NODES where i < j  /\ adj[i,j],
                          k in NODES where j < k /\ adj[i,k] /\ adj[j,k]]);

你可以使用内建 :mzn:`trace` :index:`函数 <trace>` 来帮助决定在生成元内发生了什么。

.. defblock:: 追踪
  
  函数 :mzn:`trace(s,e)` 在对表达式 :mzn:`e` 求值并返回它的值之前就输出字符串 :mzn:`s` 。它可以在任何情境下使用。

例如，我们可以查看在两种计算方式下的内部循环中分别进行了多少次测试。

.. literalinclude:: examples/count1.mzn
  :language: minizinc
  :lines: 8-15

得到输出：

.. code-block:: none

  ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  ----------

表示内部循环进行了64次，而

.. literalinclude:: examples/count2.mzn
  :language: minizinc
  :lines: 13-14

得到输出

.. code-block:: none

  ++++++++++++++++
  ----------

表示内部循环进行了16次。

注意你可以在 :mzn:`trace` 中使用单独的字符串来帮助你理解模型创建过程中发生了什么。 

.. literalinclude:: examples/count3.mzn
  :language: minizinc
  :lines: 13-15

会输出在计算过程中找到的每个三角形。得到输出

.. code-block:: none

  (1,2,3)
  ----------

我们要承认这里我们有一点点作弊: 在某些情况下, MiniZinc编译器实际上会把 ``where`` 从句中的参数自动重新排序, 所以他们会尽快地被计算。在这种情况下, 加入 ``trace`` 函数实际上 *阻止* 了这种优化. 一般来说, 通过分离 ``where`` 从句把它们摆到尽量接近生成元, 这其实是一个很好的主意来帮助编译器运作正常。


冗余约束
---------------------

.. index::
  single: constraint; redundant

模型的形式会影响约束求解器求解它的效率。
在很多情况下加入冗余约束，即被现有的模型逻辑上隐含的约束，可能会让求解器在更早时候产生更多可用的信息从而提高找寻解的搜索效率。

回顾下第 :ref:`sec-complex` 节中的魔术串问题。

运行 :mzn:`n = 16` 时的模型： 

.. code-block:: bash

  $ minizinc --all-solutions --statistics magic-series.mzn -D "n=16;"

:mzn:`n = 16`

.. code-block:: none

  s = [12, 2, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0];
  ----------
  ==========

统计数据显示需要有89个失败。

我们可以在模型中加入冗余约束。由于序列中的每个数字是用来计算某一个数字出现的次数，我们知道它们的和肯定是 :mzn:`n` 。
类似地，由于这个序列是魔术的，我们知道 :mzn:`s[i] * i` 的和肯定也是 :mzn:`n` 。
使用如下方式把这些约束加入我们的模型 :numref:`ex-magic-series2`.

.. literalinclude:: examples/magic-series2.mzn
  :language: minizinc
  :name: ex-magic-series2
  :caption: 使用冗余约束求解魔术串问题模型 (:download:`magic-series2.mzn <examples/magic-series2.mzn>`).

像之前那样求解同一个问题

.. code-block:: bash

  $ minizinc --all-solutions --statistics magic-series2.mzn -D "n=16;"

产生了同样的输出。但是统计显示只搜索了13个决策点。这些冗余约束使得求解器更早地去剪枝。


模型选择
-----------------

在MiniZinc中有很多方式去给同一个问题建模，尽管其中有些模型或许会比另外的一些模型更自然。
不同的模型或许会产生不同的求解效率。更糟糕的是，在不同的求解后端中，不同的模型或许会更好或更差。
但是，我们还是可以给出一些关于普遍情况下产生更好的模型的指导:

.. defblock:: 模型之间的选择

  一个好的模型倾向于有以下特征

  - 更少量的变量，或者至少是更少量的没有被其他变量功能上定义的变量。
  - 更小的变量定义域范围
  - 模型的约束定义更简洁或者直接
  - 尽可能地使用全局约束

  实际情况中，所有这些都需要通过检查这个模型的搜索到底多有效率来断定模型好坏。通常除了用实验之外，我们很难判断搜索是否高效。

观察如下问题，我们要找寻1到 :math:`n` 这 :math:`n` 个数字的排列，使得相邻数字的差值也形成一个1到 :math:`n` 的排列。
:numref:`ex-allint` 中给出了一个用直观的方式来建模此问题的模型。注意变量 :mzn:`u` 被变量 :mzn:`x` 功能性定义。所以最差情况下的搜索空间是 :math:`n^n`。

.. literalinclude:: examples/allinterval.mzn
  :language: minizinc
  :name: ex-allint
  :caption: 对于CSPlib ``prob007`` 所有间隔系列问题的模型 (:download:`allinterval.mzn <examples/allinterval.mzn>`).

在这个模型中，数组 :mzn:`x` 代表 :mzn:`n` 个数字的排序。约束自然地可用 :mzn:`alldifferent` 来表示。

求解模型

.. code-block:: bash

  $ minizinc --solver g12fd --all-solutions --statistics allinterval.mzn -D "n=10;"

在84598个决策点和3秒的时间内找到了所有的解。

另外一个模型是使用数组 :mzn:`y` ，其中 :mzn:`y[i]` 代表数字 :mzn:`i` 在序列中的位置。
我们同时也使用变量 :mzn:`v` 来建模表示差的位置。 :mzn:`v[i]` 表示了绝对值差 :mzn:`i` 在序列出现的位置。
如果 :mzn:`y[i]` 和 :mzn:`y[j]` 差别为一，其中 :mzn:`j > i` ，则代表了它们的位置是相邻的。所以 :mzn:`v[j-i]` 被约束为两个位置中最早的那个。
我们可以给这个模型加入两个冗余约束：由于我们知道差值 :mzn:`n-1` 肯定会产生，我们就可以推断出1和 :mzn:`n` 的位置必须是相邻的 :mzn:`abs( y[1] - y[n] ) = 1` 。同时也告诉我们差值 :mzn:`n-1` 的位置就是在 :mzn:`y[1]` 和 :mzn:`y[n]` 中的最早的那个位置，即 :mzn:`v[n-1] = min(y[1], y[n])` 。有了这些之后，我们可以建模此问题为:numref:`ex-allint2` 。
输出语句从位置数组 :mzn:`y` 里重现了原本的序列 :mzn:`x` 。

.. literalinclude:: examples/allinterval2.mzn
  :language: minizinc
  :name: ex-allint2
  :caption: CSPlib中全区间序列问题 ``prob007`` 的一个逆向模型。 (:download:`allinterval2.mzn <examples/allinterval2.mzn>`).

逆向模型跟初始模型有同样的变量和定义域大小。但是相对于给变量 :mzn:`x` 和 :mzn:`u` 的关系建模，逆向模型使用了一个更加非直接的方式来给变量 :mzn:`y` 和 :mzn:`v` 的关系建模。所以我们或许期望初始模型更好些。

命令

.. code-block:: bash

  $ minizinc --solver g12fd --all-solutions --statistics allinterval2.mzn -D "n=10;"

在75536个决策点和18秒内找到了所有的解。
有趣的是，尽管这个模型不是简洁的，在变量 :mzn:`y` 上搜索比在变量 :mzn:`x` 上搜索更加有效率。
简洁的缺乏意味着尽管搜索需要更少的决策点，但是在时间上实质上会更慢。

.. _sec-multiple-modelling-and-channels:

多重建模和连通
-------------------------------

当我们对同一个问题有两个模型时，由于每个模型可以给求解器不同的信息，通过把两个模型中的变量系到一起从而同时使用两个模型或许对我们是有帮助的。

.. literalinclude:: examples/allinterval3.mzn
  :language: minizinc
  :name: ex-allint3
  :caption: CSPlib中全区间序列问题 ``prob007`` 的一个双重模型。 (:download:`allinterval3.mzn <examples/allinterval3.mzn>`).

:numref:`ex-allint3` 给出了一个结合 :download:`allinterval.mzn <examples/allinterval.mzn>` 和 :download:`allinterval2.mzn <examples/allinterval2.mzn>` 特征的双重模型。
模型的开始来自于 :download:`allinterval.mzn <examples/allinterval.mzn>` 。我们接着介绍了来自于 :download:`allinterval2.mzn <examples/allinterval2.mzn>` 中的变量 :mzn:`y` 和 :mzn:`v` 。我们使用全局约束 :mzn:`inverse` 来把变量绑到一起： :mzn:`inverse(x,y)` 约束 :mzn:`y` 为 :mzn:`x` 的逆向函数（反之亦然），即， :mzn:`x[i] = j <-> y[j] = i` 。 :numref:`ex-inverse` 中给出了它的一个定义。这个模型没有包含把变量 :mzn:`y` 和 :mzn:`v` 关联起来的约束，它们是冗余的（实际上是传播冗余）。所以它们不会给基于传播的求解器多余的信息。 :mzn:`alldifferent` 也不见了。原因是它们被逆向约束变得冗余了（传播冗余）。
唯一的约束是关于变量 :mzn:`x` 和 :mzn:`u` 和关系的约束以及 :mzn:`y` 和 :mzn:`v` 的冗余约束。

.. literalinclude:: examples/inverse.mzn
  :language: minizinc
  :name: ex-inverse
  :caption: 全局约束 ``inverse`` 的一个定义 (:download:`inverse.mzn <examples/inverse.mzn>`).

双重模型的一个优点是我们可以有更多的定义不同搜索策略的视角。运行双重模型，

.. code-block:: bash

  $ minizinc --solver g12fd --all-solutions --statistics allinterval3.mzn -D "n=10;"

注意它使用逆向模型的搜索策略，标记变量 :mzn:`y` ，在1714决策点和0.5秒内找到了所有的解。注意标记变量 :mzn:`x` 来运行同样的模型，需要13142个决策点和1.5秒。

对称
--------

对称在约束满足和优化问题中是常见的。我们可以再次通过 :numref:`ex-queens` 来看一下这个问题。在 :numref:`fig-queens-sym` 棋盘的左上方展示了一个8皇后问题的解(标记为 "original")。剩下的棋盘展示了7个解的对称版本:旋转90度,180度和270度,还有垂直翻转。

.. _fig-queens-sym:

.. figure:: figures/queens_symm.*
  
  8皇后问题解的对称变化

如果我们想要穷举8皇后问题的 *所有* 解,很明显我们需要通过穷举 *彼此之间不对称的* 解 为求解器省下一些工作,然后生成这些的对称版本。这是我们想要在约束模型中摆脱对称的一个理由。另外一个更重要的理由是,我们的求解器也可能会 **探索非解状态的对称版本!**

举个例子,一个典型的约束求解器可能会尝试把第1列皇后放在第1行上(这是可以的),然后尝试把第2列的皇后放到第3行上。 这在第一眼看是没有违反任何约束的。然而,这种设置不能被完成成为一个解(求解器会在一些搜索之后发现这一点). :numref:`fig-queens-sym-unsat` 在左上方的棋盘展示了这种设置。现在没有任何东西会阻止求解器去尝试,比如,在 :numref:`fig-queens-sym-unsat` 最低一行的左边第二种设置。其中第1列的皇后仍然在第1行,而第3列的皇后放在第2行。于是,即便是搜索一个解,求解器可能需要探索很多它已经看到并证明不可满足状态的对称状态!

.. _fig-queens-sym-unsat:

.. figure:: figures/queens_symm_unsat.*
  
  8皇后问题不可满足约束的部分解的对称版本

静态的对称性破缺
~~~~~~~~~~~~~~~~~~~~~~~~

解决对称问题的建模技巧叫做 *对称性破缺* , 在它的最简单形式里, 需要在模型中加入约束使一个(不完整的)赋值的所有对称变换去除掉而保留一个。 这些约束称作 *静态的对称性破缺约束* 。

对称性破缺背后基本的想法是加入 *顺序* 。举个例子, 我们可以通过简单地加入约束使第一列的皇后必须在棋盘的上半部分,从而去除掉所有棋盘垂直翻转的。

.. code-block:: minizinc

  constraint q[1] <= n div 2;

请相信以上约束会去除掉在 :numref:`fig-queens-sym` 中所有对称变换的一半。 为了去除 *所有* 对称,我们需要更多工作。 

当我们把所有对称都表示成数组变量的排列,一组 *字典顺序约束* 可以用于破坏所有对称。 举个例子,如果数组变量名为 :mzn:`x` ,而翻转数组是这个问题的一种对称,那么以下约束可以破坏那种对称:

.. code-block:: minizinc

  constraint lex_lesseq(x, reverse(x));

那么二维数组又怎么样呢?字典顺序同样适用,我们只需要把数组转换成一维的. 举个例子,下面的约束破坏了沿着其中一个对角线翻转数组的对称性(注意到第二个生成式里对换的数组下标):

.. code-block:: minizinc

  array[1..n,1..n] of var int: x;
  constraint lex_lesseq([ x[i,j] | i,j in 1..n ], [ x[j,i] | i,j in 1..n ]);

字典排序约束的好处在于我们可以加入多个(同时破坏几个对称性),而不需要它们互相干扰,只要我们保持第一个参数中的顺序一致即可。 

对于n皇后问题,很不幸的是这个技巧不能马上适用, 因为又一些对称不能被描述成数组 :mzn:`q` 的排列。 克服这个问题的技巧是把n皇后问题表示成布尔变量。 对于每个棋盘的每个格子, 布尔变量表示是否有一个皇后在上面。现在所有的对称性都可以表示成这个数组的排列。 因为主要的n皇后问题的主要约束在整型数组 :mzn:`q` 里面更加容易表达, 我们只需要两个模型都用起来,然后在它们之间加入连通约束。 正如 :ref:`sec-multiple-modelling-and-channels` 中解释的一样。

加入布尔变量,连通约束和对称性破缺约束的完整模型展示在 :numref:`ex-queens-sym` 里面。 我们可以做一些小实验来检查它是否成功的破坏所有对称性。 尝试用不断增加的 :mzn:`n` 运行模型, 比如从1到10, 数一下解的个数(比如,使用 Gecode求解器的 ``-s`` 标志, 或者选择IDE中"Print all solutions"和"Statistics for solving")。 你应该可以获得以下数列的解: 1, 0, 0, 1, 2, 1, 6, 12, 46, 92。 你可以搜索 *On-Line Encyclopedia of Integer Sequences* (http://oeis.org) 来校验这个序列。 

.. literalinclude:: examples/nqueens_sym.mzn
  :language: minizinc
  :name: ex-queens-sym
  :start-after: % 可选的
  :end-before: % 搜索
  :caption: n皇后问题对称性破缺的部分模型 (full model: :download:`nqueens_sym.mzn <examples/nqueens_sym.mzn>`).


其他对称的例子
~~~~~~~~~~~~~~~~~~~~~~~~~~

许多其他问题都有内在的对称性,破坏这些对称性常常会令求解表现不一样。以下是一些常见的例子:

- 装箱问题: 当尝试把物品装入箱子时,任意两个有相同容量的箱子都是对称的
- 涂色问题: 当尝试为一个图涂色使得所有相邻的节点都有不同的颜色时,我们通常用整型变量对颜色建模。但是,对颜色的任意排列都是一种合法的涂色方案。
- 车辆路线问题: 如果任务是给顾客分配一些车辆,任何两辆有相同容量的车可能是对称的(这跟装箱问题是相似的)
- 排班/时间表问题: 两个有相同能力的职员可能是可以相互交换的,就像两个有相同容量或者设备的的房间一样
