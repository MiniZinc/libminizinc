.. _sec-modelling:

MiniZinc基本模型
===========================

.. highlight:: minizinc
  :linenothreshold: 5

在此节中，我们利用两个简单的例子来介绍一个MiniZinc模型的基本结构。

第一个实例
-----------------

.. _fig-aust:

.. figure:: figures/aust.*
  
  澳大利亚各州

作为我们的第一个例子，假设我们要去给 :numref:`fig-aust` 中的澳大利亚地图涂色。
它包含了七个不同的州和地区，而每一块都要被涂一个颜色来保证相邻的区域有不同的颜色。


.. literalinclude:: examples/aust.mzn
  :language: minizinc
  :caption: 一个用来给澳大利亚的州和地区涂色的MiniZinc模型 :download:`aust.mzn <examples/aust.mzn>` 
  :name: ex-aust

我们可以很容易的用MiniZinc给此问题建模。此模型在 :numref:`ex-aust` 中给出。

模型中的第一行是注释。注释开始于 ``%``  来表明此行剩下的部分是注释。
MiniZinc同时也含有C语言风格的由 ``/*`` 开始和 ``*/`` 结束的块注释。

模型中的下一部分声明了模型中的变量。
此行

::

  int: nc = 3;

定义了一个问题的 :index:`参数` 来代表可用的颜色个数。
在很多编程语言中，参数和变量是类似的。它们必须被声明并且指定一个类型 :index:`type`。
在此例中，类型是 :mzn:`int`。
通过 :index:`赋值`，它们被设了值。
MiniZinc允许变量声明时被赋值（就像上面那一行）或者单独给出一个赋值语句。
因此下面的表示跟上面的只一行表示是相等的

::

  int: nc;
  nc = 3;

和很多编程语言中的变量不一样的是，这里的参数只可以被赋 *唯一* 的值。
如果一个参数出现在了多于一个的赋值中，就会出现错误。

基本的 :index:`参数类型 <single: type; parameter>` 包括 :index:`整型 <integer>` (:mzn:`int`)，
浮点型(:mzn:`float`)， :index:`布尔型 <Boolean>` (:mzn:`bool`) 以及 :index:`字符串型 <string>` (:mzn:`string`)。
同时也支持数组和集合。

.. index::
  see: decision variable; variable

MiniZinc模型同时也可能包含另一种类型的变量 *决策变量* 。 :index:`决策变量 <variable>` 是数学的或者逻辑的变量。
和一般的编程语言中的参数和变量不同，建模者不需要给决策变量一个值。
而是在开始时，一个决策变量的值是不知道的。只有当MiniZinc模型被执行的时候，
求解系统才来决定决策变量是否可以被赋值从而满足模型中的约束。若满足，则被赋值。

在我们的模型例子中，我们给每一个区域一个 *决策变量* ``wa``, ``nt``, ``sa``, ``q``, ``nsw``, ``v`` 和 ``t``。
它们代表了会被用来填充区域的（未知）颜色。

.. index::
  single: domain

对于每一个决策变量，我们需要给出变量可能的取值集合。这个被称为变量的 *定义域* 。
定义域部分可以在 :index:`变量声明 <variable; declaration>` 的时候同时给出，
这时决策变量的 :index:`类型` 就会从定义域中的数值的类型推断出。

MiniZinc中的决策变量的类型可以为布尔型，整型，浮点型或者集合。
同时也可以是元素为决策变量的数组。
在我们的MiniZinc模型例子中，我们使用整型去给不用的颜色建模。通过使用 :mzn:`var` 声明，
我们的每一个决策变量
被声明为定义域为一个整数类型的范围表示 :mzn:`1..nc` ，
来表明集合 :math:`\{ 1, 2, \dots, nc \}` 。
所有数值的类型为整型，所以模型中的所有的变量是整型决策变量。

