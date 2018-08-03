.. _sec-sat:

在MiniZinc中对布尔可满足性问题建模
============================================

MiniZinc 可以被用来给布尔可满足性问题建模，这种问题的变量被限制为是布尔型 (:mzn:`bool`)。MiniZinc 可以使用有效率的布尔可满足性求解器来有效地解决求得的模型。


整型建模
------------------

很多时候，尽管我们想要使用一个布尔可满足性求解器，我们可能也需要给问题的 整数部分建模。
有三种通用的方式使用布尔型变量对定义域为范围 :math:`0 \dots m` 内的整型变量 :math:`I` 建模，其中 :math:`m = 2^{k}-1` 。

- 二元： :math:`I` 被表示为 :math:`k` 个二元变量 :math:`i_0, \ldots, i_{k-1}` ，其中 :math:`I = 2^{k-1} i_{k-1} + 2^{k-2} i_{k-2} + \cdots + 2 i_1 + i_0` 。在MiniZinc 中，这可表示为

  .. code-block:: minizinc

    array[0..k-1]  of var bool: i;
    var 0..pow(2,k)-1: I = sum(j in 0..k-1)(bool2int(i[j])*pow(2,j));

- 一元: :math:`I` 被表示为 :math:`m` 个二元变量 :math:`i_1, \ldots, i_m` 且 :math:`i = \sum_{j=1}^m \mathtt{bool2int}(i_j)` 。由于在一元表 示中有大量的冗余表示，我们通常要求 :math:`i_j \rightarrow i_{j-1}, 1 < j \leq m` 。在MiniZinc中，这可表示为 

  .. code-block:: minizinc

    array[1..m]  of var bool: i;
    constraint forall(j in 2..m)(i[j] -> i[j-1]);
    var 0..m: I = sum(j in 1..m)(bool2int(i[j]);

- 值: 其中 :math:`I` 被表示为 :math:`m+1` 个二元变量 :math:`i_0, \ldots, i_m` ，其中 :math:`i = k \Leftrightarrow i_k` 并且 :math:`i_0, \ldots, i_m` 中最多有一个为真。在MiniZinc中，这可表示为

  .. code-block:: minizinc

    array[0..m]  of var bool: i;
    constraint sum(j in 0..m)(bool2int(i[j]) == 1;
    var 0..m: I;
    constraint foall(j in 0..m)(I == j <-> i[j]);

每种表示都有其优点和缺点。这取决于模型中需要对整数做什么样的操作，而这些 操作在哪一种表示上更为方便。

非等式建模
---------------------

接下来，让我们考虑如何为一个拉丁方问题建模。一个拉丁方问题是在 :math:`n \times n` 个网 格上放置 :math:`1..n` 之间的数值使得每个数在每行每列都仅出现一次。图 :numref:`ex-latin` 中给出了拉丁方问题的的一个整数模型。

.. literalinclude:: examples/latin.mzn
  :language: minizinc
  :name: ex-latin
  :caption: 拉丁方问题的整数模型 (:download:`latin.mzn <examples/latin.mzn>`).

整型变量直接的唯一的约束实际上是非等式，而它在约束 :mzn:`alldifferent`  中被编码。 数值表示是表达非等式的最佳方式。图 :numref:`ex-latinbool` 给出了一个关于拉丁方问题的只含有布尔型 变量的模型。注意每个整型数组元素 :mzn:`a[i,j]` 被替换为一个布尔型数组。我们使用谓词 :mzn:`exactlyone` 来约束每个数值在每行每列都仅出现一次，也用来约束有且仅有一个布尔型 变量对应于整型数组元素 :mzn:`a[i,j]` 为真。

.. literalinclude:: examples/latinbool.mzn
  :language: minizinc
  :name: ex-latinbool
  :caption: 拉丁方问题的布尔型模型 (:download:`latinbool.mzn <examples/latinbool.mzn>`).

势约束建模
---------------------

让我们来看下如何对点灯游戏建模。这个游戏由一个矩形网格组成，每个网格为空 白或者被填充。每个被填充的方格可能包含1到4之间的数字，或者没有数字。我们的 目标是放置灯泡在空白网格使得

- 每个空白的网格是“被照亮的”，也就是说它可以透过一行没有被打断的空白网格 看到光亮。
- 任何两个灯泡都不能看到彼此。
- 一个有数值的填充的网格相邻的灯泡个数必须等于这个网格中的数值。

:numref:`fig-lightup` 给出了点灯游戏的一个例子以及 :numref:`fig-lightup-sol` 给出了它的解。

.. _fig-lightup:

.. figure:: figures/lightup.*
  
  点灯游戏的一个例子展示

.. _fig-lightup-sol:

.. figure:: figures/lightup2.*
  
  点灯游戏的完整的解

这个问题很自然地可以使用布尔型变量建模。布尔型变量用来决定哪一个网格包含有一个点灯以及哪一个没有。同时我们也有一些作用于填充的网格上的整数算术运算要 考虑。


.. literalinclude:: examples/lightup.mzn
  :language: minizinc
  :name: ex-lightup
  :caption: 点灯游戏的SAT模型 (:download:`lightup.mzn <examples/lightup.mzn>`).

图 :numref:`ex-lightup` 中给出了这个问题的一个模型。图 :numref:`fig-lightup` 中给出的问题的数据文件在图 :numref:`fig-lightupdzn` 中给出。 

.. literalinclude:: examples/lightup.dzn
  :language: minizinc
  :name: fig-lightupdzn
  :caption: 点灯游戏的 :numref:`fig-lightup` 中实例的数据文件

模型利用了一个布尔型求和谓词 

.. code-block:: minizinc

  predicate bool_sum_eq(array[int] of var bool:x, int:s);

使得一个布尔型数组的和等于一个固定的整数。多种方式都能使用布尔型变量给 *cardinality* 约束建模。

- 加法器网络：我们可以使用包含加法器的一个网络给布尔型总和建立一个二元布 尔型表达式
- 排序网络：我们可以通过使用一个排序网络去分类布尔型数组来创建一个布尔型 总和的一元表达式
- 二元决策图：我们可以创建一个二维决策图（BDD）来编码势约束。


.. literalinclude:: examples/bboolsum.mzn
  :language: minizinc
  :name: ex-bboolsum
  :caption: 使用二元加法器网络表示势约束 (:download:`bboolsum.mzn <examples/bboolsum.mzn>`).

.. literalinclude:: examples/binarysum.mzn
  :language: minizinc
  :name: ex-binarysum
  :caption: 创建二元求和网络的代码 (:download:`binarysum.mzn <examples/binarysum.mzn>`).

我们可以使用图 :numref:`ex-bboolsum` 给出的二元加法器网络代码实现 :mzn:`bool_sum_eq` 。图 :numref:`ex-binarysum` 中定义的 谓词 :mzn:`binary_sum` 创建了一个 :mzn:`x` 总和的二维表示法。它把列表分为两部分，把每一部分 分别加起来得到它们的一个二元表示，然后用 :mzn:`binary_add` 把这两个二元数值加起来。 如果 :mzn:`x`  列大小是奇数，则最后一位被保存起来作为二元加法时的进位来使用。

.. \pjs{Add a picture of an adding network}

.. literalinclude:: examples/uboolsum.mzn
  :language: minizinc
  :name: ex-uboolsum
  :caption: 使用二元加法器网络表示势约束 (:download:`uboolsum.mzn <examples/uboolsum.mzn>`).

.. literalinclude:: examples/oesort.mzn
  :language: minizinc
  :name: ex-oesort
  :caption: 奇偶归并排序网络 (:download:`oesort.mzn <examples/oesort.mzn>`).

我们可以使用图 :numref:`ex-uboolsum` 中给出的一元排序网络代码来实现 :mzn:`bool_sum_eq` 。势约束通过扩展输入 :mzn:`x` 长度为2 的次幂，然后使用奇偶归并排序网络给得到的位排序来实现。奇偶归 并排序工作方式在图 :numref:`ex-oesort` 中给出，它递归地把输入列表拆为两部分，给每一部分排序，然 后再把有序的两部分归并起来。
 
.. \pjs{Add much more stuff on sorting networks}



.. \pjs{Add a picture of an adding network}

.. literalinclude:: examples/bddsum.mzn
  :language: minizinc
  :name: ex-bddsum
  :caption: 使用二元加法器网络表示势约束 (:download:`bddsum.mzn <examples/bddsum.mzn>`).

我们可以使用图 :numref:`ex-bddsum` 中给出的二元决策图代码来实现 :mzn:`bool_sum_eq` 。势约束被分为两种情况：或者第一个元素 :mzn:`x[1]` 为 :mzn:`true` 并且剩下位的总和是 :mzn:`s-1` ，或者 :mzn:`x[1]` 为 :mzn:`false` 并 且剩下位的总和是 :mzn:`s` 。它的效率的提高依赖于去除共同子表达式来避免产生太多的相同的约束。


.. \pjs{Add a picture of a bdd network network}


