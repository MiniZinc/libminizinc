.. _sec-flattening:

FlatZinc和展平
=======================

.. \pjs{Maybe show the toolset at this point?}

约束求解器不会直接支持MiniZinc模型.为了运行一个MiniZinc模型,它被翻译成一个MiniZinc的简单子集叫FlatZinc. 
FlatZinc反映了大多数约束求解器只会求解具有 :math:`\bar{exists} c_1 \wedge \cdots \wedge c_m` 的满足问题
或者有 :math:`\text{minimize } z \text{ subject to }  c_1 \wedge \cdots \wedge c_m` 的优化问题, 其中
:math:`c_i` 是基本的约束而 :math:`z` 是一个具有某些限定形式的整型或者浮点表达式.

.. index::
  single: minizinc -c

``minizinc`` 工具包含了MiniZinc *编译器* ,它可以用一个MiniZinc模型和数据文件来创建
一个展平后的FlatZinc模型,它等价于给定数据的MiniZinc模型表达为之前提到的受限制的形式. 
通常来说构建一个给求解器的FlatZinc模型是对用户隐藏的,不过你也可以通过以下命令查看结合
数据 ``data.dzn`` 来展平一个模型 ``model.mzn`` 的结果:

.. code-block:: bash

  minizinc -c model.mzn data.dzn

这会创建一个FlatZinc模型叫 ``model.fzn`` . 

在这一章中我们探索把MiniZinc翻译成FlatZinc的过程.

展平表达式
----------------------

底层求解器的限制意味着复杂的MiniZinc表达式需要被 *展平* 为内部不具有更复杂结构项的基本约束的合取式.

思考以下保证两个在长方形箱子的两个圆不会重叠的模型:

.. literalinclude:: examples/cnonoverlap.mzn
  :language: minizinc
  :caption: 两个不会重叠的圆的模型 (:download:`cnonoverlap.mzn <examples/cnonoverlap.mzn>`).
  :name: fig-nonoverlap

简化和求值
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

给定数据文件

.. code-block:: minizinc

  width = 10.0;
  height = 8.0;
  r1 = 2.0;
  r2 = 3.0;

转换到FlatZinc首先通过替换所有参数为它们的值来简化模型, 然后求出所有取值已经固定的表达式的值. 
在这步简化后参数的值将不再需要.
一个例外是大型数组的参数值.如果它们被适用多于一次,那么为了避免重复大的表达式这个参数会被保留.

在简化后, :numref:`fig-nonoverlap` 的模型的变量和参数声明部分变为

.. literalinclude:: examples/cnonoverlap.fzn
  :language: minizinc
  :start-after: % 变量
  :end-before: %

.. _sec-flat-sub:

定义子表达式
~~~~~~~~~~~~~~~~~~~~~~~

现在没有约束求解器可以直接处理像在 :numref:`fig-nonoverlap` 里复杂的约束表达式.
作为替代,我们可以命名表达式中的每一个子表达式,我们创建约束来构建及代表子表达式的数值.
让我们来看看约束表达式的子表达式. :mzn:`(x1 - x2)` 是一个子表达式, 如果我们将它命名为
:mzn:`FLOAT01` 我们可以定义它为 :mzn:`constraint FLOAT01 = x1 - x2;` . 注意到这个表达式
只在模型中出现两次. 我们只需要构建这个值一次,然后我们可以重复使用它. 这就是 *共同子表达式消除* .
子表达式 :mzn:`(x1 - x2)*(x1 - x2)` 可以命名为 :mzn:`FLOAT02` , 而我们可以定义它为
:mzn:`constraint FLOAT02 = FLOAT01 * FLOAT01;` . 我们可以命名 :mzn:`constraint FLOAT03 = y1 - y2;` 
和 :mzn:`constraint FLOAT04 = FLOAT03 * FLOAT03;` 最后 :mzn:`constraint FLOAT05 = FLOAT02 * FLOAT04;` .
不等约束本身则变成 :mzn:`constraint FLOAT05 >= 25.0;` 因为 :mzn:`(r1+r2)*(r1 + r2)` 计算出结果为 :mzn:`25.0` .
于是这个约束被展平为

.. code-block:: minizinc

  constraint FLOAT01 = x1 - x2;
  constraint FLOAT02 = FLOAT01 * FLOAT01;
  constraint FLOAT03 = y1 - y2;
  constraint FLOAT04 = FLOAT03 * FLOAT03;
  constraint FLOAT05 = FLOAT02 * FLOAT04;
  constraint FLOAT05 >= 25.0

.. _sec-flat-fzn:

FlatZinc约束形式
~~~~~~~~~~~~~~~~~~~~~~~~