.. defblock:: 标识符

  用来命名参数和变量的标识符是一列由大小写字母，数字以及
  下划线 ``_`` 字符组成的字符串。它们必须开始于一个字母字符。因此 ``myName_2`` 是一个有效的标识符。
  MiniZinc（和Zinc）的 *关键字* 不允许被用为标识符名字。它们在 :ref:`spec-identifiers` 中被列出。
  所有的MiniZinc *操作符* 都不能被用做标识符名字。它们在 :ref:`spec-Operators` 中被列出。

MiniZinc仔细地区别了以下两种模型变量：参数和决策变量。利用决策变量创建的表达式类型比利用参数可以创建的表达式类型更局限。
但是，在任何可以用决策变量的地方，同类型的参数变量也可以被应用。

.. defblock:: 整型变量声明

  一个 :index:`整型参数变量 <variable; declaration; integer>` 可以被声明为以下两种方式：

  .. code-block:: minizincdef

    int : <变量名>
    <l> .. <u> : <变量名>

  :mzndef:`<l>` 和 :mzndef:`<u>` 是固定的整型表达式。

  一个整型决策变量被声明为以下两种方式：

  .. code-block:: minizincdef
    
    var int : <变量名>
    var <l>..<u> : <变量名>

  :mzndef:`<l>` 和 :mzndef:`<u>` 是固定的整型表达式。

参数和决策变量形式上的区别在于对变量的 *实例化*。
变量的实例化和类型的结合被叫为 :index:`类型-实例化` 。
既然你已经开始使用MiniZinc，毫无疑问的你会看到很多 *类型-实例化* 的错误例子。

.. index::
  single: constraint

模型的下一部分是 *约束* 。
它们详细说明了决策变量想要组成一个模型的有效解必须要满足的布尔型表达式。
在这个例子中我们有一些决策变量之间的不等式。它们规定如果两个区域是相邻的，则它们必须有不同的颜色。

.. defblock:: 关系操作符

  MiniZinc提供了以下关系操作符 :index:`关系操作符 <operator; relational>` ：

  .. index::
    single: =
    single: ==
    single: !=
    single: <
    single: <=
    single: >
    single: >=
  
  相等 (``=`` or ``==``), 不等 
  (``!=``), 
  小于 (``<``), 
  大于 (``>``), 
  小于等于 (``<=``), and
  和大于等于 (``>=``).

模型中的下一行：

::

  solve satisfy;

表明了它是什么类型的问题。
在这个例子中，它是一个 :index:`满足`  问题：
我们希望给决策变量找到一个值使得约束被满足，但具体是哪一个值却没有所谓。

.. index::
  single: output

模型的最后一个部分是 *输出* 语句。它告诉MiniZinc当模型被运行并且找到一个解 :index:`解` 的时候，
要输出什么。 

.. defblock:: 输出和字符串

  .. index::
    single: output
    single: string
    single: show

  一个输出语句跟着一 *串* 字符。
  它们通常或者是写在双引号之间的字符串常量 :index:`字符串常量 <string; literal>` 并且
  对特殊字符用类似C语言的标记法，或者是 :mzn:`show(e)` 格式的表达式，
  其中 :mzn:`e` 是MiniZinc表达式。例子中的 ``\n`` 代表换行符， ``\t`` 代表制表符。

  数字的 :mzn:`show` 有各种不同方式的表示：
  :mzn:`show_int(n,X)` 
  在至少$|n|$个字符里输出整型 ``X`` 的值，若 :math:`n > 0` 则右对齐，否则则左对齐；
  :mzn:`show_float(n,d,X)` 在至少 :math:`|n|` 个字符里输出
  浮点型 ``X`` 的值，若 :math:`n > 0` 则右对齐，否则则左对齐，并且小数点后有 :math:`d` 个字符。

  :index:`字符串常量 <string; literal>` 必须在同一行中。长的字符串常量可以利用字符串连接符 ``++``
  来分成几行。例如，字符串常量 

  ::
  
    "Invalid datafile: Amount of flour is non-negative"

  和字符串常量表达式  

  ::

    "Invalid datafile: " ++
    "Amount of flour is non-negative"

  是相等的。

  MiniZinc支持内插字符串 :index:`内插字符串 <string; literal; interpolated>` 。
  表达式可以直接插入字符串常量中。 :mzn:`"\(e)"` 形式的子字符串
  会被替代为 :mzn:`show(e)` 。例如，:mzn:`"t=\(t)\n"` 产生
  和 :mzn:`"t=" ++ show(t) ++ "\n"` 一样的字符串。

  一个模型可以包含多个输出语句。在这种情况下,所有输出会根据它们在模型中出现的顺序连接。

