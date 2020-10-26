.. _sec-search:

搜索
======

.. index::
  single: annotation

MiniZinc默认没有我们想如何搜索解的声明。这就把搜索全部都留给下层的求解器了。
但是有些时候，尤其是对组合整数问题，我们或许想规定搜索应该如何去进行。
这就需要我们和求解器沟通出一个搜索策略 :index:`search` 。注意，搜索策略 *不* 真的是模型的一部分。实际上，我们不要求每个求解器把所有可能的求解策略都实现了。
MiniZinc通过使用 *annotations* 来用一个稳定的方法跟约束求解器沟通额外的信息。

有限域搜索
--------------------

.. index::
  single: search; finite domain

利用有限域求解器搜索涉及到检查变量剩余的可能值以及选择进一步地约束一些变量。
搜索则会加一个新的约束来限制这个变量的剩余值（实际上猜测解可能存在于哪里），然后使用传播来确定其他的值是否可能存在于解中。
为了确保完全性，搜索会保留另外一个选择，而它是新约束的否定。
搜索会当有限域求解器发现所有的约束都被满足，此时一个解已经被找到，或者有约束不被满足时停止。
当不可满足出现的时候，搜索必须换另外一个不同的选择集合继续下去。通常有限域求解器使用 :index:`深度优先搜索 <search; depth first>` ，它会撤销最后一个做的选择然后尝试做一个新的选择。

.. literalinclude:: examples/nqueens.mzn
  :language: minizinc
  :name: ex-queens
  :caption: n皇后问题模型 (:download:`nqueens.mzn <examples/nqueens.mzn>`).

有限域问题的一个简单例子是 :math:`n` 皇后问题，它要求我们放置 :math:`n` 个皇后在 :math:`n \times n` 棋盘上使得任何两个都不会互相攻击。变量 :mzn:`q[i]` 记录了在 :mzn:`i` 列的皇后放置在哪一行上。 :mzn:`alldifferent` 约束确保了任何两个皇后都不会在同一行或者对角线上。 :numref:`fig-9q-a` 的左边给出了 :mzn:`n = 9` 的典型（部分）搜索树。我们首选设置 :mzn:`q[1] = 1` ，
这样就可以从其他变量的定义域里面移除一些数值，例如 :mzn:`q[2]` 不能取值1或者2.
我们接下来设置 :mzn:`q[2] = 3` ，然后进一步地从其他变量的定义域里面移除一些数值。
我们设置 :mzn:`q[3] = 5` （它最早的可能值）。在这三个决策后，棋盘的状态显示为 :numref:`fig-9q-b` 。其中皇后表示已经固定的皇后位置。星星表示此处放置的皇后会攻击到已经放置的皇后，所以我们不能在此处放置皇后。

.. _fig-9q-a:

.. figure:: figures/tree-4.*
  
  9皇后问题的部分搜索树

.. _fig-9q-b:

.. figure:: figures/chess9x9-3.*

  在加入 ``q[1] = 1``, ``q[2] = 4``, ``q[3] = 5`` 的状态

.. _fig-9q-c:

.. figure:: figures/chess9x9-4.*
  
  在进一步加入 ``q[6] = 4`` 后的初始传播

一个搜索策略决定要做哪一个选择。我们目前所做的决定都按照一个简单的策略：选择第一个还没有固定的变量，尝试设置它为它的最小可能值。按照这个策略，下一个决策应该是 :mzn:`q[4] = 7` 。变量选择的另外一个策略是选择现在可能值集合 *定义域* 最小的变量。按照这个所谓 *最先失败* 变量选择策略，下一个决策应该是 :mzn:`q[6] = 4` 。如果我们做了这个决策，则初始的传播会去除掉 :numref:`fig-9q-c` 中显示的额外的值。
但是它使得 :mzn:`q[8]` 只剩余有一个值。所以 :mzn:`q[8] = 7` 被执行。但是这又使得 :mzn:`q[7]` 和 :mzn:`q[9]` 也只剩余一个值2。因此这个时候有个约束一定会被违反。我们检测到了不满足性，求解器必须回溯取消最后一个决策 :mzn:`q[6] = 4` 并且加入它的否定 :mzn:`q[6] != 4` （引导我们到了 :numref:`fig-9q-a` 中树的状态(c)），即强制使 :mzn:`q[6] = 8` 。这使得有些值从定义域中被去除。我们接下来继续重新启用搜索策略来决定该怎么做。

很多有限域搜索被定义为这种方式：选择一个变量来进一步约束，然后选择如何进一步地约束它。