展平的最后步骤是把约束的形式转换成标准的FlatZinc形式,它总是以 :math:`p(a_1, \ldots, a_n)` 的形式出现. 
其中的 :mzn:`p` 是某个基础约束, :math:`a_1, \ldots, a_n` 是参数. FlatZinc尝试使用最少的
不同约束形式. 所以 :mzn:`FLOAT01 = x1 - x2` 首先尝试重写为 :mzn:`FLOAT01 + x2 = x1` 
然后使用 :mzn:`float_plus` 输出基本的约束. 得出的约束形式如下: 

.. literalinclude:: examples/cnonoverlap.fzn
  :language: minizinc
  :start-after: % 约束
  :end-before: %

边界分析
~~~~~~~~~~~~~~~

我们仍然缺少一项,声明引入的变量 :mzn:`FLOAT01` , ...,
:mzn:`FLOAT05` . 这些可以被声明为 :mzn:`var float` . 不过为了令求解器的任务更简单,
MiniZinc尝试通过简单的分析确定新引入变量的上界和下界. 比如因为 :mzn:`FLOAT01 = x1 - x2` 
和 :math:`2.0 \leq` :mzn:`x1` :math:`\leq 8.0` 还有 :math:`3.0 \leq` :mzn:`x2` :math:`\leq 7.0`, 所以可以得出 :math:`- 5.0 \leq` :mzn:`FLOAT01` :math:`\leq 5.0`
然后我们可以看到 :math:`-25.0 \leq` :mzn:`FLOAT02` :math:`\leq 25.0` (尽管注意到如果我们发现
相乘实际上是平方,我们可以给出更精确的边界 :math:`0.0 \leq` :mzn:`FLOAT02` :math:`\leq 25.0` ).

细心的读者会发现在 :ref:`sec-flat-sub` 和 :ref:`sec-flat-fzn` 里约束的展平形式的一点不同. 
在后面没有非等约束. 因为一元的不等式可以完全被一个变量的边界表示出来,不等关系可以令 :mzn:`FLOAT05`
的下界变为 :mzn:`25.0` , 然后这会变得冗余. 最后 :numref:`fig-nonoverlap` 的展平后形式是:

.. literalinclude:: examples/cnonoverlap.fzn
  :language: minizinc

目标函数
~~~~~~~~~~

MiniZinc 就像展平约束一样, 展平最小化和最大化目标函数. 跟其他表达式一样, 目标表达式
被展平时创建一个变量. 在FlatZinc输出求解项永远是单一变量. 看一下例子 :ref:`sec-let` .

.. \pjs{Do we need an example here?}

线性表达式
------------------

约束的一个最重要的而广泛用于建模的形式是线性约束

.. math:: a_1 x_1 + \cdots + a_n x_n \begin{array}[c]{c} = \\ \leq \\ < \end{array} a_0

其中 :math:`a_i` 是整数或者浮点数约束, 而 :math:`x_i` 是整数或者浮点数变量.
它们非常有表达能力,同时也是(整数)线性规划约束求解器唯一支持的形式. 
从MiniZinc到FlatZinc的翻译器尝试创建线性约束, 而不是把线性约束变成许多子表达式. 

.. \pjs{Maybe use the equation from SEND-MORE-MONEY instead?}

.. literalinclude:: examples/linear.mzn
  :language: minizinc
  :caption: 说明线性约束展平的MiniZinc模型 (:download:`linear.mzn <examples/linear.mzn>`).
  :name: fig-lflat

考虑在 :numref:`fig-lflat` 中的模型. 这里并没有为所有子表达式 :math:`3*x`, :math:`3*x - y` , :math:`x * z` , :math:`3*x - y + x*z` ,
:math:`x + y + z` , :math:`d * (x + y + z)` , :math:`19 + d * (x + y + z)` ,
和 :math:`19 + d * (x + y + z) - 4*d` 创建一个变量. 这里的翻译在创建一个FlatZinc约束时, 会尝试创建一个尽可能记录大部分原约束内容的线性约束. 

展平会创建线性表达式并将其视为一个单位, 而不视为每个字表达是构建中间变量.这也使创建的表达式简单化. 
从约束中抽取出线性表达式为

.. code-block:: minizinc

  var 0..80: INT01;
  constraint 4*x + z + INT01 <= 23;
  constraint INT01 = x * z;

注意到 *非线性表达式* :math:`x \times z` 是如何被抽取出来作为一个新的子表达式并赋予名字的,
与此同时剩下的项会被收集起来从而使每个变量只出现一次 (的确变量 :math:`y` 的项被移除了)

最后每个约束被写到FlatZinc形式, 从而得到:

.. code-block:: minizinc

  var 0..80: INT01;
  constraint int_lin_le([1,4,1],[INT01,x,z],23);
  constraint int_times(x,z,INT01);