我们可以通过点击MiniZinc IDE中的 *Run* 按钮,或者输入

.. code-block:: bash
  
  $ minizinc --solver Gecode aust.mzn

来评估我们的模型。
其中 :download:`aust.mzn <examples/aust.mzn>` 是包含我们的MiniZinc模型的文件名字。
我们必须使用文件扩展名 ``.mzn`` 来表明一个MiniZinc模型。
带有 ``--solver Gecode`` 选项的命令 ``minizinc`` 使用Gecode有限域求解器去评估我们的模型。如果你使用的是MiniZinc二进制发布,这个求解器实际上是预设的,所以你也可以运行 ``minizinc aust.mzn`` 。

当我们运行上面的命令后，我们得到如下的结果：

.. code-block:: none

  wa=2   nt=3  sa=1
  q=2  nsw=3   v=2
  t=1
  ----------


.. index::
  single: solution; separator ----------

10个破折号 ``----------`` 这行是自动被MiniZinc输出的，用来表明一个解已经被找到。

算术优化实例
----------------------------------

我们的第二个例子来自于要为了本地的校园游乐会烤一些蛋糕的需求。
我们知道如何制作两种蛋糕。 \footnote{警告: 请不要在家里使用这些配方制作}
一个香蕉蛋糕的制作需要250克自发酵的面粉，2个捣碎的香蕉，75克糖和100克黄油。
一个巧克力蛋糕的制作需要200克自发酵的面粉，75克可可粉，150克糖和150克黄油。
一个巧克力蛋糕可以卖\$4.50，一个香蕉蛋糕可以卖\$4.00。我们一共有4千克的自发酵面粉，
6个香蕉，2千克的糖，500克的黄油和500克的可可粉。
问题是对每一种类型的蛋糕，我们需要烤多少给游乐会来得到最大的利润。
一个可能的MiniZinc模型在 :numref:`ex-cakes` 中给出。

.. literalinclude:: examples/cakes.mzn
  :language: minizinc
  :caption: 决定为了校园游乐会要烤多少香蕉和巧克力蛋糕的模型。  (:download:`cakes.mzn <examples/cakes.mzn>`)
  :name: ex-cakes

.. index::
  single: expression; arithmetic

第一个新特征是 *arithmetic expressions* 的使用。

.. defblock:: 整数算术操作符

  .. index::
    single: operator; integer
    single: +
    single: -
    single: div
    single: *
    single: mod

  MiniZinc提供了标准的整数算术操作符。  
  加 (``+``),
  减 (``-``),
  乘 (``*``),
  整数除 (:mzn:`div`) 
  和 
  整数模 (:mzn:`mod`). 
  同时也提供了一元操作符 ``+`` 和 ``-`` 。

  整数模被定义为输出和被除数 :math:`a` 一样正负的 :math:`a` :mzn:`mod` :math:`b` 值。整数除被定义为使得
  :math:`a = b ` ``*`` :math:`(a` :mzn:`div` :math:`b) + (a` :mzn:`mod` :math:`b)` 值。

  MiniZinc 提供了标准的整数函数用来取绝对值 (:mzn:`abs`) 和幂函数 (:mzn:`pow`).
  例如 :mzn:`abs(-4)` 和 :mzn:`pow(2,5)` 分别求得数值
  ``4`` 和 ``32`` 。

  算术常量的语法是相当标准的。整数常量可以是十进制，十六进制或者八进制。例如 ``0``, ``5``,
  ``123``, ``0x1b7``, ``0o777`` 。