搜索注解
------------------

.. index::
  single: search; annotation
  single: solve

MiniZinc中的搜索注解注明了为了找到一个问题的解应如何去搜索。
注解附在求解项，在关键字 :mzn:`solve` 之后。
搜索注解

.. literalinclude:: examples/nqueens.mzn
  :language: minizinc
  :lines: 11-12

出现在求解项中。注解使用连接符 :mzn:`::` 附为模型的一部分。
这个搜索注解意思是我们应该按照从整型变量数组 :mzn:`q` 中选择拥有最小现行定义域的变量（这个是 :mzn:`first_fail` 规则），然后尝试设置其为它的最小可能值（ :mzn:`indomain_min` 值选择），纵观整个搜索树来搜索（ :mzn:`complete` 搜索）。

.. % \begin{tabular}{|c|c|c|c|c|c|c|c|c|}
.. % \hline
.. % Q & . & . & . & . & . & . & . & . \\ \hline
.. % . & . & . &   &   & . &   &   &   \\ \hline
.. % . & Q & . & . & . & . & . & . & . \\ \hline
.. % . & . & . & . &   &   &   &   &   \\ \hline
.. % . & . & Q & . & . & . & . & . & . \\ \hline
.. % . & . & . & . & . & . &   &   &   \\ \hline
.. % . & . & . &   & . & . & . &   &   \\ \hline
.. % . & . & . &   &   & . & . & . &   \\ \hline
.. % . & . & . &   &   &   & . & . & . \\ \hline
.. % \end{tabular}

.. defblock:: 基本搜素注解

  .. index::
    single: int_search
    single: bool_search
    single: set_search

  我们有三个基本搜索注解，相对应于不同的基本搜索类型：

  - :mzndef:`int_search( <变量>, <变量选择>, <约束选择>, <策略> )`
    其中 :mzndef:`<变量>` 是一个 :mzn:`var int` 类型的一维数组， :mzndef:`<变量选择>` 是一个接下来会讨论的变量选择注解， :mzndef:`<约束选择>` 是一个接下来会讨论的如何约束一个变量的选择， :mzndef:`<策略>` 是一个搜索策略，我们暂时假设为 :mzn:`complete`
  - :mzndef:`bool_search( <变量>, <变量选择>, <约束选择>, <策略> )`
    其中 :mzndef:`<变量>` 是一个 :mzn:`var bool` 类型的一维数组，剩余的和上面一样。 
  - :mzndef:`set_search( <变量>, <变量选择>, <约束选择>, <策略> )`
    其中 :mzndef:`<变量>` 是一个 :mzn:`var set of int` 类型的一维数组，剩余的和上面一样。
  - :mzndef:`float_search( <变量>, <精度>, <变量选择>, <约束选择>, <策略> )`
    其中 :mzndef:`<变量>` 是一个一维 :mzn:`var float` 数组, :mzndef:`<precision>` 是一个固定的用于表示 :math:`\epsilon` 浮点数, 其中两个数之差低于这个浮点数时被认为相等。剩余的和上面一样。

  .. index::
    single: search; variable choice
    single: input_order
    single: first_fail
    single: smallest

  变量选择注解的例子有：

  - :mzn:`input_order`: 从数组中按照顺序选择
  - :mzn:`first_fail`: 选择拥有最小定义域大小的变量，以及
  - :mzn:`smallest`: 选择拥有最小值的变量。

  .. index::
    single: search; constrain choice
    single: indomain_min
    single: indomain_median
    single: indomain_random
    single: indomain_split

  约束一个变量的方式有：

  - :mzn:`indomain_min`: 赋最小的定义域内的值给变量，
  - :mzn:`indomain_median`: 赋定义域内的中间值给变量，
  - :mzn:`indomain_random`: 从定义域中取一个随机的值赋给变量，以及
  - :mzn:`indomain_split` 把变量定义域一分为二然后去除掉上半部分。

  对于完全搜素， :mzndef:`<策略>` 基本都是 :mzn:`complete` 。关于一份完整的变量和约束选择注解，请参看MiniZinc参考文档中的FlatZinc说明书。

利用搜索构造注解，我们可以创建更加复杂的搜索策略。目前我们只有一个这样的注解。

.. index::
  single: search; sequential
  single: seq_search

.. code-block:: minizinc

  seq_search([ <搜素注解>, ..., <搜素注解> ])