.. _sec-unroll:

展开表达式
---------------------

大多数的模型需要创建一些基于输入数据的约束. 
MiniZinc通过数组类型,列表和列生成解析还有聚合函数来支持这些模型.

考虑以下从生产调度例子 :numref:`ex-prod-planning` 中出现的聚合函数表达式

.. code-block:: minizinc

  int: mproducts = max (p in Products)
                       (min (r in Resources where consumption[p,r] > 0)
                                       (capacity[r] div consumption[p,r]));

由于这用到生成器语法,我们可以把它重写成可以被编译器处理的相等的形式:

.. code-block:: minizinc

  int: mproducts = max([ min [ capacity[r] div consumption[p,r]
                             | r in Resources where consumption[p,r] > 0])
                       | p in Products]);

给定数据

.. code-block:: minizinc

  nproducts = 2; 
  nresources = 5; 
  capacity = [4000, 6, 2000, 500, 500];
  consumption= [| 250, 2, 75, 100, 0,
                | 200, 0, 150, 150, 75 |];

这首先创建 :mzn:`p = 1` 的数组 

.. code-block:: minizinc

  [ capacity[r] div consumption[p,r]
                             | r in 1..5 where consumption[p,r] > 0]          

也就是 :mzn:`[16, 3, 26, 5]` 然后计算最小值为3. 它之后为 :mzn:`p = 2` 建立相同的数组 :mzn:`[20, 13, 3, 6]` ,
并计算最小的数值为3. 然后创建数组 :mzn:`[3, 3]` 并计算最大值为3. 在FlatZinc里面没有
:mzn:`mproducts` 的表示, 这种通过计算数值3的方法会被用来代替 :mzn:`mproducts` .

在约束模型中最常见的聚合表达式是 :mzn:`forall` . Forall表达式会被展开成为多个约束.

考虑以下在SEND-MORE-MONEY例子 :numref:`ex-smm` 中使用预设的
分解 :mzn:`alldifferent` 出现的MiniZinc片段. 


.. code-block:: minizinc

  array[1..8] of var 0..9: v = [S,E,N,D,M,O,R,Y];
  constraint forall(i,j in 1..8 where i < j)(v[i] != v[j])

:mzn:`forall` 表达式为每一对需要满足 :math:`i < j` 的 :math:`i, j` 创建一个约束,
所以创建

.. code-block:: minizinc

  constraint v[1] != v[2]; % S != E
  constraint v[1] != v[3]; % S != N
  ...
  constraint v[1] != v[8]; % S != Y
  constraint v[2] != v[3]; % E != N
  ...
  constraint v[7] != v[8]; % R != Y

在FlatZinc中形成

.. code-block:: minizinc

  constraint int_neq(S,E);
  constraint int_neq(S,N);
  ...
  constraint int_neq(S,Y);
  constraint int_neq(E,N);
  ...
  constraint int_neq(R,Y);

注意到临时的数组变量 :mzn:`v[i]` 是如何在FlatZinc的输出中被原来的变量替换的. 
       
数组
------

一维变量在MiniZinc中可以有任意的下标,只要它们是相邻的整数. 
在FlatZinc所有数组都被 :mzn:`1..l` 标注, 其中 :mzn:`l` 是数组的长度. 
这意味着数组查询时需要被转换成FlatZinc下标的形式.

考虑以下MiniZinc模型来使用 :mzn:`m` 个1kg砝码来平衡一个长为 :mzn:`2 * l2` 的跷跷板, 其中上面有一个重为
:mzn:`cw` kg小孩. 

.. code-block:: minizinc

  int: cw;                               % 小孩重量
  int: l2;                               % 一半跷跷板长度
  int: m;                                % 1kg砝码的数量
  array[-l2..l2] of var 0..max(m,cw): w; % 在每个点的重量
  var -l2..l2: p;                        % 孩子的位置
  constraint sum(i in -l2..l2)(i * w[i]) = 0; % 平衡
  constraint sum(i in -l2..l2)(w[i]) = m + cw; % 所有使用的砝码
  constraint w[p] = cw;                  % 孩子在位置p
  solve satisfy;

给定 :mzn:`cw = 2`, :mzn:`l2 = 2`, 和 :mzn:`m = 3` , 展开可以产生约束

.. code-block:: minizinc

  array[-2..2] of var 0..3: w;
  var -2..2: p
  constraint -2*w[-2] + -1*w[-1] + 0*w[0] + 1*w[1] + 2*w[2] = 0;
  constraint w[-2] + w[-1] + w[0] + w[1] + w[2] = 5; 
  constraint w[p] = 2;