.. index::
  single: optimization
  single: objective
  single: maximize
  single: minimize

例子中的第二个新特征是优化。这行

::

  solve maximize 400 * b + 450 * c;

指出我们想找一个可以使solve语句中的表达式（我们叫做 *目标* ）最大化的解。
这个目标可以为任何类型的算术表达式。我们可以把关键字 :mzn:`maximize`
换为 :mzn:`minimize` 来表明一个最小化问题。

当我们运行上面这个模型时，我们得到以下的结果：

.. code-block:: none

  no. of banana cakes = 2
  no. of chocolate cakes = 2
  ----------
  ==========

.. index::
  single: solution; end `==========`

一旦系统证明了一个解是最优解，这一行 ``==========`` 在最优化问题中会自动被输出。

.. index::
  single: data file

数据文件和谓词
------------------------

此模型的一个缺点是如果下一次我们希望解决一个相似的问题，即我们需要为学校烤蛋糕（这是经常发生的），
我们需要改变模型中的约束来表明食品间拥有的原料数量。如果我们想重新利用此模型，我们最好使得每个原料的数量作为
模型的参数，然后在模型的最上层设置它们的值。

更好的办法是在一个单独的 *数据文件* 中设置这些参数的值。
MiniZinc（就像很多其他的建模语言一样）允许使用数据文件来设置在原始模型中声明的
参数的值。
通过运行不同的数据文件，使得同样的模型可以很容易地和不同的数据一起使用。

数据文件的文件扩展名必须是 ``.dzn`` ，来表明它是一个MiniZinc数据文件。
一个模型可以被任何多个数据文件运行（但是每个变量/参数在每个文件中只能被赋一个值）

.. literalinclude:: examples/cakes2.mzn
  :language: minizinc
  :caption: 独立于数据的用来决定为了校园游乐会要烤多少香蕉和巧克力蛋糕的模型。(:download:`cakes2.mzn <examples/cakes2.mzn>`)
  :name: ex-cakes2

我们的新模型在 :numref:`ex-cakes2` 中给出。
我们可以用下面的命令来运行

.. code-block: bash

  $ minizinc cakes2.mzn pantry.dzn

数据文件 :download:`pantry.dzn <examples/pantry.dzn>` 在 :numref:`fig-pantry1` 中给出。我们得到和 :download:`cakes.mzn <examples/cakes.mzn>` 同样的结果。
运行下面的命令

.. code-block:: bash

  $ minizinc cakes2.mzn pantry2.dzn

利用另外一个 :numref:`fig-pantry2` 中定义的数据集，我们得到如下结果

.. code-block:: none

  no. of banana cakes = 3
  no. of chocolate cakes = 8
  ----------
  ==========

如果我们从 :download:`cakes.mzn <examples/cakes.mzn>` 中去掉输出语句，MiniZinc会使用默认的输出。
这种情况下得到的输出是

.. code-block:: none

  b = 3;
  c = 8;
  ----------
  ==========

.. defblock:: 默认输出

  一个没有输出语句的MiniZinc模型会给每一个决策变量以及它的值一个输出行，除非决策变量已经在声明的时候被赋了一个表达式。
  注意观察此输出是如何呈现一个正确的数据文件格式的。

.. literalinclude:: examples/pantry.dzn
  :language: minizinc
  :caption: :download:`cakes2.mzn <examples/cakes2.mzn>` 的数据文件例子 (:download:`pantry.dzn <examples/pantry.dzn>`)
  :name: fig-pantry1

.. literalinclude:: examples/pantry2.dzn
  :language: minizinc
  :caption: :download:`cakes2.mzn <examples/cakes2.mzn>` 的数据文件例子 (:download:`pantry2.dzn <examples/pantry2.dzn>`)
  :name: fig-pantry2