顺序搜索构造首先执行对列表中的第一个注解所指定的变量的搜索，当这个注解中的所有的变量都固定后，它执行第二个搜索注解，等等。直到所有的搜索注解都完成。

我们来看一下 :numref:`ex-jobshop3` 中给出的车间作业调度模型。我们可以替换求解项为

.. code-block:: minizinc

  solve :: seq_search([
               int_search(s, smallest, indomain_min, complete),
               int_search([end], input_order, indomain_min, complete)])
        minimize end

通过选择可以最早开始的作业并设置其为 :mzn:`s` ，起始时间被设置为 :mzn:`s` 。
当所有的起始时间都设置完后，终止时间 :mzn:`end` 或许还没有固定。因此我们设置其为它的最小可能取值。

注解
-----------

.. index::
  single: annotation

在MiniZinc中，注解是第一类对象。我们可以在模型中声明新的注解，以及声明和赋值给注解变量。

.. defblock:: 注解

  .. index::
    single: ann
  
  注解有一个类型 :mzn:`ann` 。你可以声明一个注解参数 :index:`parameter` （拥有可选择的赋值）:

  .. code-block:: minizincdef
  
    ann : <标识符>;
    ann : <标识符> = <注解表达式> ;

  对注解变量赋值和对其他参数赋值一样操作。

  :index:`表达式 <expression>` ， :index:`变量声明 <variable; declaration>` ，和 :mzn:`solve` 项都可以通过使用 :mzn:`::` 操作符来成为注解。

  使用注解项 :index:`注解项` ，我们可以声明一个新的注解 :mzn:`annotation` :index:`项 <item; annotation>` :
  
  .. code-block:: minizincdef
  
    annotation <注解名> ( <参数定义>, ..., <参数定义> ) ;
  
.. literalinclude:: examples/nqueens-ann.mzn
  :language: minizinc
  :name: ex-queens-ann
  :caption: n皇后问题的注解模型 (:download:`nqueens-ann.mzn <examples/nqueens-ann.mzn>`).

:numref:`ex-queens-ann` 中的程序阐述了注解声明，注解和注解变量的使用。
我们声明一个新的注解 :mzn:`bitdomain` ，意思是来告诉求解器变量定义域应该通过大小为 :mzn:`nwords` 的比特数组来表示。
注解附注在变量 :mzn:`q` 的声明之后。每一个 :mzn:`alldifferent` 约束都被注解为内部注解 :mzn:`domain` ，而它指导求解器去使用 :mzn:`alldifferent` 的定义域传播版本（如果有的话）。一个注解变量 :mzn:`search_ann` 被声明和使用来定义搜索策略。我们可以在一个单独的数据文件中来给出搜素策略的值。

搜索注解的例子或许有以下几种（我们假设每一行都在一个单独的数据文件中）

.. code-block:: minizinc

  search_ann = int_search(q, input_order, indomain_min, complete);
  search_ann = int_search(q, input_order, indomain_median, complete);
  search_ann = int_search(q, first_fail, indomain_min, complete);
  search_ann = int_search(q, first_fail, indomain_median, complete);

第一个只是按顺序来选择皇后然后设置其为最小值。第二个按顺序来选择皇后，但是设置中间值给它。第三个选择定义域大小最小的皇后，然后设置最小值给它。最后一个策略选择定义域大小最小的皇后，设置中间值给它。

不同的搜索策略对于能多容易找到解有显著的差异。下面的表格给出了一个简单的关于使用4种不同的搜索策略找到n皇后问题的第一个解所要做的决策个数（其中---表示超过100,000个决策）。很明显地看到，合适的搜索策略会产生显著的提高。

.. cssclass:: table-nonfluid table-bordered

+-----+-----------+--------------+--------+-----------+
|  n  | input-min | input-median | ff-min | ff-median |
+=====+===========+==============+========+===========+
| 10  | 28        |  15          |  16    | 20        |
+-----+-----------+--------------+--------+-----------+
| 15  | 248       |  34          |  23    | 15        |
+-----+-----------+--------------+--------+-----------+
| 20  | 37330     |  97          |  114   | 43        |
+-----+-----------+--------------+--------+-----------+
| 25  | 7271      |  846         |  2637  | 80        |
+-----+-----------+--------------+--------+-----------+
| 30  | ---       |  385         |  1095  | 639       |
+-----+-----------+--------------+--------+-----------+
| 35  | ---       |  4831        |  ---   | 240       |
+-----+-----------+--------------+--------+-----------+
| 40  | ---       |  ---         |  ---   | 236       |
+-----+-----------+--------------+--------+-----------+