不过FlatZinc坚持 :mzn:`w` 数组从下标1开始. 
这意味着我们需要重写所有数组获取来使用新的下标数值. 
对于固定值数组查找这很简单,对于变量值数组查找我们可能需要创建一个新的变量.
以上公式的结果为

.. code-block:: minizinc

  array[1..5] of var 0..3: w;
  var -2..2: p
  var 1..5: INT01;
  constraint -2*w[1] + -1*w[2] + 0*w[3] + 1*w[4] + 2*w[5] = 0;
  constraint w[1] + w[2] + w[3] + w[4] + w[5] = 5; 
  constraint w[INT01] = 2;
  constraint INT01 = p + 3;

最后我们重写约束成FlatZinc的形式. 注意到变量数组下标查找的形式是如何映射到 :mzn:`array_var_int_element` 上的. 

.. code-block:: minizinc

  array[1..5] of var 0..3: w;
  var -2..2: p
  var 1..5: INT01;
  constraint int_lin_eq([2, 1, -1, -2], [w[1], w[2], w[4], w[5]], 0);
  constraint int_lin_eq([1, 1, 1, 1, 1], [w[1],w[2],w[3],w[4],w[5]], 5);
  constraint array_var_int_element(INT01, w, 2);
  constraint int_lin_eq([-1, 1], [INT01, p], -3);

MiniZinc支持多维数组,但是(目前来说)FlatZinc只支持单维度数组.
这意味着多维度数组必须映射到单维度数组上,而且多维度数组访问必须映射到
单维度数组访问.
  
考虑在有限元平面模型 :numref:`ex-laplace`: 的Laplace等式约束:

.. literalinclude:: examples/laplace.mzn
  :language: minizinc
  :start-after: % arraydec
  :end-before: % sides

假设 :mzn:`w = 4` 和 :mzn:`h = 4` ,这会创建约束

.. code-block:: minizinc

  array[0..4,0..4] of var float: t; % temperature at point (i,j)
  constraint 4.0*t[1,1] = t[0,1] + t[1,0] + t[2,1] + t[1,2];
  constraint 4.0*t[1,2] = t[0,2] + t[1,1] + t[2,2] + t[1,3];
  constraint 4.0*t[1,3] = t[0,3] + t[1,2] + t[2,3] + t[1,4];
  constraint 4.0*t[2,1] = t[1,1] + t[2,0] + t[3,1] + t[2,2];
  constraint 4.0*t[2,2] = t[1,2] + t[2,1] + t[3,2] + t[2,3];
  constraint 4.0*t[2,3] = t[1,3] + t[2,2] + t[3,3] + t[2,4];
  constraint 4.0*t[3,1] = t[2,1] + t[3,0] + t[4,1] + t[3,2];
  constraint 4.0*t[3,2] = t[2,2] + t[3,1] + t[4,2] + t[3,3];
  constraint 4.0*t[3,3] = t[2,3] + t[3,2] + t[4,3] + t[3,4];

25个元素的2维数组被转换成一维数组,而且下标也会相应改变: 所以 :mzn:`[i,j]` 下标会变成
:mzn:`[i * 5 + j + 1]` .

.. code-block:: minizinc

  array [1..25] of var float: t;
  constraint 4.0*t[7] = t[2] + t[6] + t[12] + t[8];
  constraint 4.0*t[8] = t[3] + t[7] + t[13] + t[9];
  constraint 4.0*t[9] = t[4] + t[8] + t[14] + t[10];
  constraint 4.0*t[12] = t[7] + t[11] + t[17] + t[13];
  constraint 4.0*t[13] = t[8] + t[12] + t[18] + t[14];
  constraint 4.0*t[14] = t[9] + t[13] + t[19] + t[15];
  constraint 4.0*t[17] = t[12] + t[16] + t[22] + t[18];
  constraint 4.0*t[18] = t[13] + t[17] + t[23] + t[19];
  constraint 4.0*t[19] = t[14] + t[18] + t[24] + t[20];


具体化
-----------

.. index::
  single: reification

FlatZinc模型包含了只有变量和参数声明,和一系列原始的约束. 所以当我们在MiniZinc用
布尔连接符而不是析取式来建模时,需要进行一些处理. 
处理使用连接符而不只是析取式来构建的复杂公式,其核心的方法是具体化. 
具体化一个约束 :math:`c` 创建新的约束等价于 :math:`b \leftrightarrow c` ,
即如果约束满足则布尔变量 :math:`b` 的值是 :mzn:`true` , 否则为 :mzn:`false` .

当我们有能力 *具体化* 约束,对待复杂公式的方式跟数学表达式并无不同. 我们为子表达式创建了一个名称和一个展平的约束来约束子表达式的数值. 