通过使用 :index:`命令行标示 <data file;command line>` 
``-D`` *string* ，
小的数据文件可以被直接输入而不是必须要创建一个 ``.dzn`` 文件，
其中 *string* 是数据文件里面的内容。

.. code-block:: bash

  $ minizinc cakes2.mzn -D \
       "flour=4000;banana=6;sugar=2000;butter=500;cocoa=500;"

会给出和

.. code-block:: bash

  $ minizinc cakes2.mzn pantry.dzn

一模一样的结果。

数据文件只能包含给模型中的决策变量和参数赋值的语句。

.. index::
  single: assert

防御性编程建议我们应该检查数据文件中的数值是否合理。
在我们的例子中，检查所有原料的份量是否是非负的并且若不正确则产生一个运行错误，这是明智的。
MiniZinc提供了一个内置的布尔型操作符 *断言* 用来检查参数值。格式是 :mzn:`assert(B,S)` 。
布尔型表达式 ``B`` 被检测。若它是假的，运行中断。此时字符串表达式 ``S`` 作为错误信息被输出。
如果我们想当面粉的份量是负值的时候去检测出并且产生合适的错误信息，我们可以直接加入下面的一行

::

  constraint assert(flour >= 0,"Amount of flour is non-negative");

到我们的模型中。注意 *断言* 表达式是一个布尔型表达式，所以它被看做是一种类型的约束。我们可以加入类似的行来检测其他原料的份量是否是非负值。

实数求解
-------------------

MiniZinc also supports "real number" constraint solving using
floating point variables and constraints.  Consider a problem of taking out a short loan
for one year to be repaid in 4 quarterly instalments. 
A model for this is shown in :numref:`ex-loan`. It uses a simple interest
calculation to calculate the balance after each quarter.

通过使用浮点数求解，MiniZinc也支持“实数”约束求解。考虑一个要在4季度分期偿还的一年短期贷款问题。
此问题的一个模型在 :numref:`ex-loan` 中给出。它使用了一个简单的计算每季度结束后所欠款的利息计算方式。

.. literalinclude:: examples/loan.mzn
  :language: minizinc
  :caption: 确定一年借款每季度还款关系的模型(:download:`loan.mzn <examples/loan.mzn>`)
  :name: ex-loan

注意我们声明了一个浮点型变量 ``f`` ，它和整型变量很类似。只是这里我们
使用关键字 :mzn:`float` 而不是 :mzn:`int` 。

我们可以用同样的模型来回答一系列不同的问题。第一个问题是：如果我以利息
4\%借款\$1000并且每季度还款\$260，我最终还欠款多少？这个问题在数据文件 :download:`loan1.dzn <examples/loan1.dzn>` 中被编码。

由于我们希望用实数求解，我们需要使用一个可以支持这种问题类型的求解器。Gecode(Minizinc捆绑二进制发布预设的求解器)支持浮点型变量,一个混合整数线性求解器可能更加适合这种类型的问题。
MiniZinc发布包含了这样的一个求解器。我们可以通过从求解器IDE菜单( *Run* 按钮下面的三角形)选择 ``OSICBC``  来使用, 或者在命令行中运行命令 ``minizinc --solver osicbc`` :

.. code-block:: bash

  $ minizinc --solver osicbc loan.mzn loan1.dzn

输出是

.. code-block:: none

  Borrowing 1000.00 at 4.0% interest, and repaying 260.00 
  per quarter for 1 year leaves 65.78 owing 
  ----------

第二个问题是如果我希望用4\%的利息来借款\$1000并且在最后的时候一点都不欠款，我需要在每季度还款多少？
这个问题在数据文件 :download:`loan2.dzn <examples/loan2.dzn>` 中被编码。
运行命令

.. code-block:: bash

  $ minizinc --solver osicbc loan.mzn loan2.dzn