考虑以下任务调度例子 :numref:`ex-jobshop` 中出现在约束表达式:

.. code-block:: minizinc

  constraint %% 保证任务之间没有重叠
      forall(j in 1..tasks) (
          forall(i,k in 1..jobs where i < k) ( 
              s[i,j] + d[i,j] <= s[k,j] \/ 
              s[k,j] + d[k,j] <= s[i,j]
      ) );

对于数据文件

.. code-block:: minizinc

  jobs = 2;
  tasks = 3;
  d = [| 5, 3, 4 | 2, 6, 3 |]

然后展开过程生成

.. code-block:: minizinc

  constraint s[1,1] + 5 <= s[2,1] \/ s[2,1] + 2 <= s[1,1];
  constraint s[1,2] + 3 <= s[2,2] \/ s[2,2] + 6 <= s[1,2];
  constraint s[1,3] + 4 <= s[2,3] \/ s[2,3] + 3 <= s[1,3];

具体化在析取式中出现的约束创建新的布尔变量来定义每个表达式的数值.

.. code-block:: minizinc

  array[1..2,1..3] of var 0..23: s;
  constraint BOOL01 <-> s[1,1] + 5 <= s[2,1];
  constraint BOOL02 <-> s[2,1] + 2 <= s[1,1];
  constraint BOOL03 <-> s[1,2] + 3 <= s[2,2];
  constraint BOOL04 <-> s[2,2] + 6 <= s[1,2];
  constraint BOOL05 <-> s[1,3] + 4 <= s[2,3];
  constraint BOOL06 <-> s[2,3] + 3 <= s[1,3];
  constraint BOOL01 \/ BOOL02;
  constraint BOOL03 \/ BOOL04;
  constraint BOOL05 \/ BOOL06;

每个基础的约束现在会映射到FlatZinc形式下. 
注意到二维数组 :mzn:`s` 是如何映射到一维数组里面的. 

.. code-block:: minizinc

  array[1..6] of var 0..23: s;
  constraint int_lin_le_reif([1, -1], [s[1], s[4]], -5, BOOL01);
  constraint int_lin_le_reif([-1, 1], [s[1], s[4]], -2, BOOL02);
  constraint int_lin_le_reif([1, -1], [s[2], s[5]], -3, BOOL03);
  constraint int_lin_le_reif([-1, 1], [s[2], s[5]], -6, BOOL04);
  constraint int_lin_le_reif([1, -1], [s[3], s[6]], -4, BOOL05);
  constraint int_lin_le_reif([-1, 1], [s[3], s[6]], -3, BOOL06);
  constraint array_bool_or([BOOL01, BOOL02], true);
  constraint array_bool_or([BOOL03, BOOL04], true);
  constraint array_bool_or([BOOL05, BOOL06], true);

:mzn:`int_lin_le_reif` 是线性约束 :mzn:`int_lin_le` 的具体化形式.

大多数FlatZinc基本约束 :math:`p(\bar{x})` 有一个具体化形式 :math:`\mathit{p\_reif}(\bar{x},b)` , 
它利用最后额外的参数 :math:`b` 来定义一个约束 :math:`b \leftrightarrow p(\bar{x})` . 
定义像 :mzn:`int_plus` 和 :mzn:`int_plus` 的函数式关系的FlatZinc基本约束
不需要支持具体化. 反而,有结果的函数的等式被具体化了. 

具体化的另外一个重要作用出现在当我们使用强制转换函数 :mzn:`bool2int` (可能是显式地或者隐式地把布尔表达式使用成整数表达式使用). 平整过程将创建一个布尔变量来保存一个布尔表达式参数,
以及一个整型变量 (限制到 :mzn:`0..1` )来保存这个数值. 

考虑 :numref:`ex-magic-series` 中的魔术序列问题.

.. literalinclude:: examples/magic-series.mzn
  :language: minizinc
  :end-before: solve satisfy

给定 :mzn:`n = 2` , 展开创造了

.. code-block:: minizinc

  constraint s[0] = bool2int(s[0] = 0) + bool2int(s[1] = 0);
  constraint s[1] = bool2int(s[0] = 1) + bool2int(s[1] = 1);

和展平创造了

.. code-block:: minizinc

  constraint BOOL01 <-> s[0] = 0;
  constraint BOOL03 <-> s[1] = 0;
  constraint BOOL05 <-> s[0] = 1;
  constraint BOOL07 <-> s[1] = 1;
  constraint INT02 = bool2int(BOOL01);
  constraint INT04 = bool2int(BOOL03);
  constraint INT06 = bool2int(BOOL05);
  constraint INT08 = bool2int(BOOL07);
  constraint s[0] = INT02 + INT04;
  constraint s[1] = INT06 + INT08;

最后的FlatZinc形式是

.. code-block:: minizinc

  var bool: BOOL01;
  var bool: BOOL03;
  var bool: BOOL05;
  var bool: BOOL07;
  var 0..1: INT02;
  var 0..1: INT04;
  var 0..1: INT06;
  var 0..1: INT08;
  array [1..2] of var 0..2: s;
  constraint int_eq_reif(s[1], 0, BOOL01);
  constraint int_eq_reif(s[2], 0, BOOL03);
  constraint int_eq_reif(s[1], 1, BOOL05);
  constraint int_eq_reif(s[2], 1, BOOL07);
  constraint bool2int(BOOL01, INT02);
  constraint bool2int(BOOL03, INT04);
  constraint bool2int(BOOL05, INT06);
  constraint bool2int(BOOL07, INT08);
  constraint int_lin_eq([-1, -1, 1], [INT02, INT04, s[1]], 0);
  constraint int_lin_eq([-1, -1, 1], [INT06, INT08, s[2]], 0);
  solve satisfy;

谓词
----------

MiniZinc支持许多不同求解器的的一个重要的因素是全局约束 (还有真正的FlatZinc约束)可以根据不同的
求解器专业化. 

每一个求解器生命一个谓词有时会,但有时并不会提供具体的定义. 举个例子一个求解器有一个
内建的全局 :mzn:`alldifferent` 谓词,会包含定义

.. code-block:: minizinc

  predicate alldifferent(array[int] of var int:x);

在全局约束库中,同时一个求解器预设的分解会有定义

.. code-block:: minizinc

  predicate alldifferent(array[int] of var int:x) =
      forall(i,j in index_set(x) where i < j)(x[i] != x[j]);

谓词调用 :math:`p(\bar{t})` 在平整时, 首先为每个参数项 :math:`t_i` 创建对应变量 :math:`v_i` . 
如果谓词没有定义我们只需要使用创建的参数 :math:`p(\bar{v})` 来调用谓词. 如果一个谓词
有一个定义 :math:`p(\bar{x}) = \phi(\bar{x})` 然后我们将用谓词的定义来替换这个谓词调用 :math:`p(\bar{t})` 当中形式参数被替换为对应的参数变量, 即 :math:`\phi(\bar{v})`. 
注意到如果一个谓词调用 :math:`p(\bar{t})` 出现在具体化位置而且它没有定义,我们则
检查我们适用这个谓词的具体化版本 :math:`\mathit{p\_reif}(\bar{x},b)` .

考虑在SEND-MORE-MONEY例子 :numref:`ex-smm` 中 :mzn:`alldifferent` 约束的:

.. code-block:: minizinc

  constraint alldifferent([S,E,N,D,M,O,R,Y]);

如果这个求解器有一个内建的 :mzn:`alldifferent` 我们只需要为这个参数创建一个新的变量, 然后在调用时替换它. 

.. code-block:: minizinc

  array[1..8] of var 0..9: v = [S,E,N,D,M,O,R,Y];
  constraint alldifferent(v);

注意到边界分析尝试在新的数组变量上找到一个紧的边界. 建造这个数组参数的理由就是,如果我们中使用
相同的数组两次,FlatZinc求解器不会创建它两次. 在这种情况下因为它不是使用两次,后面的转换会把 :mzn:`v` 替换成它的定义. 

如果求解器使用预设的定义 :mzn:`alldifferent` 呢? 
然后变量 :mzn:`v` 会正常地定义,谓词调用会被替换为一个当中变量被重命名的版本, 其中 :mzn:`v` 替换了形式参数 :mzn:`x`. 
结果的程序是

.. code-block:: minizinc

  array[1..8] of var 0..9: v = [S,E,N,D,M,O,R,Y];
  constraint forall(i,j in 1..8 where i < j)(v[i] != v[j])

我们可以在 :ref:`sec-unroll` 中看到.

考虑到以下约束, 其中 :mzn:`alldifferent` 在一个具体化位置出现. 

.. code-block:: minizinc

  constraint alldifferent([A,B,C]) \/ alldifferent([B,C,D]);

如果求解器有 :mzn:`alldifferent` 的具体化形式, 这将会被展平为 

.. code-block:: minizinc

  constraint alldifferent_reif([A,B,C],BOOL01);
  constraint alldifferent_reif([B,C,D],BOOL02);
  constraint array_bool_or([BOOL01,BOOL02],true);

适用这个预设的分解,谓词替换会首先创建

.. code-block:: minizinc

  array[1..3] of var int: v1 = [A,B,C];
  array[1..3] of var int: v2 = [B,C,D];
  constraint forall(i,j in 1..3 where i<j)(v1[i] != v1[j]) \/
             forall(i,j in 1..3 where i<j)(v2[i] != v2[j]);