后的输出是

.. code-block:: none

  Borrowing 1000.00 at 4.0% interest, and repaying 275.49
  per quarter for 1 year leaves 0.00 owing
  ----------

第三个问题是如果我可以每个季度返还\$250, 我可以用4\%的利息来借款多少并且在最后的时候一点都不欠款？
这个问题在数据文件 :download:`loan3.dzn <examples/loan3.dzn>` 中被编码。
运行命令

.. code-block:: bash

  $ minizinc --solver osicbc loan.mzn loan3.dzn

后的输出是

.. code-block:: none

  Borrowing 907.47 at 4.0% interest, and repaying 250.00
  per quarter for 1 year leaves 0.00 owing
  ----------

.. literalinclude:: examples/loan1.dzn
  :language: minizinc
  :caption: :download:`loan.mzn <examples/loan.mzn>` 的数据文件例子(:download:`loan1.dzn <examples/loan1.dzn>`)

.. literalinclude:: examples/loan2.dzn
  :language: minizinc
  :caption: :download:`loan.mzn <examples/loan.mzn>` 的数据文件例子(:download:`loan2.dzn <examples/loan2.dzn>`)

.. literalinclude:: examples/loan3.dzn
  :language: minizinc
  :caption: :download:`loan.mzn <examples/loan.mzn>` 的数据文件例子(:download:`loan3.dzn <examples/loan3.dzn>`)


.. defblock:: 浮点算术操作符

  .. index:
    single: operator; float
    single: +
    single: -
    single: *
    single: /
    single: abs
    single: sqrt 
    single: ln
    single: log2 
    single: log10
    single: exp 
    single: sin 
    single: cos 
    single: tan
    single: asin 
    single: acos 
    single: atan 
    single: sinh
    single: cosh
    single: tanh
    single: asinh
    single: acosh 
    single: atanh
    single: pow

  MiniZinc提供了标准的浮点算术操作符:  
  加 (``+``), 
  减 (``-``),
  乘 (``*``) 
  和浮点除 (``/``)。 
  同时也提供了一元操作符 ``+`` 和 ``-`` 。 

  MiniZinc不会自动地强制转换整数为浮点数。内建函数 :mzn:`int2float` 被用来达到此目的。注意强制转换的一个后果是表达式 :mzn:`a / b` 总是被认为是一个浮点除。 如果你需要一个整数除,请确定使用 :mzn:`div` 操作符。

  MiniZinc同时也包含浮点型函数来计算:
  绝对值 (``abs``),
  平方根 (``sqrt``), 
  自然对数 (``ln``),
  底数为2的对数 (``log2``), 
  底数为10的对数 (``log10``),
  $e$的幂 (``exp``), 
  正弦 (``sin``), 
  余弦 (``cos``), 
  正切 (``tan``),
  反正弦 (``asin``), 
  反余弦 (``acos``), 
  反正切 (``atan``), 
  双曲正弦 (``sinh``),
  双曲余弦 (``cosh``),
  双曲正切 (``tanh``),
  双曲反正弦 (``asinh``),
  双曲反余弦 (``acosh``), 
  双曲反正切 (``atanh``),
  和唯一的二元函数次方 (``pow``)，其余的都是一元函数。

  算术常量的语法是相当标准的。浮点数常量的例子有 ``1.05``， ``1.3e-5`` 和 ``1.3E+5`` 。

.. \pjs{Should do multiple solutions????}

模型的基本结构
--------------------------

我们现在可以去总结MiniZinc模型的基本结构了。
它由多个项组成，每一个在其最后都有一个分号 ``;`` 。
项可以按照任何顺序出现。例如，标识符在被使用之前不需要被声明。

有八种类型的项 :index:`items <item>` 。