它最终会展平成FlatZinc形式

.. code-block:: minizinc

  constraint int_neq_reif(A,B,BOOL01);
  constraint int_neq_reif(A,C,BOOL02);
  constraint int_neq_reif(B,C,BOOL03);
  constraint array_bool_and([BOOL01,BOOL02,BOOL03],BOOL04);
  constraint int_neq_reif(B,D,BOOL05);
  constraint int_neq_reif(C,D,BOOL06);
  constraint array_bool_and([BOOL03,BOOL05,BOOL06],BOOL07);
  constraint array_bool_or([BOOL04,BOOL07],true);

注意到共同子表达式消除是如何利用具体化不等式 :mzn:`B != C` 的. 
(虽然有一个更好的转换把共同约束提升到最顶层的合取式中)

.. _sec-let:

Let表达式
---------------

Let表达式是MiniZinc中可用于引入新的变量的非常强大的工具. 
在展平时,let表达式被转换成变量和约束声明. 这个MiniZinc的关系语义意味着这些约束必须像在第一个包含的布尔表达式中出现.

let表达式的一个重要特征是每一次它们被使用时它们都创建新的变量. 

考虑一下展平的代码

.. code-block:: minizinc

  constraint even(u) \/ even(v);
  predicate even(var int: x) = 
            let { var int: y } in x = 2 * y;

首先谓词调用被他们的定义取代.

.. code-block:: minizinc

  constraint (let { var int: y} in u = 2 * y) \/
             (let { var int: y} in v = 2 * y);   

然后let变量会另外被重命名

.. code-block:: minizinc

  constraint (let { var int: y1} in u = 2 * y1) \/
             (let { var int: y2} in v = 2 * y2);   

最后变量声明会被抽取到第一层

.. code-block:: minizinc

  var int: y1;
  var int: y2;
  constraint u = 2 * y1 \/ v = 2 * y2;   

一旦let表达式被清除我们可以像之前那样展平.

记住let表达式可以定义新引入的变量 (对某些参数的确需要这样做). 
这些隐式地定义了必须满足的约束.

考虑婚礼座位问题 :numref:`ex-wedding2` 的复杂的目标函数.

.. code-block:: minizinc

  solve maximize sum(h in Hatreds)(
        let {  var Seats: p1 = pos[h1[h]],
               var Seats: p2 = pos[h2[h]],
               var 0..1: same = bool2int(p1 <= 6 <-> p2 <= 6) } in   
        same * abs(p1 - p2) + (1-same) * (abs(13 - p1 - p2) + 1));

为了简介我们假设只使用前两个相互敌视的人,所以

.. code-block:: minizinc

  set of int: Hatreds = 1..2;
  array[Hatreds] of Guests: h1 = [groom, carol];
  array[Hatreds] of Guests: h2 = [clara, bestman];

展平的第一步是展开 :mzn:`sum` 表达式,给定(为了简洁我们保留客人名字和参数 :mzn:`Seats` , 
在实际中他们会被他们的定义取代):

.. code-block:: minizinc

  solve maximize 
        (let { var Seats: p1 = pos[groom],
               var Seats: p2 = pos[clara],
               var 0..1: same = bool2int(p1 <= 6 <-> p2 <= 6) } in   
         same * abs(p1 - p2) + (1-same) * (abs(13 - p1 - p2) + 1)) 
        +
        (let { var Seats: p1 = pos[carol],
               var Seats: p2 = pos[bestman],
               var 0..1: same = bool2int(p1 <= 6 <-> p2 <= 6) } in   
         same * abs(p1 - p2) + (1-same) * (abs(13 - p1 - p2) + 1)); 

然后每一个在let表达式的新变量会被分别命名为

.. code-block:: minizinc

  solve maximize 
        (let { var Seats: p11 = pos[groom],
               var Seats: p21 = pos[clara],
               var 0..1: same1 = bool2int(p11 <= 6 <-> p21 <= 6) } in   
         same1 * abs(p11 - p21) + (1-same1) * (abs(13 - p11 - p21) + 1)) 
        +
        (let { var Seats: p12 = pos[carol],
               var Seats: p22 = pos[bestman],
               var 0..1: same2 = bool2int(p12 <= 6 <-> p22 <= 6) } in   
         same2 * abs(p12 - p22) + (1-same2) * (abs(13 - p12 - p22) + 1)); 

在let表达式的变量会被抽取到第一层,并且定义约束会被抽取到正确的层(在这里是最顶层).