- :index:`引用项 <item; include>` 允许另外一个文件的内容被插入模型中。
  它们有以下形式：
  
  .. code-block:: minizincdef
  
    include <文件名>;

  其中 :mzndef:`<文件名>` 是一个字符串常量。
  它们使得大的模型可以被分为小的子模型以及包含库文件中定义的约束。
  我们会在 :numref:`ex-smm` 中看到一个例子。

- :index:`变量声明 <item; variable declaration>` 声明新的变量。
  这种变量是全局变量，可以在模型中的任何地方被提到。
  变量有两种。在模型中被赋一个固定值的参数变量以及只有在模型被求解的时候才会
  被赋值的决策变量。
  我们称参数是 :index:`固定的` ，决策变量是 :index:`不固定的` 。
  变量可以选择性地被赋一个值来作为声明的一部分。形式是：

  .. index:
    single: expression; type-inst
    single: par
    single: var

  .. code-block:: minizincdef

    <类型-实例化 表达式>: <变量> [ = ] <表达式>;

  :mzndef:`<类型-实例化 表达式>`
  给了变量的类型和实例化。 这些是MiniZinc比较复杂的其中一面。
  用 :mzn:`par` 来实例化声明
  参数，用 :mzn:`var` 来实例化声明决策变量。
  如果没有明确的实例化声明，则变量是一个参数。类型可以为基类型，一个 :index:`整数或者浮点数范围 <range>`，或者一个数组或集合。
  基类型有
  :mzn:`float`,
  :mzn:`int`, 
  :mzn:`string`, 
  :mzn:`bool`,
  :mzn:`ann` 
  。其中只有
  :mzn:`float`, :mzn:`int` and :mzn:`bool` 可以被决策变量使用。
  基类型 :mzn:`ann` 是一个 :index:`注解` --
  我们会在 :ref:`sec-search` 中讨论注解。
  :index:`整数范围表达式 <range; integer>` 可以被用来代替类型 :mzn:`int`. 
  类似的， :index:`浮点数范围表达式 <range; float>` 
  可以被用来代替类型 :mzn:`float` 。
  这些通常被用来定义一个整型决策变量的定义域，但也可以被用来限制一个整型参数的范围。
  变量声明的另外一个用处是定义 :index:`枚举类型`, ---我们会在 :ref:`sec-enum` 中讨论。

- :index:`赋值项 <item; assignment>` 给一个变量赋一个值。它们有以下形式：

  .. code-block:: minizincdef

    <变量> = <表达式>;

  数值可以被赋给决策变量。在这种情况下，赋值相当于加入 :mzndef:`constraint <变量> = <表达式>` ;

- :index:`约束项 <item; constraint>` 是模型的核心。它们有以下形式：

  .. code-block:: minizincdef
  
    constraint <布尔型表达式>;

  我们已经看到了使用算术比较的简单约束以及内建函数 :mzn:`assert` 操作符。 在下一节我们会看到更加复杂的约束例子。

- :index:`求解项 <item; solve>` 详细说明了到底要找哪种类型的解。
  正如我们看到的，它们有以下三种形式：
  
  .. code-block:: minizincdef

    solve satisfy;
    solve maximize <算术表达式>;
    solve minimize <算术表达式>;

  一个模型必须有且只有一个求解项。

- :index:`输出项 <item; output>` 用来恰当的呈现模型运行后的结果。
  它们有下面的形式：
  
  .. code-block:: minizincdef

    output [ <字符串表达式>, ..., <字符串表达式> ];

  如果没有输出项，MiniZinc会默认输出所有没有被以赋值项的形式赋值的决策变量值。

- :index:`枚举类型声明 <item; enum>`.
  我们会在 :ref:`sec-arrayset` 和 :ref:`sec-enum` 中讨论。

- :index:`谓词函数和测试项 <item; predicate>` 被用来定义新的约束，函数和布尔测试。我们会在 :ref:`sec-predicates` 中讨论。


- :index:`注解项 <item; annotation>` 用来定义一个新的注解。我们会在 :ref:`sec-search` 中讨论。