.. code-block:: minizinc

  var Seats: p11;
  var Seats: p21;
  var 0..1: same1;
  constraint p12 = pos[clara];
  constraint p11 = pos[groom];
  constraint same1 = bool2int(p11 <= 6 <-> p21 <= 6);
  var Seats p12; 
  var Seats p22;
  var 0..1: same2;
  constraint p12 = pos[carol];
  constraint p22 = pos[bestman];
  constraint same2 = bool2int(p12 <= 6 <-> p22 <= 6) } in   
  solve maximize 
        same1 * abs(p11 - p21) + (1-same1) * (abs(13 - p11 - p21) + 1)) 
        +
        same2 * abs(p12 - p22) + (1-same2) * (abs(13 - p12 - p22) + 1)); 

现在我们已经构成不需要使用let表达式的等价的MiniZinc代码,和展平可以正常进行. 

为了说明没有出现在最顶层的let表达式的情况,看看以下模型

.. code-block:: minizinc

  var 0..9: x;
  constraint x >= 1 -> let { var 2..9: y = x - 1 } in 
                       y + (let { var int: z = x * y } in z * z) < 14;

我们抽取变量定义到最顶层,约束到第一个围住的布尔语境,这里是蕴含的右手边. 

.. code-block:: minizinc

  var 0..9: x;
  var 2..9: y;
  var int: z;
  constraint x >= 1 -> (y = x - 1 /\ z = x * y /\ y + z * z < 14);

注意到如果我们知道定义一个变量的等式的真值不会为假,我们可以抽取它到最顶层. 这通常可以是求解大幅加快. 

对于上面的例子,因为 :mzn:`y` 的值域对于 :mzn:`x - 1` 并不够大, 所以约束 :mzn:`y = x - 1` 可能失败. 
不过约束 :mzn:`z = x * y` 不可以(实际上边界分析会给予 :mzn:`z` 足够大的边界来包含 :mzn:`x * y` 所有的可能值).
一个更好的展平可以给出

.. code-block:: minizinc

  var 0..9: x;
  var 2..9: y;
  var int: z;
  constraint z = x * y;
  constraint x >= 1 -> (y = x - 1 /\ y + z * z < 14);

现在MiniZinc编译器通过总是使引入变量声明的边界足够大,它应该可以包含所有它定义的表达式的值.
然后在正确的语境中为let表达式加入边界约束.在上面的例子中这个结果是

.. code-block:: minizinc

  var 0..9: x;
  var -1..8: y;
  var -9..72: z;
  constraint y = x - 1;
  constraint z = x * y;
  constraint x >= 1 -> (y >= 2 /\ y + z * z < 14);

这个转换可以使求解更加高效,因为let变量的所有可能的复杂计算并没有被具体化. 

这种方法的另外一个原因是在引入变量出现在取反语境的时候它也可以被使用(只要它们有一个定义). 考虑一下与之前相似的这个例子:

.. code-block:: minizinc

  var 0..9: x;
  constraint (let { var 2..9: y = x - 1 } in 
             y + (let { var int: z = x * y } in z * z) > 14) -> x >= 5;

这个let表达式出现在否定语境中,不过每个引入变量都被定义了.展平后的代码是

.. code-block:: minizinc

  var 0..9: x;
  var -1..8: y;
  var -9..72: z;
  constraint y = x - 1;
  constraint z = x * y;
  constraint (y >= 2 /\ y + z * z > 14) -> x >= 5;

注意到作为对比的let消除方法不能给出一个正确的转换:

.. code-block:: minizinc

  var 0..9: x;
  var 2..9: y;
  var int: z;
  constraint (y = x - 1 /\ z = x * y /\ y + z * z > 14) -> x >= 5;

以上转换对于所有 :mzn:`x` 的可能值给出结果,而原来的约束除掉了 :mzn:`x = 4` 的可能性. 

对于在let表达式中的处理跟对定义的变量的处理是相似的. 你可以认为一个约束等价于定义一个新的布尔变量. 
新的布尔变量定义可以从最顶层中抽取出来, 而布尔保存在正确的语境下. 

.. code-block:: minizinc

  constraint z > 1 -> let { var int: y,
                            constraint (x >= 0) -> y = x,
                            constraint (x < 0)  -> y = -x 
                      } in y * (y - 2) >= z;

可以处理成 

.. code-block:: minizinc

  constraint z > 1 -> let { var int: y,
                            var bool: b1 = ((x >= 0) -> y = x),
                            var bool: b2 = ((x < 0)  -> y = -x),
                            constraint b1 /\ b2
                      } in y * (y - 2) >= z;

然后展平成

.. code-block:: minizinc

  constraint b1 = ((x >= 0) -> y = x);
  constraint b2 = ((x < 0)  -> y = -x);
  constraint z > 1 -> (b1 /\ b2 /\ y * (y - 2) >= z);
